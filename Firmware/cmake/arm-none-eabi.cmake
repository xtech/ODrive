set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Optional: set ARM_COMPILER_PATH to the directory containing arm-none-eabi-* binaries
if(DEFINED ARM_COMPILER_PATH AND NOT ARM_COMPILER_PATH STREQUAL "")
    set(_PFX "${ARM_COMPILER_PATH}/arm-none-eabi-")
else()
    set(_PFX "arm-none-eabi-")
endif()

set(CMAKE_C_COMPILER   "${_PFX}gcc")
set(CMAKE_CXX_COMPILER "${_PFX}g++")
set(CMAKE_ASM_COMPILER "${_PFX}gcc")
set(CMAKE_OBJCOPY      "${_PFX}objcopy" CACHE INTERNAL "")
set(CMAKE_SIZE         "${_PFX}size"    CACHE INTERNAL "")

# Don't try to link test executables against the target — they can't run on the host
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Only search target sysroot for libs/includes, not the host system
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
