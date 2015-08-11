#Import some architectural stuff that we will probably always use
import sys, os
sys.path.append(os.environ['SIMULATION_BASE']+'/modules')
sys.path.append(os.environ['SIMULATION_BASE']+'/PythonModules/')
#Simulation base class is needed because we inherit from it
import SimulationBaseClass

#import regular python objects that we need
import math
import csv
import copy
#Vehicle dynamics and avionics models
import spice_interface
import sys_model_thread
import sim_model
import six_dof_eom
import orb_elem_convert
import thruster_dynamics
import coarse_sun_sensor
import imu_sensor
#FSW algorithms that we want to call
import cssComm
import alg_contain
import vehicleConfigData
import cssWlsEst
import sunSafePoint
import imuComm
import sunSafeControl

class EMMSim(SimulationBaseClass.SimBaseClass):
 def __init__(self):
   #Create a sim module as an empty container
   SimulationBaseClass.SimBaseClass.__init__(self)
   self.CreateNewThread("DynamicsThread", int(1E7))
   self.CreateNewThread("FSWThread", int(1E8))
   self.LocalConfigData = vehicleConfigData.vehicleConfigData()
   self.SpiceObject = spice_interface.SpiceInterface()
   self.InitCSSHeads()
   #Schedule the first pyramid on the simulated sensor thread
   self.IMUSensor = imu_sensor.ImuSensor()
   self.ACSThrusterDynObject = thruster_dynamics.ThrusterDynamics()
   self.VehDynObject = six_dof_eom.SixDofEOM()
   self.VehOrbElemObject = orb_elem_convert.OrbElemConvert()
   self.InitAllDynObjects()
   self.AddModelToThread("DynamicsThread", self.SpiceObject)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid1HeadA)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid1HeadB)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid1HeadC)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid1HeadD)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid2HeadA)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid2HeadB)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid2HeadC)
   self.AddModelToThread("DynamicsThread", self.CSSPyramid2HeadD)
   self.AddModelToThread("DynamicsThread", self.IMUSensor)
   self.AddModelToThread("DynamicsThread", self.ACSThrusterDynObject)
   self.AddModelToThread("DynamicsThread", self.VehDynObject)
   self.AddModelToThread("DynamicsThread", self.VehOrbElemObject)

   self.CSSDecodeFSWConfig = cssComm.CSSConfigData()
   self.CSSAlgWrap = alg_contain.AlgContain(self.CSSDecodeFSWConfig, 
      cssComm.Update_cssProcessTelem, cssComm.SelfInit_cssProcessTelem, 
      cssComm.CrossInit_cssProcessTelem)
   self.CSSAlgWrap.ModelTag = "cssSensorDecode"

   self.IMUCommData = imuComm.IMUConfigData()
   self.IMUCommWrap = alg_contain.AlgContain(self.IMUCommData, 
      imuComm.Update_imuProcessTelem, imuComm.SelfInit_imuProcessTelem, 
      imuComm.CrossInit_imuProcessTelem)
   self.IMUCommWrap.ModelTag = "imuSensorDecode"

   self.CSSWlsEstFSWConfig = cssWlsEst.CSSWLSConfig()
   self.CSSWlsWrap = alg_contain.AlgContain(self.CSSWlsEstFSWConfig,
      cssWlsEst.Update_cssWlsEst, cssWlsEst.SelfInit_cssWlsEst, 
      cssWlsEst.CrossInit_cssWlsEst)
   self.CSSWlsWrap.ModelTag = "CSSWlsEst"
   
   self.sunSafePointData = sunSafePoint.sunSafePointConfig()
   self.sunSafePointWrap = alg_contain.AlgContain(self.sunSafePointData,
      sunSafePoint.Update_sunSafePoint, sunSafePoint.SelfInit_sunSafePoint,
      sunSafePoint.CrossInit_sunSafePoint)
   self.sunSafePointWrap.ModelTag = "sunSafePoint"

   self.sunSafeControlData = sunSafeControl.sunSafeControlConfig()
   self.sunSafeControlWrap = alg_contain.AlgContain(self.sunSafeControlData,
      sunSafeControl.Update_sunSafeControl, sunSafeControl.SelfInit_sunSafeControl,
      sunSafeControl.CrossInit_sunSafeControl)
   self.sunSafeControlWrap.ModelTag = "sunSafeControl"

   self.InitAllFSWObjects()

   self.AddModelToThread("FSWThread", self.CSSAlgWrap, self.CSSDecodeFSWConfig)
   self.AddModelToThread("FSWThread", self.IMUCommWrap, self.IMUCommData)
   self.AddModelToThread("FSWThread", self.CSSWlsWrap, self.CSSWlsEstFSWConfig)
   self.AddModelToThread("FSWThread", self.sunSafePointWrap, 
      self.sunSafePointData)
   self.AddModelToThread("FSWThread", self.sunSafeControlWrap, 
      self.sunSafeControlData)

 def SetLocalConfigData(self):
   Tstr2Bdy = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
   SimulationBaseClass.SetCArray(Tstr2Bdy, 'double', self.LocalConfigData.T_str2body)
   self.TotalSim.CreateNewMessage("adcs_config_data", 8*9+4, 2)
   self.TotalSim.WriteMessageData("adcs_config_data", 8*9+4, 0, self.LocalConfigData)
 def SetSpiceObject(self):
   self.SpiceObject.ModelTag = "SpiceInterfaceData"
   self.SpiceObject.SPICEDataPath = os.environ['SIMULATION_BASE'] + '/External/EphemerisData/'
   self.SpiceObject.UTCCalInit = "2015 June 15, 00:00:00.0"
   self.SpiceObject.OutputBufferCount = 2
   self.SpiceObject.PlanetNames = spice_interface.StringVector(["earth", "mars", "sun"])
 def SetIMUSensor(self):
   RotBiasValue = 0.0;
   RotNoiseStdValue = 0.000001;
   TransBiasValue = 0.0
   TransNoiseStdValue = 1.0E-6
   self.IMUSensor = imu_sensor.ImuSensor()
   self.IMUSensor.SensorPosStr = imu_sensor.DoubleVector([1.5, 0.1, 0.1])
   self.IMUSensor.setStructureToPlatformDCM(0.0, 0.0, 0.0)
   SimulationBaseClass.SetCArray([RotBiasValue, RotBiasValue, RotBiasValue], 
      'double', self.IMUSensor.senRotBias)
   SimulationBaseClass.SetCArray([RotNoiseStdValue, RotNoiseStdValue, RotNoiseStdValue], 
      'double', self.IMUSensor.senRotNoiseStd)
   SimulationBaseClass.SetCArray([TransBiasValue, TransBiasValue, TransBiasValue], 
      'double', self.IMUSensor.senTransBias)
   SimulationBaseClass.SetCArray([TransNoiseStdValue, TransNoiseStdValue, TransNoiseStdValue], 
      'double', self.IMUSensor.senTransNoiseStd)
 def SetACSThrusterDynObject(self):
   self.ACSThrusterDynObject.ModelTag = "ACSThrusterDynamics"
   Thruster1 = thruster_dynamics.ThrusterConfigData()
   Thruster1.ThrusterLocation = thruster_dynamics.DoubleVector([1.125, 0.0, 2.0])
   Thruster1.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      math.cos(45.0*math.pi/180.0), math.sin(45.0*math.pi/180.0), 0.0])
   Thruster1.MaxThrust = 0.9
   Thruster2 = thruster_dynamics.ThrusterConfigData()
   Thruster2.ThrusterLocation = thruster_dynamics.DoubleVector([-1.125, 0.0, 2.0])
   Thruster2.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      -math.cos(45.0*math.pi/180.0), math.sin(45.0*math.pi/180.0), 0.0])
   Thruster2.MaxThrust = 0.9
   Thruster3 = thruster_dynamics.ThrusterConfigData()
   Thruster3.ThrusterLocation = thruster_dynamics.DoubleVector([-1.125, 0.0, 2.0])
   Thruster3.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      -math.cos(45.0*math.pi/180.0), -math.sin(45.0*math.pi/180.0), 0.0])
   Thruster3.MaxThrust = 0.9
   Thruster4 = thruster_dynamics.ThrusterConfigData()
   Thruster4.ThrusterLocation = thruster_dynamics.DoubleVector([1.125, 0.0, 2.0])
   Thruster4.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      math.cos(45.0*math.pi/180.0), -math.sin(45.0*math.pi/180.0), 0.0])
   Thruster4.MaxThrust = 0.9
   Thruster5 = thruster_dynamics.ThrusterConfigData()
   Thruster5.ThrusterLocation = thruster_dynamics.DoubleVector([1.125, 0.0, 0.0])
   Thruster5.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      math.cos(45.0*math.pi/180.0), math.sin(45.0*math.pi/180.0), 0.0])
   Thruster5.MaxThrust = 0.9
   Thruster6 = thruster_dynamics.ThrusterConfigData()
   Thruster6.ThrusterLocation = thruster_dynamics.DoubleVector([-1.125, 0.0, 0.0])
   Thruster6.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      -math.cos(45.0*math.pi/180.0), math.sin(45.0*math.pi/180.0), 0.0])
   Thruster6.MaxThrust = 0.9
   Thruster7 = thruster_dynamics.ThrusterConfigData()
   Thruster7.ThrusterLocation = thruster_dynamics.DoubleVector([-1.125, 0.0, 0.0])
   Thruster7.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      -math.cos(45.0*math.pi/180.0), -math.sin(45.0*math.pi/180.0), 0.0])
   Thruster7.MaxThrust = 0.9
   Thruster8 = thruster_dynamics.ThrusterConfigData()
   Thruster8.ThrusterLocation = thruster_dynamics.DoubleVector([1.125, 0.0, 0.0])
   Thruster8.ThrusterDirection = thruster_dynamics.DoubleVector([ 
      math.cos(45.0*math.pi/180.0), -math.sin(45.0*math.pi/180.0), 0.0])
   Thruster8.MaxThrust = 0.9
   self.ACSThrusterDynObject.ThrusterData = \
      thruster_dynamics.ThrusterConfigVector([Thruster1, Thruster2, Thruster3, 
         Thruster4, Thruster5, Thruster6, Thruster7, Thruster8])
 def InitCSSHeads(self):
   #Note the re-use between different instances of the modules.  
   #Handy but not required.
   CSSNoiseStd = 0.001 #Standard deviation of white noise
   CSSNoiseBias = 0.0 #Constant bias
   CSSscaleFactor = 500.0E-6 #Scale factor (500 mu-amps) for max measurement
   CSSFOV = 90.0*math.pi/180.0 #90 degree field of view
   CSSKellyFactor = 0.1 #Used to get the curve shape correct for output

   #Platform 1 is forward, platform 2 is back notionally
   CSSPlatform1YPR = [0.0, 0.0, 0.0]
   CSSPlatform2YPR = [180.0*math.pi/180.0, 0.0, 0.0]

   #Initialize one sensor by hand and then init the rest off of it
   self.CSSPyramid1HeadA = coarse_sun_sensor.CoarseSunSensor()
   self.CSSPyramid1HeadA.ModelTag = "CSSPyramid1HeadA"
   self.CSSPyramid1HeadA.SenBias = CSSNoiseBias
   self.CSSPyramid1HeadA.SenNoiseStd = CSSNoiseStd
   self.CSSPyramid1HeadA.setStructureToPlatformDCM(CSSPlatform1YPR[0],
      CSSPlatform1YPR[1], CSSPlatform1YPR[2])
   self.CSSPyramid1HeadA.scaleFactor = CSSscaleFactor
   self.CSSPyramid1HeadA.fov = CSSFOV
   self.CSSPyramid1HeadA.KellyFactor = CSSKellyFactor
   self.CSSPyramid1HeadA.OutputDataMsg = "coarse_sun_data_pyramid1_headA"
   self.CSSPyramid1HeadB = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadA)
   self.CSSPyramid1HeadB.ModelTag = "CSSPyramid1HeadB"
   self.CSSPyramid1HeadB.OutputDataMsg = "coarse_sun_data_pyramid1_headB"
   self.CSSPyramid1HeadC = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadA)
   self.CSSPyramid1HeadC.ModelTag = "CSSPyramid1HeadC"
   self.CSSPyramid1HeadC.OutputDataMsg = "coarse_sun_data_pyramid1_headC"
   self.CSSPyramid1HeadD = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadA)
   self.CSSPyramid1HeadD.ModelTag = "CSSPyramid1HeadD"
   self.CSSPyramid1HeadD.OutputDataMsg = "coarse_sun_data_pyramid1_headD"

   #Set up the sun sensor orientation information
   #Maybe we should add the method call to the SelfInit of the CSS module
   self.CSSPyramid1HeadA.theta = 0.0
   self.CSSPyramid1HeadA.phi = 45.0*math.pi/180.0
   self.CSSPyramid1HeadA.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid1HeadB.theta = 45.0*math.pi/180.0
   self.CSSPyramid1HeadB.phi = 0.0
   self.CSSPyramid1HeadB.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid1HeadC.theta = 0.0
   self.CSSPyramid1HeadC.phi = -45.0*math.pi/180.0
   self.CSSPyramid1HeadC.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid1HeadD.theta = -45.0*math.pi/180.0
   self.CSSPyramid1HeadD.phi = 0.0
   self.CSSPyramid1HeadD.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid2HeadA = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadA)
   self.CSSPyramid2HeadA.ModelTag = "CSSPyramid2HeadA"
   self.CSSPyramid2HeadA.OutputDataMsg = "coarse_sun_data_pyramid2_headA"
   self.CSSPyramid2HeadA.setStructureToPlatformDCM(CSSPlatform2YPR[0],
      CSSPlatform2YPR[1], CSSPlatform2YPR[2])
   self.CSSPyramid2HeadA.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid2HeadB = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadB)
   self.CSSPyramid2HeadB.ModelTag = "CSSPyramid2HeadB"
   self.CSSPyramid2HeadB.OutputDataMsg = "coarse_sun_data_pyramid2_headB"
   self.CSSPyramid2HeadB.setStructureToPlatformDCM(CSSPlatform2YPR[0],
      CSSPlatform2YPR[1], CSSPlatform2YPR[2])
   self.CSSPyramid2HeadB.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid2HeadC = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadC)
   self.CSSPyramid2HeadC.ModelTag = "CSSPyramid2HeadC"
   self.CSSPyramid2HeadC.OutputDataMsg = "coarse_sun_data_pyramid2_headC"
   self.CSSPyramid2HeadC.setStructureToPlatformDCM(CSSPlatform2YPR[0],
      CSSPlatform2YPR[1], CSSPlatform2YPR[2])
   self.CSSPyramid2HeadC.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

   self.CSSPyramid2HeadD = coarse_sun_sensor.CoarseSunSensor(self.CSSPyramid1HeadD)
   self.CSSPyramid2HeadD.ModelTag = "CSSPyramid2HeadD"
   self.CSSPyramid2HeadD.OutputDataMsg = "coarse_sun_data_pyramid2_headD"
   self.CSSPyramid2HeadD.setStructureToPlatformDCM(CSSPlatform2YPR[0],
      CSSPlatform2YPR[1], CSSPlatform2YPR[2])
   self.CSSPyramid2HeadD.setUnitDirectionVectorWithPerturbation(0.0, 0.0)

 def SetVehDynObject(self):
   self.SunGravBody = six_dof_eom.GravityBodyData()
   self.SunGravBody.BodyMsgName = "sun_planet_data"
   self.SunGravBody.mu = 132712440023.310*1000*1000*1000 #meters!
   self.SunGravBody.IsCentralBody = True
   self.SunGravBody.UseJParams = False

   JParamsSelect = [2, 3, 4, 5, 6]
   EarthGravFile = os.environ['SIMULATION_BASE']+'/External/LocalGravData/GGM03S.txt'
   MarsGravFile = os.environ['SIMULATION_BASE']+'/External/LocalGravData/GGM2BData.txt'
   
   self.EarthGravBody = six_dof_eom.GravityBodyData()
   self.EarthGravBody.BodyMsgName = "earth_planet_data"
   self.EarthGravBody.IsCentralBody = False
   self.EarthGravBody.UseJParams = False
   JParams = LoadGravFromFile(EarthGravFile, self.EarthGravBody, JParamsSelect)
   self.EarthGravBody.JParams = six_dof_eom.DoubleVector(JParams)   

   self.MarsGravBody = six_dof_eom.GravityBodyData()
   self.MarsGravBody.BodyMsgName = "earth_planet_data"
   self.MarsGravBody.IsCentralBody = False
   self.MarsGravBody.UseJParams = False
   JParams = LoadGravFromFile(MarsGravFile, self.MarsGravBody, JParamsSelect)
   self.MarsGravBody.JParams = six_dof_eom.DoubleVector(JParams)   
  
   self.VehDynObject.ModelTag = "VehicleDynamicsData"
   self.VehDynObject.PositionInit = six_dof_eom.DoubleVector([2.342211275644610E+07*1000.0, -1.503236698659483E+08*1000.0, -1.786319594218582E+04*1000.0])
   self.VehDynObject.VelocityInit = six_dof_eom.DoubleVector([2.896852053342327E+01*1000.0,  4.386175246767674E+00*1000.0, -3.469168621992313E-04*1000.0])
   self.VehDynObject.AttitudeInit = six_dof_eom.DoubleVector([0.0, 0.0, 0.0])
   self.VehDynObject.AttRateInit = six_dof_eom.DoubleVector([0.0, 0.0, 0.0])
   self.VehDynObject.MassInit = 1400.0
   self.VehDynObject.InertiaInit = six_dof_eom.DoubleVector([1000, 0.0, 0.0,
                                                             0.0, 1000.0, 0.0,
                                                             0.0, 0.0, 1000.0])
   self.VehDynObject.T_Str2BdyInit = six_dof_eom.DoubleVector([1.0, 0.0, 0.0,
                                                               0.0, 1.0, 0.0,
                                                               0.0, 0.0, 1.0])
   self.VehDynObject.CoMInit = six_dof_eom.DoubleVector([1.0, 0.0, 0.0])
   #Add the three gravity bodies in to the simulation
   self.VehDynObject.AddGravityBody(self.SunGravBody)
   self.VehDynObject.AddGravityBody(self.EarthGravBody)
   self.VehDynObject.AddGravityBody(self.MarsGravBody)
   #Here is where the thruster dynamics are attached/scheduled to the overall 
   #vehicle dynamics.  Anything that is going to impact the dynamics of the vehicle 
   # should be one of these body effectors I think.
   self.VehDynObject.AddBodyEffector(self.ACSThrusterDynObject)

 def SetVehOrbElemObject(self):
   self.VehOrbElemObject.ModelTag = "VehicleOrbitalElements"
   self.VehOrbElemObject.mu = self.SunGravBody.mu

 def SetCSSDecodeFSWConfig(self):
   self.CSSDecodeFSWConfig.NumSensors = 8
   self.CSSDecodeFSWConfig.MaxSensorValue = 500E-6
   self.CSSDecodeFSWConfig.OutputDataName = "css_data_aggregate"
   ChebyList = [-1.734963346951471e+06,   3.294117146099591e+06,
           -2.816333294617512e+06,   2.163709942144332e+06,
           -1.488025993860025e+06,   9.107359382775769e+05,
           -4.919712500291216e+05,   2.318436583511218e+05,
           -9.376105045529010e+04,   3.177536873430168e+04,
           -8.704033370738143e+03,   1.816188108176300e+03,
           -2.581556805090373e+02,   1.888418924282780e+01]
   self.CSSDecodeFSWConfig.ChebyCount = len(ChebyList)
   SimulationBaseClass.SetCArray(ChebyList, 'double', 
      self.CSSDecodeFSWConfig.KellyCheby)
   CurrentName = cssComm.SensorMsgNameCarrier()
   #Arrays of c-strings are hard for SWIG/python.  Required a bit of care.
   SensorListUse = [
      "coarse_sun_data_pyramid1_headA",
      "coarse_sun_data_pyramid1_headB",
      "coarse_sun_data_pyramid1_headC",
      "coarse_sun_data_pyramid1_headD",
      "coarse_sun_data_pyramid2_headA",
      "coarse_sun_data_pyramid2_headB",
      "coarse_sun_data_pyramid2_headC",
      "coarse_sun_data_pyramid2_headD"
      ]

   i=0
   for Name in SensorListUse:
      CurrentName.SensorMsgName = Name
      cssComm.SensorNameArray_setitem(self.CSSDecodeFSWConfig.SensorList, i, 
         CurrentName)
      i += 1

 def SetIMUCommData(self):
   self.IMUCommData.InputDataName = "imu_meas_data"
   self.IMUCommData.InputPropsName = "adcs_config_data"
   self.IMUCommData.OutputDataName = "parsed_imu_data"
   platform2str = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
   SimulationBaseClass.SetCArray(platform2str, 'double', 
      self.IMUCommData.platform2StrDCM)

 def SetCSSWlsEstFSWConfig(self):
   self.CSSWlsEstFSWConfig.InputDataName = "css_data_aggregate"
   self.CSSWlsEstFSWConfig.OutputDataName = "css_wls_est"
   self.CSSWlsEstFSWConfig.UseWeights = True
   self.CSSWlsEstFSWConfig.SensorUseThresh = 0.1

   CSSConfigElement = cssWlsEst.SingleCSSConfig()
   CSSConfigElement.CBias = 1.0
   CSSConfigElement.cssNoiseStd = 0.2
   CSSOrientationList = [
      [0.70710678118654757, 0, 0.70710678118654746],
      [0.70710678118654757, 0.70710678118654746, 0],
      [0.70710678118654757, 0, -0.70710678118654746],
      [0.70710678118654757, -0.70710678118654746, 0],
      [-0.70710678118654757, 0.0, 0.70710678118654746],
      [-0.70710678118654768, -0.70710678118654735, 0],
      [-0.70710678118654757, 0.0, -0.70710678118654746],
      [-0.70710678118654746, 0.70710678118654757, 0]
   ]
   i=0
   for CSSHat in CSSOrientationList:
      SimulationBaseClass.SetCArray(CSSHat, 'double', CSSConfigElement.nHatBdy) 
      cssWlsEst.CSSWlsConfigArray_setitem(self.CSSWlsEstFSWConfig.CSSData, i,
         CSSConfigElement)
      i += 1

 def SetsunSafePoint(self):
   self.sunSafePointData.outputDataName = "sun_safe_att_err"
   self.sunSafePointData.inputSunVecName = "css_wls_est"
   self.sunSafePointData.inputIMUDataName = "parsed_imu_data"
   self.sunSafePointData.minUnitMag = 0.95
   SimulationBaseClass.SetCArray([0.0, 0.0, 1.0], 'double', 
      self.sunSafePointData.sHatBdyCmd)

 def SetsunSafeControl(self):
   self.sunSafeControlData.K = 0.0
   self.sunSafeControlData.P = 100.0
   self.sunSafeControlData.inputGuidName = "sun_safe_att_err"
   self.sunSafeControlData.outputDataName = "sun_safe_control_request"

 def InitAllDynObjects(self):
   self.SetLocalConfigData()
   self.SetSpiceObject()
   self.SetIMUSensor()
   self.SetACSThrusterDynObject()
   self.SetVehDynObject()
   self.SetVehOrbElemObject()

 def InitAllFSWObjects(self):
   self.SetCSSDecodeFSWConfig()
   self.SetIMUCommData()
   self.SetCSSWlsEstFSWConfig()
   self.SetsunSafePoint()
   self.SetsunSafeControl()

def LoadGravFromFile(FileName, GravBody, JParamsSelect):
   csvfile = open(FileName, 'rb')
   csvreader = csv.reader(csvfile)
   FirstLine = True
   NextJindex = 0
   AllJParams = []
   for row in csvreader:
      if(FirstLine == True):
         GravBody.mu = float(row[1])
         FirstLine = False
      elif(int(row[0]) == JParamsSelect[NextJindex]):
         LocalJParam = -math.sqrt(2*JParamsSelect[NextJindex]+1)*float(row[2])
         AllJParams.append(LocalJParam)
         NextJindex += 1
         if(NextJindex >= len(JParamsSelect)):
            break
   return(AllJParams)