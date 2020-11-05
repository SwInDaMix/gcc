file(GLOB _Eta32_SRC
        "*.c"
        "inc/*.h")
add_library(_Eta32 STATIC ${_Eta32_SRC})

target_link_libraries(_Eta32 _STM32F0xx_StdPeriph)

target_include_directories(_Eta32 PUBLIC ../_Common)
target_include_directories(_Eta32 PUBLIC "inc")
