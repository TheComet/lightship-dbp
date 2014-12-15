project ("plugin_test")

set (PLUGIN_TEST_VERSION_MAJOR 0)
set (PLUGIN_TEST_VERSION_MINOR 0)
set (PLUGIN_TEST_VERSION_PATCH 1)
set (PLUGIN_TEST_VERSION_STRING
    "${PLUGIN_TEST_VERSION_MAJOR}-${PLUGIN_TEST_VERSION_MINOR}-${PLUGIN_TEST_VERSION_PATCH}")

configure_file ("include/test/config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/test/config.h")
include_directories (${CMAKE_CURRENT_BINARY_DIR})

file (GLOB plugin_test_HEADERS "include/test/*.h")
file (GLOB plugin_test_SOURCES "src/*.c")

set (TEMP ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
add_library (plugin_test SHARED
    ${plugin_test_HEADERS}
    ${plugin_test_SOURCES}
)
target_link_libraries (plugin_test
    lightship_util
)
set_target_properties (plugin_test
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME "plugin_test-${PLUGIN_TEST_VERSION_STRING}"
)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TEMP})
