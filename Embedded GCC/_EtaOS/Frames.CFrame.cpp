/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Frames.CFrame.cpp
 ***********************************************************************/

#include "Frames.CFrame.hpp"
#include "Drawing.CGraphics.hpp"
#include "Keyboard.CKeyboard.hpp"
#include "images1/img1_frame_close.ih1"
#include "images1/img1_frame_maximize.ih1"
#include "images1/img1_frame_minimize.ih1"
#include "images1/img1_frame_restore.ih1"

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Frames;
using namespace System::Keyboard;

/// *********************************************************************
#define frame_cached_ptr_body(Class, Type, GName, LName, DefaultValue) \
	bool Class::_ResetValidFlags_##GName(bool stop_if_set) { stop_if_set &= (bool)d_##LName; if(!stop_if_set) { d_cached_##LName = nullptr; } return stop_if_set; } \
	Type &Class::GName##_get() { if(!d_cached_##LName) { d_cached_##LName = d_##LName ? d_##LName : (d_container_parent ? &d_container_parent->GName##_get() : DefaultValue); } return *d_cached_##LName; } \
	void Class::GName##_set(Type *value) { if(d_##LName != value) { d_##LName = value; _ResetValidFlags(false, &Class::_ResetValidFlags_##GName); } }
#define frame_cached_ptr_init(LName) d_##LName(nullptr), d_cached_##LName(nullptr)
#define frame_cached_nullable_body(Class, Type, GName, LName, DefaultValue) \
	bool Class::_ResetValidFlags_##GName(bool stop_if_set) { stop_if_set &= d_##LName.IsValueSet_get(); if(!stop_if_set) { d_cached_##LName = nullptr; } return stop_if_set; } \
	Type Class::GName##_get() { if(!d_cached_##LName.IsValueSet_get()) { d_cached_##LName = (d_##LName.IsValueSet_get() ? (Type)d_##LName : (d_container_parent ? d_container_parent->GName##_get() : DefaultValue)); } return d_cached_##LName; } \
	void Class::GName##_set(SNullable<Type> const &value) { if(d_##LName != value) { d_##LName = value; _ResetValidFlags(false, &Class::_ResetValidFlags_##GName); } }
#define frame_cached_nullable_init(LName) d_##LName(), d_cached_##LName()

static void _FrameEventFinalizer_CGraphics(CFrameEvent &event) { CGraphics *_g = reinterpret_cast<CGraphics*>(event.Object_get()); delete _g; }
/// *********************************************************************
/*static*/ CImage1 CFrame::s_img1_frame_close(img1_frame_close_imgbmp, IMG1_FRAME_CLOSE_IMAGE_WIDTH, IMG1_FRAME_CLOSE_IMAGE_HEIGHT);
/*static*/ CImage1 CFrame::s_img1_frame_maximize(img1_frame_maximize_imgbmp, IMG1_FRAME_MAXIMIZE_IMAGE_WIDTH, IMG1_FRAME_MAXIMIZE_IMAGE_HEIGHT);
/*static*/ CImage1 CFrame::s_img1_frame_minimize(img1_frame_minimize_imgbmp, IMG1_FRAME_MINIMIZE_IMAGE_WIDTH, IMG1_FRAME_MINIMIZE_IMAGE_HEIGHT);
/*static*/ CImage1 CFrame::s_img1_frame_restore(img1_frame_restore_imgbmp, IMG1_FRAME_RESTORE_IMAGE_WIDTH, IMG1_FRAME_RESTORE_IMAGE_HEIGHT);

CFrame::CFrame(CWindow &window_root, CContainer *container_parent, EStyleFlags style_flags) : d_window_root(window_root), d_container_parent(container_parent), d_cached_pt_location(), d_cached_sz_size(), d_cached_sz_size_fit(), frame_cached_ptr_init(font), frame_cached_nullable_init(font_is_bold), frame_cached_nullable_init(font_is_shadow), frame_cached_nullable_init(color_foreground), frame_cached_nullable_init(color_background), d_style_flags(style_flags) {
	typeid_ctor();
	d_frame_prev = d_frame_next = nullptr; if(d_container_parent) d_container_parent->_AttachChildFrame(*this);
	CFrameEvent::EventSendAsync(this, *this, CFrameEvent::EEvent::Created, CFrameEvent::EEventType::Direct, 0, 0, nullptr, nullptr);
	if(!d_window_root.d_frame_focus) { d_window_root.d_frame_focus = d_window_root.GetFrameFirstStyled(EStyleFlags::Focusable); }
}
/*virtual*/ CFrame::~CFrame() {
	typeid_dtor();
	CFrameEvent::EventSend(this, *this, CFrameEvent::EEvent::Closed, CFrameEvent::EEventType::Direct, 0, 0, nullptr, nullptr);
	CFrameEvent::EventDiscardAllForFrame(*this);
	if(d_container_parent) d_container_parent->_DeattachChildFrame(*this);
	if(d_window_root.d_frame_focus == this) d_window_root.d_frame_focus = d_window_root.d_frame_focus->GetFrameHierarchyNextStyled(false, EStyleFlags::Focusable);
}

SPoint const &CFrame::Location_get() { d_window_root._EnsureDiscardedSizesAndLocations(); if(!d_cached_pt_location.IsValueSet_get()) { if(d_container_parent) d_container_parent->CalcChildRects(&CFrame::SetRectFromParent); else _SetLocationFromParent(d_window_root.d_pt_location); } return d_cached_pt_location; }
SSize const &CFrame::Size_get() { d_window_root._EnsureDiscardedSizesAndLocations(); if(!d_cached_sz_size.IsValueSet_get()) { if(d_container_parent) d_container_parent->CalcChildRects(&CFrame::SetRectFromParent); else _SetSizeFromParent(SizeFit_get()); } return d_cached_sz_size; }
SSize const &CFrame::SizeFit_get() { d_window_root._EnsureDiscardedSizesAndLocations(); if(!d_cached_sz_size_fit.IsValueSet_get()) { d_cached_sz_size_fit = d_sz_min | CalcFitSize(); } return d_cached_sz_size_fit; }
SSize const &CFrame::SizeMin_get() { return d_sz_min; }
void CFrame::SizeMin_set(SSize const &sz_min) { d_sz_min = sz_min; d_window_root.d_is_discard_sizes_and_locations = true; }

frame_cached_ptr_body(CFrame, const CFont, Font, font, &FRAME_DEFAULT_FONT)
frame_cached_nullable_body(CFrame, bool, FontIsBold, font_is_bold, FRAME_DEFAULT_FONT_IS_BOLD)
frame_cached_nullable_body(CFrame, bool, FontIsShadow, font_is_shadow, FRAME_DEFAULT_FONT_IS_SHADOW)
frame_cached_nullable_body(CFrame, EColor, ColorForeground, color_foreground, FRAME_DEFAULT_COLOR_FOREGROUND)
frame_cached_nullable_body(CFrame, EColor, ColorBackground, color_background, FRAME_DEFAULT_COLOR_BACKGROUND)

void CFrame::Repaint() { if(!flags(d_style_flags & EStyleFlags::Hidden)) CFrameEvent::EventSendAsync(this, *this, CFrameEvent::EEvent::Paint, CFrameEvent::EEventType::Direct, 0, 0, new CGraphics(SRect(Location_get(), Size_get())), _FrameEventFinalizer_CGraphics); }
void CFrame::RepaintWithBackground() { if(!flags(d_style_flags & EStyleFlags::Hidden)) CFrameEvent::EventSendAsync(this, *this, CFrameEvent::EEvent::PaintWithBackground, CFrameEvent::EEventType::Direct, 0, 0, new CGraphics(SRect(Location_get(), Size_get())), _FrameEventFinalizer_CGraphics); }
/*virtual*/ CFrame *CFrame::HitTest(SPoint const &pt) { return SRect(Location_get(), Size_get()).IsInside(pt) ? this : nullptr; }

CFrame *CFrame::GetFrameNext(bool is_cyclic) const {
	CFrame *_frame = d_frame_next;
	if(is_cyclic && !_frame) { _frame = d_container_parent->ChildFrameFirst_get(); }
	return _frame;
}
CFrame *CFrame::GetFramePrev(bool is_cyclic) const {
	CFrame *_frame = d_frame_prev;
	if(is_cyclic && !_frame) { _frame = d_container_parent->ChildFrameLast_get(); }
	return _frame;
}
CFrame *CFrame::GetFrameHierarchyNext(bool is_cyclic) const {
	CContainer *_container = as_type(this, CContainer);
	CFrame *_frame = _container ? _container->ChildFrameFirst_get() : nullptr;
	if(!_frame) {
		CFrame *_frame_current = const_cast<CFrame*>(this);
		while(true) {
			_frame = _frame_current->d_frame_next;
			while(_frame && !_frame->IsParticipating_get()) { _frame = _frame->d_frame_next; }
			if(_frame) break;
			_frame_current = _frame_current->d_container_parent;
			CWindow *_window = as_type(_frame_current, CWindow);
			if(_window) {
				if(is_cyclic) {
					_frame = _window->FrameFirst_get();
					while(_frame && !_frame->IsParticipating_get()) { _frame = _frame->d_frame_next; }
				}
				break;
			}
		}
	}
	return _frame;
}
CFrame *CFrame::GetFrameHierarchyPrev(bool is_cyclic) const {
	CFrame *_frame = d_frame_prev;
	if(_frame) {
		while(true) {
			while(_frame && !_frame->IsParticipating_get()) { _frame = _frame->d_frame_prev; }
			CContainer *_container = as_type(_frame, CContainer);
			CFrame *_frame_tmp = _container ? _container->ChildFrameLast_get() : nullptr;
			if(!_frame_tmp) break;
			_frame = _frame_tmp;
		}
	}
	else {
		CContainer *_container;
		_frame = _container = d_container_parent;
		CWindow *_window = as_type(_container, CWindow);
		if(_window) _frame = is_cyclic ? _window->FrameLast_get() : nullptr;
	}
	return _frame;
}
CFrame *CFrame::GetFrameHierarchyNextStyled(bool is_cyclic, EStyleFlags style_flags) const { return _GetFrameTabStyled(is_cyclic, &CFrame::GetFrameHierarchyNext, style_flags); }
CFrame *CFrame::GetFrameHierarchyPrevStyled(bool is_cyclic, EStyleFlags style_flags) const { return _GetFrameTabStyled(is_cyclic, &CFrame::GetFrameHierarchyPrev, style_flags); }

/*virtual*/ void CFrame::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	if(_event == CFrameEvent::EEvent::PaintWithBackground || (_event == CFrameEvent::EEvent::Paint && d_color_background.IsValueSet_get())) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		SRect _rc_frame(SPoint::Empty_get(), Size_get());
		_g->RectFill(_rc_frame, ColorBackground_get());
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*virtual*/ SSize CFrame::CalcFitSize() { return SSize::Empty_get(); }

void CFrame::SetRectFromParent(SRect const &rc) { _SetLocationFromParent(rc.Location_get()); _SetSizeFromParent(rc.Size_get()); }

void CFrame::_DiscardSizesAndLocations() {
	d_cached_pt_location = nullptr; d_cached_sz_size = nullptr; d_cached_sz_size_fit = nullptr;
	CContainer *_container = as_type(this, CContainer);
	if(_container) {
		CFrame* _frame_child = _container->ChildFrameFirst_get();
		while(_frame_child) { _frame_child->_DiscardSizesAndLocations(); _frame_child = _frame_child->FrameNext_get(); }
	}
}
bool CFrame::_UpdateFocusedFrame(CFrame *frame_focus_new) {
	if(d_window_root.d_frame_focus != frame_focus_new) {
		CFrame *frame_focus_old = d_window_root.d_frame_focus; d_window_root.d_frame_focus = frame_focus_new;
		if(frame_focus_old) frame_focus_old->RepaintWithBackground();
		if(frame_focus_new) frame_focus_new->RepaintWithBackground();
		return true;
	}
	else return false;
}
void CFrame::_SetLocationFromParent(SPoint const &pt_location) { d_cached_pt_location = pt_location; }
void CFrame::_SetSizeFromParent(SSize const &sz_size) { d_cached_sz_size = sz_size; }
CFrame *CFrame::_GetFrameTabStyled(bool is_cyclic, CFrame *(CFrame::*f_next_frame)(bool is_cyclic) const, EStyleFlags frame_styles) const {
	CFrame const *_frame = this;
	while(true) {
		_frame = (_frame->*f_next_frame)(is_cyclic);
		if(!_frame) break;
		if(flags(_frame->d_style_flags & frame_styles)) break;
		if(_frame == this) {
			if(!flags(_frame->d_style_flags & frame_styles)) _frame = nullptr;
			break;
		}
	}
	return (CFrame *)_frame;
}
void CFrame::_ResetValidFlags(bool stop_if_set, bool(CFrame::*f_reset)(bool stop_if_set)) {
	if(!(this->*f_reset)(stop_if_set)) {
		CContainer *_container = as_type(this, CContainer);
		if(_container) {
			CFrame *_frame = _container->ChildFrameFirst_get();
			while (_frame) { _frame->_ResetValidFlags(true, f_reset); _frame = _frame->FrameNext_get(); }
		}
	}
}
/// *********************************************************************
CContainer::CContainer(CWindow &window_root, CContainer *container_parent, bool is_custom, EStyleFlags style_flags) : CFrame(window_root, container_parent, style_flags), d_is_custom(is_custom), frame_cached_nullable_init(separation_width) { typeid_ctor(); }
/*override*/ CContainer::~CContainer() { typeid_dtor(); }

frame_cached_nullable_body(CContainer, uint8_t, SeparationWidth, separation_width, FRAME_DEFAULT_SEPARATION_WIDTH)

/*override*/ void CContainer::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	if(!d_is_custom && (_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground)) {
		CFrame *_frame = ChildFrameFirst_get();
		while (_frame) { _frame->Repaint(); _frame = _frame->FrameNext_get(); }
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}

	CFrame::OnEvent(event);
}

/*override*/ bool CContainer::IsFrameParticipating(CFrame const &frame) const { return !d_is_custom && frame.d_container_parent == this; }

void CContainer::_ResetValidFlags(bool stop_if_set, bool(CContainer::*f_reset)(bool stop_if_set)) {
	if(!(this->*f_reset)(stop_if_set)) {
		CFrame *_frame = ChildFrameFirst_get();
		while (_frame) {
			CContainer *_container = as_type(_frame, CContainer);
			if(_container) _container->_ResetValidFlags(true, f_reset);
			_frame = _frame->d_frame_next;
		}
	}
}
/// *********************************************************************
CContainerSingle::CContainerSingle(CWindow &window_root, CContainer *container_parent, EStyleFlags style_flags) : CContainer(window_root, container_parent, false, style_flags) { typeid_ctor(); d_frame_child = nullptr; }
/*override*/ CContainerSingle::~CContainerSingle() { typeid_dtor(); if(d_frame_child) delete d_frame_child; }

/*override*/ void CContainerSingle::OnEvent(CFrameEvent &event) {

	CContainer::OnEvent(event);
}

/*override*/ CFrame *CContainerSingle::ChildFrameFirst_get() const { return d_frame_child; }
/*override*/ CFrame *CContainerSingle::ChildFrameLast_get() const { return d_frame_child; }

/*override*/ CFrame *CContainerSingle::HitTest(SPoint const &pt) { CFrame *_frame_hit = d_frame_child ? d_frame_child->HitTest(pt) : nullptr; return _frame_hit ? _frame_hit : CContainer::HitTest(pt); }

/*override*/ void CContainerSingle::_AttachChildFrame(CFrame &frame) { if(d_frame_child) { delete d_frame_child; } d_frame_child = &frame; d_window_root.d_is_discard_sizes_and_locations = true; }
/*override*/ void CContainerSingle::_DeattachChildFrame(CFrame &frame) { d_frame_child = nullptr; d_window_root.d_is_discard_sizes_and_locations = true; }
/// *********************************************************************
CContainerMulti::CContainerMulti(CWindow &window_root, CContainer *container_parent, bool is_custom, EStyleFlags style_flags) : CContainer(window_root, container_parent, is_custom, style_flags) { typeid_ctor(); d_frame_child_first = d_frame_child_last = nullptr; }
/*override*/ CContainerMulti::~CContainerMulti() { typeid_ctor(); while (d_frame_child_first) { delete d_frame_child_first; } }

/*override*/ CFrame *CContainerMulti::ChildFrameFirst_get() const { return d_frame_child_first; }
/*override*/ CFrame *CContainerMulti::ChildFrameLast_get() const { return d_frame_child_last; }

/*override*/ void CContainerMulti::OnEvent(CFrameEvent &event) {

	CContainer::OnEvent(event);
}

/*override*/ CFrame *CContainerMulti::HitTest(SPoint const &pt) { return !d_is_custom ? HitTestMulti(pt) : nullptr; }

CFrame *CContainerMulti::HitTestMulti(SPoint const &pt) {
	CFrame *_frame = d_frame_child_first; CFrame *_frame_hit = nullptr;
	while(_frame) {
		_frame_hit = _frame->HitTest(pt);
		if(_frame_hit) break;
		_frame = _frame->d_frame_next;
	}
	return _frame_hit ? _frame_hit : CContainer::HitTest(pt);
}

/*override*/ void CContainerMulti::_AttachChildFrame(CFrame &frame) {
	if(d_frame_child_last) d_frame_child_last->d_frame_next = &frame;
	frame.d_frame_prev = d_frame_child_last; d_frame_child_last = &frame;
	if (!d_frame_child_first) d_frame_child_first = &frame;
	d_window_root.d_is_discard_sizes_and_locations = true;
}
/*override*/ void CContainerMulti::_DeattachChildFrame(CFrame &frame) {
	if (frame.d_frame_prev) frame.d_frame_prev->d_frame_next = frame.d_frame_next; else d_frame_child_first = frame.d_frame_next;
	if (frame.d_frame_next) frame.d_frame_next->d_frame_prev = frame.d_frame_prev; else d_frame_child_last = frame.d_frame_prev;
	d_window_root.d_is_discard_sizes_and_locations = true;
}
/// *********************************************************************
/*static*/ CWindow *CWindow::s_window_first;
/*static*/ CWindow *CWindow::s_window_last;

CWindow::CWindow(SPoint const &pt_location, SSize const &sz_min, SNullable<uint8_t> separation_width, char const *str_caption, EStyleFlags frame_style_flags) : CContainerSingle(*this, nullptr, (CFrame::EStyleFlags)frame_style_flags), d_frame_focus(nullptr), d_pt_location(pt_location), d_str_caption(str_caption) { typeid_ctor(); d_sz_min = sz_min; d_separation_width = separation_width; _AttachWindowAsFirst(); }
/*override*/ CWindow::~CWindow() { typeid_ctor(); _DeattachWindow(); }

/*override*/ void CWindow::OnEvent(CFrameEvent &event) {
	CFrameEvent::EEvent _event = event.Event_get();

	if(_event == CFrameEvent::EEvent::KeyDialog) {

		EKeyDialog _key_dialog = (EKeyDialog)event.Value1_get();
		if(_key_dialog == EKeyDialog::Dialog_Frame_Next) {
			CFrame *_frame_focus_new;
			if(d_frame_focus) _frame_focus_new = d_frame_focus->GetFrameHierarchyNextStyled(true, CFrame::EStyleFlags::Focusable);
			else _frame_focus_new = GetFrameFirstStyled(CFrame::EStyleFlags::Focusable);
			if(_UpdateFocusedFrame(_frame_focus_new)) event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
		}
		else if(_key_dialog == EKeyDialog::Dialog_Frame_Prev) {
			CFrame *_frame_focus_new;
			if(d_frame_focus) _frame_focus_new = d_frame_focus->GetFrameHierarchyPrevStyled(true, CFrame::EStyleFlags::Focusable);
			else _frame_focus_new = GetFrameLastStyled(CFrame::EStyleFlags::Focusable);
			if(_UpdateFocusedFrame(_frame_focus_new)) event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
		}
		else event.MarkFlags(CFrameEvent::EEventResultFlags::Canceled);
	}

	CContainerSingle::OnEvent(event);

	if(_event == CFrameEvent::EEvent::Paint || _event == CFrameEvent::EEvent::PaintWithBackground) {
		CGraphics *_g = (CGraphics *)event.Object_get();
		SRect _rc_frame(SPoint::Empty_get(), Size_get());
		if(flags(d_style_flags & EStyleFlags::Border)) { _g->RectFrame3D(_rc_frame, true, false, EColor::Black, EColor::White); _rc_frame -= SEdge(2); }
		if(d_str_caption) {
			uint32_t _caption_x = _rc_frame.X_get(), _caption_y = _rc_frame.Y_get();
			_g->RectGradient(SRect(_caption_x, _caption_y, _rc_frame.Width_get(), FRAME_CAPTION_HEIGHT), EOrientation::Horizontal, FRAME_CAPTION_COLOR1, FRAME_CAPTION_COLOR2);
			_g->StringDrawEx(SPoint(_caption_x + FRAME_CAPTION_FONT_MARGIN, _caption_y + ((FRAME_CAPTION_HEIGHT - FRAME_CAPTION_FONT_HEIGHT) >> 1)), EColor::White, EColor::Black, true, false, FRAME_CAPTION_FONT, true, EAlignment::Start, nullptr, 0, 0, d_str_caption);
			_rc_frame -= SEdge(0, FRAME_CAPTION_HEIGHT, 0, 0);
		}
		event.MarkFlags(CFrameEvent::EEventResultFlags::Handled);
	}
}
/*override*/ SSize CWindow::CalcFitSize() {
	uint8_t _separation_width = SeparationWidth_get(); bool _is_border = flags(d_style_flags & EStyleFlags::Border);
	uint32_t _def_width = (_is_border ? 4 : 0) + (_separation_width << 1), _def_height = (_is_border ? 4 : 0) + (_separation_width << 1);
	if(d_str_caption) { _def_height += FRAME_CAPTION_HEIGHT; }
	return d_frame_child ? d_frame_child->SizeFit_get() + SSize(_def_width, _def_height) : SSize(_def_width, _def_height);
}
/*override*/ void CWindow::CalcChildRects(void(CFrame::*f_set_rect)(SRect const &rc)) {
	if(d_frame_child) {
		SSize const &_sz_actual = Size_get(); uint8_t _separation_width = SeparationWidth_get(); bool _is_border = flags(d_style_flags & EStyleFlags::Border);
		uint32_t _off_x = (_is_border ? 2 : 0) + _separation_width, _off_y = (_is_border ? 2 : 0) + _separation_width, _def_width = (_is_border ? 4 : 0) + (_separation_width << 1), _def_height = (_is_border ? 4 : 0) + (_separation_width << 1);
		if(d_str_caption) { _off_y += FRAME_CAPTION_HEIGHT; _def_height += FRAME_CAPTION_HEIGHT; }
		(d_frame_child->*f_set_rect)(SRect(d_pt_location.X_get() + _off_x, d_pt_location.Y_get() + _off_y, _sz_actual.Width_get() - _def_width, _sz_actual.Height_get() - _def_height));
	}
}

void CWindow::_AttachWindowAsFirst() {
	d_window_prev = nullptr;
	d_window_next = s_window_first; s_window_first = this;
	if(d_window_next) d_window_next->d_window_prev = this;
	if(!s_window_last) s_window_last = this;
}
void CWindow::_DeattachWindow() {
	if (d_window_prev) d_window_prev->d_window_next = d_window_next; s_window_first = d_window_next;
	if (d_window_next) d_window_next->d_window_prev = d_window_prev; s_window_last = d_window_prev;
}
void CWindow::_EnsureDiscardedSizesAndLocations() {
	if(d_is_discard_sizes_and_locations) _DiscardSizesAndLocations();
	d_is_discard_sizes_and_locations = false;
}

CFrame *CWindow::FrameFirst_get() const { return d_frame_child; }
CFrame *CWindow::FrameLast_get() const {
	CFrame *_frame = d_frame_child;
	if(_frame) {
		while(true) {
			CContainer *_container = as_type(_frame, CContainer);
			CFrame *_frame_tmp = _container ? _container->ChildFrameLast_get() : nullptr;
			if(!_frame_tmp) break;
			_frame = _frame_tmp;
		}
	}
	return _frame;
}
CFrame *CWindow::GetFrameFirstStyled(CFrame::EStyleFlags style_flags) const { CFrame *_frame = FrameFirst_get(); if(_frame && !flags(_frame->d_style_flags & style_flags)) { _frame = _frame->GetFrameHierarchyNextStyled(false, style_flags); } return _frame; }
CFrame *CWindow::GetFrameLastStyled(CFrame::EStyleFlags style_flags) const { CFrame *_frame = FrameLast_get(); if(_frame && !flags(_frame->d_style_flags & style_flags)) { _frame = _frame->GetFrameHierarchyPrevStyled(false, style_flags); } return _frame; }

void CWindow::BringToTop() { _DeattachWindow(); _AttachWindowAsFirst(); this->Repaint(); }
/// *********************************************************************
