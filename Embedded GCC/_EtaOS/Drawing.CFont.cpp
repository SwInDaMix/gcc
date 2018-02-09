/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CFont.cpp
 ***********************************************************************/

#include "Drawing.CFont.hpp"

#include "fonts/font_small.fh"
#include "fonts/font_large.fh"

using namespace System::Drawing;

/// *********************************************************************
/*static*/ CFont const CFont::s_font_small(font_small_bmp_regular, font_small_bmp_bold, FONT_SMALL_HEIGHT, FONT_SMALL_CHARS);
/*static*/ CFont const CFont::s_font_large(font_large_bmp_regular, font_large_bmp_bold, FONT_LARGE_HEIGHT, FONT_LARGE_CHARS);

CFont::CFont(uint8_t const *fntbmp_regular, uint8_t const *fntbmp_bold, uint32_t height, uint32_t chars) : d_fntbmp_regular(fntbmp_regular), d_fntbmp_bold(fntbmp_bold), d_height(height), d_chars(chars) {  }
/*virtual*/ CFont::~CFont() {  }
/// *********************************************************************
