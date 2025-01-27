.. _examples:


Example Scripts
===============

To learn how to use Basilisk it is often convenient to study sample scripts and see how they were implemented.  Below are a series of example Basilisk scripts grouped by topics and complexity.  In each script the primary simulation is executed by the ``run`` method.  The reader can learn from this implementation and the numerous associated in-line comments.  The first sections illustrate how to setup simple spacecraft simulations and then the following sections begin to add more complexity.

The example script source code can be viewed directly within the browser by clicking on the Source link next to the ``run`` method name as shown in the image below:

.. image:: /_images/static/scenarioSourceLink.png
   :width: 1316px
   :height: 106px
   :scale: 50%
   :alt: scenario python source link
   :align: center

The python code contains additional comments to explain what is being setup and tested here.  The example instructions are all stored as extensive comments within the python script.  Read the script source to study how this example functions.


Orbital Simulations
-------------------

.. toctree::
   :maxdepth: 1

   Basic Orbit Simulations <scenarioBasicOrbit>
   Delta_v Orbit Maneuvers <scenarioOrbitManeuver>
   Multiple Gravitational Bodies <scenarioOrbitMultiBody>
   Defining Motion Relative to Planet <scenarioCentralBody>
   Simulating Trajectory about Multiple Celestial Bodies <scenarioPatchedConics>


Attitude Simulations
--------------------

Attitude Regulation Control
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Inertial Attitude Pointing <scenarioAttitudeFeedback>
   Using Separate Task Group for Control <scenarioAttitudeFeedback2T>
   Basic Attitude Pointing in Deep Space <scenarioAttitudePointing>
   Complex Attitude Pointing in Deep Space <scenarioAttitudeFeedbackNoEarth>


Attitude Guidance
^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Hill Frame Pointing on Elliptic Orbit <scenarioAttitudeGuidance>
   Velocity Frame Pointing on Hyperbolic Orbit <scenarioAttGuideHyperbolic>


Attitude Control with Actuators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Pointing with Reaction Wheels <scenarioAttitudeFeedbackRW>
   Pointing with Attitude Thrusters <scenarioAttitudeFeedback2T_TH>


Attitude Steering Control
^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   MRP Steering Law <scenarioAttitudeSteering>

Planetary Environments
----------------------

Magnetic Field Models
^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Centered Dipole Model <scenarioMagneticFieldCenteredDipole>
   World Magnetic Model WMM <scenarioMagneticFieldWMM>

Gravity Gradient Torque
^^^^^^^^^^^^^^^^^^^^^^^
.. toctree::
   :maxdepth: 1

   Gravity  Gradient Perturbed Hill Pointing <scenarioAttitudeGG>

Ground Station on Planet Surface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. toctree::
   :maxdepth: 1

   Satellite Communicating to Earth Ground Station <scenarioGroundDownlink>

Planet Albedo
^^^^^^^^^^^^^
.. toctree::
   :maxdepth: 1

   Albedo CSS simulation about Multiple Celestial Objects <scenarioAlbedo>


Spacecraft Sensors
------------------

Coarse Sun Sensors
^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Adding CSS Sensors <scenarioCSS>
   Estimating Sun Heading with CSS <scenarioCSSFilters>


Three-Axis Magnetometers
^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Adding a Three-Axis Magnetometer (TAM)  <scenarioTAM>



Spacecraft Sub-Systems
----------------------

Power Sub-System
^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Basic Power Usage and Tracking  <scenarioPowerDemo>
   Power Usage with RW-Based ADCS  <scenarioAttitudeFeedbackRWPower>

Data Sub-System
^^^^^^^^^^^^^^^^

.. toctree::
  :maxdepth: 1

  Basic Data Generation and Transmission  <scenarioDataDemo>

Complex Spacecraft Dynamics Simulations
---------------------------------------

.. toctree::
   :maxdepth: 1

   Fuel Slosh <scenarioFuelSlosh>
   Flexible (Hinged) Panels <scenarioHingedRigidBody>
   Sensors Attached to a Rotating Panel <scenarioRotatingPanel>


bskSim()-Based Simulation
-------------------------

.. toctree::
   :maxdepth: 1

   Basic Orbital Simulation  <BskSim/scenarios/scenario_BasicOrbit>
   Attitude Detumble Control  <BskSim/scenarios/scenario_FeedbackRW>
   Hill Pointing Attitude Control  <BskSim/scenarios/scenario_AttGuidance>
   Velocity Frame Pointing Control  <BskSim/scenarios/scenario_AttGuidHyperbolic>
   MRP Steering Attitude Control  <BskSim/scenarios/scenario_AttSteering>
   Sun Pointing Mode Include Eclipse Evaluation  <BskSim/scenarios/scenario_AttEclipse>

Optical Navigation Simulations
------------------------------

.. toctree::
   :maxdepth: 1

   BSK OpNav Sim  <OpNavScenarios/BSK_OpNav>
   Hough Circles for Pointing and Orbit Determination  <OpNavScenarios/scenariosOpNav/scenario_OpNavAttOD>
   Limb-based method for Pointing and Orbit Determination  <OpNavScenarios/scenariosOpNav/scenario_OpNavAttODLimb>
   CNN for Pointing and Orbit Determination   <OpNavScenarios/scenariosOpNav/scenario_CNNAttOD>
   Simultaneously run two OpNav methods  <OpNavScenarios/scenariosOpNav/scenario_DoubleOpNavOD>
   Perform fault detection with two OpNav methods  <OpNavScenarios/scenariosOpNav/scenario_faultDetOpNav>
   Orbit Determination with Hough Circles  <OpNavScenarios/scenariosOpNav/scenario_OpNavOD>
   Orbit Determination with Limb-based method   <OpNavScenarios/scenariosOpNav/scenario_OpNavODLimb>
   Pointing with Hough Circles  <OpNavScenarios/scenariosOpNav/scenario_OpNavPoint>
   Pointing with Limb-based method   <OpNavScenarios/scenariosOpNav/scenario_OpNavPointLimb>
   Filter Heading measurements  <OpNavScenarios/scenariosOpNav/scenario_OpNavHeading>



Monte Carlo Simulations
-----------------------

.. toctree::
   :maxdepth: 1

   MC run with RW control  <scenarioMonteCarloAttRW>
   MC run using Python Spice setup  <scenarioMonteCarloSpice>
   MC run using BSK Sim Framework  <MonteCarloExamples/scenario_AttFeedbackMC>
   MC rerun using prior MC run parameters  <MonteCarloExamples/scenarioRerunMonteCarlo>
   MC Plotting using datashaders and bokeh  <MonteCarloExamples/scenarioAnalyzeMonteCarlo>



Spacecraft Formation Flying
---------------------------

Formation Flying Dynamics
^^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Two-Spacecraft Formation using BskSim  <BskSim/scenarios/scenario_BasicOrbitFormation>
   Basic Servicer/Debris Simulation <scenarioFormationBasic>


Formation Flying Control
^^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Relative Pointing Control  <BskSim/scenarios/scenario_RelativePointingFormation>
   Mean orbit element based relative motion control <scenarioFormationMeanOEFeedback>
   Impulsive feedback control of relative motion <scenarioFormationReconfig>



Advanced Simulation Options
---------------------------

.. toctree::
   :maxdepth: 1

   Setting Type of Integrator <scenarioIntegrators>
   Using a Python BSK Module <scenarioAttitudePythonPD>
   Changing the bskLog Verbosity from Python <scenarioBskLog>



Advanced Visualizations
-----------------------

Live Plotting
^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Regular Basilisk simulation using Live Plotting  <scenarioBasicOrbitLivePlot>
   bskSim Basilisk simulation using Live Plotting <BskSim/scenarios/scenario_BasicOrbit_LivePlot>


Interfacing with Vizard
^^^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Live Streaming to Vizard  <scenarioBasicOrbitStream>
   Pointing a Vizard Camera  <scenarioVizPoint>
   Convert Simulation Data file to Vizard File <scenarioDataToViz>




