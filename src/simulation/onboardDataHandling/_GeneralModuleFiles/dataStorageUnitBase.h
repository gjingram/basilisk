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

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <cstring>
#include "_GeneralModuleFiles/sys_model.h"
#include "simMessages/scPlusStatesSimMsg.h"
#include "simMessages/dataStorageStatusSimMsg.h"
#include "simMessages/dataNodeUsageSimMsg.h"
#include "utilities/bskLogging.h"

#ifndef BASILISK_DATASTORAGEUNITBASE_H
#define BASILISK_DATASTORAGEUNITBASE_H

/*! @brief on-board data handling base class */
class DataStorageUnitBase: public SysModel {
public:
    DataStorageUnitBase();
    ~DataStorageUnitBase();
    void SelfInit();
    void CrossInit();
    void Reset(uint64_t CurrentSimNanos);
    void addDataNodeToModel(std::string tmpNodeMsgName); //!< Adds dataNode to the storageUnit
    void UpdateState(uint64_t CurrentSimNanos);
    std::vector<dataInstance> getStoredDataAll(); //!< Getter function for the storedData vector.
    double getStoredDataSum(); //!< Getter function for the storedDataSum

protected:
    void writeMessages(uint64_t CurrentClock);
    bool readMessages();
    virtual void integrateDataStatus(double currentTime); //!< Integrates the dataStatus over all of the dataNodes
    virtual void customSelfInit(); //!< Custom output input reading method.  This allows a child class to add additional functionality.
    virtual void customCrossInit(); //!< Custom subscription method, similar to customSelfInit.
    virtual void customReset(uint64_t CurrentClock); //!< Custom Reset method, similar to customSelfInit.
    virtual void customWriteMessages(uint64_t CurrentClock); //!< custom Write method, similar to customSelfInit.
    virtual bool customReadMessages(); //!< Custom read method, similar to customSelfInit; returns `true' by default.
    int messageInStoredData(DataNodeUsageSimMsg *tmpNodeMsg); //!< Returns index of the dataName if it's already in storedData
    double sumAllData(); //!< Sums all of the data in the storedData vector

public:
    std::vector<std::string> nodeDataUseMsgNames; //!< Vector of data node input message names
    std::string storageUnitDataOutMsgName; //!< Vector of message names to be written out by the storage unit
    double storageCapacity; //!< Storage capacity of the storage unit
    BSKLogger bskLogger;    //!< logging variable

protected:
    std::vector<std::int64_t> nodeDataUseMsgIds; //!< Vector of all the data node messages IDs the storage unit is subscribed to
    int64_t storageUnitDataOutMsgId; //!< Message ID of storage Unit output message
    DataStorageStatusSimMsg storageStatusMsg; //!< class variable
    std::vector<DataNodeUsageSimMsg> nodeBaudMsgs; //!< class variable
    double storedDataSum; //!< [bits] Stored data in bits.
    std::vector<dataInstance> storedData; //!< Vector of data. Represents the makeup of the data buffer.
    double previousTime; //!< Previous time used for integration
    double currentTimestep;//!< [s] Timestep duration in seconds.
    double netBaud; //!< Net baud rate at a given time step
    uint64_t outputBufferCount;     //!< class variable
};

#endif //BASILISK_DATASTORAGEUNITBASE_H
