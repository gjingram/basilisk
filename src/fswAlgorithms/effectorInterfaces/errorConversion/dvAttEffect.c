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

#include "effectorInterfaces/errorConversion/dvAttEffect.h"
#include "simulation/utilities/linearAlgebra.h"
#include "simulation/utilities/rigidBodyKinematics.h"
#include <string.h>
#include <math.h>

/*! This method initializes the configData for the sun safe ACS control.
 It checks to ensure that the inputs are sane and then creates the
 output message
 @return void
 @param configData The configuration data associated with the sun safe control
 @param moduleID The ID associated with the configData
 */
void SelfInit_dvAttEffect(dvAttEffectConfig *configData, int64_t moduleID)
{
    uint32_t i;

    /*! - Loop over number of thruster blocks and create output messages */
    for(i=0; i<configData->numThrGroups; i=i+1)
    {
        configData->thrGroups[i].outputMsgID = CreateNewMessage(
            configData->thrGroups[i].outputDataName, sizeof(THRArrayOnTimeCmdIntMsg),
            "THRArrayOnTimeCmdIntMsg", moduleID);
    }
 
    
}

/*! This method performs the second stage of initialization for the sun safe ACS
 interface.  It's primary function is to link the input messages that were
 created elsewhere.
 @return void
 @param configData The configuration data associated with the sun safe ACS control
 @param moduleID The ID associated with the configData
 */
void CrossInit_dvAttEffect(dvAttEffectConfig *configData, int64_t moduleID)
{
    /*! - Get the control data message ID*/
    configData->inputMsgID = subscribeToMessage(configData->inputControlName,
        sizeof(CmdTorqueBodyIntMsg), moduleID);
    
}
void Reset_dvAttEffect(dvAttEffectConfig *configData, uint64_t callTime,
                        int64_t moduleID)
{
    uint32_t i;
    THRArrayOnTimeCmdIntMsg nullEffect;
    
    memset(&(nullEffect), 0x0, sizeof(THRArrayOnTimeCmdIntMsg));
    
    for(i=0; i<configData->numThrGroups; i=i+1)
    {
        memcpy(&(configData->thrGroups[i].cmdRequests), &nullEffect,
            sizeof(THRArrayOnTimeCmdIntMsg));
        WriteMessage(configData->thrGroups[i].outputMsgID, callTime,
            sizeof(THRArrayOnTimeCmdIntMsg), (void*)
            &(configData->thrGroups[i].cmdRequests), moduleID);
    }

}

/*! This method takes the estimated body-observed sun vector and computes the
 current attitude/attitude rate errors to pass on to control.
 @return void
 @param configData The configuration data associated with the sun safe ACS control
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The ID associated with the configData
 */
void Update_dvAttEffect(dvAttEffectConfig *configData, uint64_t callTime,
    int64_t moduleID)
{

    uint64_t timeOfMsgWritten;
    uint32_t sizeOfMsgWritten;
    uint32_t i;
    CmdTorqueBodyIntMsg cntrRequest;
    
    /*! - Read the input requested torque from the feedback controller*/
    ReadMessage(configData->inputMsgID, &timeOfMsgWritten, &sizeOfMsgWritten,
                sizeof(CmdTorqueBodyIntMsg), (void*) &(cntrRequest), moduleID);
    
    for(i=0; i<configData->numThrGroups; i=i+1)
    {
        computeSingleThrustBlock(&(configData->thrGroups[i]), callTime,
            &cntrRequest, moduleID);
    }
    
    return;
}

void computeSingleThrustBlock(ThrustGroupData *thrData, uint64_t callTime,
CmdTorqueBodyIntMsg *contrReq, int64_t moduleID)
{
    double unSortOnTime[MAX_EFF_CNT];
    effPairs unSortPairs[MAX_EFF_CNT];
    effPairs sortPairs[MAX_EFF_CNT];
    uint32_t i;
    double localRequest[3];
    
    v3Copy(contrReq->torqueRequestBody, localRequest);      /* to generate a positive torque onto the spacecraft */
    mMultV(thrData->thrOnMap, thrData->numEffectors, 3,
           localRequest, unSortOnTime);
    
    for(i=0; i<thrData->numEffectors; i=i+1)
    {
        unSortOnTime[i] = unSortOnTime[i] + thrData->nomThrustOn;
    }
    
    for(i=0; i<thrData->numEffectors; i=i+1)
    {
        if(unSortOnTime[i] < thrData->minThrustRequest)
        {
            unSortOnTime[i] = 0.0;
        }
    }
    
    for(i=0; i<thrData->numEffectors; i++)
    {
        unSortPairs[i].onTime = unSortOnTime[i];
        unSortPairs[i].thrustIndex = i;
    }
    effectorVSort(unSortPairs, sortPairs, thrData->numEffectors);
    memset(thrData->cmdRequests.OnTimeRequest, 0x0,sizeof(THRArrayOnTimeCmdIntMsg));
    for(i=0; i<thrData->maxNumCmds; i=i+1)
    {
        thrData->cmdRequests.OnTimeRequest[sortPairs[i].thrustIndex] =
        sortPairs[i].onTime;
    }
    WriteMessage(thrData->outputMsgID, callTime, sizeof(THRArrayOnTimeCmdIntMsg),
                 (void*) &(thrData->cmdRequests), moduleID);
}

void effectorVSort(effPairs *Input, effPairs *Output, size_t dim)
{
    int i, j;
    int Swapped;
    Swapped = 1;
    memcpy(Output, Input, dim*sizeof(effPairs));
    for(i=0; i<dim && Swapped > 0; i++)
    {
        Swapped = 0;
        for(j=0; j<dim-1; j++)
        {
            if(Output[j].onTime<Output[j+1].onTime)
            {
                double tempOn = Output[j+1].onTime;
                uint32_t tempIndex = Output[j+1].thrustIndex;
                Output[j+1].onTime = Output[j].onTime;
                Output[j+1].thrustIndex = Output[j].thrustIndex;
                Output[j].onTime = tempOn;
                Output[j].thrustIndex = tempIndex;
                Swapped = 1;
            }
        }
    }
}
