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

Discusses how to use guidance modules to align the spacecraft frame to the orbit or Hill frame.
This script sets up a 6-DOF spacecraft which is orbiting the Earth.

The script is found in the folder ``src/examples`` and executed by using::

    python3 scenarioAttitudeGuidance.py

The simulation layout is shown in the following illustration.  A single simulation process is created
which contains both the spacecraft simulation modules, as well as the Flight Software (FSW) algorithm
modules.

.. image:: /_images/static/test_scenarioAttitudeGuidance.svg
   :align: center

When the simulation completes 4 plots are shown for the MRP attitude history, the rate
tracking errors, the control torque vector, as well as the projection of the body-frame B
axes :math:`\hat b_1`, :math:`\hat b_2` and :math:`\hat b_3` onto the respect
Hill or Orbit frame axes :math:`\hat\imath_r`,
:math:`\hat\imath_{\theta}` and :math:`\hat\imath_h`.  This latter plot illustrates how the body
is being aligned with respect to this Hill frame.

The basic simulation setup is the same as the one used in
:ref:`scenarioAttitudeFeedback`.
The dynamics simulation is setup using a :ref:`SpacecraftPlus` module to which a gravity
effector is attached.  Note that both the rotational and translational degrees of
freedom of the spacecraft hub are turned on here to get a 6-DOF simulation.  For more
information on how to setup orbit, see :ref:`scenarioBasicOrbit`.

In contrast to the simple inertial pointing guidance example :ref:`scenarioAttitudeFeedback`,
this module also requires the
spacecraft's position and velocity information.  The planet ephemeris message relative to which the Hill pointing
is being achieved by setting the ``inputCelMessName`` message.
This is useful, for example, if orbiting the sun, and wanting to point the spacecraft back at the
Earth which is also orbiting the sun.
Note that while the celestial body ephemeris input message must be set, it can be a non-existing message.
In that case a zero message is created which corresponds to the planet having a zero position and velocity vector.
If non-zero ephemeris information is required then the input name must point
to a message of type :ref:`EphemerisIntMsg`.
In this scenario, however, the spacecraft is to point at the Earth while already orbiting the Earth and the input
message name is set to a dummy message.

Illustration of Simulation Results
----------------------------------

::

    show_plots = True, useAltBodyFrame = False

The default scenario shown has the ``useAltBodyFrame`` flag turned off.  This means that we seek
to align the body frame *B* with the Hill reference frame :math:`\cal R`.    The
resulting attitude and control torque histories are shown below.  Note that the projections
of the body frame axes onto the Hill frame axes all converge to +1, indicating that :math:`\cal B` becomes
asymptotically aligned with :math:`\cal R` as desired.

.. image:: /_images/Scenarios/scenarioAttitudeGuidance10.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeGuidance20.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeGuidance40.svg
   :align: center

::

    show_plots = True, useAltBodyFrame = True

Here the control should not align the principal body frame *B* with *R*, but rather an alternate,
corrected body frame :math:`{\cal B}_c`.  For example, consider the Earth observing sensors
to be mounted pointing in the
positive :math:`\hat b_1` direction. In earlier scenario this sensor platform is actually pointing away from
the Earth.  Thus, we define the corrected body frame orientation as a 180 deg rotation about
:math:`\hat b_2`.  This flips the orientation of the final first and third body axis.  This is achieved
through::

  attErrorConfig.sigma_R0R = [0,1,0]

The DCM :math:`[R_0R]` is the same as the body to corrected body DCM :math:`[B_cB]`.
The resulting attitude and control torque histories are shown below.  Note that the projections
of the 2nd body frame axis onto the 2nd Hill frame axes converges to +1, while the other
projections converge to -1.  This indicates that the desired asymptotic Earth observing attitude
is achieved.

.. image:: /_images/Scenarios/scenarioAttitudeGuidance11.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeGuidance21.svg
   :align: center

.. image:: /_images/Scenarios/scenarioAttitudeGuidance41.svg
   :align: center

"""

#
# Basilisk Scenario Script and Integrated Test
#
# Purpose:  Integrated test of the spacecraftPlus(), extForceTorque, simpleNav(),
#           MRP_Feedback() with attitude navigation modules.  Illustrates how
#           attitude guidance behavior can be changed in a very modular manner.
# Author:   Hanspeter Schaub
# Creation Date:  Dec. 2, 2016
#

import sys
import pytest
import os
import numpy as np

# import general simulation support files
from Basilisk.utilities import SimulationBaseClass
from Basilisk.utilities import unitTestSupport  # general support file with common unit test functions
import matplotlib.pyplot as plt
from Basilisk.utilities import macros
from Basilisk.utilities import orbitalMotion
from Basilisk.utilities import RigidBodyKinematics

# import simulation related support
from Basilisk.simulation import spacecraftPlus
from Basilisk.simulation import extForceTorque
from Basilisk.utilities import simIncludeGravBody
from Basilisk.simulation import simple_nav

# import FSW Algorithm related support
from Basilisk.fswAlgorithms import MRP_Feedback
from Basilisk.fswAlgorithms import hillPoint
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


# Plotting functions
def plot_attitude_error(timeLineSet, dataSigmaBR):
    """Plot the attitude result."""
    plt.figure(1)
    fig = plt.gcf()
    ax = fig.gca()
    vectorData = unitTestSupport.pullVectorSetFromData(dataSigmaBR)
    sNorm = np.array([np.linalg.norm(v) for v in vectorData])
    plt.plot(timeLineSet, sNorm,
             color=unitTestSupport.getLineColor(1, 3),
             )
    plt.xlabel('Time [min]')
    plt.ylabel(r'Attitude Error Norm $|\sigma_{B/R}|$')
    ax.set_yscale('log')

def plot_control_torque(timeLineSet, dataLr):
    """Plot the control torque response."""
    plt.figure(2)
    for idx in range(1, 4):
        plt.plot(timeLineSet, dataLr[:, idx],
                 color=unitTestSupport.getLineColor(idx, 3),
                 label='$L_{r,' + str(idx) + '}$')
    plt.legend(loc='lower right')
    plt.xlabel('Time [min]')
    plt.ylabel('Control Torque $L_r$ [Nm]')

def plot_rate_error(timeLineSet, dataOmegaBR):
    """Plot the body angular velocity tracking error."""
    plt.figure(3)
    for idx in range(1, 4):
        plt.plot(timeLineSet, dataOmegaBR[:, idx],
                 color=unitTestSupport.getLineColor(idx, 3),
                 label=r'$\omega_{BR,' + str(idx) + '}$')
    plt.legend(loc='lower right')
    plt.xlabel('Time [min]')
    plt.ylabel('Rate Tracking Error [rad/s] ')
    return

def plot_orientation(timeLineSet, dataPos, dataVel, dataSigmaBN):
    """Plot the spacecraft orientation."""
    vectorPosData = unitTestSupport.pullVectorSetFromData(dataPos)
    vectorVelData = unitTestSupport.pullVectorSetFromData(dataVel)
    vectorMRPData = unitTestSupport.pullVectorSetFromData(dataSigmaBN)
    data = np.empty([len(vectorPosData), 3])
    for idx in range(0, len(vectorPosData)):
        ir = vectorPosData[idx] / np.linalg.norm(vectorPosData[idx])
        hv = np.cross(vectorPosData[idx], vectorVelData[idx])
        ih = hv / np.linalg.norm(hv)
        itheta = np.cross(ih, ir)
        dcmBN = RigidBodyKinematics.MRP2C(vectorMRPData[idx])
        data[idx] = [np.dot(ir, dcmBN[0]), np.dot(itheta, dcmBN[1]), np.dot(ih, dcmBN[2])]
    plt.figure(4)
    labelStrings = (r'$\hat\imath_r\cdot \hat b_1$'
                    , r'${\hat\imath}_{\theta}\cdot \hat b_2$'
                    , r'$\hat\imath_h\cdot \hat b_3$')
    for idx in range(0, 3):
        plt.plot(timeLineSet, data[:, idx],
                 color=unitTestSupport.getLineColor(idx + 1, 3),
                 label=labelStrings[idx])
    plt.legend(loc='lower right')
    plt.xlabel('Time [min]')
    plt.ylabel('Orientation Illustration')


def run(show_plots, useAltBodyFrame):
    """
    The scenarios can be run with the followings setups parameters:

    Args:
        show_plots (bool): Determines if the script should display plots
        useAltBodyFrame (bool): Specify if the alternate body frame should be aligned with Hill frame.

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
    simulationTimeStep = macros.sec2nano(0.1)
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

    # clear prior gravitational body and SPICE setup definitions
    gravFactory = simIncludeGravBody.gravBodyFactory()

    # setup Earth Gravity Body
    earth = gravFactory.createEarth()
    earth.isCentralBody = True  # ensure this is the central gravitational body
    mu = earth.mu

    # attach gravity model to spaceCraftPlus
    scObject.gravField.gravBodies = spacecraftPlus.GravBodyVector(list(gravFactory.gravBodies.values()))

    #
    #   initialize Spacecraft States with initialization variables
    #
    # setup the orbit using classical orbit elements
    oe = orbitalMotion.ClassicElements()
    oe.a = 10000000.0  # meters
    oe.e = 0.1
    oe.i = 33.3 * macros.D2R
    oe.Omega = 48.2 * macros.D2R
    oe.omega = 347.8 * macros.D2R
    oe.f = 85.3 * macros.D2R
    rN, vN = orbitalMotion.elem2rv(mu, oe)
    scObject.hub.r_CN_NInit = rN  # m   - r_CN_N
    scObject.hub.v_CN_NInit = vN  # m/s - v_CN_N
    scObject.hub.sigma_BNInit = [[0.1], [0.2], [-0.3]]  # sigma_BN_B
    scObject.hub.omega_BN_BInit = [[0.001], [-0.01], [0.03]]  # rad/s - omega_BN_B

    # setup extForceTorque module
    # the control torque is read in through the messaging system
    extFTObject = extForceTorque.ExtForceTorque()
    extFTObject.ModelTag = "externalDisturbance"
    # use the input flag to determine which external torque should be applied
    # Note that all variables are initialized to zero.  Thus, not setting this
    # vector would leave it's components all zero for the simulation.
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

    # setup hillPoint guidance module
    attGuidanceConfig = hillPoint.hillPointConfig()
    attGuidanceWrap = scSim.setModelDataWrap(attGuidanceConfig)
    attGuidanceWrap.ModelTag = "hillPoint"
    attGuidanceConfig.inputNavDataName = sNavObject.outputTransName
    # if you want to set attGuidanceConfig.inputCelMessName, then you need a planet ephemeris message of
    # type EphemerisIntMsg.  In the line below a non-existing message name is used to create an empty planet
    # ephemeris message which puts the earth at (0,0,0) origin with zero speed.
    attGuidanceConfig.inputCelMessName = "empty_earth_msg"
    attGuidanceConfig.outputDataName = "guidanceOut"
    scSim.AddModelToTask(simTaskName, attGuidanceWrap, attGuidanceConfig)

    # setup the attitude tracking error evaluation module
    attErrorConfig = attTrackingError.attTrackingErrorConfig()
    attErrorWrap = scSim.setModelDataWrap(attErrorConfig)
    attErrorWrap.ModelTag = "attErrorInertial3D"
    scSim.AddModelToTask(simTaskName, attErrorWrap, attErrorConfig)
    attErrorConfig.outputDataName = "attErrorMsg"
    if useAltBodyFrame:
        attErrorConfig.sigma_R0R = [0, 1, 0]
    attErrorConfig.inputRefName = attGuidanceConfig.outputDataName
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
    mrpControlConfig.Ki = -1.0  # make value negative to turn off integral feedback
    mrpControlConfig.P = 30.0
    mrpControlConfig.integralLimit = 2. / mrpControlConfig.Ki * 0.1

    #
    #   Setup data logging before the simulation is initialized
    #
    numDataPoints = 100
    samplingTime = simulationTime // (numDataPoints - 1)
    scSim.TotalSim.logThisMessage(mrpControlConfig.outputDataName, samplingTime)
    scSim.TotalSim.logThisMessage(attErrorConfig.outputDataName, samplingTime)
    scSim.TotalSim.logThisMessage(sNavObject.outputTransName, samplingTime)
    scSim.TotalSim.logThisMessage(sNavObject.outputAttName, samplingTime)

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

    # if this scenario is to interface with the BSK Viz, uncomment the following lines
    # vizSupport.enableUnityVisualization(scSim, simTaskName, simProcessName, gravBodies=gravFactory, saveFile=fileName)

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
    dataVel = scSim.pullMessageLogData(sNavObject.outputTransName + ".v_BN_N", list(range(3)))
    dataSigmaBN = scSim.pullMessageLogData(sNavObject.outputAttName + ".sigma_BN", list(range(3)))
    np.set_printoptions(precision=16)

    #
    #   plot the results
    #
    timeLineSet = dataSigmaBR[:, 0] * macros.NANO2MIN
    plt.close("all")  # clears out plots from earlier test runs

    plot_attitude_error(timeLineSet, dataSigmaBR)
    figureList = {}
    pltName = fileName + "1" + str(int(useAltBodyFrame))
    figureList[pltName] = plt.figure(1)

    plot_control_torque(timeLineSet, dataLr)
    pltName = fileName + "2" + str(int(useAltBodyFrame))
    figureList[pltName] = plt.figure(2)

    plot_rate_error(timeLineSet, dataOmegaBR)

    plot_orientation(timeLineSet, dataPos, dataVel, dataSigmaBN)
    pltName = fileName + "4" + str(int(useAltBodyFrame))
    figureList[pltName] = plt.figure(4)

    if show_plots:
        plt.show()

    # close the plots being saved off to avoid over-writing old and new figures
    plt.close("all")

    return dataPos, dataSigmaBN, numDataPoints, figureList


#
# This statement below ensures that the unit test scrip can be run as a
# stand-along python script
#
if __name__ == "__main__":
    run(
        True,  # show_plots
        False  # useAltBodyFrame
    )
