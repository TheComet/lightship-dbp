add_subdirectory ("main_loop")
add_subdirectory ("renderer_gl")
add_subdirectory ("input")
add_subdirectory ("yaml")
add_subdirectory ("menu")

# D plugins
if (HAVE_D_COMPILER)
    add_subdirectory ("d_example")
endif ()
