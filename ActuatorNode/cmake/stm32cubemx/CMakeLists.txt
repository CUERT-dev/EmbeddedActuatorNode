cmake_minimum_required(VERSION 3.22)

# Interface library for CubeMX configurations
add_library(stm32cubemx INTERFACE)

target_compile_definitions(stm32cubemx INTERFACE
    STM32G431xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
)
