#
#  ISC License
#
#  Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder
#
#  Permission to use, copy, modify, and/or distribute this software for any
#  purpose with or without fee is hereby granted, provided that the above
#  copyright notice and this permission notice appear in all copies.
#
#  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

r"""
Overview
--------

Demonstrates how to simulate an attitude control scenario without having any gravity
bodies present. In essence, the spacecraft is hovering in deep space.  The goal is to
stabilize the tumble of a spacecraft and point it towards a fixed inertial direction.
This script sets up a 6-DOF spacecraft which is hovering in deep space.  The scenario duplicates
the scenario in :ref:`scenarioAttitudeFeedback` without adding
Earth gravity body to the simulation.

The script is found in the folder ``src/examples`` and executed by using::

      python3 scenarioAttitudeFeedbackNoEarth.py

When the simulation completes 3 plots are shown for the MRP attitude history, the rate
tracking errors, as well as the control torque vector.

The simulation layout is shown in the following illustration.  A single simulation process is created
which contains both the spacecraft simulation modules, as well as the Flight Software (FSW) algorithm
modules.

.. image:: /_images/static/test_scenarioAttitudeFeedbackNoEarth.svg
   :align: center

The spacecraft simulation is identical to :ref:`scenarioAttitudeFeedback`.
The primary difference is that the gravity body is not included.


Illustration of Simulation Results
----------------------------------
The following simulation runs should output identical results to the scenario runs in
:ref:`scenarioAttitudeFeedback` as the attitude pointing goal
is an inertial orientation, and thus independent of the satellite being in Earth orbit or
hovering in deep space.

::

    show_plots = True, useUnmodeledTorque = False, useIntGain = False, useKnownTorque = False

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth1000.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth2000.svg
   :align: center

::

    show_plots = True, useUnmodeledTorque = True, useIntGain = False, useKnownTorque = False

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth1100.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth2100.svg
   :align: center

::

    show_plots = True, useUnmodeledTorque = True, useIntGain = True, useKnownTorque = False

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth1110.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth2110.svg
   :align: center

::

    show_plots = True, useUnmodeledTorque = True, useIntGain = False, useKnownTorque = True

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth1101.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeFeedbackNoEarth2101.svg
   :align: center


"""

#
# Basilisk Scenario Script and Integrated Test
#
# Purpose:  Integrated test of the spacecraftPlus(), extForceTorque, simpleNav() and
#           MRP_Feedback() modules.  Illustrates spacecraft attitude control in deep
#           space without a planet or gravity body setup.
# Author:   Hanspeter Schaub
# Creation Date:  Sept. 13, 2018
#
import sys
import os
import numpy as np

# import general simulation support files
from Basilisk.utilities import SimulationBaseClass
from Basilisk.utilities import unitTestSupport  # general support file with common unit test functions
import matplotlib.pyplot as plt
from Basilisk.utilities import macros
from Basilisk.utilities import orbitalMotion

# import simulation related support
from Basilisk.simulation import spacecraftPlus
from Basilisk.simulation import extForceTorque
from Basilisk.utilities import simIncludeGravBody
from Basilisk.simulation import simple_nav

# import FSW Algorithm related support
from Basilisk.fswAlgorithms import MRP_Feedback
from Basilisk.fswAlgorithms import inertial3D
from Basilisk.fswAlgorithms import attTrackingError

# import message declarations
from Basilisk.fswAlgorithms import fswMessages


# attempt to import vizard
from Basilisk.utilities import vizSupport

# The path to the location of Basilisk
# Used to get the location of supporting data.
from Basilisk import __path__
bskPath = __path__[0]
fileName = os.path.basename(os.path.splitext(__file__)[0])



def run(show_plots, useUnmodeledTorque, useIntGain, useKnownTorque):
    """
    The scenarios can be run with the followings setups parameters:

    Args:
        show_plots (bool): Determines if the script should display plots
        useUnmodeledTorque (bool): Specify if an external torque should be included
        useIntGain (bool): Specify if the feedback control uses an integral feedback term
        useKnownTorque (bool): Specify if the external torque is feed forward in the contro

    """

    # Create simulation variable names
    simTaskName = "simTask"
    simProcessName = "simProcess"

    #  Create a sim module as an empty container
    scSim = SimulationBaseClass.SimBaseClass()

    # set the simulation time variable used later on
    simulationTime = macros.min2nano(10.)

    #
    #  create the simulation process
    #
    dynProcess = scSim.CreateNewProcess(simProcessName)

    # create the dynamics task and specify the integration update time
    simulationTimeStep = macros.sec2nano(.1)
    dynProcess.addTask(scSim.CreateNewTask(simTaskName, simulationTimeStep))

    #
    #   setup the simulation tasks/objects
    #

    # initialize spacecraftPlus object and set properties
    scObject = spacecraftPlus.SpacecraftPlus()
    scObject.ModelTag = "spacecraftBody"
    # define the simulation inertia
    I = [900., 0., 0.,
         0., 800., 0.,
         0., 0., 600.]
    scObject.hub.mHub = 750.0  # kg - spacecraft mass
    scObject.hub.r_BcB_B = [[0.0], [0.0], [0.0]]  # m - position vector of body-fixed point B relative to CM
    scObject.hub.IHubPntBc_B = unitTestSupport.np2EigenMatrix3d(I)

    # add spacecraftPlus object to the simulation process
    scSim.AddModelToTask(simTaskName, scObject)


    # setup extForceTorque module
    # the control torque is read in through the messaging system
    extFTObject = extForceTorque.ExtForceTorque()
    extFTObject.ModelTag = "externalDisturbance"
    # use the input flag to determine which external torque should be applied
    # Note that all variables are initialized to zero.  Thus, not setting this
    # vector would leave it's components all zero for the simulation.
    if useUnmodeledTorque:
        extFTObject.extTorquePntB_B = [[0.25], [-0.25], [0.1]]
    scObject.addDynamicEffector(extFTObject)
    scSim.AddModelToTask(simTaskName, extFTObject)

    # add the simple Navigation sensor module.  This sets the SC attitude, rate, position
    # velocity navigation message
    sNavObject = simple_nav.SimpleNav()
    sNavObject.ModelTag = "SimpleNavigation"
    scSim.AddModelToTask(simTaskName, sNavObject)

    #
    #   setup the FSW algorithm tasks
    #

    # setup inertial3D guidance module
    inertial3DConfig = inertial3D.inertial3DConfig()
    inertial3DWrap = scSim.setModelDataWrap(inertial3DConfig)
    inertial3DWrap.ModelTag = "inertial3D"
    scSim.AddModelToTask(simTaskName, inertial3DWrap, inertial3DConfig)
    inertial3DConfig.sigma_R0N = [0., 0., 0.]  # set the desired inertial orientation
    inertial3DConfig.outputDataName = "guidanceInertial3D"

    # setup the attitude tracking error evaluation module
    attErrorConfig = attTrackingError.attTrackingErrorConfig()
    attErrorWrap = scSim.setModelDataWrap(attErrorConfig)
    attErrorWrap.ModelTag = "attErrorInertial3D"
    scSim.AddModelToTask(simTaskName, attErrorWrap, attErrorConfig)
    attErrorConfig.outputDataName = "attErrorInertial3DMsg"
    attErrorConfig.inputRefName = inertial3DConfig.outputDataName
    attErrorConfig.inputNavName = sNavObject.outputAttName

    # setup the MRP Feedback control module
    mrpControlConfig = MRP_Feedback.MRP_FeedbackConfig()
    mrpControlWrap = scSim.setModelDataWrap(mrpControlConfig)
    mrpControlWrap.ModelTag = "MRP_Feedback"
    scSim.AddModelToTask(simTaskName, mrpControlWrap, mrpControlConfig)
    mrpControlConfig.inputGuidName = attErrorConfig.outputDataName
    mrpControlConfig.vehConfigInMsgName = "vehicleConfigName"
    mrpControlConfig.outputDataName = extFTObject.cmdTorqueInMsgName
    mrpControlConfig.K = 3.5
    if useIntGain:
        mrpControlConfig.Ki = 0.0002  # make value negative to turn off integral feedback
    else:
        mrpControlConfig.Ki = -1  # make value negative to turn off integral feedback
    mrpControlConfig.P = 30.0
    mrpControlConfig.integralLimit = 2. / mrpControlConfig.Ki * 0.1
    if useKnownTorque:
        mrpControlConfig.knownTorquePntB_B = [0.25, -0.25, 0.1]

    #
    #   Setup data logging before the simulation is initialized
    #
    numDataPoints = 50
    samplingTime = simulationTime // (numDataPoints - 1)
    scSim.TotalSim.logThisMessage(mrpControlConfig.outputDataName, samplingTime)
    scSim.TotalSim.logThisMessage(attErrorConfig.outputDataName, samplingTime)
    scSim.TotalSim.logThisMessage(sNavObject.outputTransName, samplingTime)

    #
    # create simulation messages
    #

    # create the FSW vehicle configuration message
    vehicleConfigOut = fswMessages.VehicleConfigFswMsg()
    vehicleConfigOut.ISCPntB_B = I  # use the same inertia in the FSW algorithm as in the simulation
    unitTestSupport.setMessage(scSim.TotalSim,
                               simProcessName,
                               mrpControlConfig.vehConfigInMsgName,
                               vehicleConfigOut)

    # The primary difference is that the gravity body is not included.
    # When initializing the spacecraft states, only the attitude states must be set.  The position and velocity
    # states are initialized automatically to zero.
    scObject.hub.sigma_BNInit = [[0.1], [0.2], [-0.3]]  # sigma_BN_B
    scObject.hub.omega_BN_BInit = [[0.001], [-0.01], [0.03]]  # rad/s - omega_BN_B

    # if this scenario is to interface with the BSK Viz, uncomment the following lines
    # vizSupport.enableUnityVisualization(scSim, simTaskName, simProcessName, saveFile=fileName)

    #
    #   initialize Simulation
    #
    scSim.InitializeSimulationAndDiscover()

    #
    #   configure a simulation stop time time and execute the simulation run
    #
    scSim.ConfigureStopTime(simulationTime)
    scSim.ExecuteSimulation()

    #
    #   retrieve the logged data
    #
    dataLr = scSim.pullMessageLogData(mrpControlConfig.outputDataName + ".torqueRequestBody", list(range(3)))
    dataSigmaBR = scSim.pullMessageLogData(attErrorConfig.outputDataName + ".sigma_BR", list(range(3)))
    dataOmegaBR = scSim.pullMessageLogData(attErrorConfig.outputDataName + ".omega_BR_B", list(range(3)))
    dataPos = scSim.pullMessageLogData(sNavObject.outputTransName + ".r_BN_N", list(range(3)))
    np.set_printoptions(precision=16)

    #
    #   plot the results
    #
    plt.close("all")  # clears out plots from earlier test runs
    plt.figure(1)
    for idx in range(1, 4):
        plt.plot(dataSigmaBR[:, 0] * macros.NANO2MIN, dataSigmaBR[:, idx],
                 color=unitTestSupport.getLineColor(idx, 3),
                 label=r'$\sigma_' + str(idx) + '$')
    plt.legend(loc='lower right')
    plt.xlabel('Time [min]')
    plt.ylabel(r'Attitude Error $\sigma_{B/R}$')
    figureList = {}
    pltName = fileName + "1" + str(int(useUnmodeledTorque)) + str(int(useIntGain))+ str(int(useKnownTorque))
    figureList[pltName] = plt.figure(1)

    plt.figure(2)
    for idx in range(1, 4):
        plt.plot(dataLr[:, 0] * macros.NANO2MIN, dataLr[:, idx],
                 color=unitTestSupport.getLineColor(idx, 3),
                 label='$L_{r,' + str(idx) + '}$')
    plt.legend(loc='lower right')
    plt.xlabel('Time [min]')
    plt.ylabel('Control Torque $L_r$ [Nm]')
    pltName = fileName + "2" + str(int(useUnmodeledTorque)) + str(int(useIntGain)) + str(int(useKnownTorque))
    figureList[pltName] = plt.figure(2)

    plt.figure(3)
    for idx in range(1, 4):
        plt.plot(dataOmegaBR[:, 0] * macros.NANO2MIN, dataOmegaBR[:, idx],
                 color=unitTestSupport.getLineColor(idx, 3),
                 label=r'$\omega_{BR,' + str(idx) + '}$')
    plt.legend(loc='lower right')
    plt.xlabel('Time [min]')
    plt.ylabel('Rate Tracking Error [rad/s] ')

    if show_plots:
        plt.show()

    # close the plots being saved off to avoid over-writing old and new figures
    plt.close("all")


    return dataPos, dataSigmaBR, dataLr, numDataPoints, figureList


#
# This statement below ensures that the unit test scrip can be run as a
# stand-along python script
#
if __name__ == "__main__":
    run(
        True,  # show_plots
        False,  # useUnmodeledTorque
        False,  # useIntGain
        False  # useKnownTorque
    )
