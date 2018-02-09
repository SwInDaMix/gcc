/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CPrimitives.cpp
 ***********************************************************************/

#include "Drawing.CGraphics.hpp"
#include "Frames.CFrame.hpp"
#include "Frames.CFrameEvent.hpp"
#include "Frames.CPrimitives.hpp"

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Frames;

/// *********************************************************************
CPrimitiveString::CPrimitiveString(CContainer &container_parent, EAlignment alignment_horizontal, EAlignment alignment_vertical, char const *str_string) : CFrame(container_parent.WindowRoot_get(), &container_parent, (EStyleFlags)0), d_alignment_horizontal(alignment_horizontal), d_alignment_vertical(alignment_vertical), d_str_string(str_string) { typeid_ctor(); }
/*override*/ CPrimitiveString::~CPrimitiveString() { typeid_dtor(); }

/*override*/ void CPrimitiveString::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	CFrame::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		CFont const &_font = Font_get(); bool _font_is_bold = FontIsBold_get(), _font_is_shadow = FontIsShadow_get();
		uint32_t _lines_len[32]; uint32_t *_lines_len_ptr = nullptr; uint32_t _lines_len_count = 0, _lines_len_max = 0; uint32_t _diff_width = 0, _diff_height = 0;
		if(d_alignment_horizontal != EAlignment::Start || d_alignment_vertical != EAlignment::Start) {
			SSize const &_sz_actual = Size_get();
			_lines_len_ptr = _lines_len; _lines_len_count = sizeof_array(_lines_len);
			SSize _sz_str = CGraphics::StringMeasure(_font, _font_is_bold, _lines_len, &_lines_len_count, d_str_string); _lines_len_max = _sz_str.Width_get();
			if(d_alignment_horizontal != EAlignment::Start) {
				_diff_width = _sz_actual.Width_get() - _lines_len_max;
				if(d_alignment_horizontal == EAlignment::Middle) _diff_width >>= 1;
			}
			if(d_alignment_vertical != EAlignment::Start) {
				_diff_height = _sz_actual.Height_get() - _sz_str.Height_get();
				if(d_alignment_vertical == EAlignment::Middle) _diff_height >>= 1;
			}
		}
		_g->StringDrawEx(SPoint(_diff_width, _diff_height), ColorForeground_get(), EColor::_Invalid, true, _font_is_shadow, _font, _font_is_bold, d_alignment_horizontal, _lines_len_ptr, _lines_len_count, _lines_len_max, d_str_string);
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CPrimitiveString::CalcFitSize() {
	CFont const &_font = Font_get(); bool _font_is_bold = FontIsBold_get();
	uint32_t _lines_len[32]; uint32_t _lines_len_count = sizeof_array(_lines_len);
	return CGraphics::StringMeasure(_font, _font_is_bold, _lines_len, &_lines_len_count, d_str_string);
}
/// *********************************************************************
CPrimitiveImage::CPrimitiveImage(CContainer &container_parent, EAlignment alignment_horizontal, EAlignment alignment_vertical, CImage const &image, SNullable<EColor> const &clr_transparent) : CFrame(container_parent.WindowRoot_get(), &container_parent, (EStyleFlags)0), d_alignment_horizontal(alignment_horizontal), d_alignment_vertical(alignment_vertical), d_type(clr_transparent.IsValueSet_get() ? EType::ColorWithTransparent : EType::Color), d_image(image), d_clr_transparent(clr_transparent) { typeid_ctor(); }
CPrimitiveImage::CPrimitiveImage(CContainer &container_parent, EAlignment alignment_horizontal, EAlignment alignment_vertical, CImage1 const &image, SNullable<EColor> const &clr_fore, SNullable<EColor> const &clr_back) : CFrame(container_parent.WindowRoot_get(), &container_parent, (EStyleFlags)0), d_alignment_horizontal(alignment_horizontal), d_alignment_vertical(alignment_vertical), d_type(EType::Monochrome), d_image(image), d_clr_transparent(clr_back) { typeid_ctor(); ColorForeground_set(clr_fore); ColorBackground_set(clr_back); }
/*override*/ CPrimitiveImage::~CPrimitiveImage() { typeid_dtor(); }

/*override*/ void CPrimitiveImage::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	CFrame::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		uint32_t _diff_width = 0, _diff_height = 0;
		if(d_alignment_horizontal != EAlignment::Start || d_alignment_vertical != EAlignment::Start) {
			SSize const &_sz_actual = Size_get(); SSize const &_sz_img = d_image.Size_get();
			if(d_alignment_horizontal != EAlignment::Start) {
				_diff_width = _sz_actual.Width_get() - _sz_img.Width_get();
				if(d_alignment_horizontal == EAlignment::Middle) _diff_width >>= 1;
			}
			if(d_alignment_vertical != EAlignment::Start) {
				_diff_height = _sz_actual.Height_get() - _sz_img.Height_get();
				if(d_alignment_vertical == EAlignment::Middle) _diff_height >>= 1;
			}
		}
		if(d_type == EType::Color) _g->ImageDraw(SPoint(_diff_width, _diff_height), d_image);
		else if(d_type == EType::ColorWithTransparent) _g->ImageDraw(SPoint(_diff_width, _diff_height), d_image, d_clr_transparent);
		else if(d_type == EType::Monochrome) _g->ImageDraw1(SPoint(_diff_width, _diff_height), (CImage1&)d_image, ColorForeground_get(), ColorBackground_get());
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CPrimitiveImage::CalcFitSize() { return d_image.Size_get(); }
/// *********************************************************************
CPrimitiveSeparator::CPrimitiveSeparator(CContainer &container_parent, EOrientation orientation, bool is_inside) : CFrame(container_parent.WindowRoot_get(), &container_parent, (EStyleFlags)0), d_orientation(orientation), d_is_inside(is_inside) { typeid_ctor(); }
/*override*/ CPrimitiveSeparator::~CPrimitiveSeparator() { typeid_dtor(); }

/*override*/ void CPrimitiveSeparator::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	CFrame::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		SSize const &_sz_actual = Size_get(); uint32_t _width_actual = _sz_actual.Width_get(), _height_actual = _sz_actual.Height_get(), _length, _offset;
		if(d_orientation == EOrientation::Horizontal) { _length = _width_actual; _offset = _height_actual; } else { _length = _height_actual; _offset = _width_actual; }
		if(_length) {
			_offset = (_offset - 2) >> 1; SPoint _pt_static = SPoint(); if(d_orientation == EOrientation::Horizontal) _pt_static.Y_set(_offset); else _pt_static.X_set(_offset);
			_g->LineStatic(_pt_static, d_orientation, _length, d_is_inside, FRAME_COLOR_MIDDLE, FRAME_COLOR_LIGHT);
		}
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CPrimitiveSeparator::CalcFitSize() { return d_orientation == EOrientation::Horizontal ? SSize(0, 2) : SSize(2, 0); }
/// *********************************************************************
CPrimitiveSpace::CPrimitiveSpace(CContainer &container_parent, SSize const &sz_test) : CFrame(container_parent.WindowRoot_get(), &container_parent, (EStyleFlags)0), d_sz_test(sz_test) { typeid_ctor(); }
/*override*/ CPrimitiveSpace::~CPrimitiveSpace() { typeid_dtor(); }

/*override*/ void CPrimitiveSpace::OnEvent(CFrameEvent &event) {

	CFrame::OnEvent(event);

	CFrameEvent::EEvent _event = event.Event_get();
	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		SSize const &_sz_actual = Size_get(); uint32_t _width_actual = _sz_actual.Width_get(), _height_actual = _sz_actual.Height_get();
		SSize const &_sz_fit = SizeFit_get(); uint32_t _width_fit = _sz_fit.Width_get(), _height_fit = _sz_fit.Height_get();
		_g->RectDraw(SRect(0, 0, _width_actual, _height_actual), false, EColor::Red);
		_g->RectDrawFill(SRect((_width_actual - _width_fit) >> 1, (_height_actual - _height_fit) >> 1, _width_fit, _height_fit), false, EColor::Red, EColor::Yellow);
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CPrimitiveSpace::CalcFitSize() { return d_sz_test; }
/// *********************************************************************
