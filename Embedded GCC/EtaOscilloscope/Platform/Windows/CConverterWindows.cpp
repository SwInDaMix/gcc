/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CConverterWindows.cpp
 ***********************************************************************/

#include "CConverterWindows.hpp"

using namespace System;
using namespace System::Frames;

/// *********************************************************************
/*static*/ void CConverter::Init() {  }

/*static*/ bool CConverter::KBD_KeyConvertDialog(EKey key, EKeyModifierFlags key_modifier, CFrame::EKeyDialog *key_dialog) {
	bool _res = false;
	if(!flags(key_modifier)) {
		if(key == EKey::Return) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_OK_Yes_Next; }
		else if(key == EKey::Escape) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Cancel_No_Back; }
		else if(key == EKey::ArrowLeft) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Left; }
		else if(key == EKey::ArrowRight) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Right; }
		else if(key == EKey::ArrowUp) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Up; }
		else if(key == EKey::ArrowDown) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Down; }
		else if(key == EKey::ContextMenuKey) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Menu_Options; }
		else if(key == EKey::Tab) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Frame_Next; }
	}
	if(key_modifier == EKeyModifierFlags::Shift) {
		if(key == EKey::Tab) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Frame_Prev; }
	}
	return _res;
}
/// *********************************************************************
