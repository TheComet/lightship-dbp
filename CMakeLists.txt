cmake_minimum_required (VERSION 2.6)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (DEFINED CMAKE_BUILD_TYPE)
    if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
        set (CMAKE_BUILD_TYPE Debug)
    endif ()
    set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
    set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()

###############################################################################
# set output locations
###############################################################################
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if (MSVC)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else ()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif ()

###############################################################################
# compiler definitions and flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-pedantic)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-pedantic)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(
        -D_DEBUG
        -DDEBUG
    )
endif ()

###############################################################################
# generate missing folders and files
###############################################################################

file (MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
# ignore auto generated file config.h
file (WRITE "${CMAKE_SOURCE_DIR}/util/include/util/.gitignore" "config.h")

###############################################################################
# Dependency settings
###############################################################################

include_directories ("lightship/include")
include_directories ("util/include")

add_subdirectory ("lightship")
add_subdirectory ("plugins")
add_subdirectory ("util")