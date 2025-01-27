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

#ifndef PY_BATTERY_OUT_MSG_H
#define PY_BATTERY_OUT_MSG_H

#include <string>
#include <stdint.h>


//! @brief Container for all battery output data
/*! This structure contains all data output by the python battery module*/
typedef struct {
    double stateOfCharge;                       //!< [%] Battery state of charge as %-full
    double stateOfChargeAh;                     //!< [Ah] Battery state of charge un-scaled
    double solarArrayTemperature;               //!< [K] Temperature of solar arrays
    double busVoltage;                          //!< [V] Bus voltage
    double batteryCurrent;                      //!< [A] Total current flowing through battery
    double solarArrayCurrent;                   //!< [A] Current sent to battery from solar arrays
    double batteryEMF;                          //!< [V] Electro-motive force of battery at this current form look-up table
    double batteryESR;                          //!< [Ohms] Equivalent Series Resistance of battery from look-up table
    double batteryVoltage;                      //!< [V] total voltage across battery
}PyBatteryOutMsg;


#endif
