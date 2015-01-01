project ("util" C)

###############################################################################
# compiler flags for this library
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# generate config.h for this library
###############################################################################

set (PROJECT_NAME "LIGHTSHIP")
set (BUILD_TYPE "SHARED")
set (BUILTIN_NAMESPACE_NAME "builtin" CACHE STRING "This is the namespace under which builtin events and services are registered")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    option (MEMORY_ENABLE_MEMORY_REPORT "Keeps track of the number of allocations and de-allocations and prints a report when the program shuts down" ON)
    option (MEMORY_ENABLE_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" ON)
else ()
    option (MEMORY_ENABLE_MEMORY_REPORT "Keeps track of the number of allocations and de-allocations and prints a report when the program shuts down" OFF)
    option (MEMORY_ENABLE_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" OFF)
endif ()

option (LOG_ENABLE_TIMESTAMPS "Whether or not to prefix log messages with timestamps" ON)

configure_file ("include/util/config.h.in"
                "include/util/config.h")
execute_process (COMMAND cmake -E copy
    "${CMAKE_BINARY_DIR}/util/include/util/config.h"
    "${CMAKE_SOURCE_DIR}/util/include/util/config.h"
)

###############################################################################
# select platform specific implementation source files
###############################################################################

if (${PLATFORM} MATCHES "LINUX")
    set (PLATFORM_HEADER_DIRS "include/util/platform/linux/*.h")
    set (PLATFORM_SOURCE_DIRS "src/platform/linux/*.c")
elseif (${PLATFORM} MATCHES "MAC")
    set (PLATFORM_HEADER_DIRS "include/util/platform/osx/*.h")
    set (PLATFORM_SOURCE_DIRS "src/platform/osx/*.c")
elseif (${PLATFORM} MATCHES "WINDOWS")
    set (PLATFORM_HEADER_DIRS "include/util/platform/win/*.h")
    set (PLATFORM_SOURCE_DIRS "src/platform/win/*.c")
endif ()

###############################################################################
# source files and library definition
###############################################################################

file (GLOB lightship_util_HEADERS "include/util/*.h" ${PLATFORM_HEADER_DIRS})
file (GLOB lightship_util_SOURCES "src/*.c" ${PLATFORM_SOURCE_DIRS})

set (lightship_util_HEADERS ${lightship_util_HEADERS}
    "include/util/config.h.in"
)

add_library (lightship_util ${BUILD_TYPE}
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)

###############################################################################
# install targets
###############################################################################

# only install if shared library
if (${BUILD_TYPE} STREQUAL "SHARED")
    install (
        TARGETS
            lightship_util
        LIBRARY DESTINATION
            "lib"
        RUNTIME DESTINATION
            "bin"
    )
endif ()
