include_directories(..)

add_doctest(
        NAME test_linear_algebra
        SOURCES test_linear_algebra.cpp
        OBJDEP basilisk_logging
)