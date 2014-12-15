project ("plugin_input")

###############################################################################
# generate config.h for this plugin
###############################################################################

set (PLUGIN_INPUT_VERSION_MAJOR 0)
set (PLUGIN_INPUT_VERSION_MINOR 0)
set (PLUGIN_INPUT_VERSION_PATCH 1)
set (PLUGIN_INPUT_VERSION_STRING
    "${PLUGIN_INPUT_VERSION_MAJOR}-${PLUGIN_INPUT_VERSION_MINOR}-${PLUGIN_INPUT_VERSION_PATCH}")

configure_file ("include/input/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/input/config.h")
include_directories (${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
# source files and library definition
###############################################################################

set (plugin_input_OUTPUT_NAME
    "plugin_input-${PLUGIN_INPUT_VERSION_STRING}")

file (GLOB plugin_input_HEADERS "include/input/*.h")
file (GLOB plugin_input_SOURCES "src/*.c")

add_library (plugin_input SHARED
    ${plugin_input_HEADERS}
    ${plugin_input_SOURCES}
)

target_link_libraries (plugin_input
    lightship_util
)

set_target_properties (plugin_input
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_input-${PLUGIN_INPUT_VERSION_STRING}"
)

add_custom_command (TARGET plugin_input POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_input_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")
