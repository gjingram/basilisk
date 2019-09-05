# Basilisk Release Notes {#bskReleaseNotes}

**Note:**
We are very excited by the great capabilities that this software already has, and plan to be updating this software regularly.  There is some documentation in terms of Doxygen generated HTML documentation, but also documentation within the code, and several Basilisk modules that are math heavy have LaTeX'd documentation folders as well.  Best place to start is to run the integrated tutorial scripts inside the `src/tests/scenarios` folder.  More documentation and tutorial scripts are in development.

## In Progress Features
* ability to integrate custom Basilisk modules that are kept outside of the core Basilisk folder
* GPU based methods to evaluate solar radiation pressure forces and torques
* atmospheric drag evaluation using multi-faceted spacecraft model
* Updates to the Vizard Unity-based Basilisk visualization tool
* Ability to playback BSK simulation file within Vizard
* live plotting capability during a BSK run


## Version 0.9.0
<ul>
    <li>Updated the MD help file on how to compile from the command line environment using a custom configuration of Python.</li>
     <li>Created new optical navigation filter that estimates bias in the measurements. This filter takes in pixel and line data directly.</li>
    <li>Added the ability to specify Vizard settings from Basilisk `vizInterface` module settings.  This way Basilisk simulations can set the desired Vizard settings from within the simulation script. </li>
    <li>Added a new MD help file to discuss the helper methods that setup Vizard features</li>
    <li>Added a python helper function to setup cameraConfigMsg message and create a custom camera view. </li>
    <li>Added the ability to script what starfield Vizard should use.</li>
    <li>Made the Vizard helper check that correct keywords are being used.</li>
    <li>The cmake file now turns ON by default the `USE_PROTOBUFFERS` and `USE_ZMQ` build flag options.  This enables out of the box support for saving BSK data to Vizard binary files.</li>
</ul>

## Version 0.8.1
<ul>
    <li>Added a new kind of dispersion for Monte Carlos which disperses the orbit with classic orbital elements instead of cartesian postion and velocity. </li>
    <li>Added a new module that provides the Earth atmospheric neutral density using the MSIS model.</li>
    <li>Updated the Doxygen HTML documentation layout</li>
</ul>

## Version 0.8.0
<ul>
    <li>ADDED PYTHON 3 SUPPORT!  This is a major step for Basilisk.  Python 2 remains suppored, but is now treated as a depreciated capability.  It is possible to compile BSK for P3 into a `dist3` folder, and for P2 into a `dist` folder at the same time.</li>
    <li>Updated the BSK installation notes to reflect a default installation using Python 3</li>
    <li>Updated all unit test BSK scripts to work in both Python 2 and 3</li>
    <li>Updated all tutorial scripts to work in both Python 3 and 2.  Default instructions are now for Python 3</li>
    <li>Added a new support file with tips on migrating a Python 2 BSK script to function in both Python 3 and 2.  This is called Migrating BSK Scripts to Python 3.</li>
</ul>

## Version 0.7.2
<ul>
    <li>Added a new Earth magnetic field model based on the <a href="https://www.ngdc.noaa.gov/geomag/WMM/DoDWMM.shtml">World Magnetic Model</a> (WMM).  The module has PDF documetnation, and extensive unit test within the source code folder, as well as a tutorial script demonstrating how to run this. </li>
    <li>Updated the `spice_interface` module to be able to read in an epoch message</li>
    <li>Updated scenarios to use the epoch message</li>
    <li>Created a new support macro to convert a general date and time string into an epoch message</li>
    <li>updated the `VizInterface` module to now provide the reaction wheel and thruster states to Vizard</li>
    <li>Cleaned up `VizInterface` to only subscribe to BSK messages that are already created</li>
    <li>Adjust `simpleNav` to only subscribe to the sun message it is already created</li>
    <li>Update all the tutorial scenario and bskSim simulations to use the updated `vizSupport.enableUnityVisualization` method</li>
    <li>Fixed and cleaned up bugs in heading and opnav UKFs, pixelLineConverter, houghCircles, and vizInterface</li>
    <li>Added validity falg to OpNav messages in order to exclude potential measurements</li>
    <li>Fixed camera orientation given the Unity camera frame definition</li>
    <li>Updated BSK installation instructions to warn about not using swig v4</li>
</ul>


## Version 0.7.1
<ul>
    <li>Added a new plotting utility library to support interactive plotting using datashaders with Python3.</li>
    <li>Fixed a garbage collecting leak in the monte carlo controller to minimize impact on computer memory.</li>
</ul>

## Version 0.7.0
   <ul>
    <li>Added the enableViz method to the bskSim scnearios.</li>
    <li>Added dvGuidance PDF module description</li>
    <li>Added new orbital simulation tutorial on a transfer orbit from Earth to Jupiter using a patched-conic Delta_v</li>
    <li>Added the first image processing FSW module using OpenCV's HoughCirlces.</li>
    <li>Added the a module to convert pixel/line and apparent diameter data from circle-finding algorithm to a OpNav message with relative position and covariance.</li>
    <li>New faceted model for atmospheric drag evaluation</li>
    <li>Updated RW and Thruster Simulation factory classes to use ordered dictionary lists.  This ensures that the devices are used in the order they are added. </li>
    <li>Fixed issue where the Viz would show a custom camera window on startup if playing back a data file from bskSim scenarios.</li>
    <li>Added relative Orbit Determination filter (relativeODuKF) in fswAlgorithms/opticalNavigation. This filter reads measurements treated by the image processing block to estimate spacecraft position and velocity</li>
    <li>Changed the C++ message ID to consitently be of type int64_t, not uint64_t</li>
    <li>Rearchitected how data is retained in BSK monte carlo runs using Pandas.  The python pandas package is now required to run MC runs.</li>
    <li>Updated the CMake to handle both Microsoft Visual Studio 2017 and 2019</li>
    <li>Added a new attitude control scenario that uses a cluster of thrusters to produce the required ADCS control torque.</li>
   </ul>
   
##  Version 0.6.2
<ul>
    <li>
        hot fix that adds back a missing method in sim_model.c/h that causes the `enableViz` support method to not work.
    </li>
    <li>
        updated Viz_Interface module with opNavMode flag. This triggers logic to link Basilisk and Vizard with a TCP connection. This is ground work for closed loop visual navigation capabilities.
    </li>
    <li>
        updated enableUnityViz python function in utilities/vizSupport. It now takes in key word arguments to simplify the user interface. It also reliably saves Vizard files for play back in the same directory as the scenario that calls it. 
    </li>
</ul>
   
##  Version 0.6.1
<ul>
    <li>
      Created a user guide MD file that is included in the BSK Doxygen HTML documentation.    
    </li>
    <li>
      Removed the BOOST library from Basilisk as it is no longer needed.  This makes the BSK repository much leaner.  Note that this removes the capability to communicate with the old Qt-based Visualization that is now defunct and replaced with the new Vizard Visualization.
    </li>
    <li>
      Updated switch unscented kalman filter for sunline estimation with code cleanup and documentation updates.
    </li>
    <li>
        updated `pytest` environment to have markers registered
    </li>
    <li>
        added a PPTX support file that explains the core Basilisk architecture.  HTML documentation is updated to link to this.
    </li>
    <li>
        Creates new simulation module called `planetEphemeris` which creates a planet Spice ephemeris message given a set of classical orbit elements.
    </li>
    <li>
        updated the `thrMomentumDumping` module to read in the `thrMomentumManagement` module output message to determine if a new momentum dumping sequence is required.  
    </li>
    <li>
        updated the hillPoint and velocityPoint scenarios on how to connect a planet ephemeris message.
    </li>
    <li>
        updated `hillPoint` and `velocityPoint` to meet BSK coding guidelines
    </li>
    <li>
        updated BSK_PRINT macro to automatically now add a new line symbol at the end of the message
    </li>
</ul>

##  Version 0.6.0
<ul>
    <li>
        Added a new `vizInterface` module.  This version is able to record a BSK simulation which can then be played back in the BSK Vizard visualization program.  Vizard must be downloaded separately.  To enable this capabilty, see the scenario tutorial files.  `scenariosBasicOrbit.py` discusses how to enable this.  The python support macro `vizSupport.enableUnityVisualization()` is commented out by default.  Further, to compile `vizInterface` the CMake flags `USE_PROTOBUFFERS` and `USE_ZEROMQ` must be turned on.  A new MD FAQ support file discusses the Cmake options.
    </li>
    <li>
        Updated `inertialUKF` module documentation and unit tests.
    </li>
    <li>
        Updated unit test and documentation of `dvAccumulation`.
    </li>
    <li>
        added a small include change to fix BSK compiling on Windows
    </li>
    <li>
        updated unit test and documentation of `sunlineEphem()`
    </li>
    <li>
        updated cmake files to set the policy for CMP0086 required by Cmake 3.14.x and higher
    </li>
    <li>
        updated `thrForceMapping` module after code review with new expansive unit tests and updated PDF documentation
    </li>
</ul>


##  Version 0.5.1
<ul>
    <li>
        updated the `orbitalMotion.c/h` support library to have more robust `rv2elem()` and `elem2rv()` functions.  They now also handle retrograde orbits.  The manner in covering parabolic cases has changed slightly.
    </li>
    <li>
        This module implements and tests a Switch Unscented Kalman Filter in order to estimate the sunline direction.
    </li>
    <li>
    Added documentation to the `dvAccumulation` module and included proper time info in the output message.
    </li>
    <li>
    Providing new support functions to enable the upcoming Vizard Basilisk Visualization.
    </li>
    <li>
        updated the 'oeStateEphem()' module to fit radius at periapses instead of SMA, and have the option to fit true versus mean anomaly angles.  
    </li>
    <li>
        updated 'sunlineSuKF` module which provides a switch Sunline UKF estimation filter.  New documentation and unit tests.
    </li>
    <li>
        updated 'MRP_Steering' module documentation and unit tests
    </li>
    <li>
        updated orbital motion library functions `rv2elem()` and elem2rv()`
    </li>
    <li>
        updated `rateServoFullNonlinear` module documentation and unit tests.
    </li>
</ul>

## Version 0.5.0
<ul>
    <li>
       `attTrackingError` has updated documentation and unit tests.
    </li>
    <li>
        navAggregate module has new documentation and unit tests.
    </li>
    <li>
    small FSW algorithm enhancements to ensure we never divide by zero
    </li>
    <li>
    new unit test for RW-config data
    </li>
    <li> included a new environment abstract class that creates a common interface to space environment modules like atmospheric density, or magnetic fields in the future.  This currently implements the exponential model, but will include other models in the future.  NOTE: this change breaks earlier simulation that used atmospheric drag.  The old `exponentialAtmosphere` model usage must be updated. See the integrated and unit tests for details, as well as the module documentation.  
    </li>
    <li>
    added new documentation on using the new atmosphere module to simulate the atmospheric density and temperature information for a series of spacecraft locations about a planet.
    <li>updated documentation and unit tests of `celestialTwoBodyPoint`</li>
    <li>added a new planetary magnetic field module. Currently it provides centered dipole models for Mercury, Earth, Jupiter, Saturn, Uranus and Neptune.  This will be expanded to provide convenient access to other magnetic field models in the future.
    </li>
    <li>
    updated `eulerRotation()` to remove optional output message and did general code clean-up
    </li>
    <li>
    updated `mrpRotation()`, new PDF documentation, did code cleanup, updated unit tests, removed optional module output that is not needed
    </li>
    <li>updated `MRP_Feedback()`, new PDF documentation,  did code cleanup, updated unit tests to cover all code branches.
    </li>
    <li>
    Added a new tutorial on using the magnetic field model.
    </li>
    <li>
    Updated `mrpMotorTorque()` with code cleanup, updated doxygen comments, PDF documentation and comprehensive unit test.
    </li>
    <li> Added documentation to `thrFiringRemainder` module
    </li>
    <li>Added documentation to `thrFiringSchmitt` module</li>
    <li>Updated documentation of `thrMomentumManagement` module</li>
    <li>Updated documentation of `thrMomentumDumping` module</li>
    <li>Added documentation of `MRP_PD` module</li>
    <li>added a new tutorial on how to use the planetary magnetic field model.</li>
</ul>



## Version 0.4.1
<ul>
    <li>
        cssComm has updated documentation and unit tests.
    </li>
    <li>
        updated Documentation on `rwNullSpace` FSW module
    </li>
    <li>
        updated how the FSW and Simulation modules are displayed with the DOxygen HTML documenation, as well as how the messages are shown.  Now the use can click on the "Modules" tab in the web page to find a cleaner listing of all BSK modules, messages, utilities and architecture documentation. 
    </li>
    <li>
        modified the `cmake` file to allow the build type to be passed in from the command line
    </li>
    <li>
        updated Doxygen documentation on `cssWlsEst()`
    </li>
    <li>
        updated documentation and unit tests of `cssComm()` module
    </li>
</ul>


## Version 0.4.0
<uL>
    <li>
    Integrated the  `conan` package management system.  This requires conan to be installed and configured. See the updated Basilisk installation instructions.  It is simple to add this to a current install.  Further, the CMake GUI application can't be used directly with this implementation if the app is double-clicked.  Either the GUI is launched form a terminal (see macOS installation instructions), or `cmake` is run from the command line (again see your platform specific installation instructions). Using `conan` now enables BSK to be compiled with specific support packages, and will allow us to integrate other packages like OpenCV, Protobuffers, etc. into the near future in a flexible manner.
    </li>
    <li>
    updated install instructions to allow for pytest version 4.0.0 or newer
    </li>
    <li>updated code to remove some depreciated python function call warnings
    </li>
    <li>
    Added a new sun heading module computed exclusively from ephemeris data and spacecraft attitude (sunlineEphem). Documentation and a unit test are included.
    </li>
    <li>
    Added a new scenario that shows how to simulate multiple spacecraft in one simulation instance.
    </li>
    <li>
    Added a spacecraftPointing module that allows a deputy spacecraft to point at a chief spacecraft. Besides that, added a scenario that demonstrates the use of this new module.
    </li>
    <li>
    added the ability to the thrForceMapping FSW module to handle thruster saturation better by scaling the resulting force solution set.
    </li>
    <li>
    Added lots of new unit tests to BSK modules
    </li>
    <li>
    rwNullSpace() module now sets ups module states in reset() instead of crossInit(), and includes new documentation and unit tests
    </li>
</ul>

## Version 0.3.3 
<ul>
    <li>
    Added a new message output with the CSS fit residuals.  This is optional.  If the output message is not set, then this information is not computed.
    </li>
    <li>
    Updated `sunSafePoint()` to allow for a nominal spin rate to be commanded about the sun heading vector.  The unit tests and module documentation is updated accordingly.  
    </li>
    <li>
    Added a new scenario `scenarioAttitudeFeedbackNoEarth.py` which illustrates how to do an attitude only simulation without any gravity bodies present.
    </li>
    <li>
    Updated the macOS Basilisk installation instructions to make them easier to follow, and illustrate how to use the macOS provided Python along with all the Python packages installed in the user Library directory.  This provides for a cleaner and easier to maintain Basilisk installation.
    </li>
    <li>
     Created new switched CSS sun heading estimation algorithms called `Sunline_SuKF` and `Sunline_SEKF`.  These switch between two body frames to avoid singularities, but with direct body rate estimation. Previous filters `Sunline_UKF`, `Sunline_EKF`, and  `OKeefe_EKF` either subtract unobservability or difference sunheading estimate for a rate approximation.
    </li>
    <li>
    Updated the Windows specific install instructions to include explicit steps for setting up and installing Basilisk on machine with a fresh copy of Windows 10.
    </li>
    <li>Added policy statements to the CMake files.  This now silences the warnings that were showing up in CMake 3.12 and 3.13</li>
    <li>Modified CMake to silence the excessive warnings in XCode that `register` class is no depreciated in C++</li>
</ul>

## Version 0.3.2
<ul>
    <li>
    Fixed an issue with the eclipse unit test.
    </li>
    <li>
    updated the installation instructions to warn about an incompatibility between the latest version of `pytest` (version 3.7.1).  Users should use a version of `pytest` that is 3.6.1 or older for now until this issue is resolved.
    </li>
    <li>
    Updated the `.gitignore` file to exclude the `.pytest_cache` folder that pytest generates with the newer versions of this program
    </li>
</ul>

## Version 0.3.1
<ul>
    <li>
    Tutorials added for BSK_Sim architecture. Added the ability to customize the frequency for FSW and/or dynamics modules.
    </li>
    <li>
    Updated the dynamics thruster factor classes.  This streamlines how thrusters can be added to the dynamics.  Also, a new blank thruster object is included in this factory class to allow the user to specify all the desired values.
    </li>
    <li>
    bskSim now adds 8 thrusters to the spacecraft.  These are not used yet, but will be in future bskSim scenarios.
    </li>
    <li>
    Modified how bskSim now includes CSS sensors in the spacecraft dynamics setup
    </li>
    <li>
    Modified the FSW `sunSafePoint()` guidance module to read in the body angular velocity information from standard `NavAttIntMsg`.  This will break any earlier simulation that uses `sunSafePoint()`.
    <ul>
    <li>FIX: update the `sunSafePoint()` input connection to use the current message format.</li>
    </ul>
    </li>
    <li>Fixed an issue with energy not conserving if the fully coupled  VSCMG imbalance model is used. This imbalanced gimbal and wheel version now conserves momentum and energy!</li>
    <li>Added initial draft of VSCMG module documentation</li>
    <li>Added documentation to all the bskSim scenarios inside `src/test/bskSimScenarios`.  The documentation now outlines how the bskSim class can get setup and used to create complex spacecraft behaviors with little code.</li>
</ul>


## Version 0.3.0
<ul>
    <li>
    Updated cssWlsEst() module to also compute a partial angular velocity vector.
    </li>
    <li>
    New FSW Guidance module `mrpRotation()` to perform a constant body rate rotation.  The initial attitude is specified through a MRP set.
    </li>
    <li>
    Enhanced Linux installation instructions
    </li>
    <li>
        Updated the simIncludeThruster to use the same factor class as the RW factory class.  This will break old scripts that use the old method of setting up Thrusters with this helper function.
        <ul>
    <li>FIX: Update the script to use the new factory class.  Examples are seen in `src/simulation/dynamics/Thrusters/_UnitTest/test_thruster_integrated.py`.</li>
    </ul>
    </li>
    <li>
    Updated bskSim to use the RW factory class to setup the simulation RW devices, as well as to use fsw helper functions to setup the RW FSW config messages
    </li>
    <li>
    At supportData/EphermerisData, updated the leap second kernel version to from 0011 to 0012.
    </li>
    <li>
    Added a force and torque calculation method in the stateEffector abstract class, and provided the necessary method calls in spacecraftPlus. This allows for stateEffectors to calculate the force and torque that they are imparting on the rigid body hub. The hingedRigidBodyStateEffector and the linearSpringMassDamper classes provide their implementation of these calculations.
    </li>
    <li>
    Fixed an issue with `extForceTorque` effector where the flag about having a good input message was not being initialized properly.  This caused a rare failure in the unit test.
    </li>
    <li>
    Reaction wheel state effector has an updated friction model that allows the user to implement coulomb, viscous, and static friction.
    </li>
    <li>
    Reaction wheel state effector now has max torque saturation logic in which the wheels can only implement a maximum wheel torque and max wheel speed saturation logic in which if the wheel speed goes over the maximum wheel speed, then the wheel torque is set to zero.
    </li>
    <li>
    A new method called writeOutputStateMessages was added to the stateEffector abstract class which allows for stateEffectors to write their states as messages in the system and the states will always be written out to the system after integration. This fixed an issue with reaction wheels where the commanded torque information needs to be tasked before the spacecraft but the reaction wheel state messages need to be written out after integration.
    </li>
    <li>
    A new dynamics class called `spacecraftDynamics` has been created.  This allow multiple complex spacecraft systems to be either rigidly connected or free-flying.  This allow for example a mother craft to house a daughter craft which has its own RWs, etc, and then release the daughter craft at a specified time.  
    </li>
    <li>
    Cleaned up the gravity effector class variable names, and streamlined the evaluation logic.  The gravity effector documentation has been updated to include information on the the multi-body gravity acceleration is evaluated.
    </li>
    <li>
    Updated the FSW modules `MRP_Feedback`,`MRP_Steering`, `dvAccumulation` and `oeStateEphem` to zero out the output message first in the `Update()` routine.
    </li>
    <li>
    Fixed an issue with the RW factory class and the Stribeck friction model not being turned off by default.
    </li>
    <li>
    added a new bskSim based tutorial scenario that illustrates a sun-pointing control while the spacecraft goes through a planets shadow.
    </li>
</ul>

## Version 0.2.3 (June 12, 2018)
<ul>
    <li>Improved how the `fuelSloshSpringMassDamper` effector class works.  It is now renamed to `LinearSpringMassDamper`.  It can be used to simulate both fuel sloshing, but also structural modes.  If the `LinearSpringMassDamper` is connected to a fuel tank, then it's mass depends on the amount of fuel left. The associated unit test illustrated how to setup this last capability.  The module also contains documentation on the associated math.
    </li>
    <li>A new `SphericalPendulum` effector class has been added.  For rotations a spherical pendulum is a better approximation rotational fuel slosh.  This effector can model rotational fuel slosh if connected to a tank (see unit test again), or it can model a torsional structural mode if not connected to a tank. Associated math documentation is included with the class.
    </li>
    <li>The booleans useTranslation and useRotation have been removed from the `HubEffector()` class. The defaults in hubEffector for mass properties: `mHub = 1`, `IHubPntBc_B = diag`(1), and `r_BcB_B = zeros(3)`, enable us to evaluate the same code no matter if the desire is only to have translational states, only rotational states, or both. This allows for less logic in hubEffector and removes possibility of fringe cases that result in unexpected results from a developer standpoint. The fix for if your python script is not working related to this change:
    <ul>
    <li>FIX: Remove any instances of useTranslation or useRotation defined in the hubEffector class.</li>
    </ul>
    <li>Changed name of the method `computeBodyForceTorque` to `computeForceTorque` in the `dynamicEffector` abstract class and any inheriting classes. This avoids the confusion of thinking that only body frame relative forces can be defined, but in reality this class gives the ability to define both external forces defined in the body frame and the inertial frame.
    </li>
    <li>
    Fixed an issue in `RadiationPressure` where the cannonball model was not computed in the proper frame.  An integrated test has been added, and the unit test is updated.  Note that the `RadiationPressure` model specification has changes slightly.  The default model is still the cannonball model.  To specify another model, the python methods `setUseCannonballModel()` or `setUseFacetedCPUModel()` are used.  Note that these take no argument anymore.
    <ul>
        <li>FIX: remove the argument from `setUseCannonballModel(true)` and use the methods `setUseCannonballModel()` or `setUseFacetedCPUModel()` without any arguments instead.</li>
    </ul>
    </li>
</ul>

## Version 0.2.2 (May 14, 2018)
<ul>
    <li>Fixed a build issues on the Windows platform is Visual Studio 2017 or later is used.</li>
    <li>Unified the Coarse Sun Sensor (CSS) sun heading filtering modules to use the same I/O messages.  All used messages are now in the fswMessage folder.</li>
    <li>Made the CSS sun heading filter messages consistently use the CBias value.  This allows particular sensors to have an individual (known) scaling correction factor.  For example, if the return of one sensor is 10% stronger then that of the other sensors, then CBias is set to 1.10.  Default value is 1.0 assuming all CSS units have the same gain.</li>
    <li>The `src\tests\bskSimScenarios` folder now functions properly with the `bskSim` spacecraft class.</li>
    <li>The tutorial scripts in `src\tests\scenarios` are now simplified to pull out the unit testing functionality.  The unit testing is now down with the `test_XXX.py` scripts inside the `src\tests\testScripts` folder.</li>
    <li>The `bskSim` tutorial files are now tested through pytest as well.  The file `testScripts\bskTestScript.py` calls all the `bskSim` tutorial fails and ensures they run without error.</li>
</ul>

## Version 0.2.1
<ul>
    <li>Added messages for current fuel tank mass, fuel tank mDot, and thruster force and torque</li>
    <li>Changed the linearAlgebra.c/h support library to avoid using any dynamic memory allocation.</li>
    <li>Added some new function to linearAlgebra.c/h while making the library use the new BSK_PRINT() function.</li>
    <li>Added ability to simulate noise to the RW devices.</li>
    <li>Created a more complete spacecraft python simulation class called BSKsim, and recreated some BSK tutorial scripts to use BSKsim instead of the more manual spacecraft setup in the earlier scripts.</li>
    <li>Developed general functions to add saturation, discretization and Gauss Markov processes to signals.</li>
    <li>Created a new BSK_PRINT() function.  Here the coder can tag a message as an ERROR, WARNING, DEBUG or INFORMATION status.  The printout can be set to selectively show these print statements. </li>
</ul>

## Version 0.2.0 (First public beta)
<ul>
    <li>First open beta release of Basilisk</li>
    <li>Moved to a new file architecture.  This means older BSK python scripts need to be updated as the method to import BSK has changed.</li>
    <li>The source an now be forked from Bitbucket</li>
    <li>Precompiled binaries are provided through a python pip install wheel file.</li>
    <li>The Doxygen documentation now pulls in the BSK module description PDF file and makes it available via the class definition html page.</li>
    <li>The tutorial python scripts are now moved to `src/test/scenarios`</li>
    <li>The `pytest` common should now be run within the `src` sub-directory</li>
    <li>Updated fuel slosh model documentation</li>
    <li>Updated fuel tank documentation</li>
    <li>Adding noise and corruptions using a new utility to the BSK modules (in progress)</li>
    <li>New N-panel hinged rigid body module</li>
    <li>New 2-panel hinged rigid body module</li>
    <li>Added CSS sun-heading estimation tutorial script</li>
    <li>Added O'Keefe CSS sun-heading estimation module</li>
    </ul>



## Version 0.1.7
<ul>
    <li>New Monte-Carlo capability that uses multiple cores and hyperthreading to accelerate the MC evaluations.  Data is retained and stored for each MC run for robustness.  See `test_scenarioMonteCarloAttRW.py` for an example.</li>
    <li>Coarse Sun Sensor (CSS) modules can now scale the sensor output with the distance from the sun.</li>
    <li>CSS now have updated documentation that includes validation results.</li>
    <li>CSS, IMU have updated means to apply sensor corruptions.</li>
    <li>IMU, simple_nav and star tracker modules have been updated to use now internally Eigen vectors rather than C-Arrays.  <b>NOTE: if you have simulation scripts that use these modules you may have to update the script to set sensor states as Eigen vectors from python.</b></li>
    <li>All the dynamics, thruster and sensor simulation modules have expanded documentation and valdiation unit and integrated tests.  The validation results are automatically included in the module TeX documentation.</li>
</ul>

## Version 0.1.6
<ul>
    <li>new unit tests to validate the multi-body gravity simulation code in `SimCode/dynamics/gravityEffector/_UnitTest/test_gavityDynEffector.py` </li>
    <li>new hinged rigid body tutorial script in `SimScenarios/test_scenarioAttGuideHyperbolic.py`</li>
    <li>new tutorial to do velicity frame pointing on a hyperbolic orbit in `SimScenarios/test_scenarioHingedRigidBody.py` </li>
    <li>fixed various unit test issues that came up on the non-macOS builds</li>
    <li>added reaction wheel effector documentation</li>
    <li>added `orb_elem_convert` documentation</li>
    <li>added `boreAngCalc` documentation</li>

</ul>

## Version 0.1.5
<ul>
    <li>
        Lots of new module documentation which includes a discussion of what is being modeled, the validation tests, as well as a user guide to the module.  The new documentation includes:
        <ul>
            <li>Thruster `DynEffector` module</li>
            <li>ephemeris conversion module</li>
            <li>Coarse Sun Sensor module</li>
            <li>Updated BSK module template documentation</li>            
            <li>Updated documentation for IMU Sensor module</li>            
            <li>Gravity Effector module</li>            
            <li>SimpleNav Sensor module</li>            
            <li>Hinged Panel `StateEffector` module</li>            
        </ul>
    </li>
    <li>New tutorial scripts on
        <ul>
            <li>using CSS modules</li>
            <li>using fuel tank module and the fuel slosh particle `StateEffector`</li>
            <li>How to use `MRP_Steering()` along with the rate tracking sub-servo module</li>
        </ul>
    </li>
    <li>The CSS modules now use the planetary shadow message information to simulated being in a planet's shadow</li>
    <li>SRP DynEffector modules now simulates the impact of being in a planets shadow</li>
    <li>Included a method to validate all the AVS C-Function libraries like `rigidBodyKinematics`, `linearAlgebra` and `orbitalMotion` when the Basilisk `pytest` command is called.  There is also some documentation on using these libraries in `/SimCode/utilitiesSelfCheck/_Documentation` </li>
    <li>Updated the RW and gravitational body (i.e. adding Earth, sun, etc. to the simulation) to use new factory classes.  If you did use the older `simIncludeRW.py` or `simIncludeGravity.py` libraries, you'll need to update your python code to work with the new factory classes.</li>
</ul>


## Version 0.1.4
<ul>
    <li>A planetary eclipse model has been added.  This allows for the shadow of one or multiple planets to be taken into account, including the penumbra region.  This module writes an output message indicating if the spacecraft is in full sun light, partial shadow, or full shadow of the sun.</li>
	   <li>The body-fixed spacecraft structure frame has now been removed from the simulation and flight algorithm codes.  All spacecraft vectors and tensors are now set directly in the body frame B.  If the spacecraft parameters are given in terms of an alternate structure frame, these vectors and tensor must be transformed into the body frame first before being set in BSK.</li>
	   <li>The integrated tutorial test for using a Python based BSK module now has some documentation.</li>
	   <li>Created a method to compute the orbital potential and angular momentum energy.  This allows for the kinetic energy and angular momentum checks to flat-line even if the satellite is in orbit.  The spherical harmonics of the planet are taken into account as well.</li>
	   <li>Included a new Extended Kalman Filter module that determines the body-relative sun heading using the CSS signals.</li>
</ul>

## Version 0.1.3
<ul>
	   <li>There is a new capability to now write BSK modules in Python, and integrated them directly with the C and C++ BSK modules.  Documentation is still in progress, but a sample is found in <code>SimScenarios/test_scenarioAttitudePythonPD.py</code>.</li>
	   <li>A new Variable Speed Control Moment Gyroscope (VSCMG) state effector module has been created.  This module provides a torque-level VSCMG simulation which also includes the gyro frame or wheel being imbalanced.  If the latter modes are engaged, the simulation does slow down noticeably, but you get the full physics.</li>
	   <li>In the simulation the initial spacecraft position and velocity states are now specified now using the spacecraft center of mass location C, not the body fixed point B.  This greatly simplifies the simulation setup.  Upon initialization, the sim determines what the true center of mass of the spacecraft is using all time varying mass components, and sets the proper B point position and velocity vectors.</li>
	   <li>Specifying the initial spacecraft position and velocity states can now be done anywhere before the BSK initialization.  The user sets init versions of the position and velocity vectors.  The setState() method on the state engine thus doesn't have to be used. </li>
	   <li>There is a new <code>initializeSimulationAndDiscover</code> method to init the BSK simulation that automatically checks if messages are shared across multiple simulation threads.  See the modified <code> SimScenarios/test_scenarioAttitudeFeedback2T.py</code> file for how this simplifies the dual-threaded setup.</li>
	   <li>The <code>MRP_Steering</code> and <code>PRV_Steering</code> FSW modules have been broken up into a separate kinematic steering command (commanded desired angular velocity vector) and an associated angular velocity servo module name <code>rateServoFullNonlinear</code>.  This will break any existing code that used either of these two attitude steering modules.  The Python simulation code must be updated to to account for these new modules as done in the MRP_Steering integrated test <code>test_MRP_steeringInt.py</code>.</li>    

</ul>

## Version 0.1.2
<ul>
	   <li>All unit and integrated tests now pass on Linux.  The root issue was a variable length string variable in an output message.  These strings have now been removed as they are no longer needed.</li>
	   <li>The position and velocity of the center of mass of the spacecraft was added to the messaging system, so now the spacecraft’s translational states can be logged by the center of mass of the spacecraft (r_CN_N and v_CN_N) or the origin of the body frame which is fixed to the hub (r_BN_N and v_BN_N). Additionally, the mass properties of the spacecraft was organized into an updateSCMassProps method that incapsulates the calculations of mass property calculations.</li>
	   <li>Updated UKF FSW module to be able to run on gryo only information when the star tracker is not available.</li>
</ul>

## Version 0.1.1
<ul>
	   <li>On Linux, simplified the processing running BSK modules that require boost.  This makes the Viz related communication modules working again.</li>
	   <ul>
	       <li>Added boost libs built on Ubunutu against gcc 5.4.0 20160609.</li>
	       <li>Added RPATH settings to allow for build directory to be placed outside source directory</li>
	   </ul>
	   <li>Major addition with new depleatable mass dynamic modeling, including some fuel tank dynamic models.</li>
	   <li>minor fix for Monte Carlo dispersions</li>
</ul>


## Version 0.1.0
### Simulation modules include:
<ul>
<li>Flexible integration structure with fixed time step RK1, RK2 and RK4 included</li>
	   <li>Rigid spacecraft simulated through <code>spacecratPlus()</code> module.  The spacecraft object makes it simple to add external disturbances through <code>dynEffectors</code> and state depended actuation through <code>stateEffectors</code>.
	   <ul>
    	   <li>Dynamics Effectors (acuation methods which do not have their own states to integrate)</li>
    	       <ul>
    	           <li>External force or torque module</li>
    	           <li>Solar radiation pressure module</li>
    	           <li>Thruster module</li>
    	       </ul>
    	   <li>State Effectors (actuation methods which have states to integrate)</li>
    	       <ul>
    	           <li>Fuel Tank model with fuel slosh particles</li>
    	           <li>Hinged panel model to simulate flexing structures such as solar panels</li>
    	           <li>Reaction wheel module with 3 modes (perfectly balanced, simple jitter with the disturbance modeled as an external force and torque, fully coupled imbalanced RW model)
    	       </ul>
        </ul>
        <li>RW voltage interface module that mapes an input voltage to a RW motor torque</li>
        <li>integrate Spice ephemeris information</li>
        <li>simple navigation module that produces the position and attitude measurement states</li>
        <li>IMU sensor</li>
        <li>Star Tracker module</li>
        <li>Coarse Sun Sensor (CSS) module</li>
        <li>Added the ability to simulate the gravity from multiple celestial objects, as well as include spherical harmonic expansion of a particular celestial body.</li>
</ul>

### The AVS Lab Flight Algorithm folder contains:
<ul>
<li>FSW template module</li>
	   <li>CSS based sun heading estimation module</li>
	   <li>UKF filter to determine inertial attitude</li>
	   <li>UKF fitler to determine CSS based body-relative sun heading</li>
	   <li>Attitude Guidance modules:</li>
	       <ul>
	           <li>Pointing towards two celestial objects</li>
	           <li>Inertial Pointing</li>
	           <li>Hill Frame Pointing</li>
	           <li>Euler rotation sequence to add dynamics capabiliteis to the attitude reference generation</li>
	           <li>Spinning about an inertially fixed axis</li>
	           <li>A raster manager module that can change the guidance module states</li>
	           <li>Velocity frame pointing</li>
	           <li>attitude tracking error evaluation module</li>
	           <li>Deadband module for attitude trackign error</li>
	       </ul>
	   <li>DV guidance module</li>
	   <li>Effector Interfaces</li>
	       <ul>
	           <li>mapping of control torque onto RW motor torques</li>
	           <li>Converting RW motor torques to voltages</li>
	           <li>RW null motion module to equalize the wheel speeds continuously</li>
	           <li>Thruster (THR) firing logic using a Schmitt trigger</li>
	           <li>THR firign logic using a remainder calculation</li>
	           <li>mappign of a command torque onto a set of THR devices</li>
	           <li>module to evalute the net momentum to dump with thrusters</li>
	       </ul>
</ul>