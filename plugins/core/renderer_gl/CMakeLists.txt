project ("plugin_renderer_gl")

set (PLUGIN_RENDERER_GL_VERSION_MAJOR 0)
set (PLUGIN_RENDERER_GL_VERSION_MINOR 0)
set (PLUGIN_RENDERER_GL_VERSION_PATCH 1)
set (PLUGIN_RENDERER_GL_VERSION_STRING
    "${PLUGIN_RENDERER_GL_VERSION_MAJOR}-${PLUGIN_RENDERER_GL_VERSION_MINOR}-${PLUGIN_RENDERER_GL_VERSION_PATCH}")

configure_file ("include/renderer_gl/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/renderer_gl/config.h")
include_directories (${CMAKE_CURRENT_BINARY_DIR})

file (GLOB plugin_renderer_gl_HEADERS "include/renderer_gl/*.h")
file (GLOB plugin_renderer_gl_SOURCES "src/*.c")

set (TEMP ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
add_library (plugin_renderer_gl SHARED
    ${plugin_renderer_gl_HEADERS}
    ${plugin_renderer_gl_SOURCES}
)
target_link_libraries (plugin_renderer_gl lightship_util)
set_target_properties (plugin_renderer_gl
    PROPERTIES
        PREFIX ""
	OUTPUT_NAME "plugin_renderer_gl-${PLUGIN_RENDERER_GL_VERSION_STRING}"
)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TEMP})
