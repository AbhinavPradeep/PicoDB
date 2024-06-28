cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PICO_BOARD pico_w)

project(picoBlink)

pico_sdk_init()

add_executable(picoBlink
    main.c
)

target_link_libraries(picoBlink 
	pico_cyw43_arch_none
	pico_stdlib
	hardware_flash
	hardware_gpio
	hardware_sync)

target_compile_options(picoBlink PRIVATE -fstack-usage)
target_compile_options(picoBlink PRIVATE -save-temps -fverbose-asm)

pico_enable_stdio_usb(picoBlink 1)
pico_enable_stdio_uart(picoBlink 0)

pico_add_extra_outputs(picoBlink)
#example_auto_set_url(picoBlink)
