project (lightship_tests)

add_subdirectory ("gmock")

include_directories ("gmock/include")
include_directories ("gmock/gtest/include")
include_directories ("include")

file (GLOB tests_HEADERS "include/tests/*.hpp")
file (GLOB tests_SOURCES "src/*.cpp")

add_executable (lightship_tests
    ${tests_HEADERS}
    ${tests_SOURCES}
)

target_link_libraries (lightship_tests
    lightship_util
    gmock_main
)
