cmake_minimum_required(VERSION 3.22)
project(ODriveFirmware C CXX ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)

set(FW_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# ── Section A: Host-native unit tests ─────────────────────────────────────────
# When no toolchain file is provided CMAKE_CROSSCOMPILING is FALSE.
# Build tests with the host compiler and exit — nothing below applies.
if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()

    add_executable(odrive_tests
        Tests/test_rotate.cpp
        Tests/test_runner.cpp
        Tests/test_timer.cpp
        Tests/test_trap_traj.cpp
    )

    target_compile_options(odrive_tests PRIVATE -O3)

    target_include_directories(odrive_tests PRIVATE
        "${FW_SRC_DIR}"
        "${FW_SRC_DIR}/MotorControl"
        "${FW_SRC_DIR}/fibre-cpp/include"
        "${FW_SRC_DIR}/Drivers/DRV8301"
        "${FW_SRC_DIR}/doctest"
    )

    add_test(NAME odrive_unit_tests COMMAND odrive_tests)
    return()
endif()

# ── Section B: Cross-compiled firmware ────────────────────────────────────────

# ── Board version ──────────────────────────────────────────────────────────────
set(BOARD_VERSION "" CACHE STRING
    "Target board variant. One of: v3.1 v3.2 v3.3 v3.4-24V v3.4-48V v3.5-24V v3.5-48V v3.6-24V v3.6-56V v4.0-56V v4.1-58V")

if(BOARD_VERSION STREQUAL "")
    message(FATAL_ERROR
        "BOARD_VERSION is not set.\n"
        "Pass -DBOARD_VERSION=v3.6-56V or use a CMake preset.\n"
        "Valid values: v3.1 v3.2 v3.3 v3.4-24V v3.4-48V v3.5-24V v3.5-48V v3.6-24V v3.6-56V v4.0-56V v4.1-58V")
endif()

# ── Build options ──────────────────────────────────────────────────────────────
option(DEBUG   "Debug build: -Og -gdwarf-2 instead of -O2" OFF)
option(USE_LTO "Enable link-time optimisation"              OFF)
option(STRICT  "Treat warnings as errors (-Werror)"         OFF)
option(NO_DRM  "Disable DRM (-DNO_DRM)"                     OFF)

# ── Decode board variant ───────────────────────────────────────────────────────
if(BOARD_VERSION STREQUAL "v3.1")
    set(HW_MAJOR 3)
    set(HW_MINOR 1)
    set(HW_VOLTAGE 24)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.2")
    set(HW_MAJOR 3)
    set(HW_MINOR 2)
    set(HW_VOLTAGE 24)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.3")
    set(HW_MAJOR 3)
    set(HW_MINOR 3)
    set(HW_VOLTAGE 24)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.4-24V")
    set(HW_MAJOR 3)
    set(HW_MINOR 4)
    set(HW_VOLTAGE 24)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.4-48V")
    set(HW_MAJOR 3)
    set(HW_MINOR 4)
    set(HW_VOLTAGE 48)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.5-24V")
    set(HW_MAJOR 3)
    set(HW_MINOR 5)
    set(HW_VOLTAGE 24)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.5-48V")
    set(HW_MAJOR 3)
    set(HW_MINOR 5)
    set(HW_VOLTAGE 48)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.6-24V")
    set(HW_MAJOR 3)
    set(HW_MINOR 6)
    set(HW_VOLTAGE 24)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v3.6-56V")
    set(HW_MAJOR 3)
    set(HW_MINOR 6)
    set(HW_VOLTAGE 56)
    set(HW_FAMILY "v3")
elseif(BOARD_VERSION STREQUAL "v4.0-56V")
    set(HW_MAJOR 4)
    set(HW_MINOR 0)
    set(HW_VOLTAGE 56)
    set(HW_FAMILY "v4")
elseif(BOARD_VERSION STREQUAL "v4.1-58V")
    set(HW_MAJOR 4)
    set(HW_MINOR 1)
    set(HW_VOLTAGE 58)
    set(HW_FAMILY "v4")
else()
    message(FATAL_ERROR "Unknown BOARD_VERSION: '${BOARD_VERSION}'")
endif()

message(STATUS "Board: ${BOARD_VERSION}  (HW v${HW_MAJOR}.${HW_MINOR}, ${HW_VOLTAGE}V)")

# ── Python venv auto-setup ─────────────────────────────────────────────────────
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(VENV_DIR   "${FW_SRC_DIR}/.venv")
set(VENV_STAMP "${CMAKE_BINARY_DIR}/venv_ready.stamp")

if(WIN32)
    set(PYTHON_VENV_EXE "${VENV_DIR}/Scripts/python.exe")
else()
    set(PYTHON_VENV_EXE "${VENV_DIR}/bin/python")
endif()

add_custom_command(
    OUTPUT  "${VENV_STAMP}"
    COMMAND "${Python3_EXECUTABLE}" -m venv "${VENV_DIR}"
    COMMAND "${PYTHON_VENV_EXE}" -m pip install --upgrade pip --quiet
    COMMAND "${PYTHON_VENV_EXE}" -m pip install
                -r "${FW_SRC_DIR}/requirements-codegen.txt" --quiet
    COMMAND "${CMAKE_COMMAND}" -E touch "${VENV_STAMP}"
    DEPENDS "${FW_SRC_DIR}/requirements-codegen.txt"
    COMMENT "Setting up Python venv for code generation"
    VERBATIM
)
add_custom_target(venv_setup DEPENDS "${VENV_STAMP}")

# ── Code generation ───────────────────────────────────────────────────────────
set(AUTOGEN_DIR "${CMAKE_BINARY_DIR}/autogen")
file(MAKE_DIRECTORY "${AUTOGEN_DIR}")
set(INTERFACE_YAML "${FW_SRC_DIR}/odrive-interface.yaml")
set(GEN_STUB       "${FW_SRC_DIR}/interface_generator_stub.py")

if(HW_FAMILY STREQUAL "v3")
    set(GEN_ENDPOINTS_ARG "ODrive3")
else()
    set(GEN_ENDPOINTS_ARG "ODrive4")
endif()

# Helper macro: generates one autogen header from a Jinja2 template.
# Usage: add_codegen_header(template_basename output_filename [extra cmake args...])
macro(add_codegen_header TEMPLATE OUTPUT_FILE)
    set(_out "${AUTOGEN_DIR}/${OUTPUT_FILE}")
    set(_tpl "${FW_SRC_DIR}/fibre-cpp/${TEMPLATE}")
    add_custom_command(
        OUTPUT  "${_out}"
        COMMAND "${PYTHON_VENV_EXE}" -B "${GEN_STUB}"
                    --definitions "${INTERFACE_YAML}"
                    --template    "${_tpl}"
                    ${ARGN}
                    --output      "${_out}"
        DEPENDS "${_tpl}" "${INTERFACE_YAML}" "${VENV_STAMP}"
        COMMENT "Generating ${OUTPUT_FILE}"
        VERBATIM
    )
endmacro()

add_codegen_header(interfaces_template.j2     interfaces.hpp)
add_codegen_header(function_stubs_template.j2 function_stubs.hpp)
add_codegen_header(endpoints_template.j2      endpoints.hpp
                   --generate-endpoints "${GEN_ENDPOINTS_ARG}")
add_codegen_header(type_info_template.j2      type_info.hpp)

# version.c — uses only stdlib; no venv needed
add_custom_command(
    OUTPUT  "${AUTOGEN_DIR}/version.c"
    COMMAND "${Python3_EXECUTABLE}" -B
                "${FW_SRC_DIR}/../tools/odrive/version.py"
                --output "${AUTOGEN_DIR}/version.c"
    DEPENDS "${FW_SRC_DIR}/../.git/HEAD"
    COMMENT "Generating version.c"
    VERBATIM
)

add_custom_target(odrive_codegen
    DEPENDS
        "${AUTOGEN_DIR}/interfaces.hpp"
        "${AUTOGEN_DIR}/function_stubs.hpp"
        "${AUTOGEN_DIR}/endpoints.hpp"
        "${AUTOGEN_DIR}/type_info.hpp"
        "${AUTOGEN_DIR}/version.c"
)

# ── Board-family configuration ─────────────────────────────────────────────────

set(BOARD_CPU_FLAGS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16)

set(BOARD_COMPILE_DEFS
    ARM_MATH_CM4
    FPU_FPV4
    STM32F405xx
    HW_VERSION_MAJOR=${HW_MAJOR}
    HW_VERSION_MINOR=${HW_MINOR}
    HW_VERSION_VOLTAGE=${HW_VOLTAGE}
    BOARD_CONFIG_HEADER="board-v${HW_MAJOR}.${HW_MINOR}.h"
)

set(BOARD_LINKER_SCRIPT "${FW_SRC_DIR}/Board/v3/STM32F405RGTx_FLASH.ld")
set(BOARD_MATH_LIB arm_cortexM4lf_math)

set(BOARD_INCLUDE_DIRS
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Inc"
    "${FW_SRC_DIR}/Board/v3/Inc"
    "${FW_SRC_DIR}/Drivers/DRV8301"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F"
)

set(BOARD_SOURCES
    "${FW_SRC_DIR}/Board/v3/startup_stm32f405xx.s"
    "${FW_SRC_DIR}/Board/v3/board.cpp"
    "${FW_SRC_DIR}/Board/v3/Src/stm32f4xx_hal_timebase_TIM.c"
    "${FW_SRC_DIR}/Board/v3/Src/tim.c"
    "${FW_SRC_DIR}/Board/v3/Src/dma.c"
    "${FW_SRC_DIR}/Board/v3/Src/freertos.c"
    "${FW_SRC_DIR}/Board/v3/Src/main.c"
    "${FW_SRC_DIR}/Board/v3/Src/usbd_conf.c"
    "${FW_SRC_DIR}/Board/v3/Src/spi.c"
    "${FW_SRC_DIR}/Board/v3/Src/usart.c"
    "${FW_SRC_DIR}/Board/v3/Src/usbd_cdc_if.c"
    "${FW_SRC_DIR}/Board/v3/Src/adc.c"
    "${FW_SRC_DIR}/Board/v3/Src/stm32f4xx_hal_msp.c"
    "${FW_SRC_DIR}/Board/v3/Src/usbd_desc.c"
    "${FW_SRC_DIR}/Board/v3/Src/stm32f4xx_it.c"
    "${FW_SRC_DIR}/Board/v3/Src/usb_device.c"
    "${FW_SRC_DIR}/Board/v3/Src/can.c"
    "${FW_SRC_DIR}/Board/v3/Src/system_stm32f4xx.c"
    "${FW_SRC_DIR}/Board/v3/Src/gpio.c"
    "${FW_SRC_DIR}/Board/v3/Src/i2c.c"
    "${FW_SRC_DIR}/Drivers/DRV8301/drv8301.cpp"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
    # STM32F4xx HAL
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c"
    "${FW_SRC_DIR}/ThirdParty/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c"
)



# ── Fibre library sources ──────────────────────────────────────────────────────
# Configured with: enable_server=true, enable_client=false, allow_heap=false,
# max_log_verbosity=0 (→ logging.cpp excluded per package.lua logic)
set(FIBRE_SOURCES
    "${FW_SRC_DIR}/fibre-cpp/fibre.cpp"
    "${FW_SRC_DIR}/fibre-cpp/channel_discoverer.cpp"
    "${FW_SRC_DIR}/fibre-cpp/legacy_protocol.cpp"
)

set(FIBRE_COMPILE_DEFS
    FIBRE_ENABLE_SERVER=1
    FIBRE_ENABLE_CLIENT=0
    FIBRE_ENABLE_EVENT_LOOP=0
    FIBRE_ALLOW_HEAP=0
    FIBRE_MAX_LOG_VERBOSITY=0
    FIBRE_DEFAULT_LOG_VERBOSITY=2
    FIBRE_ENABLE_LIBUSB_BACKEND=0
    FIBRE_ENABLE_TCP_SERVER_BACKEND=0
    FIBRE_ENABLE_TCP_CLIENT_BACKEND=0
)

# ── Common firmware sources ────────────────────────────────────────────────────
set(COMMON_SOURCES
    "${FW_SRC_DIR}/syscalls.c"
    "${FW_SRC_DIR}/FreeRTOS-openocd.c"
    "${CMAKE_BINARY_DIR}/autogen/version.c"
    "${FW_SRC_DIR}/MotorControl/utils.cpp"
    "${FW_SRC_DIR}/MotorControl/arm_sin_f32.c"
    "${FW_SRC_DIR}/MotorControl/arm_cos_f32.c"
    "${FW_SRC_DIR}/MotorControl/low_level.cpp"
    "${FW_SRC_DIR}/MotorControl/axis.cpp"
    "${FW_SRC_DIR}/MotorControl/motor.cpp"
    "${FW_SRC_DIR}/MotorControl/thermistor.cpp"
    "${FW_SRC_DIR}/MotorControl/encoder.cpp"
    "${FW_SRC_DIR}/MotorControl/endstop.cpp"
    "${FW_SRC_DIR}/MotorControl/acim_estimator.cpp"
    "${FW_SRC_DIR}/MotorControl/mechanical_brake.cpp"
    "${FW_SRC_DIR}/MotorControl/controller.cpp"
    "${FW_SRC_DIR}/MotorControl/foc.cpp"
    "${FW_SRC_DIR}/MotorControl/open_loop_controller.cpp"
    "${FW_SRC_DIR}/MotorControl/oscilloscope.cpp"
    "${FW_SRC_DIR}/MotorControl/sensorless_estimator.cpp"
    "${FW_SRC_DIR}/MotorControl/trapTraj.cpp"
    "${FW_SRC_DIR}/MotorControl/pwm_input.cpp"
    "${FW_SRC_DIR}/MotorControl/main.cpp"
    "${FW_SRC_DIR}/Drivers/STM32/stm32_system.cpp"
    "${FW_SRC_DIR}/Drivers/STM32/stm32_gpio.cpp"
    "${FW_SRC_DIR}/Drivers/STM32/stm32_nvm.c"
    "${FW_SRC_DIR}/Drivers/STM32/stm32_spi_arbiter.cpp"
    "${FW_SRC_DIR}/communication/communication.cpp"
    "${FW_SRC_DIR}/communication/ascii_protocol.cpp"
    "${FW_SRC_DIR}/communication/interface_uart.cpp"
    "${FW_SRC_DIR}/communication/interface_usb.cpp"
    "${FW_SRC_DIR}/communication/interface_i2c.cpp"
    # FreeRTOS kernel
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/croutine.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/event_groups.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/list.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/queue.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/stream_buffer.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/tasks.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/timers.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/portable/MemMang/heap_4.c"
    # USB device library
    "${FW_SRC_DIR}/ThirdParty/STM32_USB_Device_Library/Core/Src/usbd_core.c"
    "${FW_SRC_DIR}/ThirdParty/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c"
    "${FW_SRC_DIR}/ThirdParty/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c"
    "${FW_SRC_DIR}/ThirdParty/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c"
)

# ── Common include directories ─────────────────────────────────────────────────
set(COMMON_INCLUDE_DIRS
    "${FW_SRC_DIR}"
    "${FW_SRC_DIR}/MotorControl"
    "${FW_SRC_DIR}/fibre-cpp/include"
    "${CMAKE_BINARY_DIR}"
    "${FW_SRC_DIR}/ThirdParty/CMSIS/Include"
    "${FW_SRC_DIR}/ThirdParty/CMSIS/Device/ST/STM32F4xx/Include"
    "${FW_SRC_DIR}/ThirdParty/CMSIS/Device/ST/STM32F7xx/Include"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/include"
    "${FW_SRC_DIR}/ThirdParty/FreeRTOS/Source/CMSIS_RTOS"
    "${FW_SRC_DIR}/ThirdParty/STM32_USB_Device_Library/Core/Inc"
    "${FW_SRC_DIR}/ThirdParty/STM32_USB_Device_Library/Class/CDC/Inc"
    "${FW_SRC_DIR}/Drivers/STM32"
)

# ── Common compile flags (option-driven) ───────────────────────────────────────
set(COMMON_COMPILE_FLAGS
    -mthumb
    -mfloat-abi=hard
    -Wall
    -Wdouble-promotion
    -Wfloat-conversion
    -Wno-psabi
    -Wno-nonnull
    -fdata-sections
    -ffunction-sections
    -g
)

if(DEBUG)
    list(APPEND COMMON_COMPILE_FLAGS -gdwarf-2 -Og)
else()
    list(APPEND COMMON_COMPILE_FLAGS -O2)
endif()

if(USE_LTO)
    list(APPEND COMMON_COMPILE_FLAGS -flto)
endif()

if(STRICT)
    list(APPEND COMMON_COMPILE_FLAGS -Werror)
endif()

set(COMMON_COMPILE_DEFS
    "__weak=__attribute__((weak))"
    "__packed=__attribute__((__packed__))"
    USE_HAL_DRIVER
    FIBRE_ENABLE_SERVER
)

if(NO_DRM)
    list(APPEND COMMON_COMPILE_DEFS NO_DRM)
endif()

# ── Firmware executable ────────────────────────────────────────────────────────
add_executable(ODriveFirmware
    ${COMMON_SOURCES}
    ${BOARD_SOURCES}
    ${FIBRE_SOURCES}
)

# Autogen headers must exist before any source file is compiled
add_dependencies(ODriveFirmware odrive_codegen)

target_compile_options(ODriveFirmware PRIVATE
    $<$<COMPILE_LANGUAGE:C>:  -std=c99>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -Wno-register>
    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>
    ${COMMON_COMPILE_FLAGS}
    ${BOARD_CPU_FLAGS}
)

target_compile_definitions(ODriveFirmware PRIVATE
    ${COMMON_COMPILE_DEFS}
    ${BOARD_COMPILE_DEFS}
    ${FIBRE_COMPILE_DEFS}
)

target_include_directories(ODriveFirmware PRIVATE
    ${COMMON_INCLUDE_DIRS}
    ${BOARD_INCLUDE_DIRS}
)

target_link_options(ODriveFirmware PRIVATE
    ${BOARD_CPU_FLAGS}
    -T${BOARD_LINKER_SCRIPT}
    -mthumb
    -mfloat-abi=hard
    -specs=nosys.specs
    "SHELL:-u _printf_float"
    "SHELL:-u _scanf_float"
    -Wl,--cref
    -Wl,--gc-sections
    -Wl,--undefined=uxTopUsedPriority
    -flto
    -lc -lm -lnosys
    -Wl,-Map=${CMAKE_BINARY_DIR}/ODriveFirmware.map
)

target_link_libraries(ODriveFirmware PRIVATE ${BOARD_MATH_LIB})

target_link_directories(ODriveFirmware PRIVATE
    "${FW_SRC_DIR}/ThirdParty/CMSIS/Lib/GCC"
)

# Re-link when the linker script is modified
set_target_properties(ODriveFirmware PROPERTIES
    LINK_DEPENDS "${BOARD_LINKER_SCRIPT}"
    SUFFIX ".elf"
)

# ── Post-build: generate .hex and .bin ────────────────────────────────────────
add_custom_command(TARGET ODriveFirmware POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}" -O ihex
            "$<TARGET_FILE:ODriveFirmware>"
            "${CMAKE_BINARY_DIR}/ODriveFirmware.hex"
    COMMAND "${CMAKE_OBJCOPY}" -O binary -S
            "$<TARGET_FILE:ODriveFirmware>"
            "${CMAKE_BINARY_DIR}/ODriveFirmware.bin"
    COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:ODriveFirmware>"
    COMMENT "Generating ODriveFirmware.hex / .bin"
    VERBATIM
)
