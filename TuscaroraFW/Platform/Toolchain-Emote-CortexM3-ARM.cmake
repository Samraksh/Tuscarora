# the name of the target operating system
#
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm-eabi)
set(CMAKE_SYSTEM_VERSION 3)

set(REPO_PATH D:/Main/Repos)
#set(REPO_PATH /mnt/d/Main/Repos)
set(TOOLCHAIN_PATH ${REPO_PATH}/TestSystem/Compilers/GCC5.4.1)
#set(SDK_PATH_NATIVE ${SDK_PATH}/sysroots/i686-arago-linux)
set(SDK_PATH_TARGET ${TOOLCHAIN_PATH}/arm-none-eabi)

set(CMAKE_FIND_ROOT_PATH ${SDK_PATH_TARGET})

# Specify the cross compiler
set (COMPILER_PATH ${TOOLCHAIN_PATH}/bin)
set (CMAKE_C_COMPILER  ${COMPILER_PATH}/arm-none-eabi-gcc.exe)
set (CMAKE_CXX_COMPILER ${COMPILER_PATH}/arm-none-eabi-g++.exe)
set (CMAKE_AR ${COMPILER_PATH}/arm-none-eabi-ar.exe)
set (ARCH_CMAKE_AR ${COMPILER_PATH}/arm-none-eabi-ar.exe)
set (CMAKE_ASM_COMPILER     ${COMPILER_PATH}/arm-none-eabi-as.exe)
set (CMAKE_OBJCOPY     	   ${COMPILER_PATH}/arm-none-eabi-objcopy.exe)
set (CMAKE_OBJDUMP     	   ${COMPILER_PATH}/arm-none-eabi-objdump.exe)

set(CMAKE_SYSROOT ${SDK_PATH_TARGET})

#Set this to where ever your toolchain is

SET(CMAKE_C_FLAGS "--specs=nosys.specs -mthumb -mcpu=cortex-m3 -fno-builtin -Wall -std=c++11 -fdata-sections -ffunction-sections" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "--specs=nosys.specs -mthumb -mcpu=cortex-m3 -fno-builtin -Wall -std=c++11 -fdata-sections -ffunction-sections" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "asm compiler flags")
SET(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections -mthumb -mcpu=cortex-m4" CACHE INTERNAL "exe link flags")

#link_directories(${SDK_PATH_TARGET}/lib)

include_directories(Platform/eMote)
include_directories(${TOOLCHAIN_PATH}/lib/gcc/arm-none-eabi/5.4.1/include)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment,
# search programs in the host environment
#
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
