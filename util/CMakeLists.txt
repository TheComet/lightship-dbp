project (util C)

set (PROJECT_NAME "LIGHTSHIP")
set (BUILD_TYPE "SHARED")

configure_file ("include/util/config.h.in"
                "${CMAKE_SOURCE_DIR}/util/include/util/config.h")

if (${PLATFORM} MATCHES "LINUX")
    set (PLATFORM_HEADER_DIRS "include/util/platform/linux/*.h")
    set (PLATFORM_SOURCE_DIRS "src/platform/linux/*.c")
elseif (${PLATFORM} MATCHES "MAC")
    set (PLATFORM_HEADER_DIRS "include/util/platform/osx/*.h")
    set (PLATFORM_SOURCE_DIRS "src/platform/osx/*.c")
elseif (${PLATFORM} MATCHES "WINDOWS")
    set (PLATFORM_HEADER_DIRS "include/util/platform/win/*.h")
    set (PLATFORM_SOURCE_DIRS "src/platform/win/*.c")
endif ()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-pedantic -fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-pedantic -fPIC)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()
                
file (GLOB lightship_util_HEADERS "include/util/*.h" ${PLATFORM_HEADER_DIRS})
file (GLOB lightship_util_SOURCES "src/*.c" ${PLATFORM_SOURCE_DIRS})

add_library (lightship_util STATIC
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)
create_vcproj_userfile (lightship_util)