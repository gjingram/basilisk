
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

#ifndef BASILISK_SPACETOGROUNDTRANSMITTER_H
#define BASILISK_SPACETOGROUNDTRANSMITTER_H

#include "onboardDataHandling/_GeneralModuleFiles/dataNodeBase.h"
#include "simMessages/accessSimMsg.h"
#include "utilities/bskLogging.h"

/*! @brief space to ground data transmitter class */
class SpaceToGroundTransmitter: public DataNodeBase {
public:
    SpaceToGroundTransmitter();
    ~SpaceToGroundTransmitter();
    void addStorageUnitToTransmitter(std::string tmpStorageUnitMsgName);
    void addAccessMsgToTransmitter(std::string tmpAccessMsgName);

private:
    void evaluateDataModel(DataNodeUsageSimMsg *dataUsageMsg, double currentTime);
    bool customReadMessages();
    void customCrossInit();

public:
    double packetSize; //!< Size of packet to downklink (bytes)
    int numBuffers; //!< Number of buffers the transmitter can access
    std::vector<std::string> storageUnitMsgNames;           //!< Vector of data node input message names
    std::vector<std::int64_t> storageUnitMsgIds;            //!< class variable
    std::vector<std::string> groundLocationAccessMsgNames;  //!< class variable
    std::vector<std::int64_t> groundLocationAccessMsgIds;   //!< class variable
    std::vector<DataStorageStatusSimMsg> storageUnitMsgs;   //!< class variable
    uint64_t hasAccess;                                     //!< class variable
    BSKLogger bskLogger;                                    //!< class variable

private:
    double packetTransmitted; //!< Amount of packet downlinked (bytes)
    double currentTimestep; //!< Current timestep tracked for data packet integration
    double previousTime; //!< Previous timestep tracked for data packet integration
    std::vector<dataInstance> storedData; //! Vector of data. Represents the makeup of the data buffer. Created from input messages.
    std::vector<AccessSimMsg> groundLocationAccessMsgs; //!< class variable
};
#endif //BASILISK_SPACETOGROUNDTRANSMITTER_H
