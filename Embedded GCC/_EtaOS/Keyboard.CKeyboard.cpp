/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Keyboard.CKeyboard.cpp
***********************************************************************/

#include "Keyboard.CKeyboard.hpp"
#include "Frames.CFrame.hpp"
#include "CConverter.hpp"

using namespace System;
using namespace System::Frames;
using namespace System::Keyboard;

/// *********************************************************************
static void _FrameEventFinalizer_KeyPressedBeep(CFrameEvent &event) {
	CFrameEvent::EEventResultFlags _event_result_flags = event.EventResultFlags_get();
	if(flags(_event_result_flags & CFrameEvent::EEventResultFlags::Canceled)) CPeriph::SND_Beep(CPeriph::EBeepType::Error);
	else if(flags(_event_result_flags & CFrameEvent::EEventResultFlags::Handled)) CPeriph::SND_Beep(CPeriph::EBeepType::Notify);
}

void _FireEvent(CFrameEvent::EEvent event, uint32_t value1,  uint32_t value2, void (*event_finalizer)(CFrameEvent &event)) {
	CWindow *_window_top = CWindow::WindowFirst_get();
	if(_window_top) {
		CFrame *_frame_focus = _window_top->FrameFocus_get();
		if(_frame_focus) { CFrameEvent::EventSendAsync(nullptr, *_frame_focus, event, CFrameEvent::EEventType::Bubbling, value1, value2, nullptr, event_finalizer); }
	}
}

/*static*/ void CKeyboard::FireKeyDown(EKeyScan key_scan) { _FireEvent(CFrameEvent::EEvent::KeyDown, (uint32_t)key_scan, 0, nullptr); }
/*static*/ void CKeyboard::FireKeyUp(EKeyScan key_scan) { _FireEvent(CFrameEvent::EEvent::KeyUp, (uint32_t)key_scan, 0, nullptr); }
/*static*/ void CKeyboard::FireKeyCanceled() { _FireEvent(CFrameEvent::EEvent::KeyCanceled, 0, 0, nullptr); }
/*static*/ void CKeyboard::FireKeyPressed(EKey key, EKeyModifierFlags key_modifier) {
	_FireEvent(CFrameEvent::EEvent::KeyPressed, (uint32_t)key, (uint32_t)key_modifier, nullptr);
	CFrame::EKeyDialog _key_dialog;
	if(CConverter::KBD_KeyConvertDialog(key, key_modifier, &_key_dialog)) _FireEvent(CFrameEvent::EEvent::KeyDialog, (uint32_t)_key_dialog, 0, _FrameEventFinalizer_KeyPressedBeep);
}
/// *********************************************************************
