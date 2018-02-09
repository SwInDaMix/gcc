/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CGraphics.cpp
***********************************************************************/

#include "Drawing.CGraphics.hpp"
#include "eta_string_format.h"

using namespace System;
using namespace System::Drawing;

/// *********************************************************************
static uint32_t s_active_graphics_index_iterrator;
static uint32_t s_active_graphics_index_current = -1;

struct SGraphics_ElipseContext {
	uint32_t d_prev_left, d_prev_top, d_prev_right, d_prev_bottom;
	bool d_is_first;

	SGraphics_ElipseContext() { d_is_first = true; d_prev_left = d_prev_top = d_prev_right = d_prev_bottom = 0; }
};
/// **********************************
struct SGraphics_DrawStringContext {
	bool const d_do_draw;
	bool const d_is_transparent_back;
	bool const d_is_shadow;
	uint32_t d_x_org, d_x, d_y;
	EColor const d_clr_fore, d_clr_back;
	CFont const &d_font;
	bool const d_is_bold;
	EAlignment const d_alignmen;
	uint32_t *d_lines_len;
	uint32_t d_lines_len_count, d_lines_len_max;
	uint32_t d_len_line, d_len_x, d_len_y;

	SGraphics_DrawStringContext(bool do_draw, bool is_transparent_back, bool is_shadow, uint32_t x, uint32_t y, EColor clr_fore, EColor clr_back, CFont const &font, bool is_bold, EAlignment alignmen, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max)	:
		d_do_draw(do_draw), d_is_transparent_back(is_transparent_back), d_is_shadow(is_shadow), d_x(x), d_y(y), d_clr_fore(clr_fore), d_clr_back(clr_back), d_font(font), d_is_bold(is_bold), d_alignmen(alignmen), d_lines_len(lines_len), d_lines_len_count(lines_len_count), d_lines_len_max(lines_len_max) { d_x_org = d_x; d_len_line = d_len_x = d_len_y = 0; }
};
/// **********************************
CGraphics::CGraphics(SRect const &rc_screen) : d_active_graphics_index(s_active_graphics_index_iterrator++), d_rc_screen(rc_screen) {  }
/*virtual*/ CGraphics::~CGraphics() {  }

static void _RectFocus_Line(bool is_vertical, uint32_t x, uint32_t y, uint32_t len, EColor clr) {
	EColor _clrbuf[len]; EColor *_clrbuf_ptr = _clrbuf; bool _is_first_gap = ((x ^ y) & 1);
	SRect _rc(x, y, is_vertical ? 1 : len, is_vertical ? len : 1);
	CPeriph::LCD_RectRead(_rc, _clrbuf);
	for(uint32_t _i = len; _i; _i--) { { if(_is_first_gap) _clrbuf_ptr++; else *_clrbuf_ptr++ = clr; } _is_first_gap = !_is_first_gap; }
	CPeriph::LCD_RectWrite(_rc, _clrbuf);
}
static void _Elipse(int32_t left, int32_t top, int32_t right, int32_t bottom, EColor clr_draw, EColor clr_fill, void(*put4point)(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom, SGraphics_ElipseContext &ec, bool is_vertical, EColor clr_draw, EColor clr_fill)) {
	int32_t _width = right - left, _height = bottom - top; bool _is_vertical;
	if(_height > _width) {
		_is_vertical = true; int32_t _tmp;
		_tmp = left; left = top; top = _tmp;
		_tmp = right; right = bottom; bottom = _tmp;
		_tmp = _width; _width = _height; _height = _tmp;
	}
	else _is_vertical = false;

	int32_t _ww = _width * _width, _hh = _height * _height;
	int64_t _height_odd = _height & 1, _dx = 4 * (1 - _width) * _hh, _dy = 4 * (_height_odd + 1) * _ww;
	int64_t _err = _dx + _dy + _height_odd * _ww;

	bottom -= (_height + 2) / 2 - 1; top = bottom - _height_odd;
	_width = 8 * _ww; _height_odd = 8 * _hh;
	SGraphics_ElipseContext _ec;
	do {
		if(_is_vertical) put4point(top, left, bottom, right, _ec, _is_vertical, clr_draw, clr_fill);
		else put4point(left, top, right, bottom, _ec, _is_vertical, clr_draw, clr_fill);
		int64_t _err2 = _err << 1;
		if (_err2 <= _dy) { top--; bottom++; _err += _dy += _width; }
		if (_err2 >= _dx || 2 * _err > _dy) { left++; right--; _err += _dx += _height_odd; }
	} while(left <= right);
}
static void _Elipse_Draw(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom, SGraphics_ElipseContext &ec, bool is_vertical, EColor clr_draw, EColor clr_fill) {
	CPeriph::LCD_PixelSet(SPoint(left, top), clr_draw);
	if(top != bottom) { CPeriph::LCD_PixelSet(SPoint(left, bottom), clr_draw); }
	if(left != right) {
		CPeriph::LCD_PixelSet(SPoint(right, top), clr_draw);
		CPeriph::LCD_PixelSet(SPoint(right, bottom), clr_draw);
	}
}
static void _Elipse_FillCustom(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom, SGraphics_ElipseContext &ec, bool is_vertical, EColor clr, bool is_inside) {
	if(is_inside) {
		if(!ec.d_is_first) {
			if(is_vertical) {
				uint32_t _prev_height = ec.d_prev_bottom - ec.d_prev_top - 1, _prev_top = ec.d_prev_top + 1;
				if(ec.d_prev_left != left && _prev_height) CPeriph::LCD_RectFill(SRect(ec.d_prev_left, _prev_top, 1, _prev_height), clr);
				if(ec.d_prev_right != right && ec.d_prev_right != ec.d_prev_left && _prev_height) CPeriph::LCD_RectFill(SRect(ec.d_prev_right, _prev_top, 1, _prev_height), clr);
			}
			else {
				uint32_t _prev_width = ec.d_prev_right - ec.d_prev_left - 1, _prev_left = ec.d_prev_left + 1;
				if(ec.d_prev_top != top && _prev_width) CPeriph::LCD_RectFill(SRect(_prev_left, ec.d_prev_top, _prev_width, 1), clr);
				if(ec.d_prev_bottom != bottom && ec.d_prev_bottom != ec.d_prev_top && _prev_width) CPeriph::LCD_RectFill(SRect(_prev_left, ec.d_prev_bottom, _prev_width, 1), clr);
			}
		}
	}
	else {
		if(is_vertical) {
			uint32_t _height = bottom - top + 1;
			if(ec.d_prev_left != left) CPeriph::LCD_RectFill(SRect(left, top, 1, _height), clr);
			if(ec.d_prev_right != right && right != left) CPeriph::LCD_RectFill(SRect(right, top, 1, _height), clr);
		}
		else {
			uint32_t _width = right - left + 1;
			if(ec.d_prev_top != top) CPeriph::LCD_RectFill(SRect(left, top, _width, 1), clr);
			if(ec.d_prev_bottom != bottom && top != bottom) CPeriph::LCD_RectFill(SRect(left, bottom, _width, 1), clr);
		}
	}
	ec.d_prev_left = left; ec.d_prev_top = top; ec.d_prev_right = right; ec.d_prev_bottom = bottom; ec.d_is_first = false;
}
static void _Elipse_Fill(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom, SGraphics_ElipseContext &ec, bool is_vertical, EColor clr_draw, EColor clr_fill) { _Elipse_FillCustom(left, top, right, bottom, ec, is_vertical, clr_fill, false); }
static void _Elipse_DrawFill(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom, SGraphics_ElipseContext &ec, bool is_vertical, EColor clr_draw, EColor clr_fill) { _Elipse_Draw(left, top, right, bottom, ec, is_vertical, clr_draw, clr_fill); _Elipse_FillCustom(left, top, right, bottom, ec, is_vertical, clr_fill, true); }
static bool _ImageDraw(uint32_t x, uint32_t y, uint32_t width, uint32_t height, CImage::CPixelEnumerator &pixel_enumerator, EColor *clr_transparent) {
	#define GRAPHICS_IMAGE_BUFFER 256
	uint32_t _clr_buf_len = max(GRAPHICS_IMAGE_BUFFER, width), _clr_buf_lines = min(height, _clr_buf_len / width); _clr_buf_len = _clr_buf_lines * width; bool _ret = true;
	EColor _clr_buf[_clr_buf_len];
	while(_ret && height) {
		EColor *_clr_buf_ptr = _clr_buf;
		SRect _rc(x, y, width, _clr_buf_lines);
		if(clr_transparent) CPeriph::LCD_RectRead(_rc, _clr_buf);
		uint32_t _clr_buf_len_tmp = _clr_buf_len; while(_clr_buf_len_tmp--) {
			if(!(_ret = pixel_enumerator.MoveToNextPixel())) break;
			EColor _clr = pixel_enumerator.CurrentPixel_get();
			if(!clr_transparent || *clr_transparent != _clr) *_clr_buf_ptr = _clr;
			_clr_buf_ptr++;
		}
		if(!_ret) break;
		CPeriph::LCD_RectWrite(_rc, _clr_buf);
		y += _clr_buf_lines; height -= _clr_buf_lines;
		if(_clr_buf_lines > height) { _clr_buf_lines = height; _clr_buf_len = _clr_buf_lines * width; }
	}
	return _ret;
}
static void _StringDraw_Char_FillPixels(uint8_t const *_fnt_char, EColor *clr_row_ptr, EColor clr_fore, uint32_t _fnt_height, uint32_t _char_len, uint32_t _char_len_plus) {
	for(uint32_t _y = 0; _y < _fnt_height; _y++) {
		uint8_t _fnt_char_row = *_fnt_char++;
		for(uint32_t _x = 0; _x < _char_len; _x++) {
			if(_fnt_char_row & 1) clr_row_ptr[_x] = clr_fore;
			_fnt_char_row >>= 1;
		}
		clr_row_ptr += _char_len_plus;
	}
}
static void _StringDraw_Char(SGraphics_DrawStringContext &dsc, char c) {
	uint32_t _fnt_height = dsc.d_font.Height_get();
	if((uint8_t)c >= 0x20 && (uint8_t)c < (0x20 + dsc.d_font.Chars_get())) {
		uint8_t const *_fnt_char = (dsc.d_is_bold && dsc.d_font.FntBmpBold_get() ? dsc.d_font.FntBmpBold_get() : dsc.d_font.FntBmpRegular_get()) + ((c - 0x20) * (_fnt_height + 1));
		uint32_t _char_len = _fnt_char[_fnt_height];
		uint32_t _fnt_height_plus = _fnt_height, _char_len_plus = _char_len; if(dsc.d_is_shadow) { _fnt_height_plus++; _char_len_plus++; }
		if(!dsc.d_len_line) {
			if(dsc.d_len_y) dsc.d_len_y++; dsc.d_len_y += _fnt_height;
			if(dsc.d_do_draw && dsc.d_alignmen != EAlignment::Start && dsc.d_lines_len && dsc.d_lines_len_count) {
				uint32_t _off = dsc.d_lines_len_max - *dsc.d_lines_len++;
				dsc.d_x += (dsc.d_alignmen == EAlignment::Middle ? (_off >> 1) : (_off)); dsc.d_lines_len_count--;
			}
		}
		if(dsc.d_len_line) {
			if(dsc.d_do_draw && !dsc.d_is_transparent_back && !dsc.d_is_shadow) CPeriph::LCD_RectFill(SRect(dsc.d_x, dsc.d_y, 1, _fnt_height_plus), dsc.d_clr_back);
			dsc.d_len_line++; dsc.d_x++;
		}
		if(dsc.d_do_draw) {
			uint32_t _char_clrs = _char_len_plus * _fnt_height_plus;
			EColor _clr_row[_char_clrs]; SRect _rc(dsc.d_x, dsc.d_y, _char_len_plus, _fnt_height_plus);
			if(dsc.d_is_transparent_back) CPeriph::LCD_RectRead(_rc, _clr_row);
			else { uint32_t _char_clrs_tmp = _char_clrs; EColor *_clr_row_ptr = _clr_row; while(_char_clrs_tmp--) { *_clr_row_ptr++ = dsc.d_clr_back; } }

			if(dsc.d_is_shadow) _StringDraw_Char_FillPixels(_fnt_char, _clr_row + _char_len_plus + 1, dsc.d_clr_fore, _fnt_height, _char_len, _char_len_plus);
			_StringDraw_Char_FillPixels(_fnt_char, _clr_row, dsc.d_is_shadow ? ColorLow50(dsc.d_clr_fore) : dsc.d_clr_fore, _fnt_height, _char_len, _char_len_plus);

			CPeriph::LCD_RectWrite(_rc, _clr_row);
		}
		dsc.d_len_line += _char_len; dsc.d_x += _char_len;
	}
	else if(c == '\n') {
		dsc.d_len_x = max(dsc.d_len_x, dsc.d_len_line);
		if(!dsc.d_do_draw && dsc.d_lines_len && dsc.d_lines_len_count) { *dsc.d_lines_len++ = dsc.d_len_line; dsc.d_lines_len_count--; }
		dsc.d_len_line = 0; dsc.d_x = dsc.d_x_org; dsc.d_y += _fnt_height + 1;
	}
}
static void _StringDrawFmt_Char(sFmtStream *strm, char c) { _StringDraw_Char(*(SGraphics_DrawStringContext *)strm->ptr, c); }
static void _StringDraw_Finish(SGraphics_DrawStringContext &dsc, uint32_t *lines_len, uint32_t *lines_len_count) {
	dsc.d_len_x = max(dsc.d_len_x, dsc.d_len_line);
	if(!dsc.d_do_draw && dsc.d_lines_len && dsc.d_lines_len_count) { *dsc.d_lines_len++ = dsc.d_len_line; dsc.d_lines_len_count--; }
	if(lines_len) { if(dsc.d_len_line) { *dsc.d_lines_len++ = dsc.d_len_line; dsc.d_lines_len_count++; } if(lines_len_count) { *lines_len_count = *lines_len_count - dsc.d_lines_len_count; } }
}
static SSize _StringDraw(bool do_draw, bool is_transparent_back, bool is_shadow, uint32_t x, uint32_t y, EColor clr_fore, EColor clr_back, CFont const &font, bool is_bold, EAlignment alignmen, uint32_t *lines_len, uint32_t *lines_len_count, uint32_t lines_len_max, char const *str) {
	SGraphics_DrawStringContext _dsc(do_draw, is_transparent_back, is_shadow, x, y, clr_fore, clr_back, font, is_bold, alignmen, lines_len, lines_len_count ? *lines_len_count : -1, lines_len_max);
	char _c; while((_c = *str++)) { _StringDraw_Char(_dsc, _c); }
	_StringDraw_Finish(_dsc, lines_len, lines_len_count);
	return SSize(_dsc.d_len_x, _dsc.d_len_y);
}
static SSize _StringDrawFmt(bool do_draw, bool is_transparent_back, bool is_shadow, uint32_t x, uint32_t y, EColor clr_fore, EColor clr_back, CFont const &font, bool is_bold, EAlignment alignmen, uint32_t *lines_len, uint32_t *lines_len_count, uint32_t lines_len_max, char const *fmt, va_list ap) {
	SGraphics_DrawStringContext _dsc(do_draw, is_transparent_back, is_shadow, x, y, clr_fore, clr_back, font, is_bold, alignmen, lines_len, lines_len_count ? *lines_len_count : -1, lines_len_max);
	sFmtStream _strm; _strm.putc = _StringDrawFmt_Char; _strm.limit = -1; _strm.flags = sFormatFlags(0); _strm.ptr = &_dsc;
	formatString(fmt, ap, &_strm);
	_StringDraw_Finish(_dsc, lines_len, lines_len_count);
	return SSize(_dsc.d_len_x, _dsc.d_len_y);
}

void CGraphics::Clear(EColor clr) { _EnsureIndex(); CPeriph::LCD_RectFill(d_rc_screen, clr); }

bool CGraphics::RectDraw(SRect const &rc, bool is_round, EColor clr) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = ((is_round && _width >= 3 && _height >= 3) || (_width && _height)))) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		SRect _rc_tmp(is_round ? _x + 1 : _x, _y, is_round ? _width - 2 : _width, 1);
		CPeriph::LCD_RectFill(_rc_tmp, clr);
		if(_height >= 3) {
			CPeriph::LCD_RectFill(SRect(_x, _y + 1, 1, _height - 2), clr);
			CPeriph::LCD_RectFill(SRect(_x + _width - 1, _y + 1, 1, _height - 2), clr);
		}
		if(_height >= 2) { _rc_tmp.Y_set(_y + _height - 1); CPeriph::LCD_RectFill(_rc_tmp, clr); }
	}
	return _ret;
}
bool CGraphics::RectFill(SRect const &rc, EColor clr) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width && _height)) { _EnsureIndex(); CPeriph::LCD_RectFill(SRect(_x + d_rc_screen.X_get(), _y + d_rc_screen.Y_get(), _width, _height), clr); }
	return _ret;
}
bool CGraphics::RectDrawFill(SRect const &rc, bool is_round, EColor clr_draw, EColor clr_fill) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	_ret = RectDraw(rc, is_round, clr_draw);
	if(_ret && _width > 3 && _height > 3) _ret = RectFill(SRect(_x + 1, _y + 1, _width - 2, _height - 2), clr_fill);
	return _ret;
}
bool CGraphics::RectFocus(SRect const &rc, EColor clr) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width && _height)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		_RectFocus_Line(false, _x, _y, _width, clr);
		if(_height >= 3) {
			_RectFocus_Line(true, _x, _y + 1, _height - 2, clr);
			_RectFocus_Line(true, _x + _width - 1, _y + 1, _height - 2, clr);
		}
		_RectFocus_Line(false, _x, _y + _height - 1, _width, clr);
	}
	return _ret;
}
bool CGraphics::RectFrame3D(SRect const &rc, bool is_bold, bool is_inside, EColor clr1, EColor clr2) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = ((is_bold && _width >= 4 && _height >= 4) || (_width >= 2 && _height >= 2)))) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		EColor _clr_mid = ColorTrans(clr1, clr2, 128), _clr_midlight = ColorTrans(clr1, clr2, 224);
		if(is_inside) {
			CPeriph::LCD_RectFill(SRect(_x, _y, _width - 1, 1), _clr_mid); CPeriph::LCD_RectFill(SRect(_x, _y + 1, 1, _height - 2), _clr_mid);
			CPeriph::LCD_RectFill(SRect(_x, _y + _height - 1, _width, 1), clr2); CPeriph::LCD_RectFill(SRect(_x + _width - 1, _y, 1, _height - 1), clr2);
			if(is_bold) {
				_x++; _y++; _width -= 2; _height -= 3;
				CPeriph::LCD_RectFill(SRect(_x, _y, _width - 1, 1), clr1); CPeriph::LCD_RectFill(SRect(_x, _y + 1, 1, _height - 1), clr1);
				CPeriph::LCD_RectFill(SRect(_x, _y + _height, _width, 1), _clr_midlight); CPeriph::LCD_RectFill(SRect(_x + _width - 1, _y, 1, _height), _clr_midlight);
			}
		}
		else {
			EColor _clr_tmp1 = is_bold ? _clr_midlight : clr2; CPeriph::LCD_RectFill(SRect(_x, _y, is_bold ? _width - 1 : _width, 1), _clr_tmp1); CPeriph::LCD_RectFill(SRect(_x, _y + 1, 1, _height - (is_bold ? 2 : 1)), _clr_tmp1);
			EColor _clr_tmp2 = is_bold ? clr1 : _clr_mid; CPeriph::LCD_RectFill(SRect(is_bold ? _x : _x + 1, _y + _height - 1, is_bold ? _width : _width - 1, 1), _clr_tmp2); CPeriph::LCD_RectFill(SRect(_x + _width - 1, is_bold ? _y : _y + 1, 1, _height - (is_bold ? 1 : 2)), _clr_tmp2);
			if(is_bold) {
				_x++; _y++; _width -= 2; _height -= 3;
				CPeriph::LCD_RectFill(SRect(_x, _y, _width, 1), clr2); CPeriph::LCD_RectFill(SRect(_x, _y + 1, 1, _height - 1), clr2);
				CPeriph::LCD_RectFill(SRect(_x, _y + _height, _width, 1), _clr_mid); CPeriph::LCD_RectFill(SRect(_x + _width - 1, _y, 1, _height), _clr_mid);
			}
		}
	}
	return _ret;
}
bool CGraphics::RectFrameStatic(SRect const &rc, bool is_inside, EColor clr1, EColor clr2) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width >= 4 && _height >= 4)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		if(is_inside) { EColor _clr_tmp = clr2; clr2 = clr1; clr1 = _clr_tmp; }
		CPeriph::LCD_RectFill(SRect(_x, _y, _width - 1, 1), clr2); CPeriph::LCD_RectFill(SRect(_x, _y + 1, 1, _height - 2), clr2);
		CPeriph::LCD_RectFill(SRect(_x + 1, _y + _height - 2, _width - 2, 1), clr2); CPeriph::LCD_RectFill(SRect(_x + _width - 2, _y + 1, 1, _height - 3), clr2);
		CPeriph::LCD_RectFill(SRect(_x + 1, _y + 1, _width - 3, 1), clr1); CPeriph::LCD_RectFill(SRect(_x + 1, _y + 2, 1, _height - 4), clr1);
		CPeriph::LCD_RectFill(SRect(_x, _y + _height - 1, _width, 1), clr1); CPeriph::LCD_RectFill(SRect(_x + _width - 1, _y, 1, _height - 1), clr1);
	}
	return _ret;
}
bool CGraphics::RectGradient(SRect const &rc, EOrientation orientation, EColor clr_from, EColor clr_to) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width && _height)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		int32_t _rf = ((int32_t)clr_from) & 0x1F, _gf = ((int32_t)clr_from) & 0x7E0, _bf = ((int32_t)clr_from) & 0xF800;
		int32_t _rd = (((int32_t)clr_to) & 0x1F) - _rf, _gd = (((int32_t)clr_to) & 0x7E0) - _gf, _bd = (((int32_t)clr_to) & 0xF800) - _bf;
		if(orientation == EOrientation::Horizontal) { int32_t _ww = _width - 1; for(int32_t _i = 0; _i < (int32_t)_width; _i++) { CPeriph::LCD_RectFill(SRect(_x + _i, _y, 1, _height), (EColor)((_rf + (_rd * _i / _ww)) | ((_gf + (_gd * _i / _ww)) & 0x7E0) | ((_bf + (_bd * _i / _ww)) & 0xF800))); } }
		else { int32_t _hh = _height - 1; for(int32_t _i = 0; _i < (int32_t)_height; _i++) { CPeriph::LCD_RectFill(SRect(_x, _y + _i, _width, 1), (EColor)((_rf + (_rd * _i / _hh)) | ((_gf + (_gd * _i / _hh)) & 0x7E0) | ((_bf + (_bd * _i / _hh)) & 0xF800))); } }
	}
	return _ret;
}
bool CGraphics::ElipseDraw(SRect const &rc, EColor clr) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width && _height)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		_Elipse(_x, _y, _x + _width - 1, _y + _height - 1, clr, clr, _Elipse_Draw);
	}
	return _ret;
}
bool CGraphics::ElipseFill(SRect const &rc, EColor clr) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width && _height)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		_Elipse(_x, _y, _x + _width - 1, _y + _height - 1, clr, clr, _Elipse_Fill);
	}
	return _ret;
}
bool CGraphics::ElipseDrawFill(SRect const &rc, EColor clr_draw, EColor clr_fill) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get(); bool _ret;
	if((_ret = _width && _height)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		_Elipse(_x, _y, _x + _width - 1, _y + _height - 1, clr_draw, clr_fill, _Elipse_DrawFill);
	}
	return _ret;
}
bool CGraphics::LineDraw(SPoint const &pt_start, SSizeSigned const &szs, EColor clr) {
	uint32_t _x = pt_start.X_get(), _y = pt_start.Y_get(), _screen_width = d_rc_screen.Width_get(), _screen_height = d_rc_screen.Height_get();
	int32_t _width = szs.Width_get(), _height = szs.Height_get(), _sx = _width >= 0 ? 1 : -1, _sy = _height >= 0 ? 1 : -1; _width -= _sx; _height -= _sy;
	int32_t _right = _x + _width, _bottom = _y + _height; bool _ret;
	if((_ret = _width && _height && _right >= 0 && _bottom >= 0 && _right < (int32_t)_screen_width && _bottom < (int32_t)_screen_height)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		int32_t _dx = abs(_width), _dy = neg(_height), _err = _dx + _dy;
		while(true) {
			CPeriph::LCD_PixelSet(SPoint(_x, _y), clr);
			if ((int32_t)_x == _right && (int32_t)_y == _bottom) break;
			int32_t _e2 = 2 * _err;
			if (_e2 >= _dy) { _err += _dy; _x += _sx; }
			if (_e2 <= _dx) { _err += _dx; _y += _sy; }
		}
	}
	return _ret;
}
bool CGraphics::LineStatic(SPoint const &pt_start, EOrientation orientation, uint32_t length, bool is_inside, EColor clr1, EColor clr2) {
	uint32_t _x = pt_start.X_get(), _y = pt_start.Y_get(); bool _ret;
	if((_ret = length)) {
		_EnsureIndex();
		_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
		if(is_inside) { EColor _clr_tmp = clr2; clr2 = clr1; clr1 = _clr_tmp; }
		if(orientation == EOrientation::Horizontal) { CPeriph::LCD_RectFill(SRect(_x, _y, length, 1), clr2); CPeriph::LCD_RectFill(SRect(_x, _y + 1, length, 1), clr1); }
		else { CPeriph::LCD_RectFill(SRect(_x, _y, 1, length), clr2); CPeriph::LCD_RectFill(SRect(_x + 1, _y, 1, length), clr1); }
	}
	return _ret;
}
bool CGraphics::ImageDraw(SPoint const &pt_origin, CImage const &image) {
	_EnsureIndex();
	uint32_t _x = pt_origin.X_get(), _y = pt_origin.Y_get(), _width = image.Width_get(), _height = image.Height_get(); bool _ret;
	_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
	CImage::CPixelEnumerator *_pixel_enumerator = image.CreateEnumerator();
	_ret = _ImageDraw(_x, _y, _width, _height, *_pixel_enumerator, nullptr);
	delete _pixel_enumerator;
	return _ret;
}
bool CGraphics::ImageDraw(SPoint const &pt_origin, CImage const &image, EColor clr_transparent) {
	_EnsureIndex();
	uint32_t _x = pt_origin.X_get(), _y = pt_origin.Y_get(), _width = image.Width_get(), _height = image.Height_get(); bool _ret;
	_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
	CImage::CPixelEnumerator *_pixel_enumerator = image.CreateEnumerator();
	_ret = _ImageDraw(_x, _y, _width, _height, *_pixel_enumerator, &clr_transparent);
	delete _pixel_enumerator;
	return _ret;
}
bool CGraphics::ImageDraw1(SPoint const &pt_origin, CImage1 const &image1, EColor clr_fore, EColor clr_back) {
	_EnsureIndex();
	uint32_t _x = pt_origin.X_get(), _y = pt_origin.Y_get(), _width = image1.Width_get(), _height = image1.Height_get(); bool _ret;
	_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
	CImage::CPixelEnumerator *_pixel_enumerator = image1.CreateEnumerator(clr_fore, clr_back);
	_ret = _ImageDraw(_x, _y, _width, _height, *_pixel_enumerator, nullptr);
	delete _pixel_enumerator;
	return _ret;
}
bool CGraphics::ImageDraw1(SPoint const &pt_origin, CImage1 const &image1, EColor clr_fore) {
	_EnsureIndex();
	uint32_t _x = pt_origin.X_get(), _y = pt_origin.Y_get(), _width = image1.Width_get(), _height = image1.Height_get(); bool _ret;
	_x += d_rc_screen.X_get(); _y += d_rc_screen.Y_get();
	EColor _clr_back = (EColor)(~((uint16_t)clr_fore));
	CImage::CPixelEnumerator *_pixel_enumerator = image1.CreateEnumerator(clr_fore, _clr_back);
	_ret = _ImageDraw(_x, _y, _width, _height, *_pixel_enumerator, &_clr_back);
	delete _pixel_enumerator;
	return _ret;
}

/*static*/ SSize CGraphics::StringMeasure(CFont const &font, bool is_bold, uint32_t *lines_len, uint32_t *lines_len_count, char const *str) { return _StringDraw(false, false, false, 0, 0, EColor::_Invalid, EColor::_Invalid, font, is_bold, EAlignment::Start, lines_len, lines_len_count, 0, str); }
/*static*/ SSize CGraphics::StringMeasureFmt(CFont const &font, bool is_bold, uint32_t *lines_len, uint32_t *lines_len_count, char const *fmt, ...) { va_list argp; va_start(argp, fmt); SSize _sz = StringMeasureFmt(font, is_bold, lines_len, lines_len_count, fmt, argp); va_end(argp); return _sz; }
/*static*/ SSize CGraphics::StringMeasureFmt(CFont const &font, bool is_bold, uint32_t *lines_len, uint32_t *lines_len_count, char const *fmt, va_list ap) { return _StringDrawFmt(false, false, false, 0, 0, EColor::_Invalid, EColor::_Invalid, font, is_bold, EAlignment::Start, lines_len, lines_len_count, 0, fmt, ap); }
SSize CGraphics::StringDrawEx(SPoint const &pt_location, EColor clr_fore, EColor clr_back, bool is_transparent_back, bool is_shadow, CFont const &font, bool is_bold, EAlignment alignment, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max, char const *str) { _EnsureIndex(); return _StringDraw(true, is_transparent_back, is_shadow, pt_location.X_get() + d_rc_screen.X_get(), pt_location.Y_get() + d_rc_screen.Y_get(), clr_fore, clr_back, font, is_bold, alignment, lines_len, &lines_len_count, lines_len_max, str); }
SSize CGraphics::StringDrawExFmt(SPoint const &pt_location, EColor clr_fore, EColor clr_back, bool is_transparent_back, bool is_shadow, CFont const &font, bool is_bold, EAlignment alignment, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max, char const *fmt, ...) { va_list argp; va_start(argp, fmt); SSize _sz = StringDrawExFmt(pt_location, clr_fore, clr_back, is_transparent_back, is_shadow, font, is_bold, alignment, lines_len, lines_len_count, lines_len_max, fmt, argp); va_end(argp); return _sz; }
SSize CGraphics::StringDrawExFmt(SPoint const &pt_location, EColor clr_fore, EColor clr_back, bool is_transparent_back, bool is_shadow, CFont const &font, bool is_bold, EAlignment alignment, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max, char const *fmt, va_list ap) { _EnsureIndex(); return _StringDrawFmt(true, is_transparent_back, is_shadow, pt_location.X_get() + d_rc_screen.X_get(), pt_location.Y_get() + d_rc_screen.Y_get(), clr_fore, clr_back, font, is_bold, alignment, lines_len, &lines_len_count, lines_len_max, fmt, ap); }

void CGraphics::_EnsureIndex() { if(s_active_graphics_index_current != d_active_graphics_index) { CPeriph::LCD_ClipRect_set(d_rc_screen); s_active_graphics_index_current = d_active_graphics_index; } }
/// *********************************************************************
