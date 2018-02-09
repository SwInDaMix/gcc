/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CControls.cpp
 ***********************************************************************/

#include "Frames.CContainers.hpp"
#include "Frames.CControls.hpp"
#include "Frames.CFrame.hpp"
#include "Frames.CFrameEvent.hpp"
#include "Frames.CPrimitives.hpp"
#include "images1/img1_frame_checkbox_checked.ih1"

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Frames;

/// *********************************************************************
CBorder::CBorder(CContainer &container_parent, SNullable<uint8_t> separation_width, EType type, EColor clr_simple) : CContainerSingle(container_parent.WindowRoot_get(), &container_parent, (EStyleFlags)0), d_type(type), d_clr_simple(clr_simple) { typeid_ctor(); SeparationWidth_set(separation_width); }
/*override*/ CBorder::~CBorder() { typeid_dtor(); }

/*override*/ void CBorder::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	CContainerSingle::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		SSize const &_sz_actual = Size_get(); SRect _rc_border(0, 0, _sz_actual.Width_get(), _sz_actual.Height_get());

		bool _b1 = d_type == EType::SimpleRounded;
		if(d_type == EType::Simple || _b1) _g->RectDraw(_rc_border, _b1, d_clr_simple);
		else {
			_b1 = d_type == EType::Thin3DInside; bool _b2 = d_type == EType::Thick3DOutside, _b3 = d_type == EType::Thick3DInside;
			if(d_type == EType::Thin3DOutside || _b1 || _b2 || _b3) _g->RectFrame3D(_rc_border, _b2 | _b3, _b1 | _b3, FRAME_COLOR_DARK, FRAME_COLOR_LIGHT);
			else {
				_b1 = d_type == EType::StaticInside;
				if(d_type == EType::StaticOutside || _b1) _g->RectFrameStatic(_rc_border, _b1, FRAME_COLOR_MIDDLE, FRAME_COLOR_LIGHT);
			}
		}
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CBorder::CalcFitSize() {
	CFrame *_frame_child = ChildFrame_get(); uint8_t _separation_width = SeparationWidth_get();
	uint32_t _def_width = 1 + _separation_width, _def_height = 1 + _separation_width;
	if(d_type == EType::Thick3DOutside || d_type == EType::Thick3DInside || d_type == EType::StaticOutside || d_type == EType::StaticInside) { _def_width++; _def_height++; }
	_def_width <<= 1; _def_height <<= 1;
	return _frame_child ? _frame_child->SizeFit_get() + SSize(_def_width, _def_height) : SSize(_def_width, _def_height);
}
/*override*/ void CBorder::CalcChildRects(void(CFrame::*f_set_rect)(SRect const &rc)) {
	CFrame *_frame_child = ChildFrame_get();
	if(_frame_child) {
		SPoint const &_pt_actual = Location_get(); SSize const &_sz_actual = Size_get(); uint8_t _separation_width = SeparationWidth_get();
		uint32_t _off_x = 1 + _separation_width, _off_y = 1 + _separation_width, _def_width, _def_height;
		if(d_type == EType::Thick3DOutside || d_type == EType::Thick3DInside || d_type == EType::StaticOutside || d_type == EType::StaticInside) { _off_x++; _off_y++; }
		_def_width = _off_x << 1; _def_height = _off_y << 1;
		(_frame_child->*f_set_rect)(SRect(_pt_actual.X_get() + _off_x, _pt_actual.Y_get() + _off_y, _sz_actual.Width_get() - _def_width, _sz_actual.Height_get() - _def_height));
	}
}
/// *********************************************************************
CButton::CButton(CContainer &container_parent, CImage const *image, SNullable<EColor> const &clr_1, SNullable<EColor> const &clr_2, char const *str_text) : CContainerSingle(container_parent.WindowRoot_get(), &container_parent, EStyleFlags::Focusable) {
	typeid_ctor();
	if(image || str_text) {
		CContainer *_container = image && str_text ? (CContainer *)(new CStack(*this, 2, EOrientation::Horizontal, EAlignmentEx::Middle, CStack::EAlignmentStack::Middle, false)) : (CContainer *)this;
		if(image) {
			CImage1 const *_image1 = as_type(image, CImage1);
			if(_image1) new CPrimitiveImage(*_container, EAlignment::Middle, EAlignment::Middle, *_image1, clr_1, clr_2);
			else new CPrimitiveImage(*_container, EAlignment::Middle, EAlignment::Middle, *image, clr_1);
		}
		if(str_text) new CPrimitiveString(*_container, EAlignment::Middle, EAlignment::Middle, str_text);
	}
}
CButton::CButton(CContainer &container_parent) : CButton(container_parent, (CImage *)nullptr, nullptr, nullptr, nullptr) {  }
CButton::CButton(CContainer &container_parent, CImage const *image, SNullable<EColor> const &clr_transparent, char const *str_text) : CButton(container_parent, image, clr_transparent, nullptr, str_text) {  }
CButton::CButton(CContainer &container_parent, CImage1 const *image, SNullable<EColor> const &clr_fore, SNullable<EColor> const &clr_back, char const *str_text) : CButton(container_parent, (CImage *)image, clr_fore, clr_back, str_text) {  }
/*override*/ CButton::~CButton() { typeid_dtor(); }

/*override*/ void CButton::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	CContainerSingle::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics*)event.Object_get();
		SSize const &_sz_actual = Size_get(); uint32_t _width_actual = _sz_actual.Width_get(), _height_actual = _sz_actual.Height_get();
		_g->RectFrame3D(SRect(0, 0, _width_actual, _height_actual), true, false, EColor::Black, EColor::White);
		if(IsFocused_get()) _g->RectFocus(SRect(2, 2, _width_actual - 4, _height_actual - 4), EColor::Black);
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CButton::CalcFitSize() {
	CFrame *_frame_child = ChildFrame_get();
	return _frame_child ? _frame_child->SizeFit_get() + SSize(8, 8) : SSize(8, 8);
}
/*override*/ void CButton::CalcChildRects(void(CFrame::*f_set_rect)(SRect const &rc)) {
	CFrame *_frame_child = ChildFrame_get();
	if(_frame_child) {
		SPoint const &_pt_actual = Location_get(); SSize const &_sz_actual = Size_get();
		(_frame_child->*f_set_rect)(SRect(_pt_actual.X_get() + 4, _pt_actual.Y_get() + 4, _sz_actual.Width_get() - 8, _sz_actual.Height_get() - 8));
	}
}
/// *********************************************************************
/*static*/ CImage1 const CCheckBox::s_img1_frame_checkbox_checked(img1_frame_checkbox_checked_imgbmp, IMG1_FRAME_CHECKBOX_CHECKED_IMAGE_WIDTH, IMG1_FRAME_CHECKBOX_CHECKED_IMAGE_HEIGHT);

CCheckBox::CCheckBox(CContainer &container_parent, bool is_tristate, SNullable<bool> is_checked, char const *str_text) : CContainerSingle(container_parent.WindowRoot_get(), &container_parent, EStyleFlags::Focusable), d_is_tristate(is_tristate), d_is_checked(is_checked) {
	typeid_ctor();
	if(str_text) { new CPrimitiveString(*this, EAlignment::Start, EAlignment::Middle, str_text); }
	if(!d_is_tristate && !d_is_checked.IsValueSet_get()) d_is_checked = false;
}
CCheckBox::CCheckBox(CContainer &container_parent, bool is_tristate, SNullable<bool> is_checked) : CCheckBox(container_parent, is_tristate, is_checked, nullptr) {  }
/*override*/ CCheckBox::~CCheckBox() { typeid_dtor(); }

/*override*/ void CCheckBox::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	CContainerSingle::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics*)event.Object_get();
		SSize const &_sz_actual = Size_get(); uint32_t _width_actual = _sz_actual.Width_get(), _height_actual = _sz_actual.Height_get(); SSize const &_sz_img_checked = s_img1_frame_checkbox_checked.Size_get(); uint32_t _width_img_checked = _sz_img_checked.Width_get(), _height_img_checked = _sz_img_checked.Height_get(); uint8_t _separation_width = SeparationWidth_get();
		uint32_t _width_checkbox = _width_img_checked + 4, _height_checkbox = _height_img_checked + 4;
		uint32_t _off_y = (_height_actual - _height_checkbox) >> 1;
		_g->RectFrame3D(SRect(0, _off_y, _width_checkbox, _height_checkbox), true, true, FRAME_COLOR_DARK, FRAME_COLOR_LIGHT);
		if(!d_is_checked.IsValueSet_get() || (bool)d_is_checked) { _g->ImageDraw1(SPoint(2, _off_y + 2), s_img1_frame_checkbox_checked, d_is_checked.IsValueSet_get() ? FRAME_COLOR_DARK : FRAME_COLOR_INACTIVE, d_is_checked.IsValueSet_get() ? FRAME_COLOR_LIGHT : FRAME_COLOR_CONTROL); }
		else _g->RectFill(SRect(2, _off_y + 2, _width_img_checked, _height_img_checked), FRAME_COLOR_LIGHT);
		if(IsFocused_get()) _g->RectFocus(SRect(_width_checkbox + _separation_width, 0, _width_actual - _width_checkbox - _separation_width, _height_actual), EColor::Black);
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CCheckBox::CalcFitSize() {
	uint32_t _width_fit, _height_fit; SSize const &_sz_img_checked = s_img1_frame_checkbox_checked.Size_get(); _width_fit = _sz_img_checked.Width_get(); _height_fit = _sz_img_checked.Height_get();
	_width_fit += 4 + SeparationWidth_get(); _height_fit += 4;
	CFrame *_frame_child = ChildFrame_get();
	if(_frame_child) {
		SSize const &_sz_fit_child = _frame_child->SizeFit_get();
		_width_fit += _sz_fit_child.Width_get() + 4; uint32_t _height_child_fit = _sz_fit_child.Height_get() + 4; maxwith(_height_fit, _height_child_fit);
	}
	return SSize(_width_fit, _height_fit);
}
/*override*/ void CCheckBox::CalcChildRects(void(CFrame::*f_set_rect)(SRect const &rc)) {
	CFrame *_frame_child = ChildFrame_get();
	if(_frame_child) {
		SPoint const &_pt_actual = Location_get(); SSize const &_sz_actual = Size_get();
		uint32_t _width_checkbox = s_img1_frame_checkbox_checked.Size_get().Width_get() + 4 + SeparationWidth_get();
		(_frame_child->*f_set_rect)(SRect(_pt_actual.X_get() + _width_checkbox + 2, _pt_actual.Y_get() + 2, _sz_actual.Width_get() - _width_checkbox - 4, _sz_actual.Height_get() - 4));
	}
}
/// *********************************************************************
