/*
 Copyright (c) 2016, Autonomous Vehicle Systems Lab, Univeristy of Colorado at Boulder
 
 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 
 */


#include "dataFileToViz.h"
#include "architecture/messaging/system_messaging.h"
#include "simMessages/scPlusStatesSimMsg.h"
#include "utilities/linearAlgebra.h"
#include "utilities/rigidBodyKinematics.h"
#include "utilities/avsEigenSupport.h"
#include <sstream>
#include <string>
#include <string.h>
#include <vector>

/*! DataFileToViz Constructor
 */
DataFileToViz::DataFileToViz()
{
    this->dataFileName = "";
    this->numSatellites = 1;
    this->delimiter = " ";
    this->convertPosToMeters = 1000.;       /* convert km to meters */
    this->headerLine = true;
    this->attitudeType = 0;
    this->OutputBufferCount = 2;

    return;
}

/*! DataFileToViz Destructor
 */
DataFileToViz::~DataFileToViz()
{
    /* close the data file if it is open */
    if(this->fileHandle->is_open()) {
        this->fileHandle->close();
        bskLogger.bskLog(BSK_INFORMATION, "DataFileToViz:\nclosed the file: %s.", this->dataFileName.c_str());
    }

    return;
}

/*! Initialization method
 */
void DataFileToViz::SelfInit()
{
    int64_t msgId;

    // create all the environment output messages for each spacecraft
    std::vector<std::string>::iterator it;
    for (it = this->scStateOutMsgNames.begin(); it!=this->scStateOutMsgNames.end(); it++) {
        msgId = SystemMessaging::GetInstance()->CreateNewMessage(*it,
                                                                sizeof(SCPlusStatesSimMsg),
                                                                this->OutputBufferCount,
                                                                "SCPlusStatesSimMsg",
                                                                moduleID);
        this->scStateOutMsgIds.push_back(msgId);

    }

    // create thruster output messages
    if (this->thrMsgDataSC.size() > 0) {
        std::vector <std::vector <ThrClusterMap>>::iterator thrMsgData;
        for (thrMsgData = this->thrMsgDataSC.begin(); thrMsgData!=this->thrMsgDataSC.end(); thrMsgData++) {

            std::vector<ThrClusterMap>::iterator thrSet;
            for (thrSet = (*thrMsgData).begin(); thrSet!=(*thrMsgData).end(); thrSet++) {

                for (uint32_t idx = 0; idx<thrSet->thrCount; idx++) {
                    std::string thrMsgName = "thruster_" + thrSet->thrTag + "_" + std::to_string(idx) + "_data";
                    msgId = SystemMessaging::GetInstance()->CreateNewMessage(thrMsgName,
                                                                            sizeof(THROutputSimMsg),
                                                                            this->OutputBufferCount,
                                                                            "THROutputSimMsg",
                                                                            moduleID);
                    this->thrMsgIds.push_back(msgId);
                }
            }
        }
    }

    // create RW output messages
    if (this->rwMsgOutNamesSC.size() > 0) {
        std::vector <std::vector <std::string>>::iterator sc;
        for (sc = this->rwMsgOutNamesSC.begin(); sc!=this->rwMsgOutNamesSC.end(); sc++) {
            std::vector <std::string>::iterator name;
            for (name = (*sc).begin(); name!=(*sc).end(); name++) {
                msgId = SystemMessaging::GetInstance()->CreateNewMessage(*name,
                                                                        sizeof(RWConfigLogSimMsg),
                                                                        this->OutputBufferCount,
                                                                        "RWConfigLogSimMsg",
                                                                        moduleID);
                this->rwMsgIds.push_back(msgId);
            }
        }
    }

    return;
}

/*! Cross initialization. This module does not subscribe to any other messages.
 */
void DataFileToViz::CrossInit()
{
    return;
}

/*! A Reset method to put the module back into a clean state
 @param CurrentSimNanos The current sim time in nanoseconds
 */
void DataFileToViz::Reset(uint64_t CurrentSimNanos)
{
    if (this->numSatellites < 1) {
        bskLogger.bskLog(BSK_ERROR, "DataFileToViz: numSatellites must be 1 or larger, not %d.", this->numSatellites);
    }
    if (this->dataFileName.length() == 0) {
        bskLogger.bskLog(BSK_ERROR, "DataFileToViz: dataFileName must be an non-empty string.");
    }
    if (this->numSatellites != (int) this->scStateOutMsgNames.size()) {
        bskLogger.bskLog(BSK_ERROR, "DataFileToViz: numSatellites must the same size as scStateOutMsgNames vector.");
    }

    /* check thruster states */
    if (this->thrMsgDataSC.size() > 0) {

        /* evaluate total number of thrusters */
        int numThr = 0;
        std::vector <std::vector <ThrClusterMap>>::iterator thrMsgData;
        for (thrMsgData = this->thrMsgDataSC.begin(); thrMsgData!=this->thrMsgDataSC.end(); thrMsgData++) {
            std::vector<ThrClusterMap>::iterator thrSet;
            for (thrSet = (*thrMsgData).begin(); thrSet!=(*thrMsgData).end(); thrSet++) {
                numThr += thrSet->thrCount;
            }
        }

        /* check vector dimensions */
        if (numThr != (int) this->thrPosList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: thrPosList must the same size as the number of thrusters.");
        }

        if (numThr != (int) this->thrDirList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: thrDirList must the same size as the number of thrusters.");
        }

        if (numThr != (int) this->thrForceMaxList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: thrForceMaxList must the same size as the number of thrusters.");
        }
    }

    /* check RW states */
    if (this->rwMsgOutNamesSC.size() > 0) {

        if (this->numSatellites != (int) this->rwMsgOutNamesSC.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: rwMsgOutNamesSC size must be equal to numSatellites");
        }

        int numRW = 0;
        std::vector <std::vector <std::string>>::iterator nameList;
        for (nameList = this->rwMsgOutNamesSC.begin(); nameList != this->rwMsgOutNamesSC.end(); nameList++) {
            numRW += (int) (*nameList).size();
        }

        /* check vector dimensions */
        if (numRW != (int) this->rwPosList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: rwPosList must the same size as the total number of RWs.");
        }

        if (numRW != (int) this->rwDirList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: rwDirList must the same size as the total number of RWs.");
        }

        if (numRW != (int) this->rwOmegaMaxList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: rwOmegaMaxList must the same size as the total number of RWs.");
        }

        if (numRW != (int) this->rwUMaxList.size()) {
            bskLogger.bskLog(BSK_ERROR, "DataFileToViz: rwUMaxList must the same size as the total number of RWs.");
        }
    }

    /* open the data file*/
    this->fileHandle = new std::ifstream(this->dataFileName);
    if (this->fileHandle->fail()) {
        bskLogger.bskLog(BSK_ERROR, "DataFileToViz: was not able to load the file %s.", this->dataFileName.c_str());
    }
    if (this->headerLine) {
        std::string line;
        getline(*this->fileHandle, line);
    }

    bskLogger.bskLog(BSK_INFORMATION, "DataFileToViz:\nloaded the file: %s.", this->dataFileName.c_str());


    return;
}

/*!
 Add a thruster 3d position vector to the list of thruster locations
 */
void DataFileToViz::appendThrPos(double pos_B[3])
{
    this->thrPosList.push_back(cArray2EigenVector3d(pos_B));
}

/*!
 Add a thruster 3d unit direction vector to the list.  The input vectors gets normalized before being added to the list.
 */
void DataFileToViz::appendThrDir(double dir_B[3])
{
    v3Normalize(dir_B, dir_B);
    this->thrDirList.push_back(cArray2EigenVector3d(dir_B));
}

/*!
 Add a thruster maximum force value to the list of thrusters.
 */
void DataFileToViz::appendThrForceMax(double forceMax)
{
    this->thrForceMaxList.push_back(forceMax);
}

/*!
 Add a thruster cluster map for each spacecraft
*/
void DataFileToViz::appendThrClusterMap(std::vector <ThrClusterMap> thrMsgData)
{
    this->thrMsgDataSC.push_back(thrMsgData);
}

/*!
 Add a RW output msg list for each spacecraft
*/
void DataFileToViz::appendRwMsgNames(std::vector <std::string> rwMsgNameList)
{
    this->rwMsgOutNamesSC.push_back(rwMsgNameList);
}


/*!
 Add a RW maximum motor torque value to the list
 */
void DataFileToViz::appendUMax(double uMax)
{
    this->rwUMaxList.push_back(uMax);
}

/*!
 Add a RW wheel rate value to the list
 */
void DataFileToViz::appendOmegaMax(double OmegaMax)
{
    this->rwOmegaMaxList.push_back(OmegaMax);
}

/*!
 Add a thruster 3d position vector to the list of thruster locations
 */
void DataFileToViz::appendRwPos(double pos_B[3])
{
    this->rwPosList.push_back(cArray2EigenVector3d(pos_B));
}

/*!
 Add a RW spin axis unit direction vector to the list.  The input vectors gets normalized before being added to the list.
 */
void DataFileToViz::appendRwDir(double dir_B[3])
{
    v3Normalize(dir_B, dir_B);
    this->rwDirList.push_back(cArray2EigenVector3d(dir_B));
}


/*! Update this module at the task rate
 @param CurrentSimNanos The current sim time
 */
void DataFileToViz::UpdateState(uint64_t CurrentSimNanos)
{
    /* ensure that a file was opened */
    if (this->fileHandle->is_open()) {
        int thrCounter = 0;
        int rwCounter = 0;
        int scCounter = 0;

        /* read in next line*/
        std::string line;
        if (getline(*this->fileHandle, line)) {

            std::istringstream iss(line);

            /* pull time, this is not used in the BSK msg */
            pullScalar(&iss);

            // create all the state output messages for each spacecraft
            std::vector<int64_t>::iterator it;
            for (it = this->scStateOutMsgIds.begin(); it!=this->scStateOutMsgIds.end(); it++) {
                SCPlusStatesSimMsg scMsg;

                /* zero output message */
                memset(&scMsg, 0x0, sizeof(SCPlusStatesSimMsg));

                /* get inertial position */
                pullVector(&iss, scMsg.r_CN_N);
                v3Scale(this->convertPosToMeters, scMsg.r_CN_N, scMsg.r_CN_N);
                v3Copy(scMsg.r_CN_N, scMsg.r_BN_N);

                /* get inertial velocity */
                pullVector(&iss, scMsg.v_CN_N);
                v3Scale(this->convertPosToMeters, scMsg.v_CN_N, scMsg.v_CN_N);
                v3Copy(scMsg.v_CN_N, scMsg.v_BN_N);

                /* get inertial attitude and rates */
                double att[4];
                if (this->attitudeType != 1) {
                    /* 3D attitude coordinate set */
                    pullVector(&iss, att);
                } else {
                    /* 4D attitude coordinate set */
                    pullVector4(&iss, att);
                }
                switch (this->attitudeType) {
                    case 0:
                        /* MRPs */
                        v3Copy(att, scMsg.sigma_BN);
                        break;
                    case 1:
                        /* quaternions (q0, q1, q2, q3) */
                        EP2MRP(att, scMsg.sigma_BN);
                        break;
                    case 2:
                        /* (3-2-1) Euler Angles */
                        Euler3212MRP(att, scMsg.sigma_BN);
                        break;
                    default:
                        bskLogger.bskLog(BSK_ERROR, "DataFileToViz: unknown attitudeType encountered: %d", this->attitudeType);
                        break;
                }
                pullVector(&iss, scMsg.omega_BN_B);

                /* write spacecraft state message */
                SystemMessaging::GetInstance()->WriteMessage(*it,
                                                          CurrentSimNanos,
                                                          sizeof(SCPlusStatesSimMsg),
                                                          reinterpret_cast<uint8_t*>(&scMsg),
                                                          moduleID);

                /* check if thruster states are provided */
                if (this->thrMsgDataSC.size() > 0) {

                    std::vector<ThrClusterMap>::iterator thrSet;
                    for (thrSet = this->thrMsgDataSC[scCounter].begin(); thrSet!=this->thrMsgDataSC[scCounter].end(); thrSet++) {
                        for (uint32_t idx = 0; idx<thrSet->thrCount; idx++) {
                            THROutputSimMsg thrMsg;

                            /* fill out the thruster state message */
                            thrMsg.maxThrust = this->thrForceMaxList[thrCounter];
                            thrMsg.thrustForce = pullScalar(&iss);
                            eigenVector3d2CArray(this->thrPosList[thrCounter], thrMsg.thrusterLocation);
                            eigenVector3d2CArray(this->thrDirList[thrCounter], thrMsg.thrusterDirection);

                            SystemMessaging::GetInstance()->WriteMessage(this->thrMsgIds[thrCounter],
                                                                        CurrentSimNanos,
                                                                        sizeof(THROutputSimMsg),
                                                                        reinterpret_cast<uint8_t*>(&thrMsg),
                                                                        moduleID);
                            thrCounter++;
                        }
                    }
                }

                /* check if RW states are provided */
                if (this->rwMsgOutNamesSC.size() > 0) {
                    if (this->rwMsgOutNamesSC[scCounter].size() > 0) {
                        std::vector<std::string>::iterator rwDevice;
                        for( rwDevice =this->rwMsgOutNamesSC[scCounter].begin(); rwDevice != this->rwMsgOutNamesSC[scCounter].end(); rwDevice++) {

                            RWConfigLogSimMsg rwOutMsg;
                            memset(&rwOutMsg, 0x0, sizeof(RWConfigLogSimMsg));

                            /* create RW message */
                            rwOutMsg.Omega = pullScalar(&iss);
                            rwOutMsg.Omega_max = this->rwOmegaMaxList[rwCounter];
                            rwOutMsg.u_current = pullScalar(&iss);
                            rwOutMsg.u_max = this->rwUMaxList[rwCounter];
                            eigenVector3d2CArray(this->rwPosList[rwCounter], rwOutMsg.rWB_B);
                            eigenVector3d2CArray(this->rwDirList[rwCounter], rwOutMsg.gsHat_B);

                            SystemMessaging::GetInstance()->WriteMessage(this->rwMsgIds[rwCounter],
                                                                        CurrentSimNanos,
                                                                        sizeof(RWConfigLogSimMsg),
                                                                        reinterpret_cast<uint8_t*>(&rwOutMsg),
                                                                        moduleID);

                            rwCounter++;
                        }
                    }
                }

                scCounter++;
            }
        } else {
            bskLogger.bskLog(BSK_INFORMATION, "DataFileToViz: reached end of file.");
        }
    }

    return;
}

/*! pull a 3-d set of double values from the input stream
 */
void DataFileToViz::pullVector(std::istringstream *iss, double vec[3]) {
    double x,y,z;
    x = pullScalar(iss);
    y = pullScalar(iss);
    z = pullScalar(iss);
    v3Set(x, y, z, vec);
}

/*! pull a 4-d set of double values from the input stream
 */
void DataFileToViz::pullVector4(std::istringstream *iss, double *vec) {
    double q0, q1, q2, q3;
    q0 = pullScalar(iss);
    q1 = pullScalar(iss);
    q2 = pullScalar(iss);
    q3 = pullScalar(iss);
    v4Set(q0, q1, q2, q3, vec);
}


/*! pull a double from the input stream
*/
double DataFileToViz::pullScalar(std::istringstream *iss) {
    const char delimiterString = *this->delimiter.c_str();
    std::string item;

    getline(*iss, item, delimiterString);

    return stod(item);
}
