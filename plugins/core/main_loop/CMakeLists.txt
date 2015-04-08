project ("plugin_main_loop" C)

###############################################################################
# compiler flags for this project
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# generate config.h for this plugin
###############################################################################

set (PROJECT_NAME "PLUGIN_MAIN_LOOP")
set (BUILD_TYPE "SHARED")
set (PLUGIN_MAIN_LOOP_VERSION_MAJOR 0)
set (PLUGIN_MAIN_LOOP_VERSION_MINOR 0)
set (PLUGIN_MAIN_LOOP_VERSION_PATCH 1)
set (PLUGIN_MAIN_LOOP_NAME "main_loop")
set (PLUGIN_MAIN_LOOP_CATEGORY "system")
set (PLUGIN_MAIN_LOOP_AUTHOR "TheComet")
set (PLUGIN_MAIN_LOOP_DESCRIPTION "Controls the main render loop and update loop")
set (PLUGIN_MAIN_LOOP_WEBSITE "http://github.com/TheComet93/lightship")
set (PLUGIN_MAIN_LOOP_VERSION_STRING
    "${PLUGIN_MAIN_LOOP_VERSION_MAJOR}.${PLUGIN_MAIN_LOOP_VERSION_MINOR}.${PLUGIN_MAIN_LOOP_VERSION_PATCH}")

configure_file ("include/plugin_main_loop/config.h.in"
                "include/plugin_main_loop/config.h")
configure_file ("${EXPORT_H_TEMPLATE}"
                "include/plugin_main_loop/export.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_main_loop_OUTPUT_NAME
    "plugin_main_loop-${PLUGIN_MAIN_LOOP_VERSION_STRING}")

file (GLOB plugin_main_loop_HEADERS "include/plugin_main_loop/*.h")
file (GLOB plugin_main_loop_SOURCES "src/*.c")

set (plugin_main_loop_HEADERS ${plugin_main_loop_HEADERS}
    "include/plugin_main_loop/config.h.in"
    "${EXPORT_H_TEMPLATE}")

include_directories ("include")

add_library (plugin_main_loop SHARED
    ${plugin_main_loop_HEADERS}
    ${plugin_main_loop_SOURCES}
)

# adjust output name to not have the "lib" prefix and insert version number
set_target_properties (plugin_main_loop
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_main_loop-${PLUGIN_MAIN_LOOP_VERSION_STRING}"
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_main_loop)

# this library is being built
add_definitions (${DEFINE_CMD}${PROJECT_NAME}_BUILDING)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_main_loop POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_main_loop_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# link lightship utility and framework libraries
###############################################################################

target_link_libraries (plugin_main_loop
    lightship_util
    framework
)

###############################################################################
# other libs...
###############################################################################

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        plugin_main_loop
    LIBRARY DESTINATION
        "bin/plugins"
    RUNTIME DESTINATION
        "bin/plugins"
)
