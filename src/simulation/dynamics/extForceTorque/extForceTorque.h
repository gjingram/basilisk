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

#ifndef EXT_FORCE_TORQUE_H
#define EXT_FORCE_TORQUE_H

#include "_GeneralModuleFiles/sys_model.h"
#include "../_GeneralModuleFiles/dynamicEffector.h"
#include "../../simFswInterfaceMessages/cmdTorqueBodyIntMsg.h"
#include "../../simFswInterfaceMessages/cmdForceBodyIntMsg.h"
#include "../../simFswInterfaceMessages/cmdForceInertialIntMsg.h"
#include "utilities/bskLogging.h"



/*! @brief external force and torque dynamic efector class */
class ExtForceTorque: public SysModel, public DynamicEffector{
public:
    ExtForceTorque();
    ~ExtForceTorque();

    void SelfInit();
    void CrossInit();
    void UpdateState(uint64_t CurrentSimNanos);         //!< class method
    void linkInStates(DynParamManager& statesIn);       //!< class method
    void writeOutputMessages(uint64_t currentClock);    //!< class method
    void readInputMessages();
    void computeForceTorque(double integTime);

private:
    int64_t cmdTorqueInMsgID;           //!< -- Message ID for incoming data
    int64_t cmdForceInertialInMsgID;    //!< -- Message ID for incoming data
    int64_t cmdForceBodyInMsgID;        //!< -- Message ID for incoming data
    CmdTorqueBodyIntMsg incomingCmdTorqueBuffer;            //!< -- One-time allocation for savings
    CmdForceInertialIntMsg incomingCmdForceInertialBuffer;  //!< -- One-time allocation for savings
    CmdForceBodyIntMsg incomingCmdForceBodyBuffer;          //!< -- One-time allocation for savings
    bool goodTorqueCmdMsg;              //!< -- flag indicating if a torque command message was read
    bool goodForceBCmdMsg;              //!< -- flag indicating if a inertial force command message was read
    bool goodForceNCmdMsg;              //!< -- flag indicating if a body-relative force command message was read


public:
    Eigen::Vector3d extForce_N;         //!< [N]  external force in inertial  frame components
    Eigen::Vector3d extForce_B;         //!< [N]  external force in body frame components
    Eigen::Vector3d extTorquePntB_B;    //!< [Nm] external torque in body frame components
    std::string cmdTorqueInMsgName;     //!< -- message used to read torque command inputs
    std::string cmdForceInertialInMsgName; //!< -- message used to read force command inputs
    std::string cmdForceBodyInMsgName;  //!< -- message used to read force command inputs
    BSKLogger bskLogger;                      //!< -- BSK Logging

};


#endif
