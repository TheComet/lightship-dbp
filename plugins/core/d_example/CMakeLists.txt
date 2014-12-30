project ("plugin_d_example" C)

###############################################################################
# set plugin metadata
###############################################################################

set (PLUGIN_INPUT_VERSION_MAJOR 0)
set (PLUGIN_INPUT_VERSION_MINOR 0)
set (PLUGIN_INPUT_VERSION_PATCH 1)
set (PLUGIN_INPUT_NAME "d_example")
set (PLUGIN_INPUT_CATEGORY "d_example")
set (PLUGIN_INPUT_AUTHOR "nerdinand")
set (PLUGIN_INPUT_DESCRIPTION "Just an example plugin written in the D programming language")
set (PLUGIN_INPUT_WEBSITE "http://github.com/nerdinand")
set (PLUGIN_INPUT_VERSION_STRING
    "${PLUGIN_INPUT_VERSION_MAJOR}.${PLUGIN_INPUT_VERSION_MINOR}.${PLUGIN_INPUT_VERSION_PATCH}")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_d_example_OUTPUT_NAME
    "plugin_d_example-${PLUGIN_INPUT_VERSION_STRING}")

file (GLOB plugin_d_example_SOURCES "src/*.d")

add_custom_target (plugin_d_example
    COMMAND
        ${D_COMPILER}
        -fPIC
        -shared
        ${plugin_d_example_SOURCES}
        -L${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}lightship_util${CMAKE_SHARED_LIBRARY_SUFFIX}
        -of"${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_d_example_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    SOURCES
        ${plugin_d_example_SOURCES}
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_d_example)
add_dependencies (plugin_d_example lightship_util)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_d_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_d_example_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")
