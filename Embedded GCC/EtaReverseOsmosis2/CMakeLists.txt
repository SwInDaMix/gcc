cmake_minimum_required(VERSION 3.10)

include(../toolchain_arm_none_eabi.cmake)

project(EtaReverseOsmosis C CXX)
set(TARGET_NAME ${PROJECT_NAME}_Native)

set(STM_CPU STM32F030)
set(PLATFORM 030k6)
add_compile_definitions(VER_APP="1.0.00000")
add_compile_definitions(VER_PLATFORM="1.0.00000")

set(OPTIM "-Os")
set(LINKER_SCRIPT "ld_${PLATFORM}.lds")

set(WARNINGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-vla")
set(VERY_COMMON_FLAGS "-mcpu=cortex-m0 -mthumb")
set(C_COMMON_FLAGS "-msoft-float")
set(LINKER_FLAGS "-msoft-float -nostartfiles -lc -lgcc -T \"${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT}\"")
set(LST_FLAGS "-Mforce-thumb")

string(APPEND VERY_COMMON_FLAGS " -pipe")
string(APPEND C_COMMON_FLAGS " -gdwarf-2 -fno-common -fomit-frame-pointer -fwrapv -fstrict-overflow")
string(APPEND COMPILER_COMMON_FLAGS " ${DEBUG} ${OPTIM} ${WARNINGS} ${VERY_COMMON_FLAGS} -fdata-sections -ffunction-sections -ffreestanding")
string(APPEND AS_FLAGS " ${COMPILER_COMMON_FLAGS}")
string(APPEND C_FLAGS " ${COMPILER_COMMON_FLAGS} ${C_COMMON_FLAGS}")
string(APPEND CPP_FLAGS " ${COMPILER_COMMON_FLAGS} ${C_COMMON_FLAGS} -fno-rtti -fno-threadsafe-statics -Wno-psabi -fnothrow-opt -fno-exceptions")
string(APPEND LINKER_FLAGS " ${DEBUG} ${OPTIM} ${VERY_COMMON_FLAGS} -Wl,--gc-sections -Wl,--allow-shlib-undefined")
string(APPEND LST_FLAGS " -hdS")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ASM_FLAGS ${AS_FLAGS})
set(CMAKE_C_FLAGS ${C_FLAGS})
set(CMAKE_CXX_FLAGS ${CPP_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${LINKER_FLAGS})

function(make_binaries)
    separate_arguments(LST_FLAGS NATIVE_COMMAND "${LST_FLAGS}")
    add_custom_command(TARGET ${TARGET_NAME}
            COMMAND ${CMAKE_OBJCOPY_TOOL} -O binary ${TARGET_NAME} ${TARGET_NAME}.bin
            COMMAND ${CMAKE_OBJCOPY_TOOL} -O ihex ${TARGET_NAME} ${TARGET_NAME}.hex
            COMMAND ${CMAKE_OBJDUMP_TOOL} ${LST_FLAGS} ${TARGET_NAME} > ${TARGET_NAME}.lst)
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES ${TARGET_NAME}.bin)
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES ${TARGET_NAME}.hex)
    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES ${TARGET_NAME}.lst)

    add_custom_target(ObjectsSectionSizes ALL
            DEPENDS ${TARGET_NAME}
            COMMAND ${CMAKE_OBJSIZE_TOOL} \"$<JOIN:$<TARGET_OBJECTS:_STM32F0xx_StdPeriph>," ">\" \"$<JOIN:$<TARGET_OBJECTS:_Eta32>," ">\" \"$<JOIN:$<TARGET_OBJECTS:_EtaSt7565>," ">\" \"$<JOIN:$<TARGET_OBJECTS:${TARGET_NAME}>," ">\" \"${TARGET_NAME}\"
            COMMENT "Objects Section Sizes")
endfunction(make_binaries)

file(GLOB EtaReverseOsmosis_SRC
        "Platform/Common/inc/*.h"
        "Platform/Common/*.c"
        "Platform/0xx/inc/*.h"
        "Platform/0xx/*.c"
        "Application/inc/*.h"
        "Application/*.c"
        "Menus/inc/*.h"
        "Menus/*.c"
        "Config/*.h")

add_executable(${TARGET_NAME} ${EtaReverseOsmosis_SRC})
set_target_properties(${TARGET_NAME} PROPERTIES INTERFACE_LINK_DEPENDS ${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT})

add_subdirectory(../_STM32F0xx_StdPeriph _STM32F0xx_StdPeriph)
add_subdirectory(../_Eta32 _Eta32)
add_subdirectory(../_EtaSt7565 _EtaSt7565)

target_link_libraries(${TARGET_NAME} _STM32F0xx_StdPeriph)
target_include_directories(_STM32F0xx_StdPeriph PUBLIC Config)
target_include_directories(_STM32F0xx_StdPeriph PUBLIC Platform/0xx/inc)

target_link_libraries(${TARGET_NAME} _Eta32)
target_include_directories(_Eta32 PUBLIC Config)

target_link_libraries(${TARGET_NAME} _EtaSt7565)
target_include_directories(_EtaSt7565 PUBLIC Config)

target_include_directories(${TARGET_NAME} PUBLIC ../_CMSIS)
target_include_directories(${TARGET_NAME} PUBLIC ../_Common)

include_directories(Platform/Common/inc)
include_directories(Platform/0xx/inc)
include_directories(Application/inc)

file(GLOB EtaReverseOsmosis_Fonts
        Application/fonts/*.png)
add_fonts(${TARGET_NAME} "11 1 convert --static --startcode 48" ${EtaReverseOsmosis_Fonts})

file(GLOB EtaReverseOsmosis_Bitmaps
        Application/bitmaps/*.png
        Application/bitmaps/*.jpg
        Application/bitmaps/*.bmp)
add_bitmaps(${TARGET_NAME} "--static --thumbnail 128 64 --dithering" ${EtaReverseOsmosis_Bitmaps})

file(GLOB EtaReverseOsmosis_StGUI
        Application/menus/*.stgui)
add_stgui(${TARGET_NAME} ${EtaReverseOsmosis_StGUI})

make_binaries()

