''' '''
'''
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

'''


#
# Eclipse Condition Unit Test
#
# Purpose:  Test the proper function of the Eclipse environment module.
#           This is done by comparing computed expected shadow factors in
#           particular eclipse conditions to what is simulated
# Author:   Patrick Kenneally
# Creation Date:  May. 31, 2017
#

import pytest
import os
from Basilisk.utilities import SimulationBaseClass
from Basilisk.utilities import unitTestSupport
from Basilisk.simulation import spacecraftPlus
from Basilisk.utilities import macros
from Basilisk.simulation import eclipse
from Basilisk.utilities import orbitalMotion
from Basilisk.utilities import simIncludeGravBody
from Basilisk import __path__
bskPath = __path__[0]

path = os.path.dirname(os.path.abspath(__file__))

# uncomment this line if this test has an expected failure, adjust message as needed
# @pytest.mark.xfail(True)
@pytest.mark.parametrize("eclipseCondition, planet", [
("partial", "earth"), ("full", "earth"), ("none", "earth"), ("annular", "earth"),
("partial", "mars"), ("full", "mars"), ("none", "mars"), ("annular", "mars")])

def test_unitEclipse(show_plots, eclipseCondition, planet):
    """
**Test Description and Success Criteria**

The unit test validates the internal aspects of the Basilisk eclipse module by comparing simulated output with \
expected output. It validates the computation of a shadow factor for total eclipse, partial eclipse,annular eclipse, \
and no eclipse scenarios. The test is designed to analyze one type at a time for both Earth and Mars and is then \
repeated for all three.

Earth is set as the zero base for all eclipse types to test it as the occulting body. For full, partial, and \
no eclipse cases, orbital elements describing the spacecraft states are then converted to Cartesian vectors. \
These orbital elements vary for each eclipse type since the Sun and planet states are fixed. The conversion is \
made using the orbitalMotion elem2rv function, where the inputs are six orbital elements (a, e, i, Omega, omega, f) \
and the outputs are Cartesian position and velocity vectors. For the annular eclipse case, the conversion is \
avoided and a Cartesian position vector is initially provided instead. The vectors are then passed into \
spacecraftPlus and, subsequently, the eclipse module through the Basilisk messaging system.

Testing the no eclipse case with Mars as the occulting body is the same as the Earth no eclipse test, except \
Mars is set as the zero base. The Mars full, partial, and annular eclipse cases, however, are like the Earth \
annular case where Cartesian vectors are, instead, the initial inputs. Since the test is performed as a single \
step process, the velocity is not necessarily needed as an input, so only a position vector is provided \
for these cases.

The shadow factor obtained through the module is compared to the expected result, which is either trivial or \
calculated, depending on the eclipse type. Full eclipse and no eclipse shadow factors are compared without the \
need for computation, since they are just 0.0 and 1.0, respectively. The partial and annular eclipse shadow \
factors, however, vary between 0.0 and 1.0, based on the cone dimensions, and are calculated using \
MATLAB and Spice data.


**Test Parameters:**

- ``eclipseCondition``: [string]
  defines if the eclipse is partial, full, none or annular
- ``planet``: [string]
  defines which planet to use.  Options include "earth" and "mars"


**Description of Variables Being Tested**

In each test scenario the shadow eclipse variable

    ``shadowFactor``

is pulled from the log data and compared to expected truth values.

    """
    [testResults, testMessage] = unitEclipse(show_plots, eclipseCondition, planet)
    assert testResults < 1, testMessage


def unitEclipse(show_plots, eclipseCondition, planet):
    __tracebackhide__ = True

    testFailCount = 0
    testMessages = []
    testTaskName = "unitTestTask"
    testProcessName = "unitTestProcess"
    testTaskRate = macros.sec2nano(1)

    # Create a simulation container
    unitTestSim = SimulationBaseClass.SimBaseClass()
    # Ensure simulation is empty
    testProc = unitTestSim.CreateNewProcess(testProcessName)
    testProc.addTask(unitTestSim.CreateNewTask(testTaskName, testTaskRate))

    # Set up first spacecraft
    scObject_0 = spacecraftPlus.SpacecraftPlus()
    scObject_0.scStateOutMsgName = "inertial_state_output"
    unitTestSim.AddModelToTask(testTaskName, scObject_0)

    # setup Gravity Bodies
    gravFactory = simIncludeGravBody.gravBodyFactory()
    if planet == "earth":
        earth = gravFactory.createEarth()
        earth.isCentralBody = True
        earth.useSphericalHarmParams = False
    elif planet == "mars":
        mars = gravFactory.createMars()
        mars.isCentralBody = True
        mars.useSphericalHarmParams = False
    scObject_0.gravField.gravBodies = spacecraftPlus.GravBodyVector(list(gravFactory.gravBodies.values()))

    # setup Spice interface for some solar system bodies
    timeInitString = '2021 MAY 04 07:47:48.965 (UTC)'
    gravFactory.createSpiceInterface(bskPath + '/supportData/EphemerisData/'
                                     , timeInitString
                                     , spicePlanetNames = ["sun", "venus", "earth", "mars barycenter"]
                                     )

    if planet == "earth":
        if eclipseCondition == "full":
            gravFactory.spiceObject.zeroBase = "earth"
            # set up spacecraft 0 position and velocity for full eclipse
            oe = orbitalMotion.ClassicElements()
            r_0 = (500 + orbitalMotion.REQ_EARTH)  # km
            oe.a = r_0
            oe.e = 0.00001
            oe.i = 5.0 * macros.D2R
            oe.Omega = 48.2 * macros.D2R
            oe.omega = 0 * macros.D2R
            oe.f = 173 * macros.D2R
            r_N_0, v_N_0 = orbitalMotion.elem2rv(orbitalMotion.MU_EARTH, oe)
            scObject_0.hub.r_CN_NInit = r_N_0 * 1000  # convert to meters
            scObject_0.hub.v_CN_NInit = v_N_0 * 1000  # convert to meters
        elif eclipseCondition == "partial":
            gravFactory.spiceObject.zeroBase = "earth"
            # set up spacecraft 0 position and velocity for full eclipse
            oe = orbitalMotion.ClassicElements()
            r_0 = (500 + orbitalMotion.REQ_EARTH)  # km
            oe.a = r_0
            oe.e = 0.00001
            oe.i = 5.0 * macros.D2R
            oe.Omega = 48.2 * macros.D2R
            oe.omega = 0 * macros.D2R
            oe.f = 107.5 * macros.D2R
            r_N_0, v_N_0 = orbitalMotion.elem2rv(orbitalMotion.MU_EARTH, oe)
            scObject_0.hub.r_CN_NInit = r_N_0 * 1000  # convert to meters
            scObject_0.hub.v_CN_NInit = v_N_0 * 1000  # convert to meters
        elif eclipseCondition == "none":
            oe = orbitalMotion.ClassicElements()
            r_0 = 9959991.68982  # km
            oe.a = r_0
            oe.e = 0.00001
            oe.i = 5.0 * macros.D2R
            oe.Omega = 48.2 * macros.D2R
            oe.omega = 0 * macros.D2R
            oe.f = 107.5 * macros.D2R
            r_N_0, v_N_0 = orbitalMotion.elem2rv(orbitalMotion.MU_EARTH, oe)
            scObject_0.hub.r_CN_NInit = r_N_0 * 1000  # convert to meters
            scObject_0.hub.v_CN_NInit = v_N_0 * 1000  # convert to meters
        elif eclipseCondition == "annular":
            gravFactory.spiceObject.zeroBase = "earth"
            scObject_0.hub.r_CN_NInit = [-326716535628.942, -287302983139.247, -124542549301.050]

    elif planet == "mars":
        if eclipseCondition == "full":
            gravFactory.spiceObject.zeroBase = "mars barycenter"
            scObject_0.hub.r_CN_NInit = [-2930233.55919119, 2567609.100747609, 41384.23366372246] # meters
        elif eclipseCondition == "partial":
            print("partial mars")
            gravFactory.spiceObject.zeroBase = "mars barycenter"
            scObject_0.hub.r_CN_NInit = [-6050166.454829555, 2813822.447404055, 571725.5651779658] # meters
        elif eclipseCondition == "none":
            oe = orbitalMotion.ClassicElements()
            r_0 = 9959991.68982  # km
            oe.a = r_0
            oe.e = 0.00001
            oe.i = 5.0 * macros.D2R
            oe.Omega = 48.2 * macros.D2R
            oe.omega = 0 * macros.D2R
            oe.f = 107.5 * macros.D2R
            r_N_0, v_N_0 = orbitalMotion.elem2rv(orbitalMotion.MU_MARS, oe)
            scObject_0.hub.r_CN_NInit = r_N_0 * 1000  # convert to meters
            scObject_0.hub.v_CN_NInit = v_N_0 * 1000  # convert to meters
        elif eclipseCondition == "annular":
            gravFactory.spiceObject.zeroBase = "mars barycenter"
            scObject_0.hub.r_CN_NInit = [-427424601171.464, 541312532797.400, 259820030623.064]  # meters

    unitTestSim.AddModelToTask(testTaskName, gravFactory.spiceObject, None, -1)

    eclipseObject = eclipse.Eclipse()
    eclipseObject.addPositionMsgName(scObject_0.scStateOutMsgName)
    eclipseObject.addPlanetName('earth')
    eclipseObject.addPlanetName('mars barycenter')
    eclipseObject.addPlanetName('venus')
    unitTestSim.AddModelToTask(testTaskName, eclipseObject)
    unitTestSim.TotalSim.logThisMessage("eclipse_data_0")
    unitTestSim.TotalSim.logThisMessage("mars barycenter_planet_data")
    unitTestSim.TotalSim.logThisMessage("sun_planet_data")
    unitTestSim.TotalSim.logThisMessage("earth_planet_data")

    unitTestSim.InitializeSimulation()

    # Execute the simulation for one time step
    unitTestSim.TotalSim.SingleStepProcesses()

    eclipseData_0 = unitTestSim.pullMessageLogData("eclipse_data_0.shadowFactor")
    # Obtain body position vectors to check with MATLAB

    errTol = 1E-12
    if planet == "earth":
        if eclipseCondition == "partial":
            truthShadowFactor = 0.62310760206735027
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Earth partial eclipse condition")

        elif eclipseCondition == "full":
            truthShadowFactor = 0.0
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Earth full eclipse condition")

        elif eclipseCondition == "none":
            truthShadowFactor = 1.0
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Earth none eclipse condition")
        elif eclipseCondition == "annular":
            truthShadowFactor = 1.497253388113018e-04
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Earth annular eclipse condition")

    elif planet == "mars":
        if eclipseCondition == "partial":
            truthShadowFactor = 0.18745025055615416
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Mars partial eclipse condition")
        elif eclipseCondition == "full":
            truthShadowFactor = 0.0
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Mars full eclipse condition")
        elif eclipseCondition == "none":
            truthShadowFactor = 1.0
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Mars none eclipse condition")
        elif eclipseCondition == "annular":
            truthShadowFactor = 4.245137380531894e-05
            if not unitTestSupport.isDoubleEqual(eclipseData_0[0, :], truthShadowFactor, errTol):
                testFailCount += 1
                testMessages.append("Shadow Factor failed for Mars annular eclipse condition")


    if testFailCount == 0:
        print("PASSED: " + planet + "-" + eclipseCondition)
        # return fail count and join into a single string all messages in the list
        # testMessage

    print('The error tolerance for all tests is ' + str(errTol))

    #
    #  unload the SPICE libraries that were loaded by the spiceObject earlier
    #
    gravFactory.unloadSpiceKernels()

    return [testFailCount, ''.join(testMessages)]

if __name__ == "__main__":
    unitEclipse(False, "annular", "earth")
