project ("lightship" C)

###############################################################################
# compiler flags for this project
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# source files and runtime definition
###############################################################################

file (GLOB lightship_HEADERS "include/lightship/*.h")
file (GLOB lightship_SOURCES "src/*.c")

add_executable (lightship
    ${lightship_HEADERS}
    ${lightship_SOURCES}
)

target_link_libraries (lightship
    lightship_util
)
create_vcproj_userfile (lightship)

include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/yaml/include")
include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/menu/include")
include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/renderer_gl/include")
include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/main_loop/include")

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        lightship
    DESTINATION
        "bin"
)
install (
    DIRECTORY
        "${CMAKE_CURRENT_LIST_DIR}/cfg"
    DESTINATION
        "bin"
)