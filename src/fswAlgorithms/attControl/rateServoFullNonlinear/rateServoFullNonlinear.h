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

#ifndef _RATE_SERVO_FULL_NONLINEAR_
#define _RATE_SERVO_FULL_NONLINEAR_

#include "messaging/static_messaging.h"
#include "fswMessages/attGuidFswMsg.h"
#include "fswMessages/vehicleConfigFswMsg.h"
#include "fswMessages/rwArrayConfigFswMsg.h"
#include "fswMessages/rwAvailabilityFswMsg.h"
#include "fswMessages/rateCmdFswMsg.h"
#include "simFswInterfaceMessages/rwSpeedIntMsg.h"
#include "simFswInterfaceMessages/cmdTorqueBodyIntMsg.h"
#include "simulation/utilities/bskLogging.h"
#include <stdint.h>



/*! @brief The configuration structure for the rateServoFullNonlinear module.  */
typedef struct {
    /* declare module public variables */
    double P;                           //!< [N*m*s]   Rate error feedback gain applied
    double Ki;                          //!< [N*m]     Integration feedback error on rate error
    double knownTorquePntB_B[3];        //!< [N*m]     known external torque in body frame vector components
    double integralLimit;               //!< [N*m]     Integration limit to avoid wind-up issue

    /* declare module private variables */
    uint64_t priorTime;                 //!< [ns]      Last time the attitude control is called
    double z[3];                        //!< [rad]     integral state of delta_omega
    double ISCPntB_B[9];                //!< [kg m^2] Spacecraft Inertia
    RWArrayConfigFswMsg rwConfigParams; //!< [-] struct to store message containing RW config parameters in body B frame

    /* declare module IO interfaces */
    char rwParamsInMsgName[MAX_STAT_MSG_LENGTH];    //!< The name of the RWArrayConfigFswMsg input message
    int32_t rwParamsInMsgId;                        //!< [-] ID for the RWArrayConfigFswMsg ingoing message
    char vehConfigInMsgName[MAX_STAT_MSG_LENGTH];   //!< [-] name of vehicle configuration message
    int32_t vehConfigInMsgId;                       //!< [-] ID for the vehicle configuration message
    char rwAvailInMsgName[MAX_STAT_MSG_LENGTH];     //!< [-] The name of the RWs availability message
    int32_t rwAvailInMsgId;                         //!< [-] ID for the incoming  RWs availability data
    
    char outputDataName[MAX_STAT_MSG_LENGTH];       //!< The name of the commanded attitude control torque output message
    int32_t cmdTorqueOutMsgId;                      //!< [] ID for the outgoing control torque message
    char inputGuidName[MAX_STAT_MSG_LENGTH];        //!< The name of the guidance input message
    int32_t guidInMsgId;                            //!< [] ID for the incoming guidance message
    char inputRWSpeedsName[MAX_STAT_MSG_LENGTH];    //!< [] The name for the reaction wheel speeds message
    int32_t rwSpeedsInMsgId;                        //!< [] The ID for the reaction wheel speeds message
    char inputRateSteeringName[MAX_STAT_MSG_LENGTH];//!< [] the name of the steering law message
    int32_t rateSteeringInMsgId;                    //!< [] ID for the incoming steering law message

    BSKLogger *bskLogger;                             //!< BSK Logging

}rateServoFullNonlinearConfig;

#ifdef __cplusplus
extern "C" {
#endif
    
    void SelfInit_rateServoFullNonlinear(rateServoFullNonlinearConfig *configData, int64_t moduleID);
    void CrossInit_rateServoFullNonlinear(rateServoFullNonlinearConfig *configData, int64_t moduleID);
    void Update_rateServoFullNonlinear(rateServoFullNonlinearConfig *configData, uint64_t callTime, int64_t moduleID);
    void Reset_rateServoFullNonlinear(rateServoFullNonlinearConfig *configData, uint64_t callTime, int64_t moduleID);

    
#ifdef __cplusplus
}
#endif


#endif
