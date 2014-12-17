project (lightship C)

file (GLOB lightship_HEADERS "include/lightship/*.h")
file (GLOB lightship_SOURCES "src/*.c")

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-pedantic)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-pedantic)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

# dynamic linker
if (${PLATFORM} MATCHES "LINUX")
	set (DYNAMIC_LINKER_LIBRARY "dl")
elseif (${PLATFORM} MATCHES "MAC")
    set (DYNAMIC_LINKER_LIBRARY "dl")
endif ()

add_executable (lightship ${lightship_HEADERS} ${lightship_SOURCES})
target_link_libraries (lightship
    lightship_util
    ${DYNAMIC_LINKER_LIBRARY}
)
create_vcproj_userfile (lightship)