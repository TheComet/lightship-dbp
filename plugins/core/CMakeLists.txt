option (BUILD_PLUGIN_RENDERER_GL "Set to 1 if you want to build the renderer_gl plugin" ON)
option (BUILD_PLUGIN_INPUT "Set to 1 if you want to build the input plugin" ON)
option (BUILD_PLUGIN_MENU "Set to 1 if you want to build the menu plugin" ON)
option (BUILD_PLUGIN_PYTHON "Set to 1 if you want to build the python plugin" ON)

if (BUILD_PLUGIN_RENDERER_GL)
    add_subdirectory ("renderer_gl")
endif ()

if (BUILD_PLUGIN_INPUT)
    add_subdirectory ("input")
endif ()

if (BUILD_PLUGIN_MENU)
    add_subdirectory ("menu")
endif ()

if (BUILD_PLUGIN_PYTHON)
    add_subdirectory ("python")
endif ()

