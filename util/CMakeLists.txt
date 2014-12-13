project (util)

set (PROJECT_NAME "LIGHTSHIP")
set (BUILD_TYPE "SHARED")

configure_file ("include/util/config.h.in"
                "${CMAKE_SOURCE_DIR}/util/include/util/config.h")

file (GLOB_RECURSE lightship_util_HEADERS "include/util/*.h")
file (GLOB_RECURSE lightship_util_SOURCES "src/*.c")

add_library (lightship_util STATIC
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)
