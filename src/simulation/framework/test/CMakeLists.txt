include_directories(..)

file(GLOB test_srcs *.cpp)
add_doctest(
        NAME test_messaging
        SOURCES ${test_srcs}
        OBJDEP basilisk_logging system_messaging
)