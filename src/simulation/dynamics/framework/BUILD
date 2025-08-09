
"""
filegroup(
    name="dyn_framework_doc",
    srcs=glob(["*.rst"])
)

cc_library(
    name="dyn_framework",
    srcs=glob(["*.cpp"]),
    hdrs=glob(["*.h"]),
    deps=[
        "//src/simulation/utilities",
        "//src/simulation/framework:bsk_framework",
        "//src/simulation/sim_messages",
        "@eigen"
    ],
    strip_include_prefix="/src/simulation",
    visibility=["//visibility:public"],
)
"""