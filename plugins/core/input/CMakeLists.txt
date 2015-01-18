project ("plugin_input" C)

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

set (PLUGIN_INPUT_VERSION_MAJOR 0)
set (PLUGIN_INPUT_VERSION_MINOR 0)
set (PLUGIN_INPUT_VERSION_PATCH 1)
set (PLUGIN_INPUT_NAME "input")
set (PLUGIN_INPUT_CATEGORY "input")
set (PLUGIN_INPUT_AUTHOR "TheComet")
set (PLUGIN_INPUT_DESCRIPTION "Input mapper for lightship")
set (PLUGIN_INPUT_WEBSITE "http://github.com/TheComet93")
set (PLUGIN_INPUT_VERSION_STRING
    "${PLUGIN_INPUT_VERSION_MAJOR}.${PLUGIN_INPUT_VERSION_MINOR}.${PLUGIN_INPUT_VERSION_PATCH}")

configure_file ("include/plugin_input/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/input/config.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_input_OUTPUT_NAME
    "plugin_input-${PLUGIN_INPUT_VERSION_STRING}")

file (GLOB plugin_input_HEADERS "include/plugin_input/*.h")
file (GLOB plugin_input_SOURCES "src/*.c")
set (plugin_input_HEADERS ${plugin_input_HEADERS} "include/plugin_input.h")

include_directories ("include")
include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/renderer_gl/include")

add_library (plugin_input SHARED
    ${plugin_input_HEADERS}
    ${plugin_input_SOURCES}
)

# adjust output name to not have the "lib" prefix and insert version number
set_target_properties (plugin_input
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_input-${PLUGIN_INPUT_VERSION_STRING}"
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_input)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_input POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_input_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# link lightship utility static library
###############################################################################

target_link_libraries (plugin_input
    lightship_util
)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        plugin_input
    LIBRARY DESTINATION
        "bin/plugins"
    RUNTIME DESTINATION
        "bin/plugins"
)
