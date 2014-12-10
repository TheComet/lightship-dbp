project ("plugin_test")

file (GLOB plugin_test_HEADERS "include/test/*.h")
file (GLOB plugin_test_SOURCES "src/*.c")

add_library (plugin_test SHARED
    ${plugin_test_HEADERS}
    ${plugin_test_SOURCES}
)