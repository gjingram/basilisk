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
#include "environment/planetEphemeris/planetEphemeris.h"
#include <iostream>
#include <sstream>
#include "architecture/messaging/system_messaging.h"
#include <string.h>
#include "simFswInterfaceMessages/macroDefinitions.h"
#include "utilities/astroConstants.h"
#include "utilities/rigidBodyKinematics.h"
#include "utilities/avsEigenSupport.h"


/*! This constructor initializes the variables.
 */
PlanetEphemeris::PlanetEphemeris()
{
    CallCounts = 0;
    outputBufferCount = 2;
    return;
}

/*! Module deconstructor */
PlanetEphemeris::~PlanetEphemeris()
{
    return;
}

/*! This method initializes the object.
 @return void
 */
void PlanetEphemeris::SelfInit()
{
    std::vector<std::string>::iterator it;
    int c;
    //! - Loop over the planet names and create new data
    for(it = this->planetNames.begin(), c=0; it != this->planetNames.end(); it++, c++)
    {
        //! - Set the new planet name
        std::string planetMsgName = *it + "_planet_data";
        if(planetMsgName.size() >= MAX_BODY_NAME_LENGTH)
        {
            bskLogger.bskLog(BSK_ERROR, "Warning, your planet name %s is too long.", (*it).c_str());
            continue;
        }

        //! - Create the planet message ID
        uint64_t msgId = (uint32_t)SystemMessaging::GetInstance()->
                        CreateNewMessage(planetMsgName, sizeof(SpicePlanetStateSimMsg),
                                         this->outputBufferCount, "SpicePlanetStateSimMsg", this->moduleID);
        this->planetOutMsgId.push_back(msgId);
    }
}

void PlanetEphemeris::Reset(uint64_t CurrenSimNanos)
{
    this->epochTime = CurrenSimNanos*NANO2SEC;

    /*! - do sanity checks that the vector arrays for planet names and ephemeris have the same length */
    if (this->planetElements.size() != this->planetNames.size()) {
        bskLogger.bskLog(BSK_ERROR, "Only %lu planet element sets provided, but %lu plane names are present.",
                  this->planetElements.size(), this->planetNames.size());
    }

    /*! - See if planet orientation information is set */
    if(this->lst0.size() == 0 && this->rotRate.size() == 0 &&
       this->declination.size() == 0 && this->rightAscension.size() == 0) {
        this->computeAttitudeFlag = 0;
        return;
    } else {
        this->computeAttitudeFlag = 1;
    }

    /*! - check that the right number of planet local sideral time angles are provided */
    if (this->lst0.size() != this->planetNames.size()) {
        bskLogger.bskLog(BSK_ERROR, "Only %lu planet initial principal rotation angles provided, but %lu plane names are present.",
                  this->lst0.size(), this->planetNames.size());
        this->computeAttitudeFlag = 0;
    }

    /*! - check that the right number of planet polar axis right ascension angles are provided */
    if (this->rightAscension.size() != this->planetNames.size()) {
        bskLogger.bskLog(BSK_ERROR, "Only %lu planet right ascension angles provided, but %lu plane names are present.",
                  this->rotRate.size(), this->planetNames.size());
        this->computeAttitudeFlag = 0;
    }

    /*! - check that the right number of planet polar axis declination angles are provided */
    if (this->declination.size() != this->planetNames.size()) {
        bskLogger.bskLog(BSK_ERROR, "Only %lu planet declination angles provided, but %lu plane names are present.",
                  this->rotRate.size(), this->planetNames.size());
        this->computeAttitudeFlag = 0;
    }

    /*! - check that the right number of planet polar rotation rates are provided */
    if (this->rotRate.size() != this->planetNames.size()) {
        bskLogger.bskLog(BSK_ERROR, "Only %lu planet rotation rates provided, but %lu plane names are present.",
                  this->rotRate.size(), this->planetNames.size());
        this->computeAttitudeFlag = 0;
    }

    /*! - compute the polar rotation axis unit vector for each planet */
    if (this->computeAttitudeFlag) {
        Eigen::Vector3d eHat_N;
        std::vector<double>::iterator RAN;
        std::vector<double>::iterator DEC;

        for(RAN = this->rightAscension.begin(), DEC = this->declination.begin(); RAN != this->rightAscension.end(); RAN++, DEC++)
        {
            eHat_N << cos(*DEC)*cos(*RAN),cos(*DEC)*sin(*RAN),sin(*DEC);
            this->eHat_N.push_back(eHat_N);
        }
    }

    return;
}


/*! This update routine loops over all the planets and creates their heliocentric position
 and velocity vectors at the current time. If the planet orientation information is provided,
 then this is computed as well.  The default orientation information is a zero orientation.
 @return void
 @param CurrentSimNanos The current clock time for the simulation
 */
void PlanetEphemeris::UpdateState(uint64_t CurrentSimNanos)
{
    std::vector<std::string>::iterator it;
    double time;                            // [s] time since epoch
    double mu;                              // [m^3/s^2] gravity constant of the sun
    double c;                               // [] counter
    double f0;                              // [r] true anomaly at epoch
    double e;                               // []  orbit eccentricity
    double M0;                              // [r] mean anomaly at epoch
    double M;                               // [r] current mean anomaly
    double lst;                             // [r] local sidereal time angle
    double omega_NP_P[3];                   // [r/s] angular velocity of inertial frame relative to planet frame in planet frame components
    double tilde[3][3];                     // [] skew-symmetric matrix
    Eigen::Vector3d gamma;                  // [] principal rotation vector going from inertial to planet frame



    //! - set time in units of seconds
    time = CurrentSimNanos*NANO2SEC;

    //! - set sun gravity constant
    mu = MU_SUN*pow(1000.,3);

    //! - Loop over the planet names and create new data
    for(it = this->planetNames.begin(), c=0; it != this->planetNames.end(); it++, c++)
    {
        //! - Create new planet output message copy
        SpicePlanetStateSimMsg newPlanet;
        memset(&newPlanet, 0x0, sizeof(SpicePlanetStateSimMsg));
        //! - specify planet name in output message
        strcpy(newPlanet.PlanetName, it->c_str());

        //! - tag time to message
        newPlanet.J2000Current = time;

        //! - compute current planet sun-centric inertial position and velocity vectors
        f0 = this->planetElements[c].f;
        e = this->planetElements[c].e;
        M0 = E2M(f2E(f0, e), e);
        M = M0 + sqrt(mu/pow(this->planetElements[c].a, 3)) * (time - this->epochTime);
        this->planetElements[c].f = E2f(M2E(M, e),e);
        elem2rv(mu, &(this->planetElements[c]), newPlanet.PositionVector, newPlanet.VelocityVector);

        //! - retain planet epoch information
        this->planetElements[c].f = f0;

        if (computeAttitudeFlag == 1) {
            //! - compute current planet principal rotation parameter vector */
            lst = this->lst0[c] + this->rotRate[c]*(time - this->epochTime);

            //! - compute the planet DCM
            gamma = this->eHat_N[c]*lst;
            PRV2C(gamma.data(), newPlanet.J20002Pfix);

            //! - compute the planet DCM rate
            v3Set(0.0, 0.0, -this->rotRate[c], omega_NP_P);
            v3Tilde(omega_NP_P, tilde);
            m33MultM33(tilde, newPlanet.J20002Pfix, newPlanet.J20002Pfix_dot);

            //! - set flag that planet orientation is computed
            newPlanet.computeOrient = 1;
        } else {
            m33SetIdentity(newPlanet.J20002Pfix); // set default zero orientation
        }

        //! - write output message
        SystemMessaging::GetInstance()->WriteMessage(this->planetOutMsgId[c], CurrentSimNanos,
                                                     sizeof(SpicePlanetStateSimMsg), reinterpret_cast<uint8_t*>(&newPlanet), this->moduleID);

    }
    return;
}
