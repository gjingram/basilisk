cmake_minimum_required(VERSION 3.22.1)
project(basilisk VERSION 1.8.3)

include(CMakeParseArguments)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(doctest REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

function(add_doctest)
    set(options)
    set(one_value NAME)
    set(multi_value SOURCES OBJDEP LIBDEP)
    cmake_parse_arguments(
            ADD_DOCTEST
            "${options}"
            "${one_value}"
            "${multi_value}"
            ${ARGN}
    )

    if (NOT ADD_DOCTEST_NAME)
        message(FATAL_ERROR "add_doctest: NAME is a required argument")
    endif()

    if (NOT ADD_DOCTEST_SOURCES)
        message(FATAL_ERROR "add_doctest: SOURCES is a required argument")
    endif()

    if (ADD_DOCTEST_SOURCE IN_LIST ADD_DOCTEST_KEYWORD_MISSING_VALUES)
        message(FATAL_ERROR "add_doctest: SOURCES requires at least one value")
    endif()

    set(odeps, "")
    foreach(dep ${ADD_DOCTEST_OBJDEP})
        list(APPEND odeps "$<TARGET_OBJECTS:${dep}>")
    endforeach()

    add_executable(
            ${ADD_DOCTEST_NAME}
            ${ADD_DOCTEST_SOURCES}
            ${CMAKE_SOURCE_DIR}/tools/doctest_main.cpp
            ${odeps}
    )

    foreach(lib IN ${ADD_DOCTEST_LIBDEP})
        target_link_libraries(${ADD_DOCTEST_NAME} PRIVATE ${lib})
    endforeach()
    target_link_libraries(${ADD_DOCTEST_NAME} PRIVATE doctest::doctest)
    add_test(NAME ${ADD_DOCTEST_NAME} COMMAND ${ADD_DOCTEST_NAME})
endfunction()

include_directories(src)
add_subdirectory(src)