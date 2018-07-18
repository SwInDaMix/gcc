cmake_minimum_required(VERSION 3.10)
project(EtaElectroBikeS12SCustom C)

include_directories("../_STM8S10x_StdPeriph")
include_directories("../_STM8S10x_StdPeriph/inc")
include_directories(Application/inc)
include_directories(Config)

file(GLOB EtaElectroBikeS12SCustom_SRC
        "Application/inc/*.h"
        "Application/*.c"
        "Config/*.h")

add_executable(EtaElectroBikeS12SCustom
        ${EtaElectroBikeS12SCustom_SRC})
