# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bendix C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
if(TARGET tinyusb_device)
    add_executable(bendix bendix.cpp processor.cpp)
    target_compile_options(bendix PRIVATE -Werror -Wall -Wextra)

    pico_set_program_name(bendix "bendix")
    pico_set_program_version(bendix "0.1")

    # Generate PIO header
    pico_generate_pio_header(bendix ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

    # Modify the below lines to enable/disable output over UART/USB
    pico_enable_stdio_uart(bendix 0)
    pico_enable_stdio_usb(bendix 1)

    # pull in common dependencies
    target_link_libraries(bendix pico_stdlib)

    # Add the standard include files to the build
    target_include_directories(bendix PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        )
    
    # Add any user requested libraries
    target_link_libraries(bendix 
        hardware_dma
        hardware_pio
        hardware_timer
        )
    
    pico_add_extra_outputs(bendix)

else()#if(PICO_ON_DEVICE)
        message("Skipping hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

# Add Catch2
# add_subdirectory(Catch2)
# add_subdirectory(tests)