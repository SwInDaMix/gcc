/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriphNative.cpp
 ***********************************************************************/

#include "eta_dbg.h"

#include "stm32f10x.h"
#include "stm32f10x_fsmc.h"
#include "IntBIOS.h"
#include "CPeriphNative.hpp"

using namespace System;
using namespace System::Drawing;

#define GPIOD_BSRR	(*((volatile uint32_t *)(0x40011400+0x10)))
#define GPIOD_BRR	(*((volatile uint32_t *)(0x40011400+0x14)))
#define LCD_PORT	(*((volatile uint16_t *)(0x60000000+0x00)))
#define DMA2_CHANEL1_CCR	(*((volatile uint32_t *)(0x40020400+0x08)))
#define DMA2_CHANEL1_CNDTR	(*((volatile uint32_t *)(0x40020400+0x0C)))
#define DMA2_CHANEL1_CMAR	(*((volatile uint32_t *)(0x40020400+0x14)))

/// *********************************************************************
static uint16_t kbd_key_flags_prev = ALL_KEYS;

static uint32_t lcd_type;

static uint32_t snd_beep_delay = 0;
static uint8_t snd_volume = 100;

static inline void _LCD_WriteData(uint16_t data) { LCD_PORT = data; }
static inline uint16_t _LCD_ReadData() { return LCD_PORT; }
static inline void _LCD_DMAWaitForReady() { while(DMA2_CHANEL1_CNDTR); }
static inline void _LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd cmd) { GPIOD_BRR = GPIO_Pin_12; GPIOD_BRR = GPIO_Pin_12; _LCD_WriteData((uint16_t)cmd); GPIOD_BSRR = GPIO_Pin_12; }
static void _LCD_SetRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, bool is_writing) {
	_LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd::SetColumnAddress);
	_LCD_WriteData(x1 >> 8);
	_LCD_WriteData(x1 & 0xFF);
	_LCD_WriteData(x2 >> 8);
	_LCD_WriteData(x2 & 0xFF);
	_LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd::SetPageAddress);
	_LCD_WriteData(y1 >> 8);
	_LCD_WriteData(y1 & 0xFF);
	_LCD_WriteData(y2 >> 8);
	_LCD_WriteData(y2 & 0xFF);
	if(is_writing) { _LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd::WriteMemoryStart); } else { _LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd::ReadMemoryStart); _LCD_ReadData(); }
}
static void _LCD_DMAProceed(uint32_t ccr, uint32_t count, EColor *clr_buf) {
	while(count) {
		DMA2_CHANEL1_CCR = ccr;
		uint32_t _cnt = min(count, 65535);
		DMA2_CHANEL1_CMAR = (uint32_t)(clr_buf);
		DMA2_CHANEL1_CNDTR = _cnt;
		DMA2_CHANEL1_CCR = (ccr | 0x0001);
		count -= _cnt;
		_LCD_DMAWaitForReady();
	}
}

/*static*/ void CPeriph::Init() {
	_LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd::DeviceCodeRead);
	_LCD_ReadData();
	lcd_type = (_LCD_ReadData() & 0xFF);
	lcd_type = (_LCD_ReadData() & 0xFF) << 24;
	lcd_type |= (_LCD_ReadData() & 0xFF) << 16;
	lcd_type |= (_LCD_ReadData() & 0xFF) << 8;
	lcd_type |= (_LCD_ReadData() & 0xFF);
	DBG_PutFormat("LCD type: %P\n", lcd_type);

	// config address mode
	_LCD_WriteCmd(CPeriphNative::eLCD_ILI9488Cmd::MemoryAccessControl);
	_LCD_WriteData(0x20);

//	#define FSMC_BCR1   (*((uint32_t *)(0xA0000000+0x00)))
//	#define FSMC_BWTR1  (*((uint32_t *)(0xA0000000+0x104)))
//	FSMC_BCR1 = 0x00005011;
//	FSMC_BWTR1 = 0x10100401;
}

/*static*/ void CPeriph::TIM_20ms() {
	uint16_t _kbd_key_flags = __Get(KEY_STATUS, 0), _kbd_key_flags_changed = _kbd_key_flags ^ kbd_key_flags_prev;
	if(_kbd_key_flags_changed & KEY1_STATUS) { if(_kbd_key_flags & KEY1_STATUS) KBD_KeyUp(EKeyScan::RunPause); else KBD_KeyDown(EKeyScan::RunPause); }
	if(_kbd_key_flags_changed & KEY2_STATUS) { if(_kbd_key_flags & KEY2_STATUS) KBD_KeyUp(EKeyScan::Stop); else KBD_KeyDown(EKeyScan::Stop); }
	if(_kbd_key_flags_changed & KEY3_STATUS) { if(_kbd_key_flags & KEY3_STATUS) KBD_KeyUp(EKeyScan::Record); else KBD_KeyDown(EKeyScan::Record); }
	if(_kbd_key_flags_changed & KEY4_STATUS) { if(_kbd_key_flags & KEY4_STATUS) KBD_KeyUp(EKeyScan::Shift); else KBD_KeyDown(EKeyScan::Shift); }
	if(_kbd_key_flags_changed & K_INDEX_D_STATUS) { if(_kbd_key_flags & K_INDEX_D_STATUS) KBD_KeyUp(EKeyScan::Navigator1_Left); else KBD_KeyDown(EKeyScan::Navigator1_Left); }
	if(_kbd_key_flags_changed & K_INDEX_I_STATUS) { if(_kbd_key_flags & K_INDEX_I_STATUS) KBD_KeyUp(EKeyScan::Navigator1_Right); else KBD_KeyDown(EKeyScan::Navigator1_Right); }
	if(_kbd_key_flags_changed & K_INDEX_S_STATUS) { if(_kbd_key_flags & K_INDEX_S_STATUS) KBD_KeyUp(EKeyScan::Navigator1_Push); else KBD_KeyDown(EKeyScan::Navigator1_Push); }
	if(_kbd_key_flags_changed & K_ITEM_D_STATUS) { if(_kbd_key_flags & K_ITEM_D_STATUS) KBD_KeyUp(EKeyScan::Navigator2_Left); else KBD_KeyDown(EKeyScan::Navigator2_Left); }
	if(_kbd_key_flags_changed & K_ITEM_I_STATUS) { if(_kbd_key_flags & K_ITEM_I_STATUS) KBD_KeyUp(EKeyScan::Navigator2_Right); else KBD_KeyDown(EKeyScan::Navigator2_Right); }
	if(_kbd_key_flags_changed & K_ITEM_S_STATUS) { if(_kbd_key_flags & K_ITEM_S_STATUS) KBD_KeyUp(EKeyScan::Navigator2_Push); else KBD_KeyDown(EKeyScan::Navigator2_Push); }
	kbd_key_flags_prev = _kbd_key_flags; if((_kbd_key_flags & ALL_KEYS) == ALL_KEYS) KBD_Reset();

	if(snd_beep_delay) snd_beep_delay--; else __Set(BEEP_VOLUME, 0);
}

/*static*/ EKeyModifierFlags CPeriph::KBD_IsModifier(EKeyScan key_scan) {
	if(key_scan == EKeyScan::Shift) return EKeyModifierFlags::Shift;
	else return (EKeyModifierFlags)0;
}

/*static*/ SSize CPeriph::LCD_Resolution_get() { return SSize(LCD_RESOLUTION_WIDTH, LCD_RESOLUTION_HEIGHT); }
/*static*/ void CPeriph::LCD_Clear(EColor clr) { LCD_RectFill(s_rc_clip, clr); }
/*static*/ void CPeriph::LCD_PixelSet(SPoint const &pt, EColor clr) {
	if(s_rc_clip.IsInside(pt)) {
		uint32_t _x = pt.X_get(), _y = pt.Y_get();
		_LCD_SetRegion(_x, _y, _x, _y, true);
		_LCD_WriteData((uint16_t)clr);
	}
}
/*static*/ EColor CPeriph::LCD_PixelGet(SPoint const &pt) {
	if(s_rc_clip.IsInside(pt)) {
		uint32_t _x = pt.X_get(), _y = pt.Y_get();
		_LCD_SetRegion(_x, _y, _x, _y, false);
		return (EColor)_LCD_ReadData();
	}
	else return EColor::Black;
}
/*static*/ void CPeriph::LCD_RectFill(SRect const &rc, EColor clr) {
	SRect _rc_tmp = rc & s_rc_clip;
	uint32_t _x = _rc_tmp.X_get(), _y = _rc_tmp.Y_get(), _width = _rc_tmp.Width_get(), _height = _rc_tmp.Height_get();
	if(_x < LCD_RESOLUTION_WIDTH && _y < LCD_RESOLUTION_HEIGHT && _x + _width <= LCD_RESOLUTION_WIDTH && _y + _height <= LCD_RESOLUTION_HEIGHT) {
		uint32_t _count = _width * _height;
		if(_count) {
			_LCD_SetRegion(_x, _y, _x + _width - 1, _y + _height - 1, true);
			_LCD_DMAProceed(0x5510, _count, &clr);
		}
	}
}
/*static*/ void CPeriph::LCD_RectRead(SRect const &rc, EColor *clr_buf) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get();
	if(_x < LCD_RESOLUTION_WIDTH && _y < LCD_RESOLUTION_HEIGHT && _x + _width <= LCD_RESOLUTION_WIDTH && _y + _height <= LCD_RESOLUTION_HEIGHT && _width && _height) {
		SRect _rc_tmp = rc & s_rc_clip;
		uint32_t _clip_x = _rc_tmp.X_get(), _clip_y = _rc_tmp.Y_get(), _clip_width = _rc_tmp.Width_get(), _clip_height = _rc_tmp.Height_get();
		_LCD_SetRegion(_clip_x, _clip_y, _clip_x + _clip_width - 1, _clip_y + _clip_height - 1, false);
		if(rc == _rc_tmp) {
			uint32_t _count = _width * _height;
			_LCD_DMAProceed(0x5580, _count, clr_buf);
		}
		else {
			if(_clip_width && _clip_height) {
				uint32_t _width_blank_left = _clip_x > _x ? _clip_x - _x : 0, _width_blank_right = _width > _width_blank_left + _clip_width ? _width - _width_blank_left - _clip_width : 0;
				if(_clip_y > _y) { uint32_t _width_tmp = _width * (_clip_y - _y); while(_width_tmp--) *clr_buf++ = EColor::Black; }
				_clip_y += _clip_height;
				while(_clip_height--) {
					if(_width_blank_left) { uint32_t _width_tmp = _width_blank_left; while(_width_tmp--) *clr_buf++ = EColor::Black; }
					_LCD_DMAProceed(0x5580, _clip_width, clr_buf); clr_buf += _clip_width;
					if(_width_blank_right) { uint32_t _width_tmp = _width_blank_right; while(_width_tmp--) *clr_buf++ = EColor::Black; }
				}
				if(_y + _height > _clip_y) { uint32_t _width_tmp = _width * (_y + _height - _clip_y); while(_width_tmp--) *clr_buf++ = EColor::Black; }
			}
			else { uint32_t _count = _width * _height; while(_count--) *clr_buf++ = EColor::Black; }
		}
	}
}
/*static*/ void CPeriph::LCD_RectWrite(SRect const &rc, EColor *clr_buf) {
	uint32_t _x = rc.X_get(), _y = rc.Y_get(), _width = rc.Width_get(), _height = rc.Height_get();
	if(_x < LCD_RESOLUTION_WIDTH && _y < LCD_RESOLUTION_HEIGHT && _x + _width <= LCD_RESOLUTION_WIDTH && _y + _height <= LCD_RESOLUTION_HEIGHT && _width && _height) {
		SRect _rc_tmp = rc & s_rc_clip;
		uint32_t _clip_x = _rc_tmp.X_get(), _clip_y = _rc_tmp.Y_get(), _clip_width = _rc_tmp.Width_get(), _clip_height = _rc_tmp.Height_get();
		_LCD_SetRegion(_clip_x, _clip_y, _clip_x + _clip_width - 1, _clip_y + _clip_height - 1, true);
		if(rc == _rc_tmp) {
			uint32_t _count = _width * _height;
			_LCD_DMAProceed(0x5590, _count, clr_buf);
		}
		else {
			if(_clip_width && _clip_height) {
				uint32_t _width_blank_left = _clip_x > _x ? _clip_x - _x : 0, _width_blank_right = _width > _width_blank_left + _clip_width ? _width - _width_blank_left - _clip_width : 0;
				if(_clip_y > _y) clr_buf += _width * (_clip_y - _y);
				_clip_y += _clip_height;
				while(_clip_height--) {
					if(_width_blank_left) { uint32_t _width_tmp = _width_blank_left; while(_width_tmp--) *clr_buf++ = EColor::Black; }
					_LCD_DMAProceed(0x5590, _clip_width, clr_buf); clr_buf += _clip_width;
					if(_width_blank_right) { uint32_t _width_tmp = _width_blank_right; while(_width_tmp--) *clr_buf++ = EColor::Black; }
				}
			}
		}
	}
}

/*static*/ void CPeriph::SND_Volume_set(uint8_t volume) { snd_volume = volume; }
/*static*/ uint8_t CPeriph::SND_Volume_get() { return snd_volume; }
/*static*/ void CPeriph::SND_Beep(EBeepType beep_type) { snd_beep_delay = beep_type == EBeepType::Error ? 10 : 1; __Set(BEEP_VOLUME, snd_volume); }
/// *********************************************************************
