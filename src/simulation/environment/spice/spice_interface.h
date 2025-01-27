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

#ifndef SpiceInterface_H
#define SpiceInterface_H

#include <vector>
#include <map>
#include "_GeneralModuleFiles/sys_model.h"
#include "utilities/linearAlgebra.h"
#include "simMessages/spicePlanetStateSimMsg.h"
#include "simMessages/spiceTimeSimMsg.h"
#include "utilities/avsEigenSupport.h"
#include "simMessages/epochSimMsg.h"
#include "utilities/bskLogging.h"


/*! @brief spice interface class */
class SpiceInterface: public SysModel {
public:
    SpiceInterface();
    ~SpiceInterface();
    
    void UpdateState(uint64_t CurrentSimNanos);
    int loadSpiceKernel(char *kernelName, const char *dataPath);
    int unloadSpiceKernel(char *kernelName, const char *dataPath);
	std::string getCurrentTimeString();         //!< class method
    void SelfInit();
    void CrossInit();
    void Reset(uint64_t CurrentSimNanos);
    void initTimeData();
    void computeGPSData();
    void computePlanetData();
    void writeOutputMessages(uint64_t CurrentClock);
    void clearKeeper();                         //!< class method
    
public:
    std::string SPICEDataPath;           //!< -- Path on file to SPICE data
    std::string referenceBase;           //!< -- Base reference frame to use
    std::string zeroBase;                //!< -- Base zero point to use for states
	std::string timeOutPicture;          //!< -- Optional parameter used to extract time strings
    bool SPICELoaded;                    //!< -- Boolean indicating to reload spice
    int charBufferSize;         //!< -- avert your eyes we're getting SPICE
    uint8_t *spiceBuffer;       //!< -- General buffer to pass down to spice
    std::string UTCCalInit;     //!< -- UTC time string for init time
    std::string outputTimePort; //!< -- Output time sampling port name to use
    uint64_t outputBufferCount; //!< -- Number of output buffers to use
    std::vector<std::string>planetNames;  //!< -- Names of planets we want to track
    std::vector<std::string>planetFrames; //!< -- Optional vector of planet frame names.  Default values are IAU_ + planet name
    
    bool timeDataInit;          //!< -- Flag indicating whether time has been init
    double J2000ETInit;         //!< s Seconds elapsed since J2000 at init
    double J2000Current;        //!< s Current J2000 elapsed time
    double julianDateCurrent;   //!< s Current JulianDate
    double GPSSeconds;          //!< s Current GPS seconds
    uint16_t GPSWeek;           //!< -- Current GPS week value
    uint64_t GPSRollovers;      //!< -- Count on the number of GPS rollovers

    std::string epochInMsgName; //!< -- Message name of the epoch message (optional)
    int64_t epochInMsgId;       //!< ID of the epoch message
    BSKLogger bskLogger;                      //!< -- BSK Logging

private:
    std::string GPSEpochTime;   //!< -- String for the GPS epoch
    double JDGPSEpoch;          //!< s Epoch for GPS time.  Saved for efficiency
    int64_t timeOutMsgID;       //!< -- Output time message ID
    std::map<uint32_t, SpicePlanetStateSimMsg> planetData; //!< -- Internal vector of planets
    
};


#endif
