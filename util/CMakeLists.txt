set (BUILD_TYPE "SHARED" CACHE STRING "The build type of the utility library")

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

if (CMAKE_BUILD_TYPE MATCHES Debug)
    option (ENABLE_MEMORY_REPORT "Keeps track of the number of allocations and de-allocations and prints a report when the program shuts down" ON)
    option (ENABLE_MEMORY_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" ON)
else ()
    option (ENABLE_MEMORY_REPORT "Keeps track of the number of allocations and de-allocations and prints a report when the program shuts down" OFF)
    option (ENABLE_MEMORY_BACKTRACE "Generates backtraces for every malloc(), making it easy to track down memory leaks" OFF)
endif ()

option (ENABLE_LOG_TIMESTAMPS "Whether or not to prefix log messages with timestamps" ON)
option (ENABLE_MULTITHREADING "Whether or not to use multithreading" ON)
if (ENABLE_MULTITHREADING)
    option (ENABLE_THREAD_POOL "Enables threading pools." ON)
endif ()

message (STATUS "Memory reports: ${ENABLE_MEMORY_REPORT}")
message (STATUS "Backtraces in memory reports: ${ENABLE_MEMORY_BACKTRACE}")
message (STATUS "Log timestamps: ${ENABLE_LOG_TIMESTAMPS}")
message (STATUS "Thread pools: ${ENABLE_THREAD_POOL}")

# disable backtraces on Windows, as those aren't implemented yet
if (WIN32)
    set (ENABLE_MEMORY_BACKTRACE 0)
endif ()

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
    if (ENABLE_THREAD_POOL)
        set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/thread_pool/platform/linux/*.c")
    endif ()
elseif (${PLATFORM} MATCHES "MAC")
    set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/util/platform/osx/*.c")
    if (ENABLE_THREAD_POOL)
        set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/thread_pool/platform/osx/*.c")
    endif ()
elseif (${PLATFORM} MATCHES "WINDOWS")
    set (PLATFORM_HEADER_DIRS ${PLATFORM_HEADER_DIRS} "include/util/platform/win/*.h")
    set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/util/platform/win/*.c")
    if (ENABLE_THREAD_POOL)
        set (PLATFORM_SOURCE_DIRS ${PLATFORM_SOURCE_DIRS} "src/thread_pool/platform/win/*.c")
    endif ()
endif ()

###############################################################################
# source files and library definition
###############################################################################

file (GLOB lightship_util_HEADERS
    "include/util/*.h"
    "include/thread_pool/*.h"
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
# ncurses for colourful terminal output
###############################################################################

find_package (Curses)
if (CURSES_FOUND)
    include_directories (${CURSES_INCLUDE_DIR})
    target_link_libraries (lightship_util ${CURSES_LIBRARIES})
    set (HAVE_CURSES 1)
endif ()

###############################################################################
# threads
###############################################################################

if (ENABLE_THREAD_POOL)
    if (${PLATFORM} MATCHES "LINUX")
        target_link_libraries(lightship_util pthread)
    endif ()
endif ()

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
