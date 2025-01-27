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

#ifndef _PIXEL_LINE_CONVERTER_H_
#define _PIXEL_LINE_CONVERTER_H_

#include "messaging/static_messaging.h"
#include "simFswInterfaceMessages/circlesOpNavMsg.h"
#include "simFswInterfaceMessages/cameraConfigMsg.h"
#include "simFswInterfaceMessages/navAttIntMsg.h"
#include "simFswInterfaceMessages/macroDefinitions.h"
#include "fswMessages/opNavFswMsg.h"
#include "utilities/linearAlgebra.h"
#include "utilities/astroConstants.h"
#include "utilities/rigidBodyKinematics.h"
#include "simulation/utilities/bskLogging.h"


/*! @brief The configuration structure for the pixelLine Converter module.*/
typedef struct {
    char opNavOutMsgName[MAX_STAT_MSG_LENGTH]; //!< [-] The name of the output navigation message for relative position
    char cameraConfigMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the camera config message
    char attInMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the attitude message
    char circlesInMsgName[MAX_STAT_MSG_LENGTH]; //!< The name of the circles message
    int32_t planetTarget; //!< The planet targeted (None = 0, Earth = 1, Mars = 2, Jupiter = 3 are allowed)
    int32_t stateOutMsgID;    //!< [-] The ID associated with the outgoing message
    int32_t attInMsgID;    //!< [-] The ID associated with the outgoing message
    int32_t circlesInMsgID;    //!< [-] The ID associated with the incoming circle message
    int32_t cameraConfigMsgID;  //!< [-] The ID associated with the incoming camera config message
    BSKLogger *bskLogger;                             //!< BSK Logging
}PixelLineConvertData;

#ifdef __cplusplus
extern "C" {
#endif
    
    void SelfInit_pixelLineConverter(PixelLineConvertData *configData, int64_t moduleID);
    void CrossInit_pixelLineConverter(PixelLineConvertData *configData, int64_t moduleID);
    void Update_pixelLineConverter(PixelLineConvertData *configData, uint64_t callTime,
        int64_t moduleID);
    void Reset_pixelLineConverter(PixelLineConvertData *configData, uint64_t callTime, int64_t moduleID);
    
#ifdef __cplusplus
}
#endif


#endif
