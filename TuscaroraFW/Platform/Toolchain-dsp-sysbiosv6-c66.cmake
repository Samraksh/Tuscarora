# This is important
#set(CMAKE_SYSTEM_NAME sysbiosv6)

# This one not so much
# -- Unclear what this is used for...

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_VERSION "1.0")
set(CMAKE_SYSTEM_PROCESSOR "C6000")

# Specify the compiler and environment
set (TI_DIR           /home/c2e/ti)
set (CGT_DIR          ${TI_DIR}/ccsv6/tools/compiler/c6000_7.4.12)
set (BIOS_INSTALL_DIR ${TI_DIR}/bios_6_40_04_47)
set (IPC_INSTALL_DIR  ${TI_DIR}/ipc_3_35_00_04_eng)
set (XDC_INSTALL_DIR  ${TI_DIR}/xdctools_3_30_04_52)

set (XDC_PATH "${TI_DIR}/edma3_lld_02_11_13_17/packages\;${IPC_INSTALL_DIR}/packages\;${BIOS_INSTALL_DIR}/packages\;${TI_DIR}/pdk_keystone2_3_01_02_05/packages\;${TI_DIR}/uia_2_00_01_34/packages\;${TI_DIR}/ccsv6/ccs_base\;${TI_DIR}/mathlib_c66x_3_1_0_0/packages\;${TI_DIR}/ctoolslib_1_1_1_0/packages")

set (HAVE_MATHLIB 1)
set (MATHLIB_DIR ${TI_DIR}/mathlib_c66x_3_1_0_0/packages)

set (HAVE_DSPLIB 1)
set (DSPLIB_DIR ${TI_DIR}/dsplib_c66x_3_4_0_0/packages)

set (C66_DEFS "-Dxdc_target_types__=ti/targets/std.h -Dxdc_target_name__=C66")
set (CMAKE_C_COMPILER   ${CGT_DIR}/bin/cl6x)
set (CMAKE_CXX_COMPILER ${CGT_DIR}/bin/cl6x)
set (CMAKE_LINKER       ${CGT_DIR}/bin/cl6x)
set (CMAKE_STRIP        ${CGT_DIR}/bin/strip6x)

# The target environment, used to search for includes and linked libraries
set(CMAKE_FIND_ROOT_PATH  ${CGT_DIR})

include_directories(${CGT_DIR}/include
                    ${BIOS_INSTALL_DIR}/packages
                    ${XDC_INSTALL_DIR}/packages
                    ${XDC_PATH})

# Search for programs in the build host directories)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set architecture specific build flags here. They will be joined
# to ARCHCC_COMPILE_FLAGS in ../CMakeLists.txt
set (COMPILE_FLAGS "-mv6600 --abi=eabi --diag_warning=225 ${C66_DEFS} -DUSE_NANOSLEEP --define=DEVICE_K2H -DAMPTASK_ARCH=ARCH_c66")

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -g")
  set(XDC_PROFILE "debug")
elseif("${CMAKE_BUILD_TYPE}" MATCHES "MinSizeRel")
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -Os -DNDEBUG")
  set(XDC_PROFILE "release")
else()
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -O3 -DNDEBUG")
  set(XDC_PROFILE "release")
endif()

set (ARCHC_COMPILE_FLAGS ${COMPILE_FLAGS})
set (ARCHCC_COMPILE_FLAGS ${COMPILE_FLAGS})

set (ARCHCC_LIBS ${CGT_DIR}/lib/rts6600_elf.lib)

# Enable stdint.h type limit macros in C++ for C66.
set (ARCHCC_COMPILE_FLAGS "${ARCHCC_COMPILE_FLAGS} -D__STDC_LIMIT_MACROS")

# Enable TI libraries.
if(${HAVE_MATHLIB})
  include_directories(${MATHLIB_DIR})
  set(ARCHC_COMPILE_FLAGS "${ARCHC_COMPILE_FLAGS} -DHAVE_MATHLIB")
  set(ARCHCC_COMPILE_FLAGS "${ARCHCC_COMPILE_FLAGS} -DHAVE_MATHLIB")
  set(ARCHCC_LIBS ${ARCHCC_LIBS} ${MATHLIB_DIR}/ti/mathlib/lib/mathlib.lib)

  # The following file _should_ be built into mathlib.lib, but it isn't for some
  # reason. It contains lookup tables used by the mathlib functions.
  #
  # Reference: http://e2e.ti.com/support/dsp/c6000_multi-core_dsps/f/639/p/317616/1106098
  set(PLATFORM_SOURCES ${PLATFORM_SOURCES} ${MATHLIB_DIR}/ti/mathlib/src/common/tables.c)
endif()

if(${HAVE_DSPLIB})
  include_directories(${DSPLIB_DIR})
  set(ARCHC_COMPILE_FLAGS "${ARCHC_COMPILE_FLAGS} -DHAVE_DSPLIB")
  set(ARCHCC_COMPILE_FLAGS "${ARCHCC_COMPILE_FLAGS} -DHAVE_DSPLIB")
  set(ARCHCC_LIBS ${ARCHCC_LIBS} ${DSPLIB_DIR}/ti/dsplib/lib/dsplib.lib)
endif()
