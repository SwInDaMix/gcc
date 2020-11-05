/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : entry.c
 ***********************************************************************/

#include "entry.h"
#include "periphs.h"
#include "melody.h"
#include "menus.h"
#include "eta_dbg.h"
#include "eta_frames.h"
#include "eta_st7565.h"
#include "eta_flash.h"
#include "eta_delay.h"

#include "eta_stgui_menus.h"
#include "eta_stgui_fonts.h"
#include "eta_stgui_bitmaps.h"
#include "eta_stgui_primitives.h"

#define REVOSM_LOWINLET_HYSTERESIS (PERIODIC_TICKS_PER_SECOND * 2)
#define REVOSM_HIGHTANK_HYSTERESIS (PERIODIC_TICKS_PER_SECOND * 2)

static sStGUI_Controller g_menu_controller;

//void Int_Update() {
//	// **************
//	// Sensors Update
//	// **************
//
//	#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
//	static bool __prev_low_inlet_switch = false; static uint16_t _rep_low_inlet = 0;
//	bool _low_inlet_switch = GPIO_ReadInputDataBit(SENS_LOWINLET_PRESSURE_PORT, (1 << SENS_LOWINLET_PRESSURE_PIN)) == Bit_SET;
//	if(__prev_low_inlet_switch != _low_inlet_switch) { __prev_low_inlet_switch = _low_inlet_switch; _rep_low_inlet = 0; }
//	else { if(_rep_low_inlet >= REVOSM_LOWINLET_HYSTERESIS) { Sens_LowInletSwitch = _low_inlet_switch; } else _rep_low_inlet++; }
//	#endif
//	#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
//	static bool __prev_high_tank_switch = false; static uint16_t _rep_high_tank = 0;
//	bool _high_tank_switch = GPIO_ReadInputDataBit(SENS_HIGHTANK_PRESSURE_PORT, (1 << SENS_HIGHTANK_PRESSURE_PIN)) == Bit_SET;
//	if(__prev_high_tank_switch != _high_tank_switch) { __prev_high_tank_switch = _high_tank_switch; _rep_high_tank = 0; }
//	else { if(_rep_high_tank >= REVOSM_HIGHTANK_HYSTERESIS) { Sens_HighTankSwitch = _high_tank_switch; } else _rep_high_tank++; }
//	#endif
//}
//
//
//void Valve_WaterInlet(bool state) {
//	static bool __prev_state;
//
//	if(__prev_state != state) {
//		__prev_state = state;
//		GPIO_WriteBit(VALVE_WATER_INLET_PORT, (1 << VALVE_WATER_INLET_PIN), state ? Bit_SET : Bit_RESET);
//	}
//}
//void Valve_CleanMembrane(bool state) {
//	static bool __prev_state;
//
//	if(__prev_state != state) {
//		__prev_state = state;
//		GPIO_WriteBit(VALVE_CLEAN_MEMBRANE_PORT, (1 << VALVE_CLEAN_MEMBRANE_PIN), state ? Bit_SET : Bit_RESET);
//	}
//}

#include "bitmaps/murchelago.hbmp"
STGUI_BITMAP_DECLARE(murchelago, MURCHELAGO);
#include "fonts/extra_large_digits.hfont"
STGUI_FONT_DECLARE(extra_large_digits, EXTRA_LARGE_DIGITS);

uint8_t _fb = 0;
bool _tsw = false;
uint8_t _ch = 0;
uint8_t _fh = 255;
uint32_t _fhhhh = 0;

void EtaReverseOsmosisMainLoop(bool volatile *is_abort_requested, bool volatile *is_inited, void (*yield)()) {
	DBG_PutString(	"\nEta Reverse Osmosis Management System"NL
					" - Platform v "VER_PLATFORM""NL
					" - Application v "VER_APP""NL);

	uint32_t _prev_global_ticks_seconds = 0, _seconds = 0;
	bool _halfsecond = false, _second_changed = false;

//	uint32_t _ttt = Global10msTicks;
//    StGUI_Primitive_ProgressWithStringDraw(10, 38, 108, 64, DRAW_MODE_XOR, &g_stgui_font_small, "Chip Erase");
//    GLCD_Flush();
//    SpiFlash_WriteEnable();
//    SpiFlash_EraseChip();
//    while (SpiFlash_IsBusy());
//    _ttt = Global10msTicks - _ttt;

//	uint32_t address = 0;
//	uint8_t *screen = GLCD_GetScreen();
//	while (address < SPIFLASH_CHIP_SIZE) {
//        size_t sz = min(GLCD_WIDTH * GLCD_ROWS, SPIFLASH_CHIP_SIZE - address);
//        SpiFlash_ReadData(address, screen, sz);
//        GLCD_Flush();
//        _delay_ms(500);
//        address += sz;
//	}

    Periph_LCDBacklight_SetParams(_fh, _fh, _fh, 10);

    __extension__ void _redraw_before() {
        uint8_t *_screen = GLCD_GetScreen();

        memset(_screen, _fb, GLCD_WIDTH * GLCD_ROWS);

        StGUI_StringDrawMaxWidth(&g_stgui_font_small, 0, 0, GLCD_WIDTH, 0, ALIGNMENT_START, DRAW_MODE_XOR, g_menu_controller.get_ms10ticks(), "This is a very very long text rotating on the screen");
        StGUI_StringDrawMaxWidth(&g_stgui_font_small, 0, 10, GLCD_WIDTH, 0, ALIGNMENT_START, DRAW_MODE_XOR, g_menu_controller.get_ms10ticks(), "ms10ticks: %X", _fhhhh);

        StGUI_Primitive_ProgressDraw(10, 32, 108, 4, _fh, DRAW_MODE_XOR, g_menu_controller.get_ms10ticks());
        StGUI_Primitive_ProgressWithStringDraw(10, 38, 108, _fh, g_menu_controller.get_ms10ticks(), &g_stgui_font_small, "Test string");
        //StGUI_Primitive_ProgressWithStringDraw(10, 52, 108, UINT16_MAX, _fhhhh, &g_stgui_font_small, "Test a long long string");
        StGUI_Primitive_ProgressWithStringDraw(10, 52, 108, UINT16_MAX, g_menu_controller.get_ms10ticks(), &g_stgui_font_small, "Test a long long string");

        //StGUI_Primitive_FrameDraw(8, 8, 100, _fh, _ch);
        //StGUI_Primitive_ArrowMarkDraw(0, 0, _ch, 0);
        //StGUI_Primitive_CheckboxMarkDraw(0, 32, _ch, 0, ~_fb);

//        memcpy(_screen, g_bitmap_murchelago, GLCD_WIDTH * GLCD_ROWS);

// ***************************************

//        memset(_screen, fillb ^ 0xFF, GLCD_WIDTH * GLCD_ROWS);
//        memset(_screen, fillb, GLCD_WIDTH * 2);
//        StGUI_BitmapDraw(&g_stgui_bitmap_murchelago, 0, ENC_Counter & 0x3F, dm);

// ***************************************

//        memset(_screen, fillb ^ 0xFF, GLCD_WIDTH * GLCD_ROWS);
//        memset(_screen, fillb, GLCD_WIDTH * 2);
//
//        StGUI_StringDraw(&g_stgui_font_large_bold, 10, ENC_Counter & 0x3F, dm, "Test String %d", ENC_Counter);
//        StGUI_StringDraw(&g_stgui_font_extra_large_digits, 10, (ENC_Counter & 0x3F) + 15, dm, "0123:4567");

// ***************************************

//        memset(_screen, 0, (GLCD_WIDTH * GLCD_HEIGHT) >> 3);
//        uint8_t _sb = 1 << (ENC_Counter & 0x7);
//        _screen += ((ENC_Counter >> 3) & 0x7) << 7;
//
//        uint8_t _cnt = GLCD_WIDTH;
//        while (_cnt--) *_screen++ = _sb;
    }
    __extension__ void _redraw_after() {
        GLCD_Flush();
    }

    g_menu_controller.redraw_before = _redraw_before;
    g_menu_controller.redraw_after = _redraw_after;
    g_menu_controller.get_control_actions = (eStGUI_ControlActions (*)())Periph_GetInput;
    g_menu_controller.get_ms10ticks = Periph_GetGlobalMs10Ticks;

    while(!(*is_abort_requested)) {
        (void)_second_changed;

        if (StGUI_RedrawFlag(&g_menu_controller)) {
            _redraw_before();
            _redraw_after();
        }

        eStGUI_ControlActions _control_actions = g_menu_controller.get_control_actions();
        if (_control_actions & MENU_CONTROL_ESCAPE) {
            StGUI_OpenMenu(&g_menu_controller, Menus_GetMain(), &g_stgui_font_large_bold, &g_stgui_font_small, UINT8_MAX);
            Dialog_Test(&g_menu_controller);
            Dialog_FlashSurfaceCheck(&g_menu_controller);
            //_tsw = !_tsw;
        }
        if (_control_actions & MENU_CONTROL_ENTER) { _fb = ~_fb; _fhhhh = 0; }
        if (_control_actions & MENU_CONTROL_FORWARD) { if (_tsw) _ch++; else if (_fh < 248) { _fh += 8; if (_fh == 0x80 || _fh == 0x88) _fh--; } Periph_LCDBacklight_SetParams(_fh, _fh, _fh, 10); _fhhhh += 2; }
        if (_control_actions & MENU_CONTROL_BACKWARD) { if (_tsw) _ch--; else if (_fh > 7) { _fh -= 8; if (_fh == 0x7F || _fh == 0x77) _fh++; } Periph_LCDBacklight_SetParams(_fh, _fh, _fh, 10); _fhhhh -= 2; }


        // Seconds processing
		_second_changed = false;
		if((_prev_global_ticks_seconds - Periph_GetGlobalTicks()) > (PERIODIC_TICKS_PER_SECOND >> 1)) {

			_prev_global_ticks_seconds += (PERIODIC_TICKS_PER_SECOND >> 1);
			//IWDG_ReloadCounter();
			if(_halfsecond) { _halfsecond = false; } else { _halfsecond = true; _seconds++; _second_changed = true; }
		}

//		if(Frames_IsFrameAvailable()) {
//			eFrameCmd _frame_cmd = (eFrameCmd)Frames_GetFrameCommand();
//			uint8_t _frame_size = Frames_GetFrameSize();
//			uint8_t _frame_data[_frame_size];
//			Frames_GetFrameData(_frame_data, 0, _frame_size);
//			Frames_RemoveFrame();
//
//			if(_frame_cmd == FRAME_CMD_TEST_COMMAND && _frame_size > 0) {
//			}
//		}
//		else if(Frames_IsTransmitterFree()) {
//			//Frames_PutFrame((uint8_t)FRAME_CMD_STATUS, &_status, sizeof(sStatus));
//		}

        yield();
	}
}
