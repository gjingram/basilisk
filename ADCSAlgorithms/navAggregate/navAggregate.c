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

#include "navAggregate/navAggregate.h"
#include "ADCSUtilities/ADCSAlgorithmMacros.h"
#include <string.h>

/*! This method initializes the ConfigData for the nav aggregation algorithm.  
    It initializes the output message in the messaging system.
 @return void
 @param ConfigData The configuration data associated with the Nav aggregation interface
 */
void SelfInit_aggregateNav(NavAggregateData *ConfigData, uint64_t moduleID)
{
    ConfigData->outputNavMsgID = CreateNewMessage(ConfigData->outputNavName,
        sizeof(NavStateOut), "NavStateOut", moduleID);
}

/*! This method performs the second stage of initialization for the nav aggregration 
    interface.  For each configured message, it subscribes to the target message 
    and saves the ID.
 @return void
 @param ConfigData The configuration data associated with the aggregate nav interface
 */
void CrossInit_aggregateNav(NavAggregateData *ConfigData, uint64_t moduleID)
{
    uint32_t i;
    for(i=0; i<ConfigData->msgCount; i=i+1)
    {
        ConfigData->navMsgs[i].inputNavID = subscribeToMessage(
            ConfigData->navMsgs[i].inputNavName, sizeof(NavStateOut), moduleID);
    }
}

/*! This method takes the navigation message snippets created by the various 
    navigation components in the FSW and aggregates them into a single complete 
    navigation message.
 @return void
 @param ConfigData The configuration data associated with the aggregate nav module
 @param callTime The clock time at which the function was called (nanoseconds)
 */
void Update_aggregateNav(NavAggregateData *ConfigData, uint64_t callTime, uint64_t moduleID)
{
    uint64_t writeTime;
    uint32_t writeSize;
    uint32_t i;
    /*! Begin method steps */
    /*! - Iterate through all of the input messages and archive their nav data*/
    for(i=0; i<ConfigData->msgCount; i=i+1)
    {
        ReadMessage(ConfigData->navMsgs[i].inputNavID, &writeTime, &writeSize,
                    sizeof(NavStateOut), &(ConfigData->navMsgs[i].msgStorage));
    }
    
    /*! - Copy out each part of the source message into the target output message*/
    ConfigData->outputData.timeTag =
        ConfigData->navMsgs[ConfigData->timeIdx].msgStorage.timeTag;
    memcpy(ConfigData->outputData.r_BN_N,
           ConfigData->navMsgs[ConfigData->posIdx].msgStorage.r_BN_N, 3*sizeof(double));
    memcpy(ConfigData->outputData.v_BN_N,
           ConfigData->navMsgs[ConfigData->velIdx].msgStorage.v_BN_N, 3*sizeof(double));
    memcpy(ConfigData->outputData.sigma_BN,
           ConfigData->navMsgs[ConfigData->attIdx].msgStorage.sigma_BN, 3*sizeof(double));
    memcpy(ConfigData->outputData.omega_BN_B,
           ConfigData->navMsgs[ConfigData->rateIdx].msgStorage.omega_BN_B, 3*sizeof(double));
    memcpy(ConfigData->outputData.vehAccumDV,
           ConfigData->navMsgs[ConfigData->dvIdx].msgStorage.vehAccumDV, 3*sizeof(double));
    memcpy(ConfigData->outputData.vehSunPntBdy,
           ConfigData->navMsgs[ConfigData->sunIdx].msgStorage.vehSunPntBdy, 3*sizeof(double));
    /*! - Write the total message out for everyone else to pick up */
    WriteMessage(ConfigData->outputNavMsgID, callTime, sizeof(NavStateOut),
                 &(ConfigData->outputData), moduleID);
    
    return;
}