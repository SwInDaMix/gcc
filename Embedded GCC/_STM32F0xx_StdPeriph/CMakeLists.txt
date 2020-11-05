file(GLOB _STM32F0xx_StdPeriph_SRC
        *.c
        inc/*.h)
add_library(_STM32F0xx_StdPeriph STATIC ${_STM32F0xx_StdPeriph_SRC})

target_compile_definitions(_STM32F0xx_StdPeriph PUBLIC ${STM_CPU} USE_STDPERIPH_DRIVER)

target_include_directories(_STM32F0xx_StdPeriph PUBLIC ../_Common)
target_include_directories(_STM32F0xx_StdPeriph PUBLIC ../_CMSIS)
target_include_directories(_STM32F0xx_StdPeriph PUBLIC inc)
