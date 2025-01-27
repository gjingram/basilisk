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
#include <cstdint>


#include "simpleStorageUnit.h"
#include "utilities/bskLogging.h"


/*! The constructor creates a SimpleStorageUnit instance with zero stored data
 @return void
 */
SimpleStorageUnit::SimpleStorageUnit(){
    this->storageCapacity = -1.0;
    this->storedDataSum = 0.0;
    return;
}

/*! Destructor
 @return void
 */
SimpleStorageUnit::~SimpleStorageUnit(){
    return;
}

/*! Custom reset function
 @param currentClock
 */
void SimpleStorageUnit::customReset(uint64_t currentClock){
    if (this->storageCapacity <= 0.0) {
        bskLogger.bskLog(BSK_INFORMATION, "The storageCapacity variable must be set to a positive value.");
    }
    return;
}

/*! Overwrites the integrateDataStatus method to create a single partition in the storage unit ("STORED DATA")
 @param currentTime
 @return void
 */
void SimpleStorageUnit::integrateDataStatus(double currentTime){
    this->currentTimestep = currentTime - this->previousTime;
    this->netBaud = 0;

    //! - loop over all the data nodes and add them to the single partition.
    std::vector<DataNodeUsageSimMsg>::iterator it;
    for(it = nodeBaudMsgs.begin(); it != nodeBaudMsgs.end(); it++) {
        if (storedData.size() == 0){
            this->storedData.push_back({{'S','T','O','R','E','D',' ','D','A','T','A'}, 0});
        }
        else if ((this->storedDataSum < this->storageCapacity) || (it->baudRate <= 0)){
            this->storedData[0].dataInstanceSum += it->baudRate * (this->currentTimestep);
        }
        this->netBaud += it->baudRate;
    }

    //!- Sum all data in storedData vector
    this->storedDataSum = this->storedData[0].dataInstanceSum;

    //!- Update previousTime
    this->previousTime = currentTime;
    return;
}
