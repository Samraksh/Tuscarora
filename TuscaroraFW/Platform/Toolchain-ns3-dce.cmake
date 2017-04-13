# This is important
set(CMAKE_SYSTEM_NAME DCE)

# This one not so much
# -- Unclear what this is used for...
set(CMAKE_SYSTEM_VERSION 3)
set(CMAKE_SYSTEM_PROCESSOR DCE)
set(SDK_PATH $ENV{HOME}/dce)
#set(SDK_PATH_NATIVE ${SDK_PATH}/sysroots/i686-arago-linux)
#set(SDK_PATH_TARGET ${SDK_PATH}/sysroots/cortexa15hf-vfp-neon-3.8-oe-linux-gnueabi)

#set(CMAKE_FIND_ROOT_PATH ${SDK_PATH_TARGET})

set (COMPILER_PATH /usr/bin)
# Specify the cross compiler
set (CMAKE_C_COMPILER  ${COMPILER_PATH}/gcc)
set (CMAKE_CXX_COMPILER ${COMPILER_PATH}/g++)
set (CMAKE_AR ${COMPILER_PATH}/ar)
set (ARCH_CMAKE_AR ${COMPILER_PATH}/ar)

#set(CMAKE_SYSROOT ${SDK_PATH_TARGET})

#Below copied form original make file
#CFLAGS = -std=c++03
#CFLAGS += -g -Wall -Werror
#CFLAGS += -fPIC -g
#CFLAGS += ${USER_DEFINES}
#LDFLAGS = -lrt -ldl
#DCE_LDFLAGS = -pie -rdynamic
#DLL_LDFLAGS = -shared

# Set architecture specific (arm) build flags here. They will be joined
# to GCC_COMPILE_FLAGS in ../CMakeLists.txt
#-DAMPTASK_ARCH=ARCH_arm
#set (COMPILE_FLAGS "-Wall -fPIC -g -pthread -Wl,--no-as-needed -DUSE_NANOSLEEP")
set (COMPILE_FLAGS "-Wall -fPIC -g ")
set (ARCHCC_COMPILE_FLAGS "-std=c++11 ${COMPILE_FLAGS}")
set (ARCHC_COMPILE_FLAGS ${COMPILE_FLAGS})
set (LINK_FLAGS "-lrt -ldl -pie -rdynamic")
set (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}" )
#set (CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${LINK_FLAGS}" )
#set (IPC_PATH $ENV{TUS}/Toolchains/ipc_3_35_00_04_eng)
#set (ARCHCC_LIBS pthread c rt stdc++ m mpmclient
#                 ${IPC_PATH}/linux/src/transport/.libs/libtitransportrpmsg.a
#                 ${IPC_PATH}/linux/src/api/.libs/libtiipc.a
#                 ${IPC_PATH}/linux/src/utils/.libs/libtiipcutils.a)
#set (NS3_VER  "325")

