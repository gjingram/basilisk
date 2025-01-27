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


#ifndef REACTIONWHEELSTATEEFFECTOR_H
#define REACTIONWHEELSTATEEFFECTOR_H

#include "../_GeneralModuleFiles/stateEffector.h"
#include "../_GeneralModuleFiles/dynParamManager.h"
#include "../_GeneralModuleFiles/dynamicEffector.h"
#include "../_GeneralModuleFiles/dynamicObject.h"
#include <Eigen/Dense>
#include "simFswInterfaceMessages/macroDefinitions.h"
#include "_GeneralModuleFiles/sys_model.h"
#include "simFswInterfaceMessages/rwSpeedIntMsg.h"
#include "../../simMessages/rwCmdSimMsg.h"
#include "../../simMessages/rwConfigSimMsg.h"
#include "../../simMessages/rwConfigLogSimMsg.h"
#include "../../simFswInterfaceMessages/arrayMotorTorqueIntMsg.h"
#include "../../simFswInterfaceMessages/macroDefinitions.h"
#include "utilities/bskLogging.h"

#include "utilities/avsEigenMRP.h"
#include "utilities/avsEigenSupport.h"



/*! @brief reaction wheel state effector class */
class ReactionWheelStateEffector:  public SysModel, public StateEffector {
public:
    ReactionWheelStateEffector();
	~ReactionWheelStateEffector();
	void registerStates(DynParamManager& states);
	void linkInStates(DynParamManager& states);
    void writeOutputStateMessages(uint64_t integTimeNanos);
    void computeDerivatives(double integTime, Eigen::Vector3d rDDot_BN_N, Eigen::Vector3d omegaDot_BN_B, Eigen::Vector3d sigma_BN);
    void updateEffectorMassProps(double integTime);  //!< -- Method for stateEffector to give mass contributions
    void updateContributions(double integTime, BackSubMatrices & backSubContr, Eigen::Vector3d sigma_BN, Eigen::Vector3d omega_BN_B, Eigen::Vector3d g_N);  //!< -- Back-sub contributions
    void updateEnergyMomContributions(double integTime, Eigen::Vector3d & rotAngMomPntCContr_B,
                                              double & rotEnergyContr, Eigen::Vector3d omega_BN_B);  //!< -- Energy and momentum calculations
	void SelfInit();
	void CrossInit();
	void addReactionWheel(RWConfigSimMsg *NewRW) {ReactionWheelData.push_back(*NewRW);}  //!< class method
	void UpdateState(uint64_t CurrentSimNanos);
	void WriteOutputMessages(uint64_t CurrentClock);
	void ReadInputs();
	void ConfigureRWRequests(double CurrentTime);
    
public:
	std::vector<RWConfigSimMsg> ReactionWheelData;  //!< -- RW information2
    Eigen::MatrixXd *g_N;           //!< [m/s^2] Gravitational acceleration in N frame components
	std::string InputCmds;                                      //!< -- message used to read command inputs
	std::string OutputDataString;                               //!< -- port to use for output data
    uint64_t OutputBufferCount;                                 //!< -- Count on number of buffers to output
	std::vector<RWCmdSimMsg> NewRWCmds;                         //!< -- Incoming attitude commands
	RWSpeedIntMsg outputStates;                                 //!< (-) Output data from the reaction wheels
    std::string nameOfReactionWheelOmegasState;                 //!< class variable
    std::string nameOfReactionWheelThetasState;                 //!< class variable
	size_t numRW;                                               //!< number of reaction wheels
	size_t numRWJitter;                                         //!< number of RW with jitter
    BSKLogger bskLogger;                                        //!< -- BSK Logging

private:
	std::vector<std::string> rwOutMsgNames;                     //!< -- vector with the message names of each RW
	std::vector<int64_t> rwOutMsgIds;                          //!< -- vector with the ID of each RW
	int64_t CmdsInMsgID;                                        //!< -- Message ID for incoming data
	int64_t StateOutMsgID;                                      //!< -- Message ID for outgoing data
	ArrayMotorTorqueIntMsg IncomingCmdBuffer;                     //!< -- One-time allocation for savings
	uint64_t prevCommandTime;                                   //!< -- Time for previous valid thruster firing

	StateData *hubSigma;                                        //!< class variable
	StateData *hubOmega;                                        //!< class variable
	StateData *hubVelocity;                                     //!< class variable
	StateData *OmegasState;                                     //!< class variable
	StateData *thetasState;                                     //!< class variable

};


#endif /* STATE_EFFECTOR_H */
