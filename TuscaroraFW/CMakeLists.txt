cmake_minimum_required(VERSION 2.8.12)
set (Type "Debug" CACHE STRING "Type of build")
#set (Target "Timer" CACHE STRING "Executable name" )
#option(Framework  CACHE STRING "Build a standalone Framework process" "No")
option (AlwaysOn "Build a standalone AlwaysOn waveform process" "OFF")
option (TestDir "Test sources directory" "OFF")
option (MainFile "Main for test resides in this File" "OFF")
set (ARCH "eMote" CACHE STRING "Platform to compile for")

add_definitions(
    -Wfatal-errors
    )
    
if ("${MainFile}" STREQUAL "OFF" )
	set (MainFile "${TestDir}/run${Target}.cpp" )
endif()

#Full path of the main file
set (MainFileFP "${CMAKE_CURRENT_SOURCE_DIR}/${MainFile}" )
MESSAGE( STATUS "File with Main:         " ${MainFileFP} )

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
if( "${ARCH}" MATCHES "x86_64" )
  message("==>compling for x86_64")
  set (AMP_PLATFORM linux)
  set (PLATFORM "x86_64" CACHE STRING "Platform to compile for")
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-linux-x86_64.cmake)
  add_definitions(-DPLATFORM_LINUX=1)
elseif("${ARCH}" MATCHES "dce" )
  message("==>compling for DCE")
  set (PLATFORM "dce" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-ns3-dce.cmake)
  add_definitions(-DPLATFORM_DCE=1)
elseif("${ARCH}" MATCHES "eMote" )
  message("==>compling for eMote")
  set (PLATFORM "eMote" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-Emote-CortexM3-ARM.cmake)
  add_definitions(-DPLATFORM_EMOTE=1)
elseif("${ARCH}" MATCHES "arm" )
  message("==>compling for arm")
  set (PLATFORM "linux" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-linux-arm-arago.cmake)
  add_definitions(-DPLATFORM_LINUX=1)
elseif("${ARCH}" MATCHES "rpi" )
  message("==>compling for Raspberry Pi Linux")
  set (PLATFORM "linux" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM linux)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-linux-arm-rpi.cmake)
  add_definitions(-DPLATFORM_LINUX=1)
elseif("${ARCH}" MATCHES "c66" )
  message("==>compling for c66")
  set (PLATFORM "DSP" CACHE STRING "Platform to compile for")
  set (AMP_PLATFORM sysbiosv6)
  set (CMAKE_TOOLCHAIN_FILE Platform/Toolchain-dsp-sysbiosv6-c66.cmake)
else ()
  message("usage: cmake -DARCH=[arm|rpi|c66|x86_64] [-Dtype=[release|debug]]")
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

MESSAGE( STATUS "Platform Compiler:         " ${CMAKE_CXX_COMPILER} )
MESSAGE( STATUS "Platform Archiver:         " ${CMAKE_AR} )
MESSAGE( STATUS "Linker flags: " ${CMAKE_EXE_LINKER_FLAGS} )
#MESSAGE( STATUS "Platform Archiver:         " ${ARCH_CMAKE_AR} )


MESSAGE( STATUS "Tuscarora_DIR:         " $ENV{TUS} )
set (CMAKE_AR ${ARCH_CMAKE_AR})

include_directories(.)
include_directories(Include/)

message(STATUS "Printing List of Include Dirs")
message(STATUS "---------------------------")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "'${dir}'")
endforeach()
message(STATUS "---------------------------")

if (NOT "${ARCH}" MATCHES "eMote" )
  message("==> Including Google Test framework ")  
  #googletest includes
  #include_directories($ENV{TUS}/googletest/googletest/)
  include_directories(googletest/googletest/include/)
  add_subdirectory(googletest/googletest/)
else()
  message("==> Skipping Google Test Framework for eMote ")  
endif()

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE FW_SRCS Framework/*.cpp)
MESSAGE( STATUS "Framework Sources:         " ${FW_SRCS} )

file(GLOB_RECURSE APP_SRCS Apps/*.cpp)
MESSAGE( STATUS "Application Sources:         " ${APP_SRCS} )

file(GLOB_RECURSE PTN_SRCS Patterns/*.cpp)
MESSAGE( STATUS "Pattern Sources:         " ${PTN_SRCS} )


file(GLOB_RECURSE LIB_SRCS Lib/*.cpp)
MESSAGE( STATUS "Common Library Sources:         " ${LIB_SRCS} )

file(GLOB_RECURSE PLAT_SRCS Platform/${PLATFORM}/*.cpp)
MESSAGE( STATUS "Platform (${PLATFORM}) Sources:         " ${PLAT_SRCS} )

if( "${ARCH}" MATCHES "dce" )
	file(GLOB_RECURSE PLAT_ADD_SRCS Platform/linux/*.cpp)
	
	#Below line removes LocationService.cpp & PlatformConfig.cpp, since its already present under dce, need a better solution
	list(REMOVE_AT PLAT_ADD_SRCS 0 8) 
	#list(FILTER PLAT_ADD_SRCS EXCLUDE REGEX "*/LocationService.cpp")
	MESSAGE( STATUS "Platform (${PLATFORM}) Posix Sources:         " ${PLAT_ADD_SRCS} )
	list(APPEND PLAT_SRCS ${PLAT_ADD_SRCS})
	
	file(GLOB_RECURSE PLAT_SHIM_SRCS Platform/Shims/DirectBinding/*.cpp)
	MESSAGE( STATUS "Platform (${PLATFORM}) Shim Sources:         " ${PLAT_SHIM_SRCS} )
	list(APPEND PLAT_SRCS ${PLAT_SHIM_SRCS})
endif ()


if( "${ARCH}" MATCHES "x86_64" )
	file(GLOB_RECURSE PLAT_SHIM_SRCS Platform/Shims/LinuxSocket/*.cpp)
	MESSAGE( STATUS "Platform (${PLATFORM}) Shim Sources:         " ${PLAT_SHIM_SRCS} )
	list(APPEND PLAT_SRCS ${PLAT_SHIM_SRCS})
endif ()


#file(GLOB INIT_SRC Tests/Init/*.cpp)
#MESSAGE(STATUS "Initialization Sources:         " ${INIT_SRC} )

file(GLOB_RECURSE TEST_SRCS Tests/*.cpp)

file(GLOB_RECURSE ALL_RUN_FILES Tests/run*.cpp)

file(GLOB_RECURSE VAL_SRCS Tests/*/validationFiles/*.cpp)

list(REMOVE_ITEM TEST_SRCS ${ALL_RUN_FILES})
list(REMOVE_ITEM TEST_SRCS ${VAL_SRCS})

#file(GLOB GTEST_SRC $ENV{TUS}/googletest/googletest/src/*.cc)
#MESSAGE(STATUS "Googletest Sources:         " ${GTEST_SRC} )

#if a CMakeLists.txt is available in the test folder include that and add those targets to the main target
#otherwise just build using default mainfile
if (EXISTS "${TestDir}/CMakeLists.txt")
	add_subdirectory(${TestDir})
	set (subTargets "sub${Target}")
	message ("Adding subtarget: " ${subTargets} )

	add_executable(${Target} ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFileFP} $<TARGET_OBJECTS:${subTargets}>)
else ()
	if ("${ARCH}" MATCHES "dce" )
		message("//////Building for DCE: Target: " ${Target} " //////")
		set (LINK_FLAGS "-lrt -ldl -pie -rdynamic")
		#set (CMAKE_MODULE_LINKER_FLAGS  "${LINK_FLAGS}" )
		#add_library(${Target} MODULE ${SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFileFP} )
		set (CMAKE_EXE_LINKER_FLAGS  "${LINK_FLAGS}" )
		add_executable(${Target} ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFileFP} )
		SET_TARGET_PROPERTIES(${Target} PROPERTIES PREFIX "" SUFFIX "")
		target_link_libraries(${Target} gtest)
		
	else ()
		add_executable(${Target} ${FW_SRCS} ${APP_SRCS} ${PTN_SRCS} ${LIB_SRCS} ${PLAT_SRCS} ${TEST_SRCS} ${MainFileFP} )
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
