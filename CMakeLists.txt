cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(nrf5340dk)

target_include_directories(app PRIVATE
    src/bsp)

target_sources(app PRIVATE 
    src/main.c
    src/myfunction.c
    src/bsp/bsp_gpio.c
    )

target_sources_ifdef(CONFIG_MYFUNCTION app PRIVATE src/myfunction.c)