cmake_minimum_required(VERSION 3.22)

# Set C and ASM standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Default to Debug build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Project Name
set(CMAKE_PROJECT_NAME ActuatorNode)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core Project Configuration
project(${CMAKE_PROJECT_NAME} LANGUAGES C ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

# Target Executable
add_executable(${CMAKE_PROJECT_NAME})

# Include CubeMX Subdirectory
add_subdirectory(cmake/stm32cubemx)

# Sources and Includes
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/main.c
    Core/Src/stm32g4xx_it.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/system_stm32g4xx.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
    startup_stm32g431xx.s
)

# Target Definitions
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    STM32G431xx
    $<$<CONFIG:Debug>:DEBUG>
)

# Remove unwanted libob.a reference
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Link Libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    stm32cubemx
)

# Generate .bin and .hex targets automatically after build
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMENT "Building ${CMAKE_PROJECT_NAME}.hex and ${CMAKE_PROJECT_NAME}.bin"
)
