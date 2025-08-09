
"""
filegroup(
    name="drag_dynamic_effector_swig_i",
    srcs=["drag_dynamic_effector.i"],
    visibility=["//visibility:public"],
)

cc_library(
    name="drag_effector",
    srcs=glob(["*.cpp"]),
    hdrs=glob(["*.h"]),
    visibility=["//visibility:public"],
    strip_include_prefix="/src/simulation",
    deps=[
        "//src/simulation/utilities",
        "//src/simulation/framework:bsk_framework",
        "//src/simulation/dynamics/framework:dyn_framework",
        "//src/simulation/sim_messages",
        "//src/simulation/sim_fsw_interface_messages"
    ]
)
"""