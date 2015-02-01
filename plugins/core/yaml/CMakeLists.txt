project ("plugin_yaml" C)

###############################################################################
# YAML library
###############################################################################

add_subdirectory ("ext")

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

set (PLUGIN_YAML_VERSION_MAJOR 0)
set (PLUGIN_YAML_VERSION_MINOR 0)
set (PLUGIN_YAML_VERSION_PATCH 1)
set (PLUGIN_YAML_NAME "yaml")
set (PLUGIN_YAML_CATEGORY "utiliy")
set (PLUGIN_YAML_AUTHOR "TheComet")
set (PLUGIN_YAML_DESCRIPTION "A YAML parser")
set (PLUGIN_YAML_WEBSITE "http://github.com/TheComet93/lightship")
set (PLUGIN_YAML_VERSION_STRING
    "${PLUGIN_YAML_VERSION_MAJOR}.${PLUGIN_YAML_VERSION_MINOR}.${PLUGIN_YAML_VERSION_PATCH}")

configure_file ("include/plugin_yaml/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/plugin_yaml/config.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_yaml_OUTPUT_NAME
    "plugin_yaml-${PLUGIN_YAML_VERSION_STRING}")

file (GLOB plugin_yaml_HEADERS "include/plugin_yaml/*.h")
file (GLOB plugin_yaml_SOURCES "src/*.c")
set (plugin_yaml_HEADERS ${plugin_yaml_HEADERS} "include/plugin_yaml.h")

include_directories ("include")

add_library (plugin_yaml SHARED
    ${plugin_yaml_HEADERS}
    ${plugin_yaml_SOURCES}
)

# adjust output name to not have the "lib" prefix and insert version number
set_target_properties (plugin_yaml
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_yaml-${PLUGIN_YAML_VERSION_STRING}"
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_yaml)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_yaml POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_yaml_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# link lightship utility static library
###############################################################################

target_link_libraries (plugin_yaml
    lightship_util
)

###############################################################################
# libyaml
###############################################################################

include_directories ("ext/yaml-0.1.5-cmake/include")
target_link_libraries (plugin_yaml yaml)

# yaml is built statically
add_definitions (-DYAML_DECLARE_STATIC)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        plugin_yaml
    LIBRARY DESTINATION
        "bin/plugins"
    RUNTIME DESTINATION
        "bin/plugins"
)
