cmake_minimum_required (VERSION 2.6)

option (NAME "The name of your plugin. MUST be all lower-case and have no dashes.")
if (NOT NAME)
    message (FATAL_ERROR "Please set a name for your plugin by passing -DNAME=\"your name\". MUST be all lower-case and  have no dashes.")
endif ()
string (TOUPPER ${NAME} NAME_CAPS)

file (MAKE_DIRECTORY "${NAME}")
file (MAKE_DIRECTORY "${NAME}/src")
file (MAKE_DIRECTORY "${NAME}/include")
file (MAKE_DIRECTORY "${NAME}/include/${NAME}")

configure_file ("CMakeLists.txt.in" "${NAME}/CMakeLists.txt" @ONLY)
configure_file ("plugin.c.in" "${NAME}/src/plugin.c" @ONLY)
configure_file ("config.h.in.in" "${NAME}/include/${NAME}/config.h.in")

execute_process (COMMAND cmake -E remove "CMakeCache.txt")
#    "${CMAKE_SOURCE_DIR}/CMakeFiles"
#    "${CMAKE_SOURCE_DIR}/cmake_install.cmake"
#    "${CMAKE_SOURCE_DIR}/Makefile"


