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

#ifndef _THRUST_RW_DESAT_H_
#define _THRUST_RW_DESAT_H_

#include "messaging/static_messaging.h"
#include "effectorInterfaces/errorConversion/dvAttEffect.h"
#include "fswMessages/vehicleConfigFswMsg.h"
#include "fswMessages/thrArrayConfigFswMsg.h"
#include "fswMessages/rwConstellationFswMsg.h"
#include "simFswInterfaceMessages/thrArrayOnTimeCmdIntMsg.h"
#include "fswMessages/vehicleConfigFswMsg.h"
#include "simulation/utilities/bskLogging.h"
#include <stdint.h>
#include <stdlib.h>




/*! @brief module configuration message */
typedef struct {
    char inputSpeedName[MAX_STAT_MSG_LENGTH]; /*!< (-) The name of the input RW speeds message*/
    char inputRWConfigData[MAX_STAT_MSG_LENGTH]; /*!< [-] The name of the RWA configuration message*/
    char inputThrConfigName[MAX_STAT_MSG_LENGTH]; /*!< [-] The name of the thruster configuration message*/
    char inputMassPropsName[MAX_STAT_MSG_LENGTH]; /*!< [-] Tha name of the input mass properties message*/
	char outputThrName[MAX_STAT_MSG_LENGTH];  /*!< (-) The name of the output thrust command block*/
	double rwAlignMap[3 * MAX_EFF_CNT]; /*!< (-) Alignment of the reaction wheel spin axes*/
	double thrAlignMap[3 * MAX_EFF_CNT]; /*!< (-) Alignment of the vehicle thrusters*/
	double thrTorqueMap[3 * MAX_EFF_CNT]; /*!< (-) Alignment of the vehicle thruster torques*/
	double maxFiring;          /*!< (s) Maximum time to fire a jet for*/
	double thrFiringPeriod;    /*!< (s) The amount of time to rest between thruster firings*/
	int    numRWAs;            /*!< (-) Number of reaction wheels being desaturated*/
	int    numThrusters;       /*!< (-) Number of thrusters available in the align map*/
	double accumulatedImp[3];  /*!< (s) The accumulated firing in the body frame*/
	double currDMDir[3];       /*!< (-) The current direction of momentum reduction*/
	double totalAccumFiring;   /*!< (s) The total thruster duration we've commanded*/
	double DMThresh;           /*!< (r/s) The point at which to stop decrementing momentum*/
	uint64_t previousFiring;   /*!< (ns) Time that the last firing command was given*/
    int32_t inputRWConfID;      /*!< [-] ID for the incoming RWA configuration data*/
    int32_t inputSpeedID;      /*!< (-) ID for the incoming RW speeds*/
    int32_t inputThrConID;     /*!< [-] ID for the thruster configuration data*/
    int32_t inputMassPropID;   /*!< [-] ID for the incoming mass property information*/
	int32_t outputThrID;       /*!< (-) ID for the outgoing thruster commands*/
  BSKLogger *bskLogger;                             //!< BSK Logging
}thrustRWDesatConfig;

#ifdef __cplusplus
extern "C" {
#endif
    
    void SelfInit_thrustRWDesat(thrustRWDesatConfig *configData, int64_t moduleID);
    void CrossInit_thrustRWDesat(thrustRWDesatConfig *configData, int64_t moduleID);
    void Update_thrustRWDesat(thrustRWDesatConfig *configData, uint64_t callTime,
        int64_t moduleID);
	void Reset_thrustRWDesat(thrustRWDesatConfig *configData, uint64_t callTime, int64_t moduleID);
    
#ifdef __cplusplus
}
#endif


#endif
