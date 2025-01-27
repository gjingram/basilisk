/*
 ISC License

 Copyright (c) 2020, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

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


%module ReactionWheelPower
%{
    #include "ReactionWheelPower.h"
%}

%include "swig_common_model.i"
%include "sys_model.h"
%include "../_GeneralModuleFiles/powerNodeBase.h"
%include "ReactionWheelPower.h"

%include "../../simMessages/powerNodeUsageSimMsg.h"
%include "../../simFswInterfaceMessages/deviceStatusIntMsg.h"
%include "../../simMessages/rwConfigLogSimMsg.h"

GEN_SIZEOF(PowerNodeUsageSimMsg)
GEN_SIZEOF(DeviceStatusIntMsg)
GEN_SIZEOF(RWConfigLogSimMsg)

%pythoncode %{
import sys
protectAllClasses(sys.modules[__name__])
%}
