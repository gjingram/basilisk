/*
 ISC License

 Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

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

#include "spaceToGroundTransmitter.h"
#include "architecture/messaging/system_messaging.h"
#include "utilities/bskLogging.h"
#include <array>

/*! Constructor, which sets the default nodeDataOut to zero.
*/
SpaceToGroundTransmitter::SpaceToGroundTransmitter(){
    this->packetSize = 0.0;
    this->nodeBaudRate = 0.0;
    this->packetTransmitted = 0.0;
    this->previousTime = 0.0;
    this->numBuffers = 0;
    return;
}

SpaceToGroundTransmitter::~SpaceToGroundTransmitter(){

    return;
}

/*! Adds a dataStorageStatusSimMsg name to be accessed by transmitter.
 @return void
 @param tmpStorageUnitMsgName A spacecraft state message name.
 */
void SpaceToGroundTransmitter::addStorageUnitToTransmitter(std::string tmpStorageUnitMsgName){
    this->storageUnitMsgNames.push_back(tmpStorageUnitMsgName);
    return;
}

/*! Adds a msg name to ground location access list
    @return void
    @param tmpAccessMsgName input name.
*/
void SpaceToGroundTransmitter::addAccessMsgToTransmitter(std::string tmpAccessMsgName){
    this->groundLocationAccessMsgNames.push_back(tmpAccessMsgName);
    return;
}

void SpaceToGroundTransmitter::customCrossInit(){
    //! - subscribe to the spacecraft messages and create associated output message buffer
    std::vector<std::string>::iterator it;
    for(it = this->storageUnitMsgNames.begin(); it != this->storageUnitMsgNames.end(); it++){
        this->storageUnitMsgIds.push_back(SystemMessaging::GetInstance()->subscribeToMessage(*it, sizeof(DataStorageStatusSimMsg), moduleID));
    }

    for(it = this->groundLocationAccessMsgNames.begin(); it != this->groundLocationAccessMsgNames.end(); it++){
        this->groundLocationAccessMsgIds.push_back(SystemMessaging::GetInstance()->subscribeToMessage(*it, sizeof(AccessSimMsg), moduleID));
    }

    return;
}

bool SpaceToGroundTransmitter::customReadMessages(){

    DataStorageStatusSimMsg nodeMsg;
    AccessSimMsg accessMsg;
    SingleMessageHeader localHeader;

    this->storageUnitMsgs.clear();
    this->groundLocationAccessMsgs.clear();

    //! - read in the data node use/supply messages
    bool dataRead = true;
    bool tmpDataRead;

    if(this->storageUnitMsgIds.size() > 0)
    {
        std::vector<int64_t>::iterator it;
        for(it = storageUnitMsgIds.begin(); it!= storageUnitMsgIds.end(); it++)
        {
            tmpDataRead = SystemMessaging::GetInstance()->ReadMessage(*it, &localHeader,
                                                                      sizeof(DataStorageStatusSimMsg),
                                                                      reinterpret_cast<uint8_t*>(&nodeMsg),
                                                                      moduleID);
            dataRead = dataRead && tmpDataRead;

            this->storageUnitMsgs.push_back(nodeMsg);
        }
    }
    else {
        bskLogger.bskLog(BSK_INFORMATION, "Data storage has no data node messages to read.");
        dataRead = false;
    }
    if(this->groundLocationAccessMsgIds.size() > 0)
    {
        std::vector<int64_t>::iterator it;
        for(it = groundLocationAccessMsgIds.begin(); it!= groundLocationAccessMsgIds.end(); it++)
        {
            tmpDataRead = SystemMessaging::GetInstance()->ReadMessage(*it, &localHeader,
                                                                      sizeof(AccessSimMsg),
                                                                      reinterpret_cast<uint8_t*>(&accessMsg),
                                                                      moduleID);
            dataRead = dataRead && tmpDataRead;

            this->groundLocationAccessMsgs.push_back(accessMsg);
        }
    }
    else {
        bskLogger.bskLog(BSK_INFORMATION, "SpaceToGroundTransmitter has no ground stations attached.");
        dataRead = false;
    }


    return true;
}

/*! Loads the nodeDataOut attribute into the dataUsageSimMessage instance.
 @param dataUsageSimMsg
 @param currentTime
*/
void SpaceToGroundTransmitter::evaluateDataModel(DataNodeUsageSimMsg *dataUsageSimMsg, double currentTime){

    this->currentTimestep = currentTime - this->previousTime;

    dataUsageSimMsg->baudRate = this->nodeBaudRate;

    //! - If we have access to any ground location, do the transmission logic
    if (std::any_of(this->groundLocationAccessMsgs.begin(), this->groundLocationAccessMsgs.end(), [](AccessSimMsg msg){return msg.hasAccess>0;})){
        //! - If we have no transmitted any of the packet, we select a new type of data to downlink
        if (this->packetTransmitted == 0.0) {

            //! - Loop through storedData, select index w/ largest amt
            double maxVal = 0.0;
            int maxIndex = 0;
            for (int i = 0; i < this->numBuffers; i++) {
                if (this->storageUnitMsgs.back().storedData[i] > maxVal) {
                    maxVal = this->storageUnitMsgs.back().storedData[i];
                    maxIndex = i;
                }
            }

            // Set nodeDataName to the maximum data name
            strncpy(this->nodeDataName, this->storageUnitMsgs.back().storedDataName[maxIndex],
                    sizeof(this->nodeDataName));

            // strncpy nodeDataName to the name of the output message
            strncpy(dataUsageSimMsg->dataName, this->nodeDataName, sizeof(dataUsageSimMsg->dataName));
            this->packetTransmitted += this->nodeBaudRate * (this->currentTimestep);

            // Check to see if maxVal is less than packet size.
            // If so, set the output message baudRate to zero
            // We do not want to start downlinking until we have enough data for one packet
            if (maxVal < (-1*(this->packetSize))) {
                dataUsageSimMsg->baudRate = 0;
                this->packetTransmitted = 0;
            }
        }
        else {
            strncpy(dataUsageSimMsg->dataName, this->nodeDataName, sizeof(dataUsageSimMsg->dataName));
            this->packetTransmitted += this->nodeBaudRate * (this->currentTimestep);
            // If the transmitted packet size has exceeded the packet size, set packetTransmitted to zero
            if (this->packetTransmitted <= this->packetSize) {
                this->packetTransmitted = 0.0;
            }
        }
        this->previousTime = currentTime;
        return;
    }
    // If we don't have access, we can't transmit anything
    else{
        dataUsageSimMsg->baudRate = 0;
        this->packetTransmitted = 0;
    }
}
