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

set (PLUGIN_MAIN_LOOP_VERSION_MAJOR 0)
set (PLUGIN_MAIN_LOOP_VERSION_MINOR 0)
set (PLUGIN_MAIN_LOOP_VERSION_PATCH 1)
set (PLUGIN_MAIN_LOOP_VERSION_STRING
    "${PLUGIN_MAIN_LOOP_VERSION_MAJOR}-${PLUGIN_MAIN_LOOP_VERSION_MINOR}-${PLUGIN_MAIN_LOOP_VERSION_PATCH}")

configure_file ("include/main_loop/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/main_loop/config.h")
include_directories (${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
# source files and library definition
###############################################################################

set (plugin_main_loop_OUTPUT_NAME
    "plugin_main_loop-${PLUGIN_MAIN_LOOP_VERSION_STRING}")

file (GLOB plugin_main_loop_HEADERS "include/main_loop/*.h")
file (GLOB plugin_main_loop_SOURCES "src/*.c")

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

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_main_loop POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_main_loop_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# link lightship utility static library
###############################################################################

target_link_libraries (plugin_main_loop
    lightship_util
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
