/*
 ISC License

 Copyright (c) 2016-2018, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

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
%module relativeODuKF
%{
   #include "relativeODuKF.h"
   #include "../_GeneralModuleFiles/ukfUtilities.h"
%}

%include "swig_conly_data.i"
%constant void Update_relODuKF(void*, uint64_t, uint64_t);
%ignore Update_relODuKF;
%constant void SelfInit_relODuKF(void*, uint64_t);
%ignore SelfInit_relODuKF;
%constant void CrossInit_relODuKF(void*, uint64_t);
%ignore CrossInit_relODuKF;
%constant void Reset_relODuKF(void*, uint64_t, uint64_t);
%ignore Reset_relODuKF;
STRUCTASLIST(OpNavMeas);
GEN_SIZEOF(NavTransIntMsg);
GEN_SIZEOF(OpNavFilterFswMsg);
GEN_SIZEOF(OpNavFswMsg);
GEN_SIZEOF(RelODuKFConfig);
%include "relativeODuKF.h"
%include "../_GeneralModuleFiles/ukfUtilities.h"
%include "../../fswMessages/opNavFswMsg.h"
%include "../../fswMessages/opNavFilterFswMsg.h"
%include "simFswInterfaceMessages/navTransIntMsg.h"


%pythoncode %{
import sys
protectAllClasses(sys.modules[__name__])
%}

