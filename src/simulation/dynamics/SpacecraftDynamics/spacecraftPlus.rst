
Executive Summary
-----------------
This module provides the spacecraft rigid body translational and rotation motion.
This is an instantiation of the :ref:`dynamicObject` abstract class that is a spacecraft with :ref:`stateEffector`'s and
:ref:`dynamicEffector`'s attached to it. The spacecraftPlus allows for both translation and
rotation. :ref:`stateEffector`'s such as RWs, flexible solar panel, fuel slosh etc can be added to the spacecraft by attaching
stateEffectors. :ref:`dynamicEffector`'s such as thrusters, external force and torque, SRP etc can be added to the spacecraft
by attaching dynamicEffectors. This class performs all of this interaction between stateEffectors, dynamicEffectors and
the hub.

The module
:download:`PDF Description </../../src/simulation/dynamics/SpacecraftDynamics/_Documentation/SpacecraftPlus/Basilisk-SPACECRAFTPLUS-20170808.pdf>`
contains further information on this module's function,
how to run it, as well as testing.


Message Connection Descriptions
-------------------------------
The following table lists all the module input and output messages.  The module msg variable name is set by the
user from python.  The msg type contains a link to the message structure definition, while the description
provides information on what this message is used for.

.. _ModuleIO_MRP_PD:
.. figure:: /../../src/simulation/dynamics/SpacecraftDynamics/_Documentation/SpacecraftPlus/Images/moduleSpacecraftPlus.svg
    :align: center

    Figure 1: ``SpacecraftPlus()`` Module I/O Illustration


.. list-table:: Module I/O Messages
    :widths: 25 25 50
    :header-rows: 1

    * - Msg Variable Name
      - Msg Type
      - Description
    * - scStateOutMsgName
      - :ref:`SCPlusStatesSimMsg`
      - Spacecraft state output message
    * - scMassStateOutMsgName
      - :ref:`SCPlusMassPropsSimMsg`
      - Output message containing the spacecraft mass properties
    * - attRefInMsgName
      - :ref:`AttRefFswMsg`
      - (Optional) Input message to specify a prescribed attitude motion

User Guide
----------
This section is to outline the steps needed to setup a SpacecraftPlus module in python using Basilisk.

#.  Import the spacecraftPlus class::

        import spacecraftPlus

#.  Create an instantiation of a spacecraft::

        scObject = spacecraftPlus.SpacecraftPlus()

#.  Define all physical parameters for the hub. For example::

        scObject.hub.IHubPntBc_B = [[100.0, 0.0, 0.0], [0.0, 50.0, 0.0], [0.0, 0.0, 50.0]]

    Do this for all of the parameters for a hub::

        scObject.hub.mHub, scObject.hub.r_BcB_B, scObject.hub.IHubPntBc_B

    seen in the spacecraft Parameters Table. If you only have translation, you only need to specify the mass (if you only have conservative forces acting on the spacecraft then you don't even need to specify a mass). If you only have rotation, you only need to specify the inertia, and if you have both, you need to specify the mass, the inertia and if you have a offset between the center of mass of the spacecraft and point :math:`B`.

#.  Define the initial conditions of the states::

        scObject.hub.r_CN_NInit,  scObject.hub.v_CN_NInit, scObject.hub.sigma_BNInit, scObject.hub.omega_BN_BInit

#.  Finally, add the spacecraft to the task::

        unitTestSim.AddModelToTask(unitTaskName, scObject)

#.  If you want to prescribe the spacecraft hub rotational motion, this can be specified through an optional
    input message of type :ref:`attRefFswMsg`::

        scObject.attRefInMsgName = "attRefMsg"

.. list-table:: Spacecraft Parameters Table
    :widths: 25 25 50
    :header-rows: 1

    * - Variable Name
      - Variable Type
      - Description
    * - r_CN_NInit
      - double[3]
      - Inertial position of S/C
    * - v_CN_NInit
      - double[3]
      - Inertial velocity of S/C
    * - sigma_BNInit
      - double[3]
      - Initial attitude of B frame represented as an MRP
    * - omega_BN_BInit
      - double[3]
      - Initial angular velocity of B frame expressed in B frame
    * - mHub
      - double[1]
      - Hub mass
    * - IHubPntBc_B
      - double[3][3]
      - Inertia in B frame
    * - r_BcB_B
      - double[3]
      - Center of mass location in B frame




















