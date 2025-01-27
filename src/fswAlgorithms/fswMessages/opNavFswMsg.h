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

#ifndef OPNAV_MESSAGE_H
#define OPNAV_MESSAGE_H

/*! @brief structure for filter-states output for the unscented kalman filter
 implementation of the sunline state estimator*/
typedef struct {
    double timeTag;                             /*!< [s] Current time of validity for output */
    int valid; /*!< Valid measurement if 1, invalid if 0 */
    double covar_N[3*3];    /*!< [m^2] Current covariance of the filter */
    double covar_B[3*3];    /*!< [m^2] Current covariance of the filter */
    double covar_C[3*3];    /*!< [m^2] Current covariance of the filter */
    double r_BN_N[3];                 /*!< [m] Current estimated state of the filter */
    double r_BN_B[3];                 /*!< [m] Current estimated state of the filter */
    double r_BN_C[3];                 /*!< [m] Current estimated state of the filter */
    int planetID;       /*!< [-] Planet being navigated, Earth=1, Mars=2, Jupiter=3 */
    int faultDetected; /*!< [-] Bool if a fault is detected */
}OpNavFswMsg;




#endif
