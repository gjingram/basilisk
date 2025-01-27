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
#include <stdint.h>
#include "simulation/utilities/bskLogging.h"

#ifndef _ACC_DATA_MESSAGE_H
#define _ACC_DATA_MESSAGE_H


/*! @brief Structure used to define accelerometer package data */
typedef struct {
    uint64_t measTime;              //!< [Tick] Measurement time for accel
    double gyro_B[3];               //!< [r/s] Angular rate measurement from gyro
    double accel_B[3];              //!< [m/s2] Acceleration in platform frame
    BSKLogger *bskLogger;   //!< BSK Logging
}AccPktDataFswMsg;


#endif
