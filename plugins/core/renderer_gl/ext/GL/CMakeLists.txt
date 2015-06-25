### GLFW ###

include_directories(
	glfw-3.0.3/include/GLFW/
	glew-1.9.0/include/
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    -lGL -lGLU -lXrandr -lXext -lX11 -lrt
    ${CMAKE_DL_LIBS}
    ${GLFW_LIBRARIES}
)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    ${CMAKE_DL_LIBS}
    ${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (UNIX)
	add_definitions (-Wno-error -Wno-pedantic)
endif ()

### GLEW ###

set(GLEW_SOURCE
	glew-1.9.0/src/glew.c
)

set(GLEW_HEADERS
)


add_library( GLEW_190 STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)

target_link_libraries(GLEW_190
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

add_subdirectory (glfw-3.0.3)
