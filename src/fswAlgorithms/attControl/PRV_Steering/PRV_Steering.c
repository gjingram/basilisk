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
    PRV_STEERING Module
 
 */

#include "attControl/PRV_Steering/PRV_Steering.h"
#include "simulation/utilities/linearAlgebra.h"
#include "simulation/utilities/rigidBodyKinematics.h"
#include "simulation/utilities/astroConstants.h"
#include <string.h>
#include <math.h>


/*! This method initializes the configData for this module.
 It checks to ensure that the inputs are sane and then creates the
 output message
 @return void
 @param configData The configuration data associated with this module
 @param moduleID The module identifier
*/
void SelfInit_PRV_Steering(PRV_SteeringConfig *configData, int64_t moduleID)
{
    /*! - Create output message for module */
    configData->outputMsgID = CreateNewMessage(configData->outputDataName,
        sizeof(RateCmdFswMsg), "RateCmdFswMsg", moduleID);
    
}

/*! This method performs the second stage of initialization for this module.
 It's primary function is to link the input messages that were created elsewhere.
 @return void
 @param configData The configuration data associated with this module
 @param moduleID The module identifier
*/
void CrossInit_PRV_Steering(PRV_SteeringConfig *configData, int64_t moduleID)
{
    /*! - Get the control data message ID*/
    configData->inputGuidID = subscribeToMessage(configData->inputGuidName,
                                                 sizeof(AttGuidFswMsg), moduleID);
}

/*! This method performs a complete reset of the module.  Local module variables that retain
 time varying states between function calls are reset to their default values.
 @return void
 @param configData The configuration data associated with the MRP steering control
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The module identifier
*/
void Reset_PRV_Steering(PRV_SteeringConfig *configData, uint64_t callTime, int64_t moduleID)
{
    return;
}

/*! This method takes the attitude and rate errors relative to the Reference frame, as well as
    the reference frame angular rates and acceleration, and computes the required control torque Lr.
 @return void
 @param configData The configuration data associated with the MRP Steering attitude control
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The module identifier
*/
void Update_PRV_Steering(PRV_SteeringConfig *configData, uint64_t callTime,
    int64_t moduleID)
{
    AttGuidFswMsg      guidCmd;            /*!< Guidance Message */
    uint64_t            timeOfMsgWritten;
    uint32_t            sizeOfMsgWritten;
        
    /*! - Read the dynamic input messages */
    ReadMessage(configData->inputGuidID, &timeOfMsgWritten, &sizeOfMsgWritten,
                sizeof(AttGuidFswMsg), (void*) &(guidCmd), moduleID);
    
    /* evalute MRP kinematic steering law */
    PRVSteeringLaw(configData, guidCmd.sigma_BR, configData->outMsg.omega_BastR_B, configData->outMsg.omegap_BastR_B);
    
    /* Store the output message and pass it to the message bus */
    WriteMessage(configData->outputMsgID, callTime, sizeof(RateCmdFswMsg),
                 (void*) &(configData->outMsg), moduleID);
    
    return;
}


/*! This method computes the PRV Steering law.  A commanded body rate is returned given the PRV
 attitude error measure of the body relative to a reference frame.  The function returns the commanded
 body rate, as well as the body frame derivative of this rate command.
 @return void
 @param configData  The configuration data associated with this module
 @param sigma_BR    MRP attitude error of B relative to R
 @param omega_ast   Commanded body rates
 @param omega_ast_p Body frame derivative of the commanded body rates
 */
void PRVSteeringLaw(PRV_SteeringConfig *configData, double sigma_BR[3], double omega_ast[3], double omega_ast_p[3])
{
    double e_hat[3];        /*!< principal rotation axis of MRP */
    double phi;             /*!< principal rotation angle of MRP */
    double sigma_Norm;      /*!< norm of the MRP attitude error */
    double value;

    sigma_Norm = v3Norm(sigma_BR);
    if (sigma_Norm > 0.00000000001) {
        v3Scale(1./sigma_Norm, sigma_BR, e_hat);
    } else {
        e_hat[0] = 1.;
        e_hat[1] = 0.;
        e_hat[2] = 0.;
    }
    phi = 4.*atan(sigma_Norm);

    value = atan(M_PI_2/configData->omega_max*(configData->K1*phi + configData->K3*phi*phi*phi))/M_PI_2*configData->omega_max;

    v3Scale(-value, e_hat, omega_ast);

    value *= (3*configData->K3*phi*phi + configData->K1)/(pow(M_PI_2/configData->omega_max*(configData->K1*phi + configData->K3*phi*phi*phi),2) + 1);

    v3Scale(value, e_hat, omega_ast_p);
    
    return;
}
