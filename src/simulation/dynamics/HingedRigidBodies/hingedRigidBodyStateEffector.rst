
Executive Summary
-----------------

The hinged rigid body class is an instantiation of the state effector abstract class. The integrated test is validating the interaction between the hinged rigid body module and the rigid body hub that it is attached to. In this case, a hinged rigid body has an inertia tensor and is attached to the hub by a single degree of freedom torsional hinged with a linear spring constant and linear damping term.



Message Connection Descriptions
-------------------------------
The following table lists all the module input and output messages.  The module msg variable name is set by the
user from python.  The msg type contains a link to the message structure definition, while the description
provides information on what this message is used for.

.. _ModuleIO_hingedRigidBodyStateEffector:
.. figure:: /../../src/simulation/dynamics/HingedRigidBodies/_Documentation/Images/moduleHRBStateEffector.svg
    :align: center

    Figure 1: ``HingedRigidBodyStateEffector()`` Module I/O Illustration


.. list-table:: Module I/O Messages
    :widths: 25 25 50
    :header-rows: 1

    * - Msg Variable Name
      - Msg Type
      - Description
    * - motorTorqueInMsgName
      - :ref:`ArrayMotorTorqueIntMsg`
      - (Optional) Input message of the hinge motor torque value
    * - hingedRigidBodyOutMsgName
      - :ref:`HingedRigidBodySimMsg`
      - Output message containing the panel hinge state angle and angle rate
    * - hingedRigidBodyConfigLogOutMsgName
      - :ref:`SCPlusStatesSimMsg`
      - Output message containing the panel inertial position and attitude states


Detailed Module Description
---------------------------

Mathematical Modeling
^^^^^^^^^^^^^^^^^^^^^
See
Allard, Schaub, and Piggott paper: `General Hinged Solar Panel Dynamics Approximating First-Order Spacecraft Flexing <http://dx.doi.org/10.2514/1.A34125>`__
for a detailed description of this model. A hinged rigid body has 2 states: theta and thetaDot

The module
:download:`PDF Description </../../src/simulation/dynamics/HingedRigidBodies/_Documentation/Basilisk-HINGEDRIGIDBODYSTATEEFFECTOR-20170703.pdf>`
contains further information on this module's function,
how to run it, as well as testing.

Module Testing
^^^^^^^^^^^^^^
The integrated tests has six scenarios it is testing. The first three are: one with gravity and no damping, one without gravity and without damping, and one without gravity with damping. These first three tests are verifying energy and momentum conservation. In the first two cases orbital energy, orbital momentum, rotational energy, and rotational angular momentum should all be conserved. In the third case orbital momentum, orbital energy, and rotational momentum should be conserved. This integrated test validates for all three scenarios that all of these parameters are conserved. The fourth scenario is verifying that the steady state deflection while a constant force is being applied matches the back of the envelope (BOE) calculation. The fifth scenario applies a constant force and removes the force and the test verifies that the frequency and amplitude match the BOE calculations. And the sixth scenario verifies that Basilisk gives identical results to a planar Lagrangian dynamical system created independently.

The document :download:`PDF Description </../../src/simulation/dynamics/HingedRigidBodies/_Documentation/Basilisk-HINGEDRIGIDBODYSTATEEFFECTOR-20170703.pdf>`
contains a more detailed discussion of the testing, as well as the expected results.

User Guide
----------
This section is to outline the steps needed to setup a Hinged Rigid Body State Effector in python using Basilisk.

#. Import the hingedRigidBodyStateEffector class::

    from Basilisk.simulation import hingedRigidBodyStateEffector

#. Create an instantiation of a Hinged Rigid body::

    panel1 = hingedRigidBodyStateEffector.HingedRigidBodyStateEffector()

#. Define all physical parameters for a Hinged Rigid Body. For example::

    IPntS_S = [[100.0, 0.0, 0.0], [0.0, 50.0, 0.0], [0.0, 0.0, 50.0]]

   Do this for all of the parameters for a Hinged Rigid Body seen in the Hinged Rigid Body 1 Parameters Table.

#. Define the initial conditions of the states::

    panel1.thetaInit = 5*numpy.pi/180.0
    panel1.thetaDotInit = 0.0

#. Define a unique name for each state::

    panel1.nameOfThetaState = "hingedRigidBodyTheta1"
    panel1.nameOfThetaDotState = "hingedRigidBodyThetaDot1"

#. Define an optional motor torque input message::

    panel1.motorTorqueInMsgName = "motorTorque"

#. The angular states of the panel are created using an output message with the name
   ``hingedRigidBodyOutMsgName``.  The default value of this string is ``hingedRigidBody_OutputStates``::

    panel1.hingedRigidBodyOutMsgName = "panel1Angles"

#. If multiple panels are used, update the default config log state message to be unique::

    panel1.hingedRigidBodyConfigLogOutMsgName = "panel1Log"

#. Add the panel to your spacecraftPlus::

    scObject.addStateEffector(panel1)

   See :ref:`spacecraftPlus` documentation on how to set up a spacecraftPlus object.

#. Add the module to the task list::

    unitTestSim.AddModelToTask(unitTaskName, panel1)




