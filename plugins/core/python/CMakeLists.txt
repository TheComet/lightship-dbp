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
# generate config.h and export.h for this plugin
###############################################################################

set (PROJECT_NAME "PLUGIN_PYTHON")
set (BUILD_TYPE "SHARED")
set (PLUGIN_PYTHON_VERSION_MAJOR 0)
set (PLUGIN_PYTHON_VERSION_MINOR 0)
set (PLUGIN_PYTHON_VERSION_PATCH 1)
set (PLUGIN_PYTHON_NAME "python" CACHE STRING "(No spaces or special chars) - Name of this plugin")
set (PLUGIN_PYTHON_CATEGORY "script" CACHE STRING "Category of this plugin, for service/event discovery")
set (PLUGIN_PYTHON_AUTHOR "TheComet" CACHE STRING "Author of this plugin")
set (PLUGIN_PYTHON_DESCRIPTION "Exposes all services and events to python scripts" CACHE STRING "Short description of what this plugin does")
set (PLUGIN_PYTHON_WEBSITE "http://github.com/TheComet/lightship" CACHE STRING "Website to refer to")
set (PLUGIN_PYTHON_VERSION_STRING
    "${PLUGIN_PYTHON_VERSION_MAJOR}.${PLUGIN_PYTHON_VERSION_MINOR}.${PLUGIN_PYTHON_VERSION_PATCH}")

configure_file ("include/plugin_python/config.h.in"
                "include/plugin_python/config.h")
configure_file ("${EXPORT_H_TEMPLATE}"
                "include/plugin_python/export.h")

include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_python_OUTPUT_NAME
    "plugin_python-${PLUGIN_PYTHON_VERSION_STRING}")

file (GLOB plugin_python_HEADERS "include/plugin_python/*.h")
file (GLOB plugin_python_SOURCES "src/*.c")

set (plugin_python_HEADERS ${plugin_python_HEADERS}
	"include/plugin_python/config.h.in"
	"${EXPORT_H_TEMPLATE}")

include_directories ("include")

add_library (plugin_python SHARED
    ${plugin_python_HEADERS}
    ${plugin_python_SOURCES}
)

# adjust output name to not have the "lib" prefix and insert version number
set_target_properties (plugin_python
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_python-${PLUGIN_PYTHON_VERSION_STRING}"
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_python)

# this library is being built
add_definitions (${DEFINE_CMD}${PROJECT_NAME}_BUILDING)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_python_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# link lightship utility and plugin framework
###############################################################################

target_link_libraries (plugin_python
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
        plugin_python
    LIBRARY DESTINATION
        "bin/plugins"
    RUNTIME DESTINATION
        "bin/plugins"
)
