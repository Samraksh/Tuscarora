cmake_minimum_required(VERSION 2.8.12)
option(Type CACHE "Type of build")
option(Target CACHE "Executable name" )
option(Framework "Build a standalone Framework process" "No")
option(AlwaysOn "Build a standalone AlwaysOn waveform process" "No")
option(TestDir CACHE "Test sources directory")
option(MainFile CACHE "Main for test resides in this File")
#set (arch "x86_64" CACHE STRING "Platform to compile for" FORCE)

add_definitions(
    -Wfatal-errors
    )
    
if ("${MainFile}" STREQUAL "OFF" )
	set (MainFile "${TestDir}/run${Target}.cpp" )
endif()

Message("using " ${MainFile} " as main file")
# specify the build type
# This has to be set _after_ specifying the project.
if( "${Type}" MATCHES "release" )
  set (CMAKE_BUILD_TYPE Release)
elseif ( "${type}" MATCHES "min" )
  set (CMAKE_BUILD_TYPE MinSizeRel)
else ()
  set (CMAKE_BUILD_TYPE Debug)
endif ()

# This has to be set _before_specifying the project.
if( "${arch}" MATCHES "x86_64" )
  message("==>compling for x86_64")
  set (AMP_PLATFORM linux)
  set (PLATFORM "x86_64" CACHE STRING "Platform to compile for")
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-linux-x86_64.cmake)
  add_definitions(-DPLATFORM_LINUX=1)
elseif("${arch}" MATCHES "dce" )
  message("==>compling for DCE")
  set (PLATFORM "dce" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-ns3-dce.cmake)
  add_definitions(-DPLATFORM_DCE=1)
elseif("${arch}" MATCHES "arm" )
  message("==>compling for arm")
  set (PLATFORM "linux" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-linux-arm-arago.cmake)
  add_definitions(-DPLATFORM_LINUX=1)
elseif("${arch}" MATCHES "rpi" )
  message("==>compling for Raspberry Pi Linux")
  set (PLATFORM "linux" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-linux-arm-rpi.cmake)
  add_definitions(-DPLATFORM_LINUX=1)
elseif("${arch}" MATCHES "c66" )
  message("==>compling for c66")
  set (PLATFORM "DSP" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM sysbiosv6)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-dsp-sysbiosv6-c66.cmake)
else ()
  message("usage: cmake -Darch=[arm|rpi|c66|x86_64] [-Dtype=[release|debug]]")
  return()
endif ()

project(Tuscarora)

# Create a configuration file to set the version information
set (VERSION_MAJOR 1)
set (VERSION_MINOR 1)
set (VERSION_BUILD 1)
set (VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD})
# The syntax is weird, but the following puts the .h version
# in the same directory as .h.in.
#configure_file (
#  "include/version.h.in"
#  "include/version.h"
#)

# Show the make commands
set (CMAKE_VERBOSE_MAKEFILE ON)

# To set architecture specific flags, modify ARCHCC_COMPILE_FLAGS in
# the appropriate Platform/*-*.cmake file
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCHCC_COMPILE_FLAGS} ${CXX_LOGGING} -fPIC")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCHC_COMPILE_FLAGS} ${CXX_LOGGING} -fPIC")
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARCH_CMAKE_EXE_LINKER_FLAGS}")

MESSAGE( STATUS "Linker flags: " ${CMAKE_EXE_LINKER_FLAGS} )
MESSAGE( STATUS "Platform Archiver:         " ${ARCH_CMAKE_AR} )
MESSAGE( STATUS "Tuscarora_DIR:         " $ENV{TUS} )
set (CMAKE_AR ${ARCH_CMAKE_AR})

#Bring the headers, such as Student.h into the project
include_directories($ENV{TUS}/)
include_directories(Include/)

#googletest includes
#include_directories($ENV{TUS}/googletest/googletest/)
include_directories($ENV{TUS}/googletest/googletest/include/)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE FW_SRCS Framework/*.cpp)
MESSAGE( STATUS "Framework Sources:         " ${FW_SRCS} )

file(GLOB_RECURSE APP_SRCS Apps/*.cpp)
MESSAGE( STATUS "Pattern Sources:         " ${APP_SRCS} )

file(GLOB_RECURSE PTN_SRCS Patterns/*.cpp)
MESSAGE( STATUS "Pattern Sources:         " ${PTN_SRCS} )


file(GLOB_RECURSE LIB_SRCS Lib/*.cpp)
MESSAGE( STATUS "Common Library Sources:         " ${LIB_SRCS} )

file(GLOB_RECURSE PLAT_SRCS Platform/${PLATFORM}/*.cpp)
MESSAGE( STATUS "Platform (${PLATFORM}) Sources:         " ${PLAT_SRCS} )

#file(GLOB INIT_SRC Tests/Init/*.cpp)
#MESSAGE(STATUS "Initialization Sources:         " ${INIT_SRC} )

file(GLOB_RECURSE TEST_SRCS Tests/*.cpp)

file(GLOB_RECURSE ALL_RUN_FILES Tests/run*.cpp)

file(GLOB_RECURSE VAL_SRCS Tests/*/validationFiles/*.cpp)

list(REMOVE_ITEM TEST_SRCS ${ALL_RUN_FILES})
list(REMOVE_ITEM TEST_SRCS ${VAL_SRCS})

#file(GLOB GTEST_SRC $ENV{TUS}/googletest/googletest/src/*.cc)
#MESSAGE(STATUS "Googletest Sources:         " ${GTEST_SRC} )

add_subdirectory($ENV{TUS}/googletest/googletest/)

#if a CMakeLists.txt is available in the test folder include that and add those targets to the main target
#otherwise just build using default mainfile
if (EXISTS "${TestDir}/CMakeLists.txt")
	add_subdirectory(${TestDir})
	set (subTargets "sub${Target}")
	message ("Adding subtarget: " ${subTargets} )
	add_executable(${Target} ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFile} $<TARGET_OBJECTS:${subTargets}>)
else ()
	if ("${arch}" MATCHES "dce" )
		message("//////Building for DCE: Target: " ${Target} " //////")
		set (LINK_FLAGS "-lrt -ldl -pie -rdynamic")
		#set (CMAKE_MODULE_LINKER_FLAGS  "${LINK_FLAGS}" )
		#add_library(${Target} MODULE ${SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFile} )
		set (CMAKE_EXE_LINKER_FLAGS  "${LINK_FLAGS}" )
		add_executable(${Target} ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFile} )
		SET_TARGET_PROPERTIES(${Target} PROPERTIES PREFIX "" SUFFIX "")
		target_link_libraries(${Target} gtest)
		
	else ()
		add_executable(${Target} ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFile} )
		target_link_libraries(${Target} gtest)
	endif()
	
endif()



if( "${Framework}" MATCHES "yes"  OR "${Framework}" MATCHES "Yes" )
	add_subdirectory(Tests/FI)
	MESSAGE( STATUS "Building the framework:         "  )
	add_executable(Framework ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} Tests/FI/runFIBasic.cpp $<TARGET_OBJECTS:FW_Init>)
endif()

if( "${AlwaysOn}" MATCHES "yes"  OR "${AlwaysOn}" MATCHES "Yes" )
	#add_subdirectory(Tests/FI)
	MESSAGE( STATUS "Building the Standalone AlwaysOn waveform with Link Estimation:         "  )
	#add_executable(Framework ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${LIN_SRCS} ${TEST_SRCS} Tests/FI/runFIBasic.cpp $<TARGET_OBJECTS:FW_Init>)
endif()
