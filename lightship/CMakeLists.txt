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

set (lightship_HEADERS ${lightship_HEADERS}
    "include/lightship/config.h.in"
    "${CMAKE_SOURCE_DIR}/common/include/common/export.h.in"
)

include_directories ("include")

add_executable (lightship
    ${lightship_HEADERS}
    ${lightship_SOURCES}
)

target_link_libraries (lightship
    lightship_util
    plugin_manager
)

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (lightship)

###############################################################################
# generate config.h and export.h
###############################################################################

set (PROJECT_NAME "LIGHTSHIP")

configure_file ("${CMAKE_SOURCE_DIR}/common/include/common/export.h.in"
                "${CMAKE_CURRENT_LIST_DIR}/include/lightship/export.h")
configure_file ("${CMAKE_CURRENT_LIST_DIR}/include/lightship/config.h.in"
                "${CMAKE_CURRENT_LIST_DIR}/include/lightship/config.h")

file (WRITE "include/lightship/.gitignore"
    "*\n"
    "export.h\n"
    "config.h\n")

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