project ("yaml" C)

###############################################################################
# options, kawalski
###############################################################################

option (BUILD_SHARED "Whether to build shared (ON) or static (OFF)" OFF)

###############################################################################
# generate config.h
###############################################################################

macro (check_for_file FILE_NAME VAR_YES_NO)
    find_path (PATH_FOUND ${FILE_NAME})
    if (PATH_FOUND)
        message (STATUS "checking for ${FILE_NAME}... yes")
        set (${VAR_YES_NO} 1)
    else ()
        message (STATUS "checking for ${FILE_NAME}... no")
        set (${VAR_YES_NO} 0)
    endif ()
endmacro ()

check_for_file ("dlfcn.h" HAVE_DLFCN_H)
check_for_file ("inttypes.h" HAVE_INTTYPES_H)
check_for_file ("memory.h" HAVE_MEMORY_H)
check_for_file ("stdint.h" HAVE_STDINT_H)
check_for_file ("stdlib.h" HAVE_STDLIB_H)
check_for_file ("strings.h" HAVE_STRINGS_H)
check_for_file ("string.h" HAVE_STRING_H)
check_for_file ("sys/stat.h" HAVE_SYS_STAT_H)
check_for_file ("sys/types.h" HAVE_SYS_TYPES_H)
check_for_file ("unistd.h" HAVE_UNISTD_H)

set (LT_OBJDIR ".libs/")

set (PACKAGE "yaml")
set (PACKAGE_BUGREPORT "http://pyyaml.org/newticket?component=libyaml")
set (PACKAGE_NAME "yaml")
set (PACKAGE_STRING "yaml 0.1.5")
set (PACKAGE_TARNAME "yaml")
set (PACKAGE_URL "")
set (STDC_HEADERS 1)
set (YAML_VERSION_MAJOR "0")
set (YAML_VERSION_MINOR "1")
set (YAML_VERSION_PATCH "5")
set (YAML_VERSION_STRING "${YAML_VERSION_MAJOR}.${YAML_VERSION_MINOR}.${YAML_VERSION_PATCH}")
set (VERSION "${YAML_VERSION_STRING}")
set (PACKAGE_VERSION "${YAML_VERSION_STRING}")

configure_file ("include/yaml/config.h.in"
                "include/yaml/config.h")
include_directories ("${CMAKE_BINARY_DIR}/plugins/core/yaml/ext/yaml-0.1.5-cmake/include")
add_definitions (-DHAVE_CONFIG_H)

###############################################################################
# compiler definitions and flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    if (BUILD_SHARED)
        add_definitions (-fPIC)
    endif ()
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

if (BUILD_SHARED)
    set (BUILD_TYPE "SHARED")
else ()
    set (BUILD_TYPE "STATIC")
endif ()

###############################################################################
# source files and library definition
###############################################################################

file (GLOB yaml_HEADERS "include/yaml/*.h")
file (GLOB yaml_SOURCES "src/*.c")

include_directories ("include")

add_library (yaml ${BUILD_TYPE}
    ${yaml_HEADERS}
    ${yaml_SOURCES}
)

###############################################################################
# install targets
###############################################################################

if (BUILD_SHARED)
    install (
        TARGETS
            yaml
        LIBRARY DESTINATION
            "bin"
        RUNTIME DESTINATION
            "bin"
    )
endif ()