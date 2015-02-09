add_subdirectory ("plugin_manager")
add_subdirectory ("core")

# D plugins
if (HAVE_D_COMPILER)
    add_subdirectory ("d")
endif ()

