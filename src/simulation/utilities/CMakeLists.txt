
add_library(
        basilisk_logging OBJECT
        bsk_logging.cpp
        bsk_logging.h
        bsk_print.h
)

add_library(
        basilisk_util OBJECT
        astro_constants.h
        avs_eigen_support.cpp
        avs_eigen_support.h
        discretize.cpp
        discretize.h
        gauss_markov.cpp
        gauss_markov.h
        geodetic_conversion.cpp
        geodetic_conversion.h
        linear_algebra.c
        linear_algebra.h
        macros.h
        orbital_motion.c
        orbital_motion.h
        rigid_body_kinematics.c
        rigid_body_kinematics.h
        saturate.cpp
        saturate.h
        signal_condition.c
        signal_condition.h
        sim_definitions.h
)
target_link_libraries(basilisk_util PUBLIC Eigen3::Eigen)

add_library(
        bsk_util SHARED
        $<TARGET_OBJECTS:basilisk_logging>
        $<TARGET_OBJECTS:basilisk_util>
)
target_link_libraries(bsk_util PUBLIC Eigen3::Eigen)

add_subdirectory(test)