project (lightship_tests)

add_subdirectory ("gmock")

include_directories ("gmock/include")
include_directories ("gmock/gtest/include")
include_directories ("include")

file (GLOB tests_SOURCES_UTIL
            "src/env/util/*.cpp"
            "src/malloc/util/*.cpp"
            "src/misc/util/*.cpp")
file (GLOB tests_SOURCES_FRAMEWORK
            "src/env/framework/*.cpp"
            "src/malloc/framework/*.cpp"
            "src/misc/framework/*.cpp")
file (GLOB tests_HEADERS
            "include/tests/*.hpp")

# death tests in debug mode
option (ENABLE_DEATH_TESTS "Whether or not to build death tests" ON)
if (ENABLE_DEATH_TESTS)
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		file (GLOB tests_SOURCES_UTIL ${tests_SOURCES_UTIL} "src/death/util/*.cpp")
		file (GLOB tests_SOURCES_FRAMEWORK ${tests_SOURCES_FRAMEWORK} "src/death/framework/*.cpp")
	endif ()
endif ()

add_definitions (${DEFINE_CMD}TESTING)

message (STATUS "------------------------------------------------------------")
message (STATUS "Settings for tests")
message (STATUS " + Death tests: ${ENABLE_DEATH_TESTS}")
message (STATUS "------------------------------------------------------------")

add_executable (lightship_tests
    ${tests_HEADERS}
    ${tests_SOURCES}
    ${tests_SOURCES_UTIL}
    ${tests_SOURCES_FRAMEWORK}
)

target_link_libraries (lightship_tests
    framework
    lightship_util
    gmock_main
)

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (lightship_tests)

add_custom_command (TARGET lightship_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/tests/test_dir"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/test_dir")

# remove the dummy file required for git to track the empty directory
# this is for one of the directory unit tests
add_custom_command(TARGET lightship_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/test_dir/empty/empty")
