/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : entry.cpp
 ***********************************************************************/

//#include "BuddyMem.h"

#include "eta_dbg.h"

#include "System.hpp"
#include "System.Drawing.hpp"
#include "System.Frames.hpp"

#include "images8/img8_cpma.ih8"
#include "images16/img16_cpma.ih16"

using namespace System;
using namespace System::Drawing;
using namespace System::Frames;

extern "C" {
	void EtaOsciloscopeMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void(*yield)());
}

void EtaOsciloscopeMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void(*yield)()) {
	DBG_PutString("\nEta DS203 Quad Alternative Firmware\n"
			 " - Platform v " VER_PLATFORM "\n"
			 " - EtaOS v " VER_ETAOS "\n"
			 " - Application v " VER_APP "\n");

	SSize _sz_screen = CPeriph::LCD_Resolution_get(); uint32_t  _width = _sz_screen.Width_get(), _height = _sz_screen.Height_get();
	CGraphics _graph(SRect(0, 0, _width, _height));
	_graph.Clear(EColor::Black);

	CPeriph::LCD_ClipRect_set(SRect(20, 20, 360, 200));

	_graph.RectDraw(SRect(0, 0, _width, _height), false, EColor::White);
	_graph.RectFocus(SRect(2, 2, _width - 4, _height - 4), EColor::White);

	_graph.RectFill(SRect(4, 4, _width - 8, 40), EColor::Red);
	_graph.RectFill(SRect(4, 45, _width - 8, 40), EColor::Lime);
	_graph.RectFill(SRect(4, 86, _width - 8, 40), EColor::Blue);
	_graph.RectFill(SRect(4, 127, _width - 8, 40), EColor::LightGray);

	_graph.ElipseDraw(SRect(80, 0, 240, 240), EColor::White);
	_graph.ElipseDraw(SRect(290, 5, 20, 3), EColor::White);
	_graph.ElipseDraw(SRect(285, 10, 3, 20), EColor::White);
	_graph.ElipseDraw(SRect(280, 10, 3, 19), EColor::White);
	_graph.ElipseDraw(SRect(290, 10, 1, 1), EColor::White);
	_graph.ElipseDraw(SRect(290, 12, 2, 2), EColor::White);
	_graph.ElipseDraw(SRect(290, 15, 3, 3), EColor::White);
	_graph.ElipseDraw(SRect(290, 19, 4, 4), EColor::White);
	_graph.ElipseDraw(SRect(290, 24, 5, 5), EColor::White);
	_graph.ElipseDraw(SRect(290, 30, 6, 6), EColor::White);
	_graph.ElipseDraw(SRect(290, 37, 7, 7), EColor::White);
	_graph.ElipseDraw(SRect(290, 45, 8, 8), EColor::White);
	_graph.ElipseDrawFill(SRect(300, 10, 40, 20), EColor::White, EColor::Gold);
	_graph.ElipseDraw(SRect(330, 10, 60, 100), EColor::White);
	_graph.ElipseDrawFill(SRect(335, 15, 50, 90), EColor::White, EColor::DeepPink);
	_graph.ElipseDraw(SRect(290, 60, 20, 20), EColor::White);
	_graph.ElipseFill(SRect(290, 81, 20, 20), EColor::White);

	_graph.LineDraw(SPoint(0, 0), SSizeSigned(400, 200), EColor::Linen);
	_graph.LineDraw(SPoint(0, 0), SSizeSigned(_width, _height), EColor::Linen);
	_graph.LineDraw(SPoint(0, _height - 1), SSizeSigned(_width, -_height), EColor::LemonChiffon);

	_graph.StringDrawExFmt(SPoint(220, 136), EColor::White, EColor::Black, true, true, CFont::FontLarge_get(), false, EAlignment::Start, nullptr, 0, 0, "This is a test string: %X", 0x12345678);
	_graph.StringDrawExFmt(SPoint(220, 150), EColor::Black, EColor::Black, true, false, CFont::FontLarge_get(), false, EAlignment::Start, nullptr, 0, 0, "This is a test string: %X", 0x12345678);

	_graph.RectGradient(SRect(120, 168, 35, 20), EOrientation::Vertical, EColor::White, EColor::Black);
	_graph.RectGradient(SRect(160, 168, 35, 20), EOrientation::Vertical, EColor::Black, EColor::White);

	_graph.RectGradient(SRect(200, 168, 20, 20), EOrientation::Vertical, EColor::Red, EColor::Lime);
	_graph.RectGradient(SRect(221, 168, 20, 20), EOrientation::Vertical, EColor::Lime, EColor::Blue);
	_graph.RectGradient(SRect(242, 168, 20, 20), EOrientation::Vertical, EColor::Blue, EColor::Red);
	_graph.RectGradient(SRect(200, 189, 20, 20), EOrientation::Vertical, EColor::Lime, EColor::Red);
	_graph.RectGradient(SRect(221, 189, 20, 20), EOrientation::Vertical, EColor::Blue, EColor::Lime);
	_graph.RectGradient(SRect(242, 189, 20, 20), EOrientation::Vertical, EColor::Red, EColor::Blue);

	_graph.RectGradient(SRect(300, 168, 20, 20), EOrientation::Horizontal, EColor::Red, EColor::Lime);
	_graph.RectGradient(SRect(321, 168, 20, 20), EOrientation::Horizontal, EColor::Lime, EColor::Blue);
	_graph.RectGradient(SRect(342, 168, 20, 20), EOrientation::Horizontal, EColor::Blue, EColor::Red);
	_graph.RectGradient(SRect(300, 189, 20, 20), EOrientation::Horizontal, EColor::Lime, EColor::Red);
	_graph.RectGradient(SRect(321, 189, 20, 20), EOrientation::Horizontal, EColor::Blue, EColor::Lime);
	_graph.RectGradient(SRect(342, 189, 20, 20), EOrientation::Horizontal, EColor::Red, EColor::Blue);

	_graph.RectFrame3D(SRect(4, 138, 30, 25), false, false, EColor::Black, EColor::White);
	_graph.RectFrame3D(SRect(36, 138, 30, 25), false, true, EColor::Black, EColor::White);
	_graph.RectFrame3D(SRect(68, 138, 30, 25), true, false, EColor::Black, EColor::White);
	_graph.RectFrame3D(SRect(100, 138, 30, 25), true, true, EColor::Black, EColor::White);
	_graph.RectFrame3D(SRect(4, 182, 30, 25), false, false, (EColor)LCD_COLOR16_FROM_R_G_B(0x20, 0x20, 0x20), EColor::DarkGray);
	_graph.RectFrame3D(SRect(36, 182, 30, 25), false, true, (EColor)LCD_COLOR16_FROM_R_G_B(0x20, 0x20, 0x20), EColor::DarkGray);
	_graph.RectFrame3D(SRect(68, 182, 30, 25), true, false, (EColor)LCD_COLOR16_FROM_R_G_B(0x20, 0x20, 0x20), EColor::DarkGray);
	_graph.RectFrame3D(SRect(100, 182, 30, 25), true, true, (EColor)LCD_COLOR16_FROM_R_G_B(0x20, 0x20, 0x20), EColor::DarkGray);

	_graph.RectFrameStatic(SRect(140, 138, 30, 25), false, EColor::Gray, EColor::White);
	_graph.RectFrameStatic(SRect(172, 138, 30, 25), true, EColor::Gray, EColor::White);

	CImage8 _img8_cpma(img8_cpma_imgbmp, IMG8_CPMA_IMAGE_WIDTH, IMG8_CPMA_IMAGE_HEIGHT);
	CImage16 _img16_cpma(img16_cpma_imgbmp, IMG16_CPMA_IMAGE_WIDTH, IMG16_CPMA_IMAGE_HEIGHT);

	_graph.ImageDraw(SPoint(265, 115), CFrame::s_img1_frame_close);
	_graph.ImageDraw(SPoint(276, 115), CFrame::s_img1_frame_maximize);
	_graph.ImageDraw(SPoint(287, 115), CFrame::s_img1_frame_minimize);
	_graph.ImageDraw(SPoint(298, 115), CFrame::s_img1_frame_restore);
	_graph.ImageDraw1(SPoint(309, 115), CFrame::s_img1_frame_restore, EColor::DarkCyan, EColor::Red);
	_graph.ImageDraw1(SPoint(320, 115), CFrame::s_img1_frame_restore, EColor::Black);
	_graph.ImageDraw(SPoint(106, 6), _img8_cpma);
	_graph.ImageDraw(SPoint(106, 56), _img16_cpma);
	_graph.ImageDraw(SPoint(346, 6), _img8_cpma, EColor::Black);
	_graph.ImageDraw(SPoint(346, 56), _img16_cpma, EColor::Black);

	char _fnt_str[17*6+1]; char *_fnt_str_ptr = _fnt_str;
	for(unsigned int _y = 0; _y < 6; _y++) {
		for(unsigned int _x = 0; _x < 16; _x++) { *_fnt_str_ptr++ = 0x20 + _y * 16 + _x; }
		*_fnt_str_ptr++ = '\n';
	}
	*_fnt_str_ptr = '\0';
	CFont const &_fnt1 = CFont::FontSmall_get();
	CFont const &_fnt2 = CFont::FontLarge_get();
	uint32_t _lines_len[32]; uint32_t _lines_len_count; uint32_t _lines_len_max;
	_lines_len_count = 32; _lines_len_max = _graph.StringMeasure(_fnt1, false, _lines_len, &_lines_len_count, _fnt_str).Width_get(); _graph.RectDraw(SRect(SPoint(5, 5), _graph.StringDrawEx(SPoint(6, 6), EColor::White, EColor::Black, false, false, _fnt1, false, EAlignment::End, _lines_len, _lines_len_count, _lines_len_max, _fnt_str) + SSize(2, 2)), true, EColor::BlueViolet);
	_lines_len_count = 32; _lines_len_max = _graph.StringMeasure(_fnt1, true, _lines_len, &_lines_len_count, _fnt_str).Width_get(); _graph.RectDraw(SRect(SPoint(155, 5), _graph.StringDrawEx(SPoint(156, 6), EColor::White, EColor::Black, false, false, _fnt1, true, EAlignment::Middle, _lines_len, _lines_len_count, _lines_len_max, _fnt_str) + SSize(2, 2)), true, EColor::BlueViolet);
	_lines_len_count = 32; _lines_len_max = _graph.StringMeasure(_fnt2, false, _lines_len, &_lines_len_count, _fnt_str).Width_get(); _graph.RectDraw(SRect(SPoint(5, 61), _graph.StringDrawEx(SPoint(6, 62), EColor::White, EColor::Black, false, false, _fnt2, false, EAlignment::End, _lines_len, _lines_len_count, _lines_len_max, _fnt_str) + SSize(2, 2)), true, EColor::BlueViolet);
	_lines_len_count = 32; _lines_len_max = _graph.StringMeasure(_fnt2, true, _lines_len, &_lines_len_count, _fnt_str).Width_get(); _graph.RectDraw(SRect(SPoint(155, 61), _graph.StringDrawEx(SPoint(156, 62), EColor::White, EColor::Black, false, false, _fnt2, true, EAlignment::Middle, _lines_len, _lines_len_count, _lines_len_max, _fnt_str) + SSize(2, 2)), true, EColor::BlueViolet);

//	lcd_color _row[_width << 2]; for(unsigned int _x = 0; _x < _width << 2; _x++) { _row[_x] = rand() ^ (rand() << 1); }
//	while(!*is_abort_requested) {
//		for(unsigned int _y = 0; _y < _height; _y++) {
//			LCD_CopyRect(0, _y, _width, 1, _row + (rand() % (_width * 3)));
//		}
//		yield();
//	}
//	while(!*is_abort_requested) {
//		unsigned int _x1 = rand() % _width, _x2 = rand() % _width; if(_x1 > _x2) { unsigned int _t = _x1; _x1 = _x2; _x2 = _t; }
//		unsigned int _y1 = rand() % _height, _y2 = rand() % _height; if(_y1 > _y2) { unsigned int _t = _y1; _y1 = _y2; _y2 = _t; }
//		lcd_color _color = rand() ^ (rand() << 1);
//		_graph.RectFill(CRect(_x1, _y1, _x2 - _x1 + 1, _y2 - _y1 + 1), (EColor)_color);
//		yield();
//	}
//	while(!*is_abort_requested) {
//		unsigned int _x1 = rand() % _width, _x2 = rand() % _width; if(_x1 > _x2) { unsigned int _t = _x1; _x1 = _x2; _x2 = _t; }
//		unsigned int _y1 = rand() % _height, _y2 = rand() % _height; if(_y1 > _y2) { unsigned int _t = _y1; _y1 = _y2; _y2 = _t; }
//		lcd_color _color1 = rand() ^ (rand() << 1);
//		lcd_color _color2 = rand() ^ (rand() << 1);
//		_graph.ElipseDrawFill(CRect(_x1, _y1, _x2 - _x1 + 1, _y2 - _y1 + 1), (EColor)_color1, (EColor)_color2);
//		yield();
//	}

//	DBG_SendFormat("Freemem init: %d\n", buddymemGetFree());
//	DBG_SendFormat("sizeof(CFrame): %d\n", sizeof(CFrame));

	CWindow *_window = new CWindow(SPoint(15, 15), SSize(_width >> 1, _height >> 1), nullptr, "Main window", CWindow::EStyleFlags::Border);
	CStack *_stack = new CStack(*_window, nullptr, EOrientation::Vertical, EAlignmentEx::Stretch, CStack::EAlignmentStack::FillFirst, false);
	new CPrimitiveSpace(*(new CBorder(*_stack, 2, CBorder::EType::Thin3DOutside, EColor::Cyan)), SSize(200, 28));
	new CPrimitiveSpace(*(new CBorder(*_stack, 2, CBorder::EType::StaticOutside, EColor::_Invalid)), SSize(230, 21));
	new CPrimitiveSpace(*(new CBorder(*_stack, 2, CBorder::EType::StaticInside, EColor::_Invalid)), SSize(180, 33));
	new CPrimitiveSeparator(*_stack, EOrientation::Horizontal, true);
	CStack *_stack2 = new CStack(*_stack, nullptr, EOrientation::Horizontal, EAlignmentEx::Stretch, CStack::EAlignmentStack::Spread, false);
	new CButton(*_stack2);
	new CButton(*_stack2, &_img16_cpma, EColor::Black, nullptr);
	new CPrimitiveSeparator(*_stack2, EOrientation::Vertical, false);
	CButton *_btn = new CButton(*_stack2, nullptr, nullptr, "The quick brown\nfox jumps over\nthe lazy snoopy-dog"); _btn->ColorForeground_set(EColor::White); _btn->FontIsShadow_set(true);
	new CButton(*_stack2, &_img16_cpma, EColor::Black, "The quick brown\nfox jumps over\nthe lazy snoopy-dog");
	CStack *_stack3 = new CStack(*_stack, nullptr, EOrientation::Horizontal, EAlignmentEx::Middle, CStack::EAlignmentStack::Middle, false);
	new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, nullptr, nullptr);
	new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, nullptr, EColor::Yellow);
	new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, EColor::Red, nullptr);
	new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, EColor::Green, EColor::White);
	CStack *_stack4 = new CStack(*_stack, nullptr, EOrientation::Horizontal, EAlignmentEx::Middle, CStack::EAlignmentStack::Middle, false);
	new CCheckBox(*_stack4, true, nullptr, "Inactive");
	new CCheckBox(*_stack4, false, true, "Checked");
	new CCheckBox(*_stack4, false, false, "Unchecked");
	_window->RepaintWithBackground();

	DBG_PutFormat("Class name is CWindow: %s\n", is_type(_window, CWindow) ? "true" : "false");
	DBG_PutFormat("Class name is CFrame: %s\n", is_type(_window, CFrame) ? "true" : "false");
	DBG_PutFormat("Class name is CFont: %s\n", is_type(_window, CFont) ? "true" : "false");

//	DBG_SendFormat("Freemem window: %d\n", buddymemGetFree());

	while(!*is_abort_requested) { bool _is_processed = CFrameEvent::_EventDispatch(); if(!_is_processed) { *is_inited = true; yield(); } }
	delete _window;

//	DBG_SendFormat("Freemem end: %d\n", buddymemGetFree());
}
