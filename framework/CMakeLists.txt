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
# generate config.h for this library
###############################################################################

set (PROJECT_NAME "FRAMEWORK")
set (BUILTIN_NAMESPACE_NAME "core" CACHE STRING "This is the namespace under which built-in events and services are registered")

message (STATUS "------------------------------------------------------------")
message (STATUS "Settings for framework")
message (STATUS " + Built-in namespace: ${BUILTIN_NAMESPACE_NAME}")
message (STATUS "------------------------------------------------------------")

configure_file ("${EXPORT_H_TEMPLATE}"
                "include/framework/export.h")
configure_file ("include/framework/config.h.in"
                "include/framework/config.h")

include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

file (GLOB lightship_util_HEADERS "include/framework/*.h")
file (GLOB lightship_util_SOURCES "src/*.c")

set (lightship_util_HEADERS ${lightship_util_HEADERS}
    "include/framework/config.h.in"
    "${CMAKE_SOURCE_DIR}/util/include/common/export.h.in"
)

include_directories ("include")

add_library (framework ${BUILD_TYPE}
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)

# The library is being built
add_definitions (${DEFINE_CMD}${PROJECT_NAME}_BUILDING)

###############################################################################
# utility library
###############################################################################

target_link_libraries (framework lightship_util)
include_directories ("${CMAKE_SOURCE_DIR}/util/include")

###############################################################################
# install targets
###############################################################################

# only install if shared library
if (${BUILD_TYPE} STREQUAL "SHARED")
    install (
        TARGETS
            framework
        LIBRARY DESTINATION
            "lib"
        RUNTIME DESTINATION
            "bin"
    )
endif ()
