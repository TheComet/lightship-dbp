project ("plugin_renderer_gl" C)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
    add_definitions( "/MP" )
endif()

add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -D_CRT_SECURE_NO_WARNINGS
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

set (PLUGIN_RENDERER_GL_VERSION_MAJOR 0)
set (PLUGIN_RENDERER_GL_VERSION_MINOR 0)
set (PLUGIN_RENDERER_GL_VERSION_PATCH 1)
set (PLUGIN_RENDERER_GL_VERSION_STRING
    "${PLUGIN_RENDERER_GL_VERSION_MAJOR}-${PLUGIN_RENDERER_GL_VERSION_MINOR}-${PLUGIN_RENDERER_GL_VERSION_PATCH}")

configure_file ("include/plugin_renderer_gl/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/plugin_renderer_gl/config.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/include")

###############################################################################
# source files and library definition
###############################################################################

set (plugin_renderer_gl_OUTPUT_NAME
    "plugin_renderer_gl-${PLUGIN_RENDERER_GL_VERSION_STRING}")

file (GLOB plugin_renderer_gl_HEADERS "include/plugin_renderer_gl/*.h")
file (GLOB plugin_renderer_gl_SOURCES "src/*.c")

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
# link lightship utility static library
###############################################################################

target_link_libraries (plugin_renderer_gl
    lightship_util
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
