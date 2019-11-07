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

//#pragma once

#ifndef BASILISK_DATANODEBASE_H
#define BASILISK_DATANODEBASE_H

#include <Eigen/Dense>
#include <vector>
#include <string>
#include "_GeneralModuleFiles/sys_model.h"
#include <simMessages/dataNodeUsageSimMsg.h>
#include "simFswInterfaceMessages/dataNodeStatusIntMsg.h"

class DataNodeBase: public SysModel {
public:
    DataNodeBase();
    ~DataNodeBase();
    void SelfInit();
    void CrossInit();
    void Reset(uint64_t CurrentSimNanos);
    void computeDataStatus(double currentTime);
    void UpdateState(uint64_t CurrentSimNanos);

protected:
    void writeMessages(uint64_t CurrentClock);
    bool readMessages();
    virtual void evaluateDataModel(DataNodeUsageSimMsg *dataUsageMsg)=0; //!< Virtual void method used to compute module-wise data usage/generation.
    virtual void customSelfInit(){};//! Custom output input reading method.  This allows a child class to add additional functionality.
    virtual void customCrossInit(){}; //! Custom subscription method, similar to customSelfInit.
    virtual void customReset(uint64_t CurrentClock){}; //! Custom Reset method, similar to customSelfInit.
    virtual void customWriteMessages(uint64_t CurrentClock){};//! custom Write method, similar to customSelfInit.
    virtual bool customReadMessages(){return true;} //! Custom read method, similar to customSelfInit; returns `true' by default.

public:
    std::string nodeDataOutMsgName; //!< Message name for the node's output message
    std::string nodeStatusInMsgName; //!< String for the message name that tells the node it's status
    double nodeBaudRate; //!< [baud] Data provided (+) or consumed (-).
    std::string nodeDataName; //!< Name of the data (Instrument 1, Instrument 2, etc)
    uint64_t dataStatus; //!< Device data mode; by default, 0 is off and 1 is on. Additional modes can fill other slots

protected:
    int64_t nodeDataOutMsgId;
    int64_t nodeStatusInMsgId;
    DataNodeUsageSimMsg nodeDataMsg;
    DataNodeStatusIntMsg nodeStatusMsg;
    double currentDataConsumption;
    double previousTime; //! Previous time used for integration
};

#endif //BASILISK_DATANODEBASE_H