if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    if (NOT MINGW) # MinGW always builds position independent code
        add_definitions (-fPIC)
    endif ()
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    if (NOT MINGW) # MinGW always builds position independent code
        add_definitions (-fPIC)
    endif ()
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
    add_definitions( "/MP" )
endif()

add_definitions(
    ${DEFINE_CMD}TW_STATIC
    ${DEFINE_CMD}TW_NO_LIB_PRAGMA
    ${DEFINE_CMD}TW_NO_DIRECT3D
    ${DEFINE_CMD}GLEW_STATIC
)

# needs to be added before setting additional compiler flags
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

set (PROJECT_NAME "PLUGIN_RENDERER_GL")
set (BUILD_TYPE "SHARED")
set (PLUGIN_RENDERER_GL_VERSION_MAJOR 0)
set (PLUGIN_RENDERER_GL_VERSION_MINOR 0)
set (PLUGIN_RENDERER_GL_VERSION_PATCH 1)
set (PLUGIN_RENDERER_GL_NAME "renderer_gl")
set (PLUGIN_RENDERER_GL_CATEGORY "graphics")
set (PLUGIN_RENDERER_GL_AUTHOR "TheComet")
set (PLUGIN_RENDERER_GL_DESCRIPTION "The rendering backend for Lightship")
set (PLUGIN_RENDERER_GL_WEBSITE "http://github.com/TheComet93/lightship")
set (PLUGIN_RENDERER_GL_VERSION_STRING
    "${PLUGIN_RENDERER_GL_VERSION_MAJOR}.${PLUGIN_RENDERER_GL_VERSION_MINOR}.${PLUGIN_RENDERER_GL_VERSION_PATCH}")

configure_file ("include/plugin_renderer_gl/config.h.in"
                "include/plugin_renderer_gl/config.h")
configure_file ("${EXPORT_H_TEMPLATE}"
                "include/plugin_renderer_gl/export.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_renderer_gl_OUTPUT_NAME
    "plugin_renderer_gl-${PLUGIN_RENDERER_GL_VERSION_STRING}")

file (GLOB plugin_renderer_gl_HEADERS "include/plugin_renderer_gl/*.h")
file (GLOB plugin_renderer_gl_SOURCES "src/*.c")

set (plugin_renderer_gl_HEADERS ${plugin_renderer_gl_HEADERS}
    "include/plugin_renderer_gl/config.h.in"
    "${EXPORT_H_TEMPLATE}")

include_directories ("include")

add_library (plugin_renderer_gl SHARED
    ${plugin_renderer_gl_HEADERS}
    ${plugin_renderer_gl_SOURCES}
)

# adjust output name to not have the "lib" prefix and insert version number
set_target_properties (plugin_renderer_gl
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME ${plugin_renderer_gl_OUTPUT_NAME}
)

# ensure plugin is built before lightship
add_dependencies (lightship plugin_renderer_gl)

# this library is being built
add_definitions (${DEFINE_CMD}${PROJECT_NAME}_BUILDING)

###############################################################################
# copy plugin to plugins/ folder after build
###############################################################################

add_custom_command (TARGET plugin_renderer_gl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_renderer_gl_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

###############################################################################
# GLFW
###############################################################################

include_directories ("ext/GL/glfw-3.0.3/include/GLFW/")
target_link_libraries (plugin_renderer_gl GLFW_303)

###############################################################################
# GLEW
###############################################################################

include_directories ("ext/GL/glew-1.9.0/include/")
target_link_libraries (plugin_renderer_gl GLEW_190)

###############################################################################
# OpenGL
###############################################################################

find_package (OpenGL REQUIRED)
include_directories (${OPENGL_INCLUDE_DIR})
target_link_libraries (plugin_renderer_gl ${OPENGL_LIBRARIES})
message (STATUS "GL: ${OPENGL_INCLUDE_DIR}")
message (STATUS "GL: ${OPENGL_LIBRARIES}")

###############################################################################
# freetype
###############################################################################

include_directories ("ext/freetype-2.5.5/include")
target_link_libraries (plugin_renderer_gl freetype)

###############################################################################
# stb
###############################################################################

include_directories ("ext/stb")

###############################################################################
# link lightship utility and framework libraries
###############################################################################

target_link_libraries (plugin_renderer_gl
    lightship_util
    framework
)

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        plugin_renderer_gl
    LIBRARY DESTINATION
        "bin/plugins"
    RUNTIME DESTINATION
        "bin/plugins"
)

install (
    DIRECTORY
        "fx"
    DESTINATION
        "bin"
)
