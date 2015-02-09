set (BUILD_TYPE "SHARED" CACHE STRING "The build type of the plugin manager library")

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
# source files and library definition
###############################################################################

file (GLOB lightship_util_HEADERS "include/plugin_manager/*.h")
file (GLOB lightship_util_SOURCES "src/*.c")

set (lightship_util_HEADERS ${lightship_util_HEADERS}
    "${CMAKE_CURRENT_LIST_DIR}/include/plugin_manager/config.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/include/plugin_manager/config.h"
)

include_directories ("include")
include_directories ("${CMAKE_SOURCE_DIR}/util/include")

add_library (plugin_manager ${BUILD_TYPE}
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)

###############################################################################
# dynamic linker
###############################################################################

if (${PLATFORM} MATCHES "LINUX")
    set (DYNAMIC_LINKER_LIBRARY "dl")
elseif (${PLATFORM} MATCHES "MAC")
    set (DYNAMIC_LINKER_LIBRARY "dl")
endif ()

target_link_libraries (plugin_manager
    ${DYNAMIC_LINKER_LIBRARY}
)

###############################################################################
# generate config.h for this library
###############################################################################

set (PROJECT_NAME "PLUGIN_MANAGER")
set (BUILTIN_NAMESPACE_NAME "builtin" CACHE STRING "This is the namespace under which built-in events and services are registered")

message (STATUS "Built-in namespace: ${BUILTIN_NAMESPACE_NAME}")

configure_file ("${CMAKE_SOURCE_DIR}/common/include/common/export.h.in"
                "${CMAKE_CURRENT_LIST_DIR}/include/plugin_manager/export.h")
configure_file ("${CMAKE_CURRENT_LIST_DIR}/include/plugin_manager/config.h.in"
                "${CMAKE_CURRENT_LIST_DIR}/include/plugin_manager/config.h")

file (WRITE "include/plugin_manager/.gitignore"
    "*\n"
    "export.h\n"
    "config.h\n")

###############################################################################
# install targets
###############################################################################

# only install if shared library
if (${BUILD_TYPE} STREQUAL "SHARED")
    install (
        TARGETS
            plugin_manager
        LIBRARY DESTINATION
            "lib"
        RUNTIME DESTINATION
            "bin"
    )
endif ()