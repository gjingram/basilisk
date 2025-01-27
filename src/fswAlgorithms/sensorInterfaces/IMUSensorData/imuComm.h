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

#ifndef _IMU_COMM_H_
#define _IMU_COMM_H_

#include "messaging/static_messaging.h"
#include "fswMessages/vehicleConfigFswMsg.h"
#include "fswMessages/imuSensorBodyFswMsg.h"
#include "simFswInterfaceMessages/imuSensorIntMsg.h"
#include "simulation/utilities/bskLogging.h"



/*! @brief module configuration message */
typedef struct {
    double dcm_BP[9];    /*!< Row major platform 2 bdy DCM*/
    char InputDataName[MAX_STAT_MSG_LENGTH]; /*!< The name of the input message*/
    char InputPropsName[MAX_STAT_MSG_LENGTH]; /*!< The name of the ADCS config data message*/
    char OutputDataName[MAX_STAT_MSG_LENGTH]; /*!< The name of the output message*/
    int32_t SensorMsgID; /*!< Sensor IDs tied to the input name*/
    int32_t PropsMsgID;  /*!< Sensor ID tied to the ADCS config data message*/
    int32_t OutputMsgID; /*!< Message ID for the output port*/
    IMUSensorBodyFswMsg LocalOutput; /*!< Output data structure*/
    BSKLogger *bskLogger;   //!< BSK Logging
}IMUConfigData;

#ifdef __cplusplus
extern "C" {
#endif
    
    void SelfInit_imuProcessTelem(IMUConfigData *configData, int64_t moduleID);
    void CrossInit_imuProcessTelem(IMUConfigData *configData, int64_t moduleID);
    void Update_imuProcessTelem(IMUConfigData *configData, uint64_t callTime,
        int64_t moduleID);
    
#ifdef __cplusplus
}
#endif


#endif
