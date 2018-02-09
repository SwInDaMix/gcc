/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CContainers.cpp
 ***********************************************************************/

#include "Types.h"
#include "Frames.CContainers.hpp"
#include "Frames.CFrame.hpp"
#include "Frames.CFrameEvent.hpp"

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Frames;

/// *********************************************************************
CStack::CStack(CContainer &container_parent, SNullable<uint8_t> separation_width, EOrientation orientation, EAlignmentEx alignment_array, EAlignmentStack alignment_stack, bool is_same_length) : CContainerMulti(container_parent.WindowRoot_get(), &container_parent, false, (EStyleFlags)0), d_orientation(orientation), d_alignment_array(alignment_array), d_alignment_stack(alignment_stack), d_is_same_length(is_same_length) { typeid_ctor(); SeparationWidth_set(separation_width); }
/*override*/ CStack::~CStack() { typeid_dtor(); }

/*override*/ void CStack::OnEvent(CFrameEvent &event) {

	CContainerMulti::OnEvent(event);
}
/*override*/ SSize CStack::CalcFitSize() {
	uint32_t _width_max, _height_max, _count;
	uint32_t _width_fit = _CalcFrameAlignment(&_width_max, &_height_max, &_count);
	return d_orientation == EOrientation::Horizontal ? SSize(_width_fit, _height_max) : SSize(_height_max, _width_fit);
}
/*override*/ void CStack::CalcChildRects(void(CFrame::*f_set_rect)(SRect const &rc)) {
	uint32_t _width_actual, _height_actual;
	SSize const &_sz_actual = Size_get(); if(d_orientation == EOrientation::Horizontal) { _width_actual = _sz_actual.Width_get(); _height_actual = _sz_actual.Height_get(); } else { _width_actual = _sz_actual.Height_get(); _height_actual = _sz_actual.Width_get(); }
	uint32_t _width_max, _height_max, _count;
	uint32_t _width_fit = _CalcFrameAlignment(&_width_max, &_height_max, &_count);
	uint32_t _diff_width = _width_actual - _width_fit, _spread_rem; div_t _spread_div;
	if(d_alignment_stack == EAlignmentStack::Spread) { _spread_div = div(_diff_width, _count); _spread_rem = 0; }
	CFrame *_frame = ChildFrameFirst_get(); uint32_t _current = 0, _x, _y;
	SPoint const &_pt = Location_get(); if(d_orientation == EOrientation::Horizontal) { _x = _pt.X_get(); _y = _pt.Y_get(); } else { _x = _pt.Y_get(); _y = _pt.X_get(); }
	if(d_alignment_stack == EAlignmentStack::Middle || d_alignment_stack == EAlignmentStack::End) {
		uint32_t _xoff = _diff_width;
		if(d_alignment_stack == EAlignmentStack::Middle) _xoff >>= 1;
		_x += _xoff;
	}
	uint8_t _separation_width = SeparationWidth_get();
	while(_frame) {
		SSize const &_sz_frame = _frame->SizeFit_get(); uint32_t _w, _h, _yoff = 0;
		if(d_orientation == EOrientation::Horizontal) { _w = _sz_frame.Width_get(); _h = _sz_frame.Height_get(); } else { _w = _sz_frame.Height_get(); _h = _sz_frame.Width_get(); }
		if(d_alignment_array != EAlignmentEx::Start && d_alignment_array != EAlignmentEx::Stretch) {
			_yoff = _height_actual - _h;
			if(d_alignment_array == EAlignmentEx::Middle) _yoff >>= 1;
		}

		uint32_t _width_frm = d_is_same_length ? _width_max : _w;
		if(d_alignment_stack == EAlignmentStack::Spread) {
			_width_frm += _spread_div.quot; _spread_rem += _spread_div.rem;
			if(_spread_rem >= _count) { _spread_rem -= _count; _width_frm++; }
		}
		else if((d_alignment_stack == EAlignmentStack::FillFirst && !_current) || (d_alignment_stack == EAlignmentStack::FillLast && _current == (_count - 1))) { _width_frm += _diff_width; }
		uint32_t _height_frm = d_alignment_array == EAlignmentEx::Stretch ? _height_actual : _h;

		if(d_orientation == EOrientation::Horizontal) { (_frame->*f_set_rect)(SRect(_x, _y + _yoff, _width_frm, _height_frm)); } else { (_frame->*f_set_rect)(SRect(_y + _yoff, _x, _height_frm, _width_frm)); }
		_frame = _frame->FrameNext_get(); _current++; _x += (_width_frm + _separation_width);
	}
}

uint32_t CStack::_CalcFrameAlignment(uint32_t *width_max, uint32_t *height_max, uint32_t *count) {
	CFrame *_frame = ChildFrameFirst_get(); uint32_t _width = 0, _width_max = 0, _height_max = 0, _count = 0, _sep_gaps = 0;
	while(_frame) {
		_count++;
		uint32_t _w, _h;
		SSize const &_sz_frm = _frame->SizeFit_get(); if(d_orientation == EOrientation::Horizontal) { _w = _sz_frm.Width_get(); _h = _sz_frm.Height_get(); } else { _w = _sz_frm.Height_get(); _h = _sz_frm.Width_get(); }
		_width += _w; maxwith(_width_max, _w); maxwith(_height_max, _h);
		_frame = _frame->FrameNext_get();
	}
	if(_count) _sep_gaps = (_count - 1) * SeparationWidth_get();
	*width_max = _width_max; *height_max = _height_max; *count = _count;
	return (d_is_same_length ? _count * _width_max : _width) + _sep_gaps;
}
/// *********************************************************************
/// *********************************************************************
CTabMulti::CTabMulti(CContainer &container_parent) : CContainerMulti(container_parent.WindowRoot_get(), &container_parent, true, (EStyleFlags)0) { typeid_ctor(); }
/*override*/ CTabMulti::~CTabMulti() { typeid_dtor(); }

/*override*/ void CTabMulti::OnEvent(CFrameEvent &event) {

	CContainerMulti::OnEvent(event);
}
/*override*/ SSize CTabMulti::CalcFitSize() {
	SSize _sz;
	CFrame *_frame = ChildFrameFirst_get();
	while(_frame) { _sz |= _frame->SizeFit_get(); _frame = _frame->FrameNext_get(); }
	return _sz;
}
/*override*/ void CTabMulti::CalcChildRects(void(CFrame::*f_set_rect)(SRect const &rc)) {
	SRect const _rc(Location_get(), Size_get());
	CFrame *_frame = ChildFrameFirst_get();
	while(_frame) { (_frame->*f_set_rect)(_rc); _frame = _frame->FrameNext_get(); }
}
/// *********************************************************************
