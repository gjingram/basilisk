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
/*
    Thruster RW Momentum Management
 
 */

#include "attControl/thrMomentumManagement/thrMomentumManagement.h"
#include "attControl/MRP_Steering/MRP_Steering.h"
#include "simFswInterfaceMessages/macroDefinitions.h"
#include "simulation/utilities/linearAlgebra.h"
#include <string.h>


/*!
 \verbatim embed:rst
    This method initializes the configData for this module.  It creates a single output message of type
    :ref:`CmdTorqueBodyIntMsg`.
 \endverbatim
 @return void
 @param configData The configuration data associated with this module
 @param moduleID The module identifier
*/
void SelfInit_thrMomentumManagement(thrMomentumManagementConfig *configData, int64_t moduleID)
{
    
    /*! - Create output message for module */
    configData->deltaHOutMsgId = CreateNewMessage(configData->deltaHOutMsgName,
                                               sizeof(CmdTorqueBodyIntMsg),
                                               "CmdTorqueBodyIntMsg",          /* add the output structure name */
                                               moduleID);

}

/*!
 \verbatim embed:rst
    This method performs the second stage of initialization for this module.
    It links to 2 required input messages of type :ref:`RWArrayConfigFswMsg` and
    :ref:`RWSpeedIntMsg`.
 \endverbatim
 @return void
 @param configData The configuration data associated with this module
 @param moduleID The module identifier
 */
void CrossInit_thrMomentumManagement(thrMomentumManagementConfig *configData, int64_t moduleID)
{
    /*! - Get the input message IDs */
    configData->rwConfInMsgId = subscribeToMessage(configData->rwConfigDataInMsgName,
                                                  sizeof(RWArrayConfigFswMsg), moduleID);
    configData->rwSpeedsInMsgId = subscribeToMessage(configData->rwSpeedsInMsgName,
                                                     sizeof(RWSpeedIntMsg), moduleID);
}

/*! This method performs a complete reset of the module.  Local module variables that retain
 time varying states between function calls are reset to their default values.
 @return void
 @param configData The configuration data associated with the module
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The module identifier
 */
void Reset_thrMomentumManagement(thrMomentumManagementConfig *configData, uint64_t callTime, int64_t moduleID)
{
    uint64_t timeOfMsgWritten;
    uint32_t sizeOfMsgWritten;

    /*! - read in the RW configuration message */
    memset(&(configData->rwConfigParams), 0x0, sizeof(RWArrayConfigFswMsg));
    ReadMessage(configData->rwConfInMsgId, &timeOfMsgWritten, &sizeOfMsgWritten,
                sizeof(RWArrayConfigFswMsg), &(configData->rwConfigParams), moduleID);

    /*! - reset the momentum dumping request flag */
    configData->initRequest = 1;
}

/*! The RW momentum level is assessed to determine if a momentum dumping maneuver is required.
 This checking only happens once after the reset function is called.  To run this again afterwards,
 the reset function must be called again.
 @return void
 @param configData The configuration data associated with the module
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The module identifier
 */
void Update_thrMomentumManagement(thrMomentumManagementConfig *configData, uint64_t callTime, int64_t moduleID)
{
    uint64_t            timeOfMsgWritten;
    uint32_t            sizeOfMsgWritten;
    RWSpeedIntMsg       rwSpeedMsg;         /* Reaction wheel speed estimate message */
    CmdTorqueBodyIntMsg controlOutMsg;      /* Control torque output message */
    double              hs;                 /* net RW cluster angular momentum magnitude */
    double              hs_B[3];            /* RW angular momentum */
    double              vec3[3];            /* temp vector */
    double              Delta_H_B[3];       /* [Nms]  net desired angular momentum change */
    int i;

    /*! - check if a momentum dumping check has been requested */
    if (configData->initRequest == 1) {

        /*! - Read the input messages */
        memset(&rwSpeedMsg, 0x0, sizeof(RWSpeedIntMsg));
        ReadMessage(configData->rwSpeedsInMsgId, &timeOfMsgWritten, &sizeOfMsgWritten,
                    sizeof(RWSpeedIntMsg), (void*) &(rwSpeedMsg), moduleID);

        /*! - compute net RW momentum magnitude */
        v3SetZero(hs_B);
        for (i=0;i<configData->rwConfigParams.numRW;i++) {
            v3Scale(configData->rwConfigParams.JsList[i]*rwSpeedMsg.wheelSpeeds[i],&configData->rwConfigParams.GsMatrix_B[i*3],vec3);
            v3Add(hs_B, vec3, hs_B);
        }
        hs = v3Norm(hs_B);

        /*! - check if momentum dumping is required */
        if (hs < configData->hs_min) {
            /* Momentum dumping not required */
            v3SetZero(Delta_H_B);
        } else {
            v3Scale(-(hs - configData->hs_min)/hs, hs_B, Delta_H_B);
        }
        configData->initRequest = 0;


        /*! - write out the output message */
        memset(&controlOutMsg, 0x0, sizeof(CmdTorqueBodyIntMsg));
        v3Copy(Delta_H_B, controlOutMsg.torqueRequestBody);

        WriteMessage(configData->deltaHOutMsgId, callTime, sizeof(CmdTorqueBodyIntMsg),
                     (void*) &controlOutMsg, moduleID);

    }

    return;
}
