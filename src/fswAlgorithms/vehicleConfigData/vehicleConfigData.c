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

#include "simFswInterfaceMessages/macroDefinitions.h"
#include "vehicleConfigData/vehicleConfigData.h"
#include "simulation/utilities/linearAlgebra.h"
#include <string.h>

/*! This method initializes the configData for the veh config algorithm.
    It initializes the output message in the messaging system.
 @return void
 @param configData The configuration data associated with the vehcle config interface
 @param moduleID The ID associated with the configData
 */
void SelfInit_vehicleConfigData(VehConfigInputData *configData, int64_t moduleID)
{
    /*! - Create the output message for the mass properties of the spacecraft*/
    configData->outputPropsID = CreateNewMessage(
        configData->outputPropsName, sizeof(VehicleConfigFswMsg),
        "VehicleConfigFswMsg", moduleID);

}

/*! This method performs the second stage of initialization for the vehicle config
    data interface.  No operations are performed here currently.
 @return void
 @param configData The configuration data associated with the veh config interface
 @param moduleID The ID associated with the configData
 */
void CrossInit_vehicleConfigData(VehConfigInputData *configData, int64_t moduleID)
{
    /*! Nothing done in this method.  Make sure this is still true!*/
}

void Reset_vehicleConfigData(VehConfigInputData *configData, uint64_t callTime, int64_t moduleID)
{
    VehicleConfigFswMsg localConfigData;
    /*! Begin function steps*/

    /*! - Zero the output message data */
    memset(&localConfigData, 0x0, sizeof(VehicleConfigFswMsg));

    /*! - Convert over the center of mass location */
    v3Copy(configData->CoM_B, localConfigData.CoM_B);

    /*! - Copy over the inertia */
    m33Copy(RECAST3X3 configData->ISCPntB_B, RECAST3X3 localConfigData.ISCPntB_B);

    /*! - Write output properties to the messaging system*/
    WriteMessage(configData->outputPropsID, callTime, sizeof(VehicleConfigFswMsg),
                 &localConfigData, moduleID);
}

/*! There are no runtime operations performed by the vehicle configuration
    module.
 @return void
 @param configData The configuration data associated with the veh config module
 @param callTime The clock time at which the function was called (nanoseconds)
 @param moduleID The ID associated with the configData
 */
void Update_vehicleConfigData(VehConfigInputData *configData, uint64_t callTime, int64_t moduleID)
{
    /*! Nothing done in this method.  Make sure this is still true!*/
    return;
}
