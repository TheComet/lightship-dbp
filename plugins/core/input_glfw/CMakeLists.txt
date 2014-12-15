project ("plugin_input_glfw")

###############################################################################
# generate config.h for this plugin
###############################################################################

set (PLUGIN_INPUT_GLFW_VERSION_MAJOR 0)
set (PLUGIN_INPUT_GLFW_VERSION_MINOR 0)
set (PLUGIN_INPUT_GLFW_VERSION_PATCH 1)
set (PLUGIN_INPUT_GLFW_VERSION_STRING
    "${PLUGIN_INPUT_GLFW_VERSION_MAJOR}-${PLUGIN_INPUT_GLFW_VERSION_MINOR}-${PLUGIN_INPUT_GLFW_VERSION_PATCH}")

configure_file ("include/input_glfw/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/input_glfw/config.h")
include_directories (${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
# source files and library definition
###############################################################################

file (GLOB plugin_input_glfw_HEADERS "include/input_glfw/*.h")
file (GLOB plugin_input_glfw_SOURCES "src/*.c")

set (TEMP ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
add_library (plugin_input_glfw SHARED
    ${plugin_input_glfw_HEADERS}
    ${plugin_input_glfw_SOURCES}
)

target_link_libraries (plugin_input_glfw
    lightship_util
)

set_target_properties (plugin_input_glfw
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_input_glfw-${PLUGIN_INPUT_GLFW_VERSION_STRING}"
)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TEMP})

###############################################################################
# GLFW
###############################################################################

include_directories ("../renderer_gl/ext/GL/glfw-3.0.3/include/GLFW/")
target_link_libraries (plugin_input_glfw GLFW_303)
