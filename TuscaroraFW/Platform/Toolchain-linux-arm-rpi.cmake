# this one is important
SET (CMAKE_SYSTEM_NAME Linux)
# this one not so much
SET (CMAKE_SYSTEM_VERSION 1)
#SET (CMAKE_SYSTEM_PROCESSOR ARM)

#Useful links
#http://stackoverflow.com/questions/19162072/installing-raspberry-pi-cross-compiler
#https://www.raspberrypi.org/forums/viewtopic.php?t=11315&p=129461

#Get this tools dir from github.com/raspberrypi/tools
SET (PI_TOOLS_HOME $ENV{HOME}/RPi/tools)
#SET (PiToolsDir ${PI_TOOLS_HOME}/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi)
#SET (CompPrefix arm-bcm2708hardfp-linux-gnueabi-)
SET (PiToolsDir ${PI_TOOLS_HOME}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64)
SET (CompPrefix arm-linux-gnueabihf-)

# specify the cross compiler
SET (CMAKE_C_COMPILER ${PiToolsDir}/bin/${CompPrefix}gcc)
SET (CMAKE_CXX_COMPILER ${PiToolsDir}/bin/${CompPrefix}g++)
SET (CMAKE_AR ${PiToolsDir}/bin/${CompPrefix}ar)
SET (ARCH_CMAKE_AR ${PiToolsDir}/bin/${CompPrefix}ar)

# where is the target environment
SET (CMAKE_FIND_ROOT_PATH ${PiToolsDir})

# search for programs in the build host directories
SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set architecture specific (x86_64) build flags here. They will be joined
# to GCC_COMPILE_FLAGS in ../CMakeLists.txt
SET (ARCH_COMPILE_FLAGS "-Wall -pthread -Wl,--no-as-needed")
SET (ARCHCC_COMPILE_FLAGS "-std=c++11 ${ARCH_COMPILE_FLAGS}")
SET (ARCHC_COMPILE_FLAGS "${ARCH_COMPILE_FLAGS}")
SET (ARCHCC_LIBS pthread c rt stdc++ m)

set (ARCH_CMAKE_EXE_LINKER_FLAGS "-lrt -ldl -pie -rdynamic")
