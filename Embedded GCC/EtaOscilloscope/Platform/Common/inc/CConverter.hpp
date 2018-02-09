/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CConverter.hpp
***********************************************************************/

#pragma once

#include "config.h"
#include "CConverter.h"
#include "CPeriph.hpp"

#include "Frames.CFrame.hpp"

namespace System {
	/// *********************************************************************
	class CConverter {
	private:

	public:
		CConverter() = delete;
		virtual ~CConverter() = delete;
		CConverter(CConverter const &other) = delete;
		CConverter &operator=(CConverter const &other) = delete;

		static void Init();

		static bool KBD_KeyConvertDialog(EKey key, EKeyModifierFlags key_modifier, System::Frames::CFrame::EKeyDialog *key_dialog);
	};
	/// *********************************************************************
}
