# 这个 CMake 是有时用其它 IDE 写代码时加智能提示用的，本身 make 不了
cmake_minimum_required(VERSION 3.20)
project(STM32-Examples C)

set(CMAKE_C_STANDARD 11)

include_directories(
        Misc
        Libraries
        Libraries/CMSIS
        Libraries/FWlib/inc
)

file(
        GLOB_RECURSE SOURCES
        "Libraries/*.*"
        "Misc/*.*"
        "Example/*.*"
)

add_definitions(-D STM32F10X_HD)
add_definitions(-D __ARM_CC)
add_definitions(-D __CMAKE)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})