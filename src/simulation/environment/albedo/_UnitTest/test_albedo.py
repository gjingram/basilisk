''' '''
'''
 ISC License

 Copyright (c) 2020, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

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
#   Unit Test Script
#   Module Name:   Planet's Albedo
#   Author:        Demet Cilden-Guler
#   Creation Date: May 28, 2020
#

import pytest
import os
import numpy as np
from Basilisk.utilities import SimulationBaseClass
from Basilisk.utilities import unitTestSupport
from Basilisk.utilities import macros
from Basilisk.utilities import orbitalMotion as om
from Basilisk.simulation import albedo
from Basilisk.utilities import simIncludeGravBody
from Basilisk.simulation import simMessages
from Basilisk import __path__
bskPath = __path__[0]

path = os.path.dirname(os.path.abspath(__file__))

# uncomment this line if this test has an expected failure, adjust message as needed
# @pytest.mark.xfail(True)
@pytest.mark.parametrize("planetCase", ['earth', 'mars'])
@pytest.mark.parametrize("modelType", ['ALBEDO_AVG', 'ALBEDO_DATA'])
@pytest.mark.parametrize("useEclipse", [True, False])

def test_unitAlbedo(show_plots, planetCase, modelType, useEclipse):
    """
    **Validation Test Description**

    This section describes the specific unit tests conducted on this module.
    The test contains 4 tests and is located at ``test_albedo.py``.
    The success criteria is to match the outputs with the generated truth.

    Args:

        planetCase (string): Defines which planet to use.  Options include "earth" and "mars".
        modelType (string):  Defines which albedo model to use. Options include "ALBEDO_AVG" and "ALBEDO_DATA".
        useEclipse (bool):  Defines if the eclipse is considered for this parameterized unit test.

    **Description of Variables Being Tested**

    In this file, we are checking the values of the variable:

    ``albedoAtInstrument``

    which are pulled from the log data to see if they match with the expected truth values.

    """
    # each test method requires a single assert method to be called
    [testResults, testMessage] = unitAlbedo(show_plots, planetCase, modelType, useEclipse)
    assert testResults < 1, testMessage


def unitAlbedo(show_plots, planetCase, modelType, useEclipse):
    __tracebackhide__ = True
    testFailCount = 0
    testMessages = []
    testTaskName = "unitTestTask"
    testProcessName = "unitTestProcess"
    testTaskRate = macros.sec2nano(1.0)
    # Create a simulation container
    unitTestSim = SimulationBaseClass.SimBaseClass()
    # Ensure simulation is empty
    testProc = unitTestSim.CreateNewProcess(testProcessName)
    testProc.addTask(unitTestSim.CreateNewTask(testTaskName, testTaskRate))
    # Albedo A1
    albModule = albedo.Albedo()
    albModule.ModelTag = "Albedo_0"
    if modelType == 'ALBEDO_DATA':
        dataPath = os.path.abspath(bskPath + "/supportData/AlbedoData/")
        if planetCase == 'earth':
            fileName = "Earth_ALB_2018_CERES_All_10x10.csv"
        else:
            fileName = "Mars_ALB_TES_10x10.csv"
        albModule.addPlanetandAlbedoDataModel(planetCase, dataPath, fileName)
    else:
        ALB_avg = 0.5
        numLat = 200
        numLon = 400
        albModule.addPlanetandAlbedoAverageModel(planetCase, ALB_avg, numLat, numLon)

    if useEclipse:
        albModule.eclipseCase = True
    # Create dummy sun message
    sunPositionMsg = simMessages.SpicePlanetStateSimMsg()
    # Create dummy planet message
    planetPositionMsg = simMessages.SpicePlanetStateSimMsg()
    planetPositionMsg.PositionVector = [0., 0., 0.]
    gravFactory = simIncludeGravBody.gravBodyFactory()
    if planetCase == 'earth':
        planet = gravFactory.createEarth()
        sunPositionMsg.PositionVector = [-om.AU * 1000., 0.0, 0.0]
    elif planetCase == 'mars':
        planet = gravFactory.createMars()
        sunPositionMsg.PositionVector = [-1.5 * om.AU * 1000., 0.0, 0.0]
    planetPositionMsg.PlanetName = planetCase
    planetPositionMsg.J20002Pfix = np.identity(3)
    planetMsgName = planetCase + '_planet_data'
    req = planet.radEquator
    sunMessage = "sun_message"
    # Create dummy spacecraft message
    scStateMsg = simMessages.SCPlusStatesSimMsg()
    rSC = req + 6000 * 1000  # meters
    alpha = 71. * macros.D2R
    scStateMsg.r_BN_N = np.dot(rSC, [np.cos(alpha), np.sin(alpha), 0.0])
    scStateMsg.sigma_BN = [0., 0., 0.]
    scMsgName = 'SC0'
    # Albedo instrument configuration
    config1 = albedo.instConfig_t()
    config1.fov = 80. * macros.D2R
    config1.nHat_B = np.array([-np.cos(alpha), -np.sin(alpha), 0.0])
    config1.r_IB_B = np.array([0., 0., 0.])
    albModule.addInstrumentConfig('Instrument0', config1)
    unitTestSupport.setMessage(unitTestSim.TotalSim,
                               testProcessName,
                               sunMessage,
                               sunPositionMsg)
    unitTestSupport.setMessage(unitTestSim.TotalSim,
                               testProcessName,
                               planetMsgName,
                               planetPositionMsg)
    unitTestSupport.setMessage(unitTestSim.TotalSim,
                               testProcessName,
                               scMsgName,
                               scStateMsg)

    albModule.spacecraftStateInMsgName = scMsgName
    albModule.sunPositionInMsgName = sunMessage
    unitTestSim.AddModelToTask(testTaskName, albModule)
    unitTestSim.TotalSim.logThisMessage(albModule.albOutMsgNames[0], testTaskRate)

    # Initialize and run simulation one step at a time
    unitTestSim.InitializeSimulation()
    # Execute the simulation for one time step
    unitTestSim.TotalSim.SingleStepProcesses()
    # This pulls the actual data log from the simulation run.
    dataAlb0 = unitTestSim.pullMessageLogData(albModule.albOutMsgNames[0] + ".albedoAtInstrument", list(range(1)))

    errTol = 1E-12
    if planetCase == 'earth':
        if modelType == 'ALBEDO_DATA':
            if useEclipse:
                truthAlb = 0.0022055492477917
            else:
                truthAlb = 0.0022055492477917
        else:
            if useEclipse:
                truthAlb = 0.0041742091531996
            else:
                truthAlb = 0.004174209177079
    else:
        if modelType == 'ALBEDO_DATA':
            if useEclipse:
                truthAlb = 0.0014001432717662
            else:
                truthAlb = 0.0014001432717662
        else:
            if useEclipse:
                truthAlb = 0.0035681407388827
            else:
                truthAlb = 0.0035681407390035

    if not unitTestSupport.isDoubleEqual(dataAlb0[0], truthAlb, errTol):
        testFailCount += 1
    #   print out success or failure message
    if testFailCount == 0:
            print("PASSED: " + albModule.ModelTag)
    else:
            print("Failed: " + albModule.ModelTag)
    print("This test uses a relative accuracy value of " + str(errTol * 100) + " percent")

    return [testFailCount, ''.join(testMessages)]
if __name__ == "__main__":
    unitAlbedo(False, 'earth', 'ALBEDO_AVG', True)
