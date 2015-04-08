project ("plugin_menu" C)

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

set (PROJECT_NAME "PLUGIN_MENU")
set (BUILD_TYPE "SHARED")
set (PLUGIN_MENU_VERSION_MAJOR 0)
set (PLUGIN_MENU_VERSION_MINOR 0)
set (PLUGIN_MENU_VERSION_PATCH 1)
set (PLUGIN_MENU_NAME "menu")
set (PLUGIN_MENU_CATEGORY "gui")
set (PLUGIN_MENU_AUTHOR "TheComet")
set (PLUGIN_MENU_DESCRIPTION "Menu management plugin")
set (PLUGIN_MENU_WEBSITE "https://github.com/TheComet93")
set (PLUGIN_MENU_VERSION_STRING
    "${PLUGIN_MENU_VERSION_MAJOR}.${PLUGIN_MENU_VERSION_MINOR}.${PLUGIN_MENU_VERSION_PATCH}")

configure_file ("include/plugin_menu/config.h.in"
                "include/plugin_menu/config.h")
configure_file ("${EXPORT_H_TEMPLATE}"
                "include/plugin_menu/export.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_menu_OUTPUT_NAME
    "plugin_menu-${PLUGIN_MENU_VERSION_STRING}")

file (GLOB plugin_menu_HEADERS "include/plugin_menu/*.h")
file (GLOB plugin_menu_SOURCES "src/*.c")

set (plugin_menu_HEADERS ${plugin_menu_HEADERS}
    "include/plugin_menu/config.h.in"
    "${EXPORT_H_TEMPLATE}")

include_directories ("include")

add_library (plugin_menu SHARED
    ${plugin_menu_HEADERS}
    ${plugin_menu_SOURCES}
)

# adjust output name to not have the "lib" prefix and insert version number
set_target_properties (plugin_menu
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME "plugin_menu-${PLUGIN_MENU_VERSION_STRING}"
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_menu)

# this library is being built
add_definitions (${DEFINE_CMD}${PROJECT_NAME}_BUILDING)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_menu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_menu_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# link lightship utility and framework libraries
###############################################################################

target_link_libraries (plugin_menu
    lightship_util
    framework
)

###############################################################################
# dependent on renderer_gl and yaml
###############################################################################

include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/renderer_gl/include")
include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/yaml/include")

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        plugin_menu
    LIBRARY DESTINATION
        "bin/plugins"
    RUNTIME DESTINATION
        "bin/plugins"
)

install (
    DIRECTORY
        "ttf"
        "cfg"
    DESTINATION
        "bin"
)
