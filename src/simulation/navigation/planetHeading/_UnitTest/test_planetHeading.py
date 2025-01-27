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

import pytest

from Basilisk.simulation.simMessages import SpicePlanetStateSimMsg
from Basilisk.simulation.simMessages import SCPlusStatesSimMsg
from Basilisk.utilities import orbitalMotion as om
from Basilisk.utilities import unitTestSupport
from Basilisk.utilities import SimulationBaseClass
from Basilisk.simulation import planetHeading


def test_planetHeading(show_plots=False, relTol=1e-8):
    """
    **Test Description**

    Test that a planet heading is properly calculated from a spacecraft and planet position and spacecraft attitude.
    To test this, the earth is placed at the inertial origin. A spacecraft with inertial attitude is placed
    at 1AU in the z-direction.  The heading is checked to be [0, 0, -1].
    These values were chosen arbitrarily. They are checked to be accurate to within a relative tolerance of the
    input ``relTol``, 1e-8 by default.

    Args:
        relTol (float): positive, the relative tolerance to which the result is checked.

    **Variables Being Tested**

    This test checks that ``headingOut`` stores the pulled log of the module ``bodyHeadingOutMsg``.

"""
    sim = SimulationBaseClass.SimBaseClass()
    sim.TotalSim.terminateSimulation()
    proc = sim.CreateNewProcess("proc")
    task = sim.CreateNewTask("task", int(1e9))
    proc.addTask(task)

    earthPositionMessage = SpicePlanetStateSimMsg()
    earthPositionMessage.PositionVector = [0., 0., 0.]
    unitTestSupport.setMessage(sim.TotalSim, proc.Name, "earth_planet_data", earthPositionMessage, "SpicePlanetStateSimMsg")

    scPositionMessage = SCPlusStatesSimMsg()
    scPositionMessage.r_BN_N = [0., 0., om.AU*1000]
    unitTestSupport.setMessage(sim.TotalSim, proc.Name, "inertial_state_output", scPositionMessage, "SCPlusStatesSimMsg")

    ph = planetHeading.PlanetHeading()
    ph.planetPositionInMsgName = "earth_planet_data"
    ph.planetHeadingOutMsgName = "planet_heading"
    sim.AddModelToTask(task.Name, ph)
    sim.TotalSim.logThisMessage("planet_heading")
    sim.InitializeSimulationAndDiscover()
    sim.TotalSim.SingleStepProcesses()
    headingOut = sim.pullMessageLogData("planet_heading.rHat_XB_B", list(range(3)))[0][1:]

    assert headingOut == pytest.approx([0., 0., -1.], rel=relTol)


if __name__ == "__main__":
    test_planetHeading()
