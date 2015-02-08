option (BUILD_PLUGIN_MAIN_LOOP "Set to 1 if you want to build the main_loop plugin" ON)
option (BUILD_PLUGIN_RENDERER_GL "Set to 1 if you want to build the renderer_gl plugin" ON)
option (BUILD_PLUGIN_INPUT "Set to 1 if you want to build the input plugin" ON)
option (BUILD_PLUGIN_YAML "Set to 1 if you want to build the yaml plugin" ON)
option (BUILD_PLUGIN_MENU "Set to 1 if you want to build the menu plugin" ON)

if (BUILD_PLUGIN_MAIN_LOOP)
    add_subdirectory ("main_loop")
endif ()

if (BUILD_PLUGIN_RENDERER_GL)
    add_subdirectory ("renderer_gl")
endif ()

if (BUILD_PLUGIN_INPUT)
    add_subdirectory ("input")
endif ()

if (BUILD_PLUGIN_YAML)
    add_subdirectory ("yaml")
endif ()

if (BUILD_PLUGIN_MENU)
    add_subdirectory ("menu")
endif ()
