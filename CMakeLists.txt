# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(kgc C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(kgc kgc.c max7219.h max7219.c keypad.h keypad.c connection.h connection.c led.h led.c errors.h errors.c)

pico_set_program_name(kgc "kgc")
pico_set_program_version(kgc "0.4.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(kgc 0)
pico_enable_stdio_usb(kgc 1)

# Add the standard library to the build
target_link_libraries(kgc
        pico_stdlib
        hardware_spi
        pico_binary_info
        m
        pico_multicore)

# Add the standard include files to the build
target_include_directories(kgc PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(kgc)

