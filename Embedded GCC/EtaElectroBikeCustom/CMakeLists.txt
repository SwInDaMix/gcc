cmake_minimum_required(VERSION 2.8.4)
project(EtaElectroBikeCustom C)


include_directories("../_STM8S10x_StdPeriph")
include_directories("../_STM8S10x_StdPeriph/inc")

file(GLOB S12S_SRC
        "S12S/Application/inc/*.h"
        "S12S/Application/*.c"
        "S12S/Config/*.h")

file(GLOB KTLCD3_SRC
        "KTLCD3/Application/inc/*.h"
        "KTLCD3/Application/*.c"
        "KTLCD3/Config/*.h")

add_library(S12S STATIC ${S12S_SRC})
add_library(KTLCD3 STATIC ${KTLCD3_SRC})

target_include_directories(S12S PUBLIC "S12S/Application/inc")
target_include_directories(S12S PUBLIC "S12S/Config")
target_include_directories(KTLCD3 PUBLIC "KTLCD3/Application/inc")
target_include_directories(KTLCD3 PUBLIC "KTLCD3/Config")

#add_executable(EtaElectroBikeS12SCustom
#        ${EtaElectroBikeS12SCustom_SRC})
