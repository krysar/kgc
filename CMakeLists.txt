cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
project(kgc C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(kgc kgc.c max7219.h max7219.c keypad.h keypad.c)

target_link_libraries(kgc
    pico_stdlib
        hardware_spi
        pico_binary_info
        m
        pico_multicore
)


pico_enable_stdio_usb(kgc 1)
pico_enable_stdio_uart(kgc 0)
pico_add_extra_outputs(kgc)
