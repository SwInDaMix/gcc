/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriphWindows.cpp
 ***********************************************************************/

#include "CPeriphWindows.hpp"

using namespace System;
using namespace System::Drawing;

volatile bool lcd_is_need_draw = false;
static volatile bool lcd_is_valid;
static uint32_t lcd_resolution_width, lcd_resolution_height;
static HDC lcd_DC;
static LPVOID lcd_buffer;
static HBITMAP lcd_bitmap;
static HANDLE lcd_old_dib;
static uint8_t snd_volume = 100;

static unsigned char const kbd_vk_table[] {
	VK_BACK,
	VK_TAB,
	VK_RETURN,
	VK_PAUSE,
	VK_CAPITAL,
	VK_ESCAPE,
	VK_SPACE,
	VK_PRIOR,
	VK_NEXT,
	VK_END,
	VK_HOME,
	VK_LEFT,
	VK_UP,
	VK_RIGHT,
	VK_DOWN,
	VK_SNAPSHOT,
	VK_INSERT,
	VK_DELETE,
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	VK_LWIN,
	VK_RWIN,
	VK_APPS,
	VK_NUMPAD0,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
	VK_MULTIPLY,
	VK_ADD,
	VK_SEPARATOR,
	VK_SUBTRACT,
	VK_DECIMAL,
	VK_DIVIDE,
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	VK_F13,
	VK_F14,
	VK_F15,
	VK_F16,
	VK_F17,
	VK_F18,
	VK_F19,
	VK_F20,
	VK_F21,
	VK_F22,
	VK_F23,
	VK_F24,
	VK_NUMLOCK,
	VK_SCROLL,
	VK_SHIFT,
	VK_CONTROL,
	VK_MENU,
};

/// *********************************************************************
void LCD_CreateContext(uint32_t width, uint32_t height) {
	width = max(width, 400); height = max(height, 240);
	if(lcd_resolution_width != width || lcd_resolution_height != height) {
		LCD_DestroyContext();
		lcd_resolution_width = width; lcd_resolution_height = height;
		lcd_DC = CreateCompatibleDC(nullptr);
		BITMAPINFO _bitmapinfo;
		_bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		_bitmapinfo.bmiHeader.biWidth = (LONG)lcd_resolution_width;
		_bitmapinfo.bmiHeader.biHeight = (LONG)-lcd_resolution_height;
		_bitmapinfo.bmiHeader.biPlanes = 1;
		_bitmapinfo.bmiHeader.biBitCount = 32;
		_bitmapinfo.bmiHeader.biCompression = BI_RGB;
		_bitmapinfo.bmiHeader.biSizeImage = 0;
		_bitmapinfo.bmiHeader.biClrUsed = 256;
		_bitmapinfo.bmiHeader.biClrImportant = 256;
		lcd_buffer = nullptr;
		lcd_bitmap = CreateDIBSection(lcd_DC, &_bitmapinfo, DIB_RGB_COLORS, &lcd_buffer, nullptr, 0);
		lcd_old_dib = SelectObject(lcd_DC, lcd_bitmap);
		lcd_is_valid = true;
	}
}
void LCD_DestroyContext() {
	if(lcd_DC) {
		lcd_is_valid = false;
		SelectObject(lcd_DC, lcd_old_dib);
		DeleteDC(lcd_DC); lcd_DC = nullptr;
		if(lcd_bitmap) { DeleteObject(lcd_bitmap); lcd_bitmap = nullptr; lcd_buffer = nullptr; }
	}
}
void LCD_PaintToDC(HDC hDC) {
	if(lcd_is_valid && lcd_DC) BitBlt(hDC, 0, 0, lcd_resolution_width, lcd_resolution_height, lcd_DC, 0, 0, SRCCOPY);
}

bool KBD_GetScan(unsigned char vk, EKeyScan *key_scan) {
	unsigned char const *_vk_table_ptr = kbd_vk_table; unsigned char _vk_table_len = sizeof_array(kbd_vk_table), _vk_table_idx = 0;
	while(_vk_table_len--) { if(*_vk_table_ptr == vk) { *key_scan = (EKeyScan)_vk_table_idx; return true; } _vk_table_ptr++; _vk_table_idx++; }
	return false;
}
void KBD_Reset() { CPeriph::KBD_Reset(); }
bool KBD_KeyDown(unsigned char vk) { EKeyScan _key_scan; bool _res; if((_res = KBD_GetScan(vk, &_key_scan))) CPeriph::KBD_KeyDown(_key_scan); return _res; }
bool KBD_KeyUp(unsigned char vk) { EKeyScan _key_scan;  bool _res; if((_res = KBD_GetScan(vk, &_key_scan))) CPeriph::KBD_KeyUp(_key_scan); return _res; }

void CPeriph::Init() {  }

/// **********************************
/// Timer Related members
/*static virtual*/ void CPeriph::TIM_20ms() {  }

/// **********************************
/// KBD Related members

/*static*/ EKeyModifierFlags CPeriph::KBD_IsModifier(EKeyScan key_scan) {
	if(key_scan == EKeyScan::Alt) return EKeyModifierFlags::Alt;
	if(key_scan == EKeyScan::Ctrl) return EKeyModifierFlags::Ctrl;
	if(key_scan == EKeyScan::Shift) return EKeyModifierFlags::Shift;
	else return (EKeyModifierFlags)0;
}

/// **********************************
/// LCD Related members

/*static*/ SSize CPeriph::LCD_Resolution_get() { return SSize(lcd_resolution_width, lcd_resolution_height); }
/*static*/ void CPeriph::LCD_Clear(EColor clr) { LCD_RectFill(s_rc_clip, clr); }
/*static*/ void CPeriph::LCD_PixelSet(SPoint const &pt, EColor clr) {
	if(s_rc_clip.IsInside(pt)) { ((uint32_t *)lcd_buffer)[pt.Y_get() * lcd_resolution_width + pt.X_get()] = LCD_WINCOLOR24_FROM_COLOR16(clr); lcd_is_need_draw = true; }
}
/*static*/ EColor CPeriph::LCD_PixelGet(SPoint const &pt) {
	if(s_rc_clip.IsInside(pt)) {
		return (EColor)LCD_COLOR16_FROM_WINCOLOR24(((uint32_t *)lcd_buffer)[pt.Y_get() * lcd_resolution_width + pt.X_get()]);
	}
	else return EColor::Black;
}
/*static*/ void CPeriph::LCD_RectFill(SRect const &rc, EColor clr) {
	SRect _rc_tmp = rc & s_rc_clip;
	uint32_t _x = _rc_tmp.X_get(), _y = _rc_tmp.Y_get(), _width = _rc_tmp.Width_get(), _height = _rc_tmp.Height_get();
	if(lcd_is_valid && _width && _height && _x < lcd_resolution_width && _y < lcd_resolution_height && _x + _width <= lcd_resolution_width && _y + _height <= lcd_resolution_height) {
		uint32_t _clr = LCD_WINCOLOR24_FROM_COLOR16(clr);
		while(_height) {
			uint32_t *_row_buf = ((uint32_t *)lcd_buffer) + _y * lcd_resolution_width + _x;
			uint32_t _width_tmp = _width; while(_width_tmp--) { *_row_buf++ = _clr; }
			_height--; _y++;
		}
		lcd_is_need_draw = true;
	}
}
/*static*/ void CPeriph::LCD_RectRead(SRect const &rc, EColor *clr_buf) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get();
	if(lcd_is_valid && _x < lcd_resolution_width && _y < lcd_resolution_height && _x + _width <= lcd_resolution_width && _y + _height <= lcd_resolution_height) {
		SRect _rc_tmp = rc & s_rc_clip; uint32_t _width_actual = _rc_tmp.Width_get();
		if(_width_actual && _rc_tmp.Height_get()) {
			uint32_t _clip_left = s_rc_clip.X_get(), _clip_top = s_rc_clip.Y_get(), _clip_bottom = _clip_top + s_rc_clip.Height_get();
			uint32_t _width_blank_left = _clip_left > _x ? _clip_left - _x : 0, _width_blank_right = _width > _width_blank_left + _width_actual ? _width - _width_blank_left - _width_actual : 0;
			while(_height) {
				if(_y < _clip_top || _y >= _clip_bottom) { uint32_t _width_tmp = _width; while(_width_tmp--) *clr_buf++ = EColor::Black; }
				else if(_y >= _clip_top && _y < _clip_bottom) {
					uint32_t *_row_buf = ((uint32_t *)lcd_buffer) + _y * lcd_resolution_width + _x;
					if(_width_blank_left) { _row_buf += _width_blank_left; uint32_t _width_tmp = _width_blank_left; while(_width_tmp--) *clr_buf++ = EColor::Black; }
					uint32_t _width_actual_tmp = _width_actual; while(_width_actual_tmp--) { *clr_buf++ = (EColor)LCD_COLOR16_FROM_WINCOLOR24(*_row_buf); _row_buf++; }
					if(_width_blank_right) { uint32_t _width_tmp = _width_blank_right; while(_width_tmp--) *clr_buf++ = EColor::Black; }
				}
				_height--; _y++;
			}
		}
		else { uint32_t _width_tmp = _width * _height; while((_width_tmp--)) *clr_buf++ = EColor::Black; }
	}
}
/*static*/ void CPeriph::LCD_RectWrite(SRect const &rc, EColor *clr_buf) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get();
	if(lcd_is_valid && _x < lcd_resolution_width && _y < lcd_resolution_height && _x + _width <= lcd_resolution_width && _y + _height <= lcd_resolution_height) {
		SRect _rc_tmp = rc & s_rc_clip; uint32_t _width_actual = _rc_tmp.Width_get();
		if(_width_actual && _rc_tmp.Height_get()) {
			uint32_t _clip_left = s_rc_clip.X_get(), _clip_top = s_rc_clip.Y_get(), _clip_bottom = _clip_top + s_rc_clip.Height_get();
			uint32_t _width_blank_left = _clip_left > _x ? _clip_left - _x : 0, _width_blank_right = _width > _width_blank_left + _width_actual ? _width - _width_blank_left - _width_actual : 0;
			while(_height) {
				if(_y < _clip_top) clr_buf += _width;
				else if(_y >= _clip_top && _y < _clip_bottom) {
					uint32_t *_row_buf = ((uint32_t *)lcd_buffer) + _y * lcd_resolution_width + _x + _width_blank_left; clr_buf += _width_blank_left;
					uint32_t _width_actual_tmp = _width_actual; while(_width_actual_tmp--) { *_row_buf++ = LCD_WINCOLOR24_FROM_COLOR16(*clr_buf); clr_buf++; }
					clr_buf += _width_blank_right;
				}
				_height--; _y++;
			}
		}
		lcd_is_need_draw = true;
	}
}

/*static*/ void CPeriph::SND_Volume_set(uint8_t volume) { snd_volume = volume; }
/*static*/ uint8_t CPeriph::SND_Volume_get() { return snd_volume; }
/*static*/ void CPeriph::SND_Beep(EBeepType beep_type) { PlaySound((LPCSTR)(beep_type == EBeepType::Error ? SND_ALIAS_SYSTEMHAND : SND_ALIAS_SYSTEMDEFAULT), NULL, SND_ALIAS_ID | SND_ASYNC); }
/// *********************************************************************
