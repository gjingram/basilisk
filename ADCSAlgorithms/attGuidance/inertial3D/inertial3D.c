/*
    Inertial 3D Spin Module
 
 * University of Colorado, Autonomous Vehicle Systems (AVS) Lab
 * Unpublished Copyright (c) 2012-2015 University of Colorado, All Rights Reserved

 */

/* modify the path to reflect the new module names */
#include "attGuidance/inertial3D/inertial3D.h"
#include <string.h>
#include "ADCSUtilities/ADCSDefinitions.h"
#include "ADCSUtilities/ADCSAlgorithmMacros.h"

/* update this include to reflect the required module input messages */
#include "attDetermination/_GeneralModuleFiles/navStateOut.h"



/* Pull in support files from other modules.  Be sure to use the absolute path relative to Basilisk directory. */
#include "SimCode/utilities/linearAlgebra.h"
#include "SimCode/utilities/rigidBodyKinematics.h"


void SelfInit_inertial3D(inertial3DConfig *ConfigData, uint64_t moduleID)
{
    /*! - Create output message for module */
    ConfigData->outputMsgID = CreateNewMessage(ConfigData->outputDataName,
                                               sizeof(attGuidOut),
                                               "attGuidOut",
                                               moduleID);
    /* Reference rates are assumed to be zero */
    v3SetZero(ConfigData->attGuidOut.omega_RN_B); /* #WHY? Isn't this redundant? */
    v3SetZero(ConfigData->attGuidOut.domega_RN_B);
    
    /* these two relative orientations labels are the same */
    ConfigData->sigma_BcB = ConfigData->sigma_R0R;
    
    /* Input ConfigData parameters are set in python:
    ConfigData->sigma_RR0
    ConfigData->sigma_R0N
    ConfigData->omega_RN_N */

}

void CrossInit_inertial3D(inertial3DConfig *ConfigData, uint64_t moduleID)
{
    /*! - Get the control data message ID*/
    ConfigData->inputNavID = subscribeToMessage(ConfigData->inputNavName,
                                                sizeof(NavStateOut),
                                                moduleID);

}

void Reset_inertial3D(inertial3DConfig *ConfigData)
{
    /* compute the initial reference frame orientation that takes the corrected body frame into account */
    double sigma_RR0[3];
    v3Scale(-1.0, ConfigData->sigma_R0R, sigma_RR0);
    addMRP(ConfigData->sigma_R0N, sigma_RR0, ConfigData->sigma_RN);
    /* #NOTE: Shouldn't this go to Init function? */
    
    /* reset the prior time flag state.  If set to zero, the control time step is not evaluated on the first function call */
    ConfigData->priorTime = 0;
}

void Update_inertial3D(inertial3DConfig *ConfigData, uint64_t callTime, uint64_t moduleID)
{
    uint64_t            clockTime;
    uint32_t            readSize;
    NavStateOut         nav;                /*!< navigation message */
    double              dt;                 /*!< [s] module update period */


    /*! - Read the input messages */
    ReadMessage(ConfigData->inputNavID, &clockTime, &readSize,
                sizeof(NavStateOut), (void*) &(nav));



    /* compute control update time */
    if (ConfigData->priorTime != 0) {       /* don't compute dt if this is the first call after a reset */
        dt = (callTime - ConfigData->priorTime)*NANO2SEC;
        if (dt > 10.0) dt = 10.0;           /* cap the maximum control time step possible */
        if (dt < 0.0) dt = 0.0;             /* ensure no negative numbers are used */
    } else {
        dt = 0.;                            /* set dt to zero to not use integration on first function call */
    }
    
    ConfigData->priorTime = callTime;


    /* compute and store output message */
    computeInertial3DAttitudeError(nav.sigma_BN,
                                     nav.omega_BN_B,
                                     ConfigData,
                                     ConfigData->attGuidOut.sigma_BR,
                                     ConfigData->attGuidOut.omega_BR_B,
                                     ConfigData->attGuidOut.omega_RN_B,
                                     ConfigData->attGuidOut.domega_RN_B);

    WriteMessage(ConfigData->outputMsgID, callTime, sizeof(attGuidOut),   /* update module name */
                 (void*) &(ConfigData->attGuidOut), moduleID);

    return;
}


/*
 * Function: computeInertial3DAttitudeError
 * Purpose: compute the attitude and rate errors. This function is designed to work both in:
 *      FSW to compute estimated pointing errors
 *      Simulation code to compute true pointing errors
 * Inputs:
 *   sigma_BN = MRP attitude of body relative to inertial
 *   omega_BN_B = body rate vector
     ConfigData = module configuration data
 * Outputs:
 *   sigma_BR = MRP attitude error of body relative to reference
 *   omega_BR_B = angular velocity vector error of body relative to reference
 *   omega_RN_B = reference angluar velocity vector in body frame components
 *   domega_RN_B = reference angular acceleration vector in body frame componets
 */
void computeInertial3DAttitudeError(double sigma_BN[3],
                                      double omega_BN_B[3],
                                      inertial3DConfig *ConfigData,
                                      double sigma_BR[3],
                                      double omega_BR_B[3],
                                      double omega_RN_B[3],
                                      double domega_RN_B[3])
{
    /* compute attitude error */
    subMRP(sigma_BN, ConfigData->sigma_RN, sigma_BR);
    v3Copy(omega_BN_B, omega_BR_B);

    /* set rate errors to zero */
    v3SetZero(omega_RN_B);
    v3SetZero(domega_RN_B);
}