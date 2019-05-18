cmake_minimum_required(VERSION 2.8.4)
project(EtaElectroBikeKTLCD3Custom C)

include_directories("../_STM8S10x_StdPeriph")
include_directories("../_STM8S10x_StdPeriph/inc")
include_directories(Application/inc)
include_directories(Config)

file(GLOB EtaElectroBikeKTLCD3Custom_SRC
        "Application/inc/*.h"
        "Application/*.c"
        "Config/*.h")

add_executable(EtaElectroBikeKTLCD3Custom
        ${EtaElectroBikeKTLCD3Custom_SRC})
