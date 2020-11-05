find_package(PythonInterp REQUIRED)

function(add_fonts TARGET OPTIONS)
    get_target_property(TARGET_SOURCE_DIR _EtaSt7565 SOURCE_DIR)
    set(ENV_IMG2FONT_TOOL ${TARGET_SOURCE_DIR}/tools/stgui_img2hfont.py)

    separate_arguments(OPTIONS NATIVE_COMMAND "${OPTIONS}")
    foreach(FNT ${ARGN})
        get_filename_component(FNTEXT ${FNT} EXT)
        string(REPLACE ${FNTEXT} ".hfont" FNTHDR ${FNT})
        list(APPEND FONT_HEADERS ${FNTHDR})
        add_custom_command(OUTPUT ${FNTHDR}
                DEPENDS ${FNT}
                COMMAND ${PYTHON_EXECUTABLE} ${ENV_IMG2FONT_TOOL} ${FNT} ${OPTIONS})
        set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES ${FNTHDR})
    endforeach()
    add_custom_target(${TARGET}_FontHeaders DEPENDS ${FONT_HEADERS})
    add_dependencies(${TARGET} ${TARGET}_FontHeaders)
endfunction()

function(add_bitmaps TARGET OPTIONS)
    get_target_property(TARGET_SOURCE_DIR _EtaSt7565 SOURCE_DIR)
    set(ENV_IMG2BMP_TOOL ${TARGET_SOURCE_DIR}/tools/stgui_img2hbmp.py)

    separate_arguments(OPTIONS NATIVE_COMMAND "${OPTIONS}")
    foreach(BMP ${ARGN})
        get_filename_component(BMPEXT ${BMP} EXT)
        string(REPLACE ${BMPEXT} ".hbmp" BMPHDR ${BMP})
        list(APPEND BITMAP_HEADERS ${BMPHDR})
        add_custom_command(OUTPUT ${BMPHDR}
                DEPENDS ${BMP}
                COMMAND ${PYTHON_EXECUTABLE} ${ENV_IMG2BMP_TOOL} ${BMP} ${OPTIONS})
        set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES ${BMPHDR})
    endforeach()
    add_custom_target(${TARGET}_BitmapHeaders DEPENDS ${BITMAP_HEADERS})
    add_dependencies(${TARGET} ${TARGET}_BitmapHeaders)
endfunction()

function(add_stgui TARGET)
    get_target_property(TARGET_SOURCE_DIR _EtaSt7565 SOURCE_DIR)
    set(ENV_STGUI_GENERATOR_TOOL ${TARGET_SOURCE_DIR}/tools/stgui_stgui2hstgui.py)

    foreach(STGUI ${ARGN})
        get_filename_component(STGUIEXT ${STGUI} EXT)
        string(REPLACE ${STGUIEXT} ".hstgui" STGUIHDR ${STGUI})
        list(APPEND STGUI_HEADERS ${STGUIHDR})
        add_custom_command(OUTPUT ${STGUIHDR}
                DEPENDS ${STGUI}
                COMMAND ${PYTHON_EXECUTABLE} ${ENV_STGUI_GENERATOR_TOOL} ${STGUI})
        set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES ${STGUIHDR})
    endforeach()
    add_custom_target(${TARGET}_StGUIHeaders DEPENDS ${STGUI_HEADERS})
    add_dependencies(${TARGET} ${TARGET}_StGUIHeaders)
endfunction()

file(GLOB _EtaSt7565_SRC
        *.c
        inc/*.h)
add_library(_EtaSt7565 STATIC ${_EtaSt7565_SRC})

target_link_libraries(_EtaSt7565 _STM32F0xx_StdPeriph)
target_link_libraries(_EtaSt7565 _Eta32)

target_include_directories(_EtaSt7565 PUBLIC ../_Common)
target_include_directories(_EtaSt7565 PUBLIC inc)

file(GLOB _EtaSt7565_Fonts
        fonts/*.png)
add_fonts(_EtaSt7565 "16 6 convert --static" ${_EtaSt7565_Fonts})

file(GLOB _EtaSt7565_Bitmaps
        bitmaps/*.png
        bitmaps/*.jpg
        bitmaps/*.bmp)
add_bitmaps(_EtaSt7565 "--static --dithering" ${_EtaSt7565_Bitmaps})
