# this one is important
SET(CMAKE_SYSTEM_NAME Windows)

# specify the cross compiler
SET(CMAKE_C_COMPILER /usr/bin/i386-pc-mingw32-gcc)
SET(CMAKE_RC_COMPILER /usr/bin/i386-pc-mingw32-windres)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH /usr/i386-pc-mingw32)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

