
 # ISC License
 #
 # Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder
 #
 # Permission to use, copy, modify, and/or distribute this software for any
 # purpose with or without fee is hereby granted, provided that the above
 # copyright notice and this permission notice appear in all copies.
 #
 # THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 # WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 # MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 # ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 # WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 # ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 # OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.




import pytest
import os, inspect
import numpy as np
import math

filename = inspect.getframeinfo(inspect.currentframe()).filename
path = os.path.dirname(os.path.abspath(filename))
bskName = 'Basilisk'
splitPath = path.split(bskName)

# Import all of the modules that we are going to be called in this simulation
from Basilisk.utilities import SimulationBaseClass
from Basilisk.utilities import unitTestSupport                  # general support file with common unit test functions
from Basilisk.simulation import simpleStorageUnit
from Basilisk.simulation import simMessages
from Basilisk.simulation import simFswInterfaceMessages
from Basilisk.utilities import macros

def test_module(show_plots):
    # each test method requires a single assert method to be called
    [testResults, testMessage] = test_storage_limits(show_plots)
    assert testResults < 1, testMessage


def test_storage_limits(show_plots):
    """
    Tests:
    1. Whether the simpleStorageUnit can add multiple nodes (core base class functionality);
    2. That the simpleStorageUnit correctly evaluates how much stored data it should have given a pair of 1200 baud input messages.
    :param show_plots: Not used; no plots to be shown.
    :return:
    """

    testFailCount = 0                       # zero unit test result counter
    testMessages = []                       # create empty array to store test log messages
    unitTaskName = "unitTask"               # arbitrary name (don't change)
    unitProcessName = "TestProcess"         # arbitrary name (don't change)

    # Create a sim module as an empty container
    unitTestSim = SimulationBaseClass.SimBaseClass()

    # Create test thread
    testProcessRate = macros.sec2nano(0.1)     # update process rate update time
    testProc = unitTestSim.CreateNewProcess(unitProcessName)
    testProc.addTask(unitTestSim.CreateNewTask(unitTaskName, testProcessRate))

    test_storage_unit = simpleStorageUnit.SimpleStorageUnit()
    test_storage_unit.storageCapacity = 2400. # bit capacity.

    dataMsg1 = simMessages.DataNodeUsageSimMsg()
    dataMsg1.baudRate = 1200. # baud
    dataMsg1.dataName = "node_1_msg"
    dataMsg2 = simMessages.DataNodeUsageSimMsg()
    dataMsg2.baudRate = 1200. # baud
    dataMsg2.dataName = "node_2_msg"

    unitTestSupport.setMessage(unitTestSim.TotalSim,
                               unitProcessName,
                               "node_1_msg",
                               dataMsg1)

    unitTestSupport.setMessage(unitTestSim.TotalSim,
                               unitProcessName,
                               "node_2_msg",
                               dataMsg2)

    #Test the addNodeToStorage method:
    test_storage_unit.addDataNodeToModel("node_1_msg")
    test_storage_unit.addDataNodeToModel("node_2_msg")
    test_storage_unit.storageUnitDataOutMsgName = 'test_storage_unit_status'

    unitTestSim.AddModelToTask(unitTaskName, test_storage_unit)

    logFreq = testProcessRate
    unitTestSim.TotalSim.logThisMessage(test_storage_unit.storageUnitDataOutMsgName,logFreq)

    unitTestSim.InitializeSimulationAndDiscover()
    unitTestSim.ConfigureStopTime(macros.sec2nano(5.0))

    unitTestSim.ExecuteSimulation()

    storedDataLog = unitTestSim.pullMessageLogData(test_storage_unit.storageUnitDataOutMsgName+".storageLevel")
    capacityLog = unitTestSim.pullMessageLogData(test_storage_unit.storageUnitDataOutMsgName+".storageCapacity")
    netBaudLog= unitTestSim.pullMessageLogData(test_storage_unit.storageUnitDataOutMsgName+".currentNetBaud")

    #   Check 1 - is net baud rate equal to 2400.?
    for ind in range(0,len(netBaudLog)):
        currentBaud = netBaudLog[ind,1]
        if currentBaud !=2400.:
            testFailCount +=1
            testMessages.append("FAILED: SimpleStorageUnit did not correctly log the net baud rate.")

    #print(netBaudLog)

    if not unitTestSupport.isDoubleEqualRelative((2400.),storedDataLog[-1,1], 1e-8):
        testFailCount+=1
        testMessages.append("FAILED: SimpleStorageUnit did not track integrated data. Returned "+str(storedDataLog[-1,1])+", expected "+str((2400.)))

    #print(storedDataLog)

    for ind in range(0,len(storedDataLog)):
        if storedDataLog[ind,1] > capacityLog[ind,1]:
            testFailCount +=1
            testMessages.append("FAILED: SimpleStorageUnit's stored data exceeded its capacity.")

        if storedDataLog[ind,1] < 0.:
            testFailCount +=1
            testMessages.append("FAILED: SimpleStorageUnit's stored data was negative.")

    #print(capacityLog)

    return [testFailCount, ''.join(testMessages)]


if __name__ == "__main__":
    print(test_module(False))