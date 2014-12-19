option (CROSS_COMPILE_i486-pc-mingw32 "cross compile for windows i486" OFF)
option (CROSS_COMPILE_x86_64-pc-mingw32 "cross compile for windows x86_64" OFF)

macro (generate_cross_arch ARCH_NAME ARCH C_COMPILER RC_COMPILER ROOT_PATH)

    # create build directory for this architecture and make sure it gets
    # ignored by git
    set (BUILD_DIR "${CMAKE_SOURCE_DIR}/build-${ARCH}")
    file (MAKE_DIRECTORY "${BUILD_DIR}")
    file (WRITE "${BUILD_DIR}/.gitignore" "*")
    
    # configure toolchain file and install file
    set (ARCH_NAME ${ARCH_NAME})
    set (C_COMPILER ${C_COMPILER})
    set (RC_COMPILER ${RC_COMPILER})
    set (ROOT_PATH ${ROOT_PATH})
    set (${ARCH}_INSTALL_PREFIX
        "${CMAKE_INSTALL_PREFIX}/${ARCH}"
    CACHE STRING
        "The install prefix of target ${ARCH}")
    configure_file ("toolchain-arch.cmake.in"
                    "${CMAKE_CURRENT_LIST_DIR}/toolchains/toolchain-${ARCH}.cmake"
                    @ONLY)
    configure_file ("install-arch.cmake.in"
                    "${CMAKE_CURRENT_BINARY_DIR}/install-${ARCH}.cmake"
                    @ONLY)

    # execute cmake to configure the project for this architecture
    execute_process (
        COMMAND
            cmake
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/toolchains/toolchain-${ARCH}.cmake
                -DCMAKE_INSTALL_PREFIX=${${ARCH}_INSTALL_PREFIX}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                ..
        WORKING_DIRECTORY
            ${BUILD_DIR}
    )

    add_custom_target (lightship-${ARCH}
        ALL
            make
        WORKING_DIRECTORY
            ${BUILD_DIR}
    )

    install (
        SCRIPT
            "${CMAKE_CURRENT_BINARY_DIR}/install-${ARCH}.cmake"
    )

endmacro ()

if (CROSS_COMPILE_i486-pc-mingw32)
    generate_cross_arch ("Windows"
                         "i486-pc-mingw32"
                         "/usr/bin/i486-pc-mingw32-gcc"
                         "/usr/bin/i486-pc-mingw32-windres"
                         "/usr/bin/i486-pc-mingw32")
endif ()
if (CROSS_COMPILE_x86_64-pc-mingw32)
    generate_cross_arch ("Windows"
                         "x86_64-pc-mingw32"
                         "/usr/bin/x86_64-pc-mingw32-gcc"
                         "/usr/bin/x86_64-pc-mingw32-windres"
                         "/usr/bin/x86_64-pc-mingw32")
endif ()
