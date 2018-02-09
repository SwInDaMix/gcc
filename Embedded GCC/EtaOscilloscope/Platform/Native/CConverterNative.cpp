/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CConverterNative.cpp
 ***********************************************************************/

#include "CConverterNative.hpp"

using namespace System;
using namespace System::Frames;

/// *********************************************************************
/*static*/ void CConverter::Init() {  }

/*static*/ bool CConverter::KBD_KeyConvertDialog(EKey key, EKeyModifierFlags key_modifier, CFrame::EKeyDialog *key_dialog) {
	bool _res = false;
	if(!flags(key_modifier)) {
		if(key == EKey::RunPause) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_OK_Yes_Next; }
		else if(key == EKey::Stop) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Cancel_No_Back; }
		else if(key == EKey::Record) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Menu_Options; }
		else if(key == EKey::Navigator2_Left) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Left; }
		else if(key == EKey::Navigator2_Right) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Right; }
		else if(key == EKey::Navigator1_Left) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Frame_Prev; }
		else if(key == EKey::Navigator1_Right) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Frame_Next; }
	}
	else if(key_modifier == EKeyModifierFlags::Shift) {
		if(key == EKey::Navigator2_Right) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Up; }
		else if(key == EKey::Navigator2_Left) { _res = true; *key_dialog = CFrame::EKeyDialog::Dialog_Down; }
	}
	return _res;
}
/// *********************************************************************
