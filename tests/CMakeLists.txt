project (lightship_tests)

add_subdirectory ("gmock")

include_directories ("gmock/include")
include_directories ("gmock/gtest/include")
include_directories ("include")

file (GLOB tests_HEADERS            "include/tests/*.hpp")
file (GLOB tests_SOURCES            "src/*.cpp")
file (GLOB tests_SOURCES_UTIL       "src/util/*.cpp")
file (GLOB tests_SOURCES_FRAMEWORK  "src/framework/*.cpp")

add_definitions (${DEFINE_CMD}TESTING)

add_executable (lightship_tests
    ${tests_HEADERS}
    ${tests_SOURCES}
    ${tests_SOURCES_UTIL}
    ${tests_SOURCES_FRAMEWORK}
)

target_link_libraries (lightship_tests
    lightship_util
    gmock_main
)

add_custom_command (TARGET lightship_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/tests/test_dir"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/test_dir")
