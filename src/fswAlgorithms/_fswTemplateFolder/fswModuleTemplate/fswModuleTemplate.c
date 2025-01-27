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
    FSW MODULE Template
 
 */

/* modify the path to reflect the new module names */
#include "fswModuleTemplate.h"
#include "string.h"



/*
 Pull in support files from other modules.  Be sure to use the absolute path relative to Basilisk directory.
 */
#include "simulation/utilities/linearAlgebra.h"


/*!
 \verbatim embed:rst
    This method initializes the configData for this module.
    It checks to ensure that the inputs are sane and then creates the
    output message of type :ref:`FswModuleTemplateFswMsg`.
 \endverbatim
 @return void
 @param configData The configuration data associated with this module
 @param moduleID The module identifier
 */
void SelfInit_fswModuleTemplate(fswModuleTemplateConfig *configData, int64_t moduleID)
{
    
    /*! - Create output message for module */
    configData->dataOutMsgID = CreateNewMessage(configData->dataOutMsgName,
                                               sizeof(FswModuleTemplateFswMsg),
                                               "FswModuleTemplateFswMsg",          /* add the output structure name */
                                               moduleID);
}

/*!
 \verbatim embed:rst
    This method performs the second stage of initialization for this module.
    It's primary function is to link the input messages that were created elsewhere.
    Nothing else should be happening in this function.  The subscribed message is
    of type :ref:`FswModuleTemplateFswMsg`.
 \endverbatim
 @return void
 @param configData The configuration data associated with this module
 @param moduleID The module identifier
*/
void CrossInit_fswModuleTemplate(fswModuleTemplateConfig *configData, int64_t moduleID)
{
    /*! - Get the ID of the subscribed input message */
    configData->dataInMsgID = subscribeToMessage(configData->dataInMsgName,
                                                sizeof(FswModuleTemplateFswMsg),
                                                moduleID);

}

/*! This method performs a complete reset of the module.  Local module variables that retain
 time varying states between function calls are reset to their default values.  The local copy of the
 message output buffer should be cleared.
 @return void
 @param configData The configuration data associated with the module
 @param callTime [ns] time the method is called
 @param moduleID The module identifier
*/
void Reset_fswModuleTemplate(fswModuleTemplateConfig *configData, uint64_t callTime, int64_t moduleID)
{
    /*! - reset any required variables */
    configData->dummy = 0.0;
    char info[MAX_LOGGING_LENGTH];
    sprintf(info, "Variable dummy set to %f in reset.",configData->dummy);
    _bskLog(configData->bskLogger, BSK_INFORMATION, info);
}

/*! Add a description of what this main Update() routine does for this module
 @return void
 @param configData The configuration data associated with the module
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The module identifier
*/
void Update_fswModuleTemplate(fswModuleTemplateConfig *configData, uint64_t callTime, int64_t moduleID)
{
    uint64_t            timeOfMsgWritten;
    uint32_t            sizeOfMsgWritten;
    double              Lr[3];              /* [unit] variable description */
    FswModuleTemplateFswMsg fswModuleOut;   /* output message */

    /*! - Read the input messages */
    ReadMessage(configData->dataInMsgID, &timeOfMsgWritten, &sizeOfMsgWritten,
                sizeof(FswModuleTemplateFswMsg), (void*) &(configData->inputVector), moduleID);



    /*! - Add the module specific code */
    v3Copy(configData->inputVector, Lr);
    configData->dummy += 1.0;
    Lr[0] += configData->dummy;

    /*! - store the output message */
    v3Copy(Lr, fswModuleOut.outputVector);

    /*! - write the module output message */
    WriteMessage(configData->dataOutMsgID, callTime, sizeof(FswModuleTemplateFswMsg),
                 (void*) &(fswModuleOut), moduleID);

    return;
}
