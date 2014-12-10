project ("plugin_test")

file (GLOB plugin_test_HEADERS "include/test/*.h")
file (GLOB plugin_test_SOURCES "src/*.c")

set (TEMP ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
add_library (plugin_test SHARED
    ${plugin_test_HEADERS}
    ${plugin_test_SOURCES}
)
set_target_properties (plugin_test
    PROPERTIES
        PREFIX ""
)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TEMP})