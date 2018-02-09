/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriph.hpp
***********************************************************************/

#pragma once

#include "config.h"
#include "CPeriph.h"

#include "Drawing.SPoint.hpp"
#include "Drawing.SRect.hpp"
#include "Drawing.SSize.hpp"
#include "Drawing.EColor.hpp"

namespace System {
	enum class EKeyScan : uint16_t;
	enum class EKey : uint16_t;
	enum class EKeyModifierFlags : uint16_t;
	/// *********************************************************************
	class CPeriph {
	public:
		/// **********************************
		enum class EBeepType {
			Notify,
			Error
		};
		/// **********************************

	private:
		static System::Drawing::SRect s_rc_clip;

	public:
		CPeriph() = delete;
		virtual ~CPeriph() = delete;
		CPeriph(CPeriph const &other) = delete;
		CPeriph &operator=(CPeriph const &other) = delete;

		static void Init();

		static void TIM_20ms();

		static void KBD_20ms();
		static void KBD_Reset();
		static void KBD_KeyDown(EKeyScan key_scan);
		static void KBD_KeyUp(EKeyScan key_scan);
		static EKeyModifierFlags KBD_IsModifier(EKeyScan key_scan);

		static System::Drawing::SRect const &LCD_ClipRect_get();
		static void LCD_ClipRect_set(System::Drawing::SRect const &rc);
		static System::Drawing::SSize LCD_Resolution_get();
		static void LCD_Clear(System::Drawing::EColor clr);
		static void LCD_PixelSet(System::Drawing::SPoint const &pt, System::Drawing::EColor clr);
		static System::Drawing::EColor LCD_PixelGet(System::Drawing::SPoint const &pt);
		static void LCD_RectFill(System::Drawing::SRect const &rc, System::Drawing::EColor clr);
		static void LCD_RectRead(System::Drawing::SRect const &rc, System::Drawing::EColor *clr_buf);
		static void LCD_RectWrite(System::Drawing::SRect const &rc, System::Drawing::EColor *clr_buf);

		static void SND_Volume_set(uint8_t volume);
		static uint8_t SND_Volume_get();
		static void SND_Beep(EBeepType beep_type);
	};
	/// *********************************************************************
	ENUM_CLASS_OPERATORS(EKeyScan, uint16_t)
	ENUM_CLASS_OPERATORS(EKey, uint16_t)
	ENUM_CLASS_OPERATORS(EKeyModifierFlags, uint16_t)
	/// *********************************************************************
}
