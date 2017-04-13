# This is important
set(CMAKE_SYSTEM_NAME Linux)

# This one not so much
# -- Unclear what this is used for...
set(CMAKE_SYSTEM_VERSION 3)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(SDK_PATH $ENV{TUS}/Toolchains/arago-2013.12)
set(SDK_PATH_NATIVE ${SDK_PATH}/sysroots/i686-arago-linux)
set(SDK_PATH_TARGET ${SDK_PATH}/sysroots/cortexa15hf-vfp-neon-3.8-oe-linux-gnueabi)

set(CMAKE_FIND_ROOT_PATH ${SDK_PATH_TARGET})

set (COMPILER_PATH ${SDK_PATH_NATIVE}/usr/bin)
# Specify the cross compiler
set (CMAKE_C_COMPILER  ${COMPILER_PATH}/arm-linux-gnueabihf-gcc)
set (CMAKE_CXX_COMPILER ${COMPILER_PATH}/arm-linux-gnueabihf-g++)
set (CMAKE_AR ${COMPILER_PATH}/arm-linux-gnueabihf-ar)
set (ARCH_CMAKE_AR ${COMPILER_PATH}/arm-linux-gnueabihf-ar)

set(CMAKE_SYSROOT ${SDK_PATH_TARGET})


# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set architecture specific (arm) build flags here. They will be joined
# to GCC_COMPILE_FLAGS in ../CMakeLists.txt
set (COMPILE_FLAGS "-Wall -pthread -Wl,--no-as-needed -march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a15 -DUSE_NANOSLEEP --sysroot=${SDK_PATH_TARGET} -DAMPTASK_ARCH=ARCH_arm")
set (ARCHCC_COMPILE_FLAGS "-std=c++11 ${COMPILE_FLAGS}")
set (ARCHC_COMPILE_FLAGS ${COMPILE_FLAGS})
set (GCC_LINK_FLAGS "--sysroot=${SDK_PATH_TARGET}")
set (IPC_PATH $ENV{TUS}/Toolchains/ipc_3_35_00_04_eng)
set (ARCHCC_LIBS pthread c rt stdc++ m mpmclient
                 ${IPC_PATH}/linux/src/transport/.libs/libtitransportrpmsg.a
                 ${IPC_PATH}/linux/src/api/.libs/libtiipc.a
                 ${IPC_PATH}/linux/src/utils/.libs/libtiipcutils.a)
