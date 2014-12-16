project ("plugin_renderer_gl")

###############################################################################
# generate config.h for this plugin
###############################################################################

set (PLUGIN_RENDERER_GL_VERSION_MAJOR 0)
set (PLUGIN_RENDERER_GL_VERSION_MINOR 0)
set (PLUGIN_RENDERER_GL_VERSION_PATCH 1)
set (PLUGIN_RENDERER_GL_VERSION_STRING
    "${PLUGIN_RENDERER_GL_VERSION_MAJOR}-${PLUGIN_RENDERER_GL_VERSION_MINOR}-${PLUGIN_RENDERER_GL_VERSION_PATCH}")

configure_file ("include/renderer_gl/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/renderer_gl/config.h")
include_directories (${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
# source files and library definition
###############################################################################

set (plugin_renderer_gl_OUTPUT_NAME
    "plugin_renderer_gl-${PLUGIN_RENDERER_GL_VERSION_STRING}")

file (GLOB plugin_renderer_gl_HEADERS "include/renderer_gl/*.h")
file (GLOB plugin_renderer_gl_SOURCES "src/*.c")

include_directories ("include")

add_library (plugin_renderer_gl SHARED
    ${plugin_renderer_gl_HEADERS}
    ${plugin_renderer_gl_SOURCES}
)

target_link_libraries (plugin_renderer_gl
    lightship_util
)

set_target_properties (plugin_renderer_gl
    PROPERTIES
        PREFIX ""
    OUTPUT_NAME ${plugin_renderer_gl_OUTPUT_NAME}
)

add_custom_command (TARGET plugin_renderer_gl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${plugin_renderer_gl_OUTPUT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/")

add_dependencies (lightship plugin_renderer_gl)

###############################################################################
# OpenGL
###############################################################################

find_package (OpenGL REQUIRED)
include_directories (${OPENGL_INCLUDE_DIR})
target_link_libraries (plugin_renderer_gl ${OPENGL_LIBRARIES})
message (STATUS "GL: ${OPENGL_INCLUDE_DIR}")
message (STATUS "GL: ${OPENGL_LIBRARIES}")

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
# GLM
###############################################################################

include_directories ("ext/GL/glm-0.9.4.0/")

add_subdirectory ("ext")

