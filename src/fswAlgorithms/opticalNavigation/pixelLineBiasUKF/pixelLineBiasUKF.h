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

#ifndef _PIXLINE_UKF_H_
#define _PIXLINE_UKF_H_

#include <stdint.h>

#include "messaging/static_messaging.h"
#include "simFswInterfaceMessages/navTransIntMsg.h"
#include "simFswInterfaceMessages/macroDefinitions.h"
#include "simFswInterfaceMessages/circlesOpNavMsg.h"
#include "simFswInterfaceMessages/cameraConfigMsg.h"
#include "simFswInterfaceMessages/navAttIntMsg.h"
#include "fswMessages/pixelLineFilterFswMsg.h"
#include "simulation/utilities/linearAlgebra.h"
#include "simulation/utilities/rigidBodyKinematics.h"
#include "simulation/utilities/bskLogging.h"
#include "utilities/astroConstants.h"




/*! @brief Top level structure for the relative OD unscented kalman filter.
 Used to estimate the spacecraft's inertial position relative to a body.
 */
typedef struct {
    char navStateOutMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the output message
    char filtDataOutMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the output filter data message
    char circlesInMsgName[MAX_STAT_MSG_LENGTH];  //!< [-] The name of the input RW speeds message
    char cameraConfigMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the camera config message
    char attInMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the attitude message
    
    int moduleId; //!< module ID
    
    size_t numStates;             //!< [-] Number of states for this filter
    size_t countHalfSPs;          //!< [-] Number of sigma points over 2
    size_t numObs;                //!< [-] Number of measurements this cycle
    double beta;                  //!< [-] Beta parameter for filter
    double alpha;                 //!< [-] Alpha parameter for filter
    double kappa;                 //!< [-] Kappa parameter for filter
    double lambdaVal;             //!< [-] Lambda parameter for filter
    double gamma;                 //!< [-] Gamma parameter for filter
    double switchMag;             //!< [-] Threshold for where we switch MRP set
    
    double dt;                     //!< [s] seconds since last data epoch
    double timeTag;                //!< [s]  Time tag for statecovar/etc
    double gyrAggTimeTag;          //!< [s] Time-tag for aggregated gyro data
    double aggSigma_b2b1[3];       //!< [-] Aggregated attitude motion from gyros
    double dcm_BdyGyrpltf[3][3];   //!< [-] DCM for converting gyro data to body frame
    double wM[2 * PIXLINE_N_STATES + 1]; //!< [-] Weighting vector for sigma points
    double wC[2 * PIXLINE_N_STATES + 1]; //!< [-] Weighting vector for sigma points
    
    double stateInit[PIXLINE_N_STATES];    //!< [-] State estimate to initialize filter to
    double state[PIXLINE_N_STATES];        //!< [-] State estimate for time TimeTag
    double statePrev[PIXLINE_N_STATES];        //!< [-] State estimate for time TimeTag at previous time
    double sBar[PIXLINE_N_STATES*PIXLINE_N_STATES];         //!< [-] Time updated covariance
    double sBarPrev[PIXLINE_N_STATES*PIXLINE_N_STATES];     //!< [-] Time updated covariance at previous time
    double covar[PIXLINE_N_STATES*PIXLINE_N_STATES];        //!< [-] covariance
    double covarPrev[PIXLINE_N_STATES*PIXLINE_N_STATES];    //!< [-] covariance at previous time
    double covarInit[PIXLINE_N_STATES*PIXLINE_N_STATES];    //!< [-] Covariance to init filter with
    double xBar[PIXLINE_N_STATES];            //!< [-] Current mean state estimate
    
    double obs[PIXLINE_N_MEAS];                               //!< [-] Observation vector for frame
    double yMeas[PIXLINE_N_MEAS*(2*PIXLINE_N_STATES+1)];        //!< [-] Measurement model data
    
    double SP[(2*PIXLINE_N_STATES+1)*PIXLINE_N_STATES];          //!< [-]    sigma point matrix
    
    double qNoise[PIXLINE_N_STATES*PIXLINE_N_STATES];       //!< [-] process noise matrix
    double sQnoise[PIXLINE_N_STATES*PIXLINE_N_STATES];      //!< [-] cholesky of Qnoise
    double measNoise[PIXLINE_N_MEAS*PIXLINE_N_MEAS];      //!< [-] Measurement Noise
    
    int planetIdInit;                    //!< [-] Planet being navigated inital value
    int planetId;                   //!< [-] Planet being navigated as per measurement
    uint32_t firstPassComplete;         //!< [-] Flag to know if first filter update
    double postFits[3];      //!< [-] PostFit residuals
    double timeTagOut;       //!< [s] Output time-tag information
    double maxTimeJump;      //!< [s] Maximum time jump to allow in propagation
    
    CirclesOpNavMsg cirlcesInMsg; //!< [-] ST sensor data read in from message bus
    CameraConfigMsg cameraSpecs;  //!< [-] Camera specs for nav
    NavAttIntMsg attInfo;         //!< [-] Att info for frame transformation
    int32_t navStateOutMsgId;     //!< -- Id for the outgoing body estimate message
    int32_t filtDataOutMsgId;     //!< [-] Id for the filter data output message
    int32_t circlesInMsgId;     //!< [-] Id for the incoming mass properties message
    int32_t attInMsgID;    //!< [-] The ID associated with the outgoing message
    int32_t cameraConfigMsgID;  //!< [-] The ID associated with the incoming camera config

    BSKLogger *bskLogger;  //!< BSK Logging

}PixelLineBiasUKFConfig;


#ifdef __cplusplus
extern "C" {
#endif
    
    void SelfInit_pixelLineBiasUKF(PixelLineBiasUKFConfig *configData, int64_t moduleId);
    void CrossInit_pixelLineBiasUKF(PixelLineBiasUKFConfig *configData, int64_t moduleId);
    void Update_pixelLineBiasUKF(PixelLineBiasUKFConfig *configData, uint64_t callTime,
                            int64_t moduleId);
    void Reset_pixelLineBiasUKF(PixelLineBiasUKFConfig *configData, uint64_t callTime,
                           int64_t moduleId);
    void pixelLineBiasUKFTwoBodyDyn(double state[PIXLINE_N_STATES], double mu, double *stateDeriv);
    int pixelLineBiasUKFTimeUpdate(PixelLineBiasUKFConfig *configData, double updateTime);
    int pixelLineBiasUKFMeasUpdate(PixelLineBiasUKFConfig *configData);
    void pixelLineBiasUKFCleanUpdate(PixelLineBiasUKFConfig *configData);
    void relODStateProp(PixelLineBiasUKFConfig *configData, double *stateInOut, double dt);
    void pixelLineBiasUKFMeasModel(PixelLineBiasUKFConfig *configData);
    
#ifdef __cplusplus
}
#endif


#endif
