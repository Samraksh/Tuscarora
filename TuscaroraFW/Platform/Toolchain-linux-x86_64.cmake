# This is important
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 3)
set(CMAKE_SYSTEM_PROCESSOR x86_64)


# The target environment, used to search for includes and linked libraries
set(CMAKE_FIND_ROOT_PATH  /)

# Specify the compiler
set (COMPILER_PATH "/usr/bin")
set (CMAKE_CC_COMPILER  ${COMPILER_PATH}/gcc)
set (CMAKE_CXX_COMPILER ${COMPILER_PATH}/g++)
set (ARCH_CMAKE_AR ${COMPILER_PATH}/ar)


# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set architecture specific (x86_64) build flags here. They will be joined
# to GCC_COMPILE_FLAGS in ../CMakeLists.txt
set (ARCH_COMPILE_FLAGS "-Wall -pthread -Wl,--no-as-needed -DAMPTASK_ARCH=ARCH_x86_64")
set (ARCHCC_COMPILE_FLAGS "-std=c++11 ${ARCH_COMPILE_FLAGS}")
set (ARCHC_COMPILE_FLAGS "${ARCH_COMPILE_FLAGS}")
set (ARCHCC_LIBS pthread c rt stdc++ m)
