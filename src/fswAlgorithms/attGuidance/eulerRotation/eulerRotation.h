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

#ifndef _EULER_ROTATION_
#define _EULER_ROTATION_

#include "messaging/static_messaging.h"
#include <stdint.h>
#include "fswMessages/attStateFswMsg.h"
#include "fswMessages/attRefFswMsg.h"
#include "simulation/utilities/bskLogging.h"




/*! @brief Top level structure for the sub-module routines. */
typedef struct {
    /* Declare module private variables */
    double angleSet[3];                         //!< [-] current euler angle 321 set R/R0  with respect to the input reference
    double angleRates[3];                       //!< [rad/s] euler angle 321 rates
    double cmdSet[3];                           //!< [] msg commanded initial Euler angle 321 set with respect to input reference
    double cmdRates[3];                         //!< [rad/s] msg commanded constant 321 Euler angle rates
    double priorCmdSet[3];                      //!< [] prior commanded 321 Euler angle set
    double priorCmdRates[3];                    //!< [rad/s] prior commanded 321 Euler angle rates
    uint64_t priorTime;                         //!< [ns] last time the guidance module is called
    double dt;                                  //!< [s] integration time-step
    
    /* Declare module IO interfaces */
    char        attRefOutMsgName[MAX_STAT_MSG_LENGTH];      //!< The name of the output message containing the Reference
    int32_t     attRefOutMsgID;                             //!< [-] ID for the outgoing Reference message
    char        attRefInMsgName[MAX_STAT_MSG_LENGTH];       //!< The name of the guidance reference Input message
    int32_t     attRefInMsgID;                              //!< [-] ID for the incoming guidance reference message
    
    char        desiredAttInMsgName[MAX_STAT_MSG_LENGTH];   //!< The name of the incoming message containing the desired EA set
    int32_t     desiredAttInMsgID;                          //!< [-] ID for the incoming EA set message

    BSKLogger *bskLogger;                             //!< BSK Logging
}eulerRotationConfig;

#ifdef __cplusplus
extern "C" {
#endif
    
    void SelfInit_eulerRotation(eulerRotationConfig *configData, int64_t moduleID);
    void CrossInit_eulerRotation(eulerRotationConfig *configData, int64_t moduleID);
    void Reset_eulerRotation(eulerRotationConfig *configData, uint64_t callTime, int64_t moduleID);
    void Update_eulerRotation(eulerRotationConfig *configData, uint64_t callTime, int64_t moduleID);
    
    void checkRasterCommands(eulerRotationConfig *configData);
    void computeTimeStep(eulerRotationConfig *configData, uint64_t callTime);
    void computeEuler321_Binv_derivative(double angleSet[3], double angleRates[3], double B_inv_deriv[3][3]);
    void computeEulerRotationReference(eulerRotationConfig *configData,
                                       double sigma_R0N[3],
                                       double omega_R0N_N[3],
                                       double domega_R0N_N[3],
                                       AttRefFswMsg *attRefOut);
    
#ifdef __cplusplus
}
#endif


#endif
