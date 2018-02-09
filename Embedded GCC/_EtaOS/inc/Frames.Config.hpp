/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.Config.hpp
 ***********************************************************************/

#pragma once

#define FRAME_THEME_WINDOWS 0

#define FRAME_COLOR_LIGHT System::Drawing::EColor::White
#define FRAME_COLOR_DARK System::Drawing::EColor::Black
#define FRAME_COLOR_MIDDLE ColorTrans(FRAME_COLOR_DARK, FRAME_COLOR_LIGHT, 128)
#define FRAME_COLOR_INACTIVE ColorTrans(FRAME_COLOR_DARK, FRAME_COLOR_LIGHT, 128)
#define FRAME_COLOR_CONTROL ColorTrans(FRAME_COLOR_DARK, FRAME_COLOR_LIGHT, 192)

#define FRAME_DEFAULT_SEPARATION_WIDTH 2
#define FRAME_DEFAULT_FONT System::Drawing::CFont::FontSmall_get()
#define FRAME_DEFAULT_FONT_IS_BOLD false
#define FRAME_DEFAULT_FONT_IS_SHADOW false
#define FRAME_DEFAULT_COLOR_BACKGROUND FRAME_COLOR_CONTROL
#define FRAME_DEFAULT_COLOR_FOREGROUND FRAME_COLOR_DARK

#define FRAME_CAPTION_FONT System::Drawing::CFont::FontLarge_get()
#define FRAME_CAPTION_FONT_HEIGHT FRAME_CAPTION_FONT.Height_get()
#define FRAME_CAPTION_FONT_MARGIN 1
#define FRAME_CAPTION_HEIGHT (FRAME_CAPTION_FONT_HEIGHT + (FRAME_CAPTION_FONT_MARGIN << 1))
#define FRAME_CAPTION_COLOR1 System::Drawing::EColor::Navy
#define FRAME_CAPTION_COLOR2 System::Drawing::EColor::SteelBlue

