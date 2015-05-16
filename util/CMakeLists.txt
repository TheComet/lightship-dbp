set (BUILD_TYPE "SHARED")

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

set (PROJECT_NAME "LIGHTSHIP_UTIL")

# memory debugging
if (CMAKE_BUILD_TYPE MATCHES Debug OR BUILD_TESTS)
    option (ENABLE_MEMORY_DEBUGGING "Global switch for memory options. Keeps track of the number of allocations and de-allocations and prints a report when the program shuts down" ON)
else ()
    option (ENABLE_MEMORY_DEBUGGING "Global switch for memory options. Keeps track of the number of allocations and de-allocations and prints a report when the program shuts down" OFF)
endif ()
if (ENABLE_MEMORY_DEBUGGING)
    if(${CMAKE_BUILD_TYPE} MATCHES Debug)
        option (ENABLE_MEMORY_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" ON)
    else ()
        option (ENABLE_MEMORY_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" OFF)
    endif ()
    if (BUILD_TESTS)
        option (ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES "Allows calls to malloc() to fail on purpose" ON)
    else ()
        option (ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES "Allows calls to malloc() to fail on purpose" OFF)
    endif ()
else ()
    option (ENABLE_MEMORY_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" OFF)
    option (ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES "Allows calls to malloc() to fail on purpose" OFF)
endif ()

# log related stuff
option (ENABLE_LOG_TIMESTAMPS "Whether or not to prefix log messages with timestamps" ON)

# thread pool and multithreading related stuff
option (ENABLE_MULTITHREADING "Global switch for whether or not to use multithreading" OFF)
if (ENABLE_MULTITHREADING)
    option (ENABLE_THREAD_POOL "Enables the thread pool (scalable multithreading). Requires multithreading to be enabled." OFF)
    option (ENABLE_RING_BUFFER_REALLOC "Causes the thread pool's ring buffer to automatically reallocate when necessary" OFF)
    set (RING_BUFFER_FIXED_SIZE "4096" CACHE STRING "If ENABLE_RING_BUFFER_REALLOC is disabled, this determins the fixed size in bytes of the ring buffer. Note that this is also the start size of the ring buffer")
    set (RING_BUFFER_MAX_SIZE "262144" CACHE STRING "Maximum allowable size of the ring buffer.")
endif ()

# detect invalid settings
if (BUILD_TESTS AND NOT ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES)
    message (FATAL_ERROR "Can't build tests without ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES")
endif ()
if (ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES AND NOT ENABLE_MEMORY_DEBUGGING)
    message (FATAL_ERROR "ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES requires ENABLE_MEMORY_DEBUGGING")
endif ()
if (ENABLE_MEMORY_BACKTRACE AND NOT ENABLE_MEMORY_DEBUGGING)
    message (FATAL_ERROR "ENABLE_MEMORY_BACKTRACE requires ENABLE_MEMORY_DEBUGGING")
endif ()
if (ENABLE_THREAD_POOL AND NOT ENABLE_MULTITHREADING)
    message (FATAL_ERROR "ENABLE_THREAD_POOL requires ENABLE_MULTITHREADING")
endif ()
if (ENABLE_RING_BUFFER_REALLOC AND NOT ENABLE_MULTITHREADING)
    message (FATAL_ERROR "ENABLE_RING_BUFFER_REALLOC requires ENABLE_MULTITHREADING")
endif ()
if (RING_BUFFER_FIXED_SIZE AND NOT ENABLE_MULTITHREADING)
    message (FATAL_ERROR "RING_BUFFER_FIXED_SIZE requires ENABLE_MULTITHREADING")
endif ()
if (RING_BUFFER_MAX_SIZE AND NOT ENABLE_MULTITHREADING)
    message (FATAL_ERROR "RING_BUFFER_MAX_SIZE requires ENABLE_MULTITHREADING")
endif ()

# print out a summary
message (STATUS "------------------------------------------------------------")
message (STATUS "Settings for utility library")
message (STATUS " + Memory debugging: ${ENABLE_MEMORY_DEBUGGING}")
if (ENABLE_MEMORY_DEBUGGING)
    message (STATUS " + Backtraces in memory reports: ${ENABLE_MEMORY_BACKTRACE}")
    message (STATUS " + Explicit malloc() failures: ${ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES}")
    message (STATUS " + Log timestamps: ${ENABLE_LOG_TIMESTAMPS}")
endif ()
message (STATUS " + Multithreading: ${ENABLE_MULTITHREADING}")
if (ENABLE_MULTITHREADING)
    message (STATUS " + Thread pools: ${ENABLE_THREAD_POOL}")
    message (STATUS " + Ring buffer reallocations: ${ENABLE_RING_BUFFER_REALLOC}")
    message (STATUS " + Ring buffer fixed size: ${RING_BUFFER_FIXED_SIZE}")
    message (STATUS " + Ring buffer max size: ${RING_BUFFER_MAX_SIZE}")
endif ()
message (STATUS "------------------------------------------------------------")

configure_file ("${EXPORT_H_TEMPLATE}"
                "include/util/export.h")
configure_file ("include/util/config.h.in"
                "include/util/config.h")

include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# select platform specific implementation source files
###############################################################################

if (${PLATFORM} MATCHES "LINUX")
    set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/util/platform/linux/*.c")
elseif (${PLATFORM} MATCHES "MACOSX")
    set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/util/platform/osx/*.c")
elseif (${PLATFORM} MATCHES "WINDOWS")
    set (PLATFORM_HEADER_DIRS ${PLATFORM_HEADER_DIRS} "include/util/platform/win/*.h")
    set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/util/platform/win/*.c")
endif ()

###############################################################################
# source files and library definition
###############################################################################

file (GLOB lightship_util_HEADERS
    "include/util/*.h"
    ${PLATFORM_HEADER_DIRS})
file (GLOB lightship_util_SOURCES
    "src/util/*.c"
    ${PLATFORM_SOURCE_DIRS})

set (lightship_util_HEADERS ${lightship_util_HEADERS}
    "include/util/config.h.in"
    "${CMAKE_SOURCE_DIR}/util/include/common/export.h.in"
)

include_directories ("include")

add_library (lightship_util ${BUILD_TYPE}
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)

# The library is being built
add_definitions (${DEFINE_CMD}${PROJECT_NAME}_BUILDING)

###############################################################################
# dynamic linker
###############################################################################

if (${PLATFORM} MATCHES "LINUX")
    set (DYNAMIC_LINKER_LIBRARY "dl")
elseif (${PLATFORM} MATCHES "MAC")
    set (DYNAMIC_LINKER_LIBRARY "dl")
endif ()

target_link_libraries (lightship_util
    ${DYNAMIC_LINKER_LIBRARY}
)

###############################################################################
# threads
###############################################################################

if (ENABLE_MULTITHREADING OR ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES)
    if (${PLATFORM} MATCHES "LINUX")
        target_link_libraries(lightship_util pthread)
    endif ()
endif ()

###############################################################################
# libyaml
###############################################################################

add_subdirectory ("ext")

include_directories ("ext/yaml-0.1.5-cmake/include")
target_link_libraries (lightship_util yaml)

# yaml is built statically
add_definitions (-DYAML_DECLARE_STATIC)

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
