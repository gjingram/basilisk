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

#include "keplerianOrbit.h"
#include "utilities/astroConstants.h"
#include <utilities/avsEigenSupport.h>

/*! This constructor initialized to an arbitrary orbit */
KeplerianOrbit::KeplerianOrbit()
{
    this->semi_major_axis = 1E5;
    this->eccentricity = 1E-5;
    this->inclination = 0.0;
    this->true_anomaly = 0.0;
    this->argument_of_periapsis = 0.0;
    this->right_ascension = 0.0;
    this->mu = MU_EARTH;
    this->change_orbit();
    return;
}

/*! The constructor requires orbital elements and a planet */
KeplerianOrbit::KeplerianOrbit(classicElements oe, GravBodyData* planet){
    this->set_planet(planet);
    this->semi_major_axis = oe.a;
    this->eccentricity = oe.e;
    this->inclination = oe.i;
    this->true_anomaly = oe.f;
    this->argument_of_periapsis = oe.omega;
    this->right_ascension = oe.Omega;
    this->mu = planet->mu;
    this->change_orbit();
    return;
}

/*! The copy constructor works with python copy*/
KeplerianOrbit::KeplerianOrbit(const KeplerianOrbit &orig){
    this->semi_major_axis = orig.semi_major_axis;
    this->eccentricity = orig.eccentricity;
    this->inclination = orig.inclination;
    this->true_anomaly = orig.true_anomaly;
    this->argument_of_periapsis = orig.argument_of_periapsis;
    this->right_ascension = orig.right_ascension;
    this->mu = orig.mu;
    this->planet = orig.planet;
    this->change_orbit();
    return;
}

/*! Generic Destructor */
KeplerianOrbit::~KeplerianOrbit()
{
    return;
}

/*!
    body position vector relative to planet
 */
Eigen::Vector3d KeplerianOrbit::r_BP_P(){
    return this->position_BP_P;
}

/*!
    body velocity vector relative to planet
 */
Eigen::Vector3d KeplerianOrbit::v_BP_P(){
    return this->velocity_BP_P;

}

/*!
    angular momentum of body relative to planet
 */
Eigen::Vector3d KeplerianOrbit::h_BP_P(){
    return this->orbital_angular_momentum_P;
}

/*! return mean anomaly angle */
double KeplerianOrbit::M(){return this->mean_anomaly;}
/*! return mean orbit rate */
double KeplerianOrbit::n(){return this->mean_motion;};                              //!< return mean orbit rate
/*! return orbit period */
double KeplerianOrbit::P(){return this->orbital_period;};                           //!< return orbital period
/*! return true anomaly */
double KeplerianOrbit::f(){return this->true_anomaly;};                             //!< return true anomaly
/*! return true anomaly rate */
double KeplerianOrbit::fDot(){return this->true_anomaly_rate;};
/*! return right ascencion of the ascending node */
double KeplerianOrbit::RAAN(){return this->right_ascension;};
/*! return argument of periapses */
double KeplerianOrbit::omega(){return this->argument_of_periapsis;};
/*! return inclination angle */
double KeplerianOrbit::i(){return this->inclination;};
/*! return eccentricty */
double KeplerianOrbit::e(){return this->eccentricity;};
/*! return semi-major axis */
double KeplerianOrbit::a(){return this->semi_major_axis;};
/*! return orbital angular momentum magnitude */
double KeplerianOrbit::h(){return this->h_BP_P().norm();};
/*! return orbital energy */
double KeplerianOrbit::Energy(){return this->orbital_energy;};
/*! return orbit radius */
double KeplerianOrbit::r(){return this->r_BP_P().norm();};
/*! return velocity magnitude */
double KeplerianOrbit::v(){return this->v_BP_P().norm();};
/*! return radius at apoapses */
double KeplerianOrbit::r_a(){return this->r_apogee;};
/*! return radius at periapses */
double KeplerianOrbit::r_p(){return this->r_perigee;};
/*! return flight path angle */
double KeplerianOrbit::fpa(){return this->flight_path_angle;};
/*! return eccentric anomaly angle */
double KeplerianOrbit::E(){return this->eccentric_anomaly;};
/*! return semi-latus rectum or the parameter */
double KeplerianOrbit::p(){return this->semi_parameter;};
/*! return radius rate */
double KeplerianOrbit::rDot(){return this->radial_rate;};
/*! return escape velocity */
double KeplerianOrbit::c3(){return this->v_infinity;};

/*! set semi-major axis */
void KeplerianOrbit::set_a(double a){this->semi_major_axis = a; this->change_orbit();};
/*! set eccentricity */
void KeplerianOrbit::set_e(double e){this->eccentricity = e; this->change_orbit();};
/*! set inclination angle */
void KeplerianOrbit::set_i(double i){this->inclination = i; this->change_orbit();};
/*! set argument of periapsis */
void KeplerianOrbit::set_omega(double omega){this->argument_of_periapsis = omega; this->change_orbit();};
/*! set right ascension of the ascending node */
void KeplerianOrbit::set_RAAN(double RAAN){this->right_ascension = RAAN; this->change_orbit();};
/*! set true anomaly angle */
void KeplerianOrbit::set_f(double f){this->true_anomaly = f; this->change_f();};



/*! This method returns the orbital element set for the orbit
 @return classicElements oe
 */
classicElements KeplerianOrbit::oe(){
    classicElements elements;
    elements.a = this->semi_major_axis;
    elements.e = this->eccentricity;
    elements.i = this->inclination;
    elements.f = this->true_anomaly;
    elements.omega = this->argument_of_periapsis;
    elements.Omega = this->right_ascension;
    elements.rApoap= this->r_apogee;
    elements.rPeriap = this->r_perigee;
    elements.alpha = 1.0 / elements.a;
    elements.rmag = this->orbit_radius;
    return elements;
}

/*! This method populates all outputs from orbital elements coherently if any of the
 * classical orbital elements are changed*/
void KeplerianOrbit::change_orbit(){
    this->change_f();
    this->orbital_angular_momentum_P = this->position_BP_P.cross(this->velocity_BP_P);
    this->mean_motion = sqrt(this->mu / pow(this->semi_major_axis, 3));
    this->orbital_period = 2 * M_PI / this->mean_motion;
    this->semi_parameter = pow(this->h(), 2) / this->mu;
    this->orbital_energy = -this->mu / 2 / this->a();
    this->r_apogee = this->a() * (1 + this->e());
    this->r_perigee = this->a() * (1 - this->e());
    return;
}
/*! This method only changes the outputs dependent on true anomaly so that one
 * orbit may be queried at various points along the orbit*/
void KeplerianOrbit::change_f(){
    double r[3];
    double v[3];
    classicElements oe = this->oe(); //
    elem2rv(this->mu, &oe, r, v); //
    this->position_BP_P = cArray2EigenVector3d(r); //
    this->velocity_BP_P = cArray2EigenVector3d(v); //
    this->true_anomaly_rate = this->n() * pow(this->a(), 2) * sqrt(1 - pow(this->e(), 2)) / pow(this->r(), 2); //
    this->radial_rate = this->r() * this->fDot() * this->e() * sin(this->f()) / (1 + this->e() * cos(this->f())); //
    this->eccentric_anomaly = acos((this->e() + cos(this->f()) / (1 + this->e() * cos(this->f())))); //
    this->mean_anomaly = this->E() - this->e() * sin(this->E()); //
    this->flight_path_angle = acos(sqrt((1 - pow(this->e(), 2)) / (1 - pow(this->e(), 2)*pow(cos(this->E()), 2)))); //
    return;
}

/*! This method sets the planet being orbited */
void KeplerianOrbit::set_planet(GravBodyData *plt){
    this->planet = plt;
    this->mu = plt->mu;
    return;
}



