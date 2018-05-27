/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.EColor.hpp
***********************************************************************/

#pragma once

#define LCD_CLRPART_R(clr) (((int32_t)clr) & 0x1F)
#define LCD_CLRPART_G(clr) (((int32_t)clr) & 0x7E0)
#define LCD_CLRPART_B(clr) (((int32_t)clr) & 0xF800)

#define LCD_COLOR16_FROM_R_G_B(r, g, b) (((r & 0xF8) >> 3) | ((g & 0xFC) << 3) | ((b & 0xF8) << 8))
#define LCD_COLOR16_FROM_COLOR24(clr) (((clr & 0xF8) >> 3) | ((clr & 0xFC00) >> 5) | ((clr & 0xF80000) >> 8))
#define LCD_COLOR16_FROM_COLOR8_correct(clr) (clr | ((clr & 0x718) >> 3) | ((clr & 0xC000) >> 2) | ((clr & 0x8000) >> 4))
#define LCD_COLOR16_FROM_COLOR8(clr) (LCD_COLOR16_FROM_COLOR8_correct((((clr & 0x07) << 2) | ((clr & 0x38) << 5) | ((clr & 0xC0) << 8))))

#include "Types.h"

typedef uint16_t lcd_color16;
typedef uint8_t lcd_color8;

namespace System {
	namespace Drawing {
		/// *********************************************************************
		enum class EColor : lcd_color16 {
			_Invalid			 = 0,
			AliceBlue			 = LCD_COLOR16_FROM_R_G_B(0xF0, 0xF8, 0xFF),
			AntiqueWhite		 = LCD_COLOR16_FROM_R_G_B(0xFA, 0xEB, 0xD7),
			Aqua				 = LCD_COLOR16_FROM_R_G_B(0x00, 0xFF, 0xFF),
			Aquamarine			 = LCD_COLOR16_FROM_R_G_B(0x7F, 0xFF, 0xD4),
			Azure				 = LCD_COLOR16_FROM_R_G_B(0xF0, 0xFF, 0xFF),
			Beige				 = LCD_COLOR16_FROM_R_G_B(0xF5, 0xF5, 0xDC),
			Bisque				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xE4, 0xC4),
			Black				 = LCD_COLOR16_FROM_R_G_B(0x00, 0x00, 0x00),
			BlanchedAlmond		 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xEB, 0xCD),
			Blue				 = LCD_COLOR16_FROM_R_G_B(0x00, 0x00, 0xFF),
			BlueViolet			 = LCD_COLOR16_FROM_R_G_B(0x8A, 0x2B, 0xE2),
			Brown				 = LCD_COLOR16_FROM_R_G_B(0xA5, 0x2A, 0x2A),
			BurlyWood			 = LCD_COLOR16_FROM_R_G_B(0xDE, 0xB8, 0x87),
			CadetBlue			 = LCD_COLOR16_FROM_R_G_B(0x5F, 0x9E, 0xA0),
			Chartreuse			 = LCD_COLOR16_FROM_R_G_B(0x7F, 0xFF, 0x00),
			Chocolate			 = LCD_COLOR16_FROM_R_G_B(0xD2, 0x69, 0x1E),
			Coral				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x7F, 0x50),
			CornflowerBlue		 = LCD_COLOR16_FROM_R_G_B(0x64, 0x95, 0xED),
			Cornsilk			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xF8, 0xDC),
			Crimson				 = LCD_COLOR16_FROM_R_G_B(0xDC, 0x14, 0x3C),
			Cyan				 = LCD_COLOR16_FROM_R_G_B(0x00, 0xFF, 0xFF),
			DarkBlue			 = LCD_COLOR16_FROM_R_G_B(0x00, 0x00, 0x8B),
			DarkCyan			 = LCD_COLOR16_FROM_R_G_B(0x00, 0x8B, 0x8B),
			DarkGoldenrod		 = LCD_COLOR16_FROM_R_G_B(0xB8, 0x86, 0x0B),
			DarkGray			 = LCD_COLOR16_FROM_R_G_B(0x60, 0x60, 0x60),
			DarkGreen			 = LCD_COLOR16_FROM_R_G_B(0x00, 0x64, 0x00),
			DarkKhaki			 = LCD_COLOR16_FROM_R_G_B(0xBD, 0xB7, 0x6B),
			DarkMagenta			 = LCD_COLOR16_FROM_R_G_B(0x8B, 0x00, 0x8B),
			DarkOliveGreen		 = LCD_COLOR16_FROM_R_G_B(0x55, 0x6B, 0x2F),
			DarkOrange			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x8C, 0x00),
			DarkOrchid			 = LCD_COLOR16_FROM_R_G_B(0x99, 0x32, 0xCC),
			DarkRed				 = LCD_COLOR16_FROM_R_G_B(0x8B, 0x00, 0x00),
			DarkSalmon			 = LCD_COLOR16_FROM_R_G_B(0xE9, 0x96, 0x7A),
			DarkSeaGreen		 = LCD_COLOR16_FROM_R_G_B(0x8F, 0xBC, 0x8B),
			DarkSlateBlue		 = LCD_COLOR16_FROM_R_G_B(0x48, 0x3D, 0x8B),
			DarkSlateGray		 = LCD_COLOR16_FROM_R_G_B(0x2F, 0x4F, 0x4F),
			DarkTurquoise		 = LCD_COLOR16_FROM_R_G_B(0x00, 0xCE, 0xD1),
			DarkViolet			 = LCD_COLOR16_FROM_R_G_B(0x94, 0x00, 0xD3),
			DeepPink			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x14, 0x93),
			DeepSkyBlue			 = LCD_COLOR16_FROM_R_G_B(0x00, 0xBF, 0xFF),
			DimGray				 = LCD_COLOR16_FROM_R_G_B(0x69, 0x69, 0x69),
			DodgerBlue			 = LCD_COLOR16_FROM_R_G_B(0x1E, 0x90, 0xFF),
			Firebrick			 = LCD_COLOR16_FROM_R_G_B(0xB2, 0x22, 0x22),
			FloralWhite			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFA, 0xF0),
			ForestGreen			 = LCD_COLOR16_FROM_R_G_B(0x22, 0x8B, 0x22),
			Fuchsia				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x00, 0xFF),
			Gainsboro			 = LCD_COLOR16_FROM_R_G_B(0xDC, 0xDC, 0xDC),
			GhostWhite			 = LCD_COLOR16_FROM_R_G_B(0xF8, 0xF8, 0xFF),
			Gold				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xD7, 0x00),
			Goldenrod			 = LCD_COLOR16_FROM_R_G_B(0xDA, 0xA5, 0x20),
			Gray				 = LCD_COLOR16_FROM_R_G_B(0x80, 0x80, 0x80),
			Green				 = LCD_COLOR16_FROM_R_G_B(0x00, 0x80, 0x00),
			GreenYellow			 = LCD_COLOR16_FROM_R_G_B(0xAD, 0xFF, 0x2F),
			Honeydew			 = LCD_COLOR16_FROM_R_G_B(0xF0, 0xFF, 0xF0),
			HotPink				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x69, 0xB4),
			IndianRed			 = LCD_COLOR16_FROM_R_G_B(0xCD, 0x5C, 0x5C),
			Indigo				 = LCD_COLOR16_FROM_R_G_B(0x4B, 0x00, 0x82),
			Ivory				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFF, 0xF0),
			Khaki				 = LCD_COLOR16_FROM_R_G_B(0xF0, 0xE6, 0x8C),
			Lavender			 = LCD_COLOR16_FROM_R_G_B(0xE6, 0xE6, 0xFA),
			LavenderBlush		 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xF0, 0xF5),
			LawnGreen			 = LCD_COLOR16_FROM_R_G_B(0x7C, 0xFC, 0x00),
			LemonChiffon		 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFA, 0xCD),
			LightBlue			 = LCD_COLOR16_FROM_R_G_B(0xAD, 0xD8, 0xE6),
			LightCoral			 = LCD_COLOR16_FROM_R_G_B(0xF0, 0x80, 0x80),
			LightCyan			 = LCD_COLOR16_FROM_R_G_B(0xE0, 0xFF, 0xFF),
			LightGoldenrodYellow = LCD_COLOR16_FROM_R_G_B(0xFA, 0xFA, 0xD2),
			LightGreen			 = LCD_COLOR16_FROM_R_G_B(0x90, 0xEE, 0x90),
			LightGray			 = LCD_COLOR16_FROM_R_G_B(0xBF, 0xBF, 0xBF),
			LightPink			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xB6, 0xC1),
			LightSalmon			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xA0, 0x7A),
			LightSeaGreen		 = LCD_COLOR16_FROM_R_G_B(0x20, 0xB2, 0xAA),
			LightSkyBlue		 = LCD_COLOR16_FROM_R_G_B(0x87, 0xCE, 0xFA),
			LightSlateGray		 = LCD_COLOR16_FROM_R_G_B(0x77, 0x88, 0x99),
			LightSteelBlue		 = LCD_COLOR16_FROM_R_G_B(0xB0, 0xC4, 0xDE),
			LightYellow			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFF, 0xE0),
			Lime				 = LCD_COLOR16_FROM_R_G_B(0x00, 0xFF, 0x00),
			LimeGreen			 = LCD_COLOR16_FROM_R_G_B(0x32, 0xCD, 0x32),
			Linen				 = LCD_COLOR16_FROM_R_G_B(0xFA, 0xF0, 0xE6),
			Magenta				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x00, 0xFF),
			Maroon				 = LCD_COLOR16_FROM_R_G_B(0x80, 0x00, 0x00),
			MediumAquamarine	 = LCD_COLOR16_FROM_R_G_B(0x66, 0xCD, 0xAA),
			MediumBlue			 = LCD_COLOR16_FROM_R_G_B(0x00, 0x00, 0xCD),
			MediumOrchid		 = LCD_COLOR16_FROM_R_G_B(0xBA, 0x55, 0xD3),
			MediumPurple		 = LCD_COLOR16_FROM_R_G_B(0x93, 0x70, 0xDB),
			MediumSeaGreen		 = LCD_COLOR16_FROM_R_G_B(0x3C, 0xB3, 0x71),
			MediumSlateBlue		 = LCD_COLOR16_FROM_R_G_B(0x7B, 0x68, 0xEE),
			MediumSpringGreen	 = LCD_COLOR16_FROM_R_G_B(0x00, 0xFA, 0x9A),
			MediumTurquoise		 = LCD_COLOR16_FROM_R_G_B(0x48, 0xD1, 0xCC),
			MediumVioletRed		 = LCD_COLOR16_FROM_R_G_B(0xC7, 0x15, 0x85),
			MidnightBlue		 = LCD_COLOR16_FROM_R_G_B(0x19, 0x19, 0x70),
			MintCream			 = LCD_COLOR16_FROM_R_G_B(0xF5, 0xFF, 0xFA),
			MistyRose			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xE4, 0xE1),
			Moccasin			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xE4, 0xB5),
			NavajoWhite			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xDE, 0xAD),
			Navy				 = LCD_COLOR16_FROM_R_G_B(0x00, 0x00, 0x80),
			OldLace				 = LCD_COLOR16_FROM_R_G_B(0xFD, 0xF5, 0xE6),
			Olive				 = LCD_COLOR16_FROM_R_G_B(0x80, 0x80, 0x00),
			OliveDrab			 = LCD_COLOR16_FROM_R_G_B(0x6B, 0x8E, 0x23),
			Orange				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xA5, 0x00),
			OrangeRed			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x45, 0x00),
			Orchid				 = LCD_COLOR16_FROM_R_G_B(0xDA, 0x70, 0xD6),
			PaleGoldenrod		 = LCD_COLOR16_FROM_R_G_B(0xEE, 0xE8, 0xAA),
			PaleGreen			 = LCD_COLOR16_FROM_R_G_B(0x98, 0xFB, 0x98),
			PaleTurquoise		 = LCD_COLOR16_FROM_R_G_B(0xAF, 0xEE, 0xEE),
			PaleVioletRed		 = LCD_COLOR16_FROM_R_G_B(0xDB, 0x70, 0x93),
			PapayaWhip			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xEF, 0xD5),
			PeachPuff			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xDA, 0xB9),
			Peru				 = LCD_COLOR16_FROM_R_G_B(0xCD, 0x85, 0x3F),
			Pink				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xC0, 0xCB),
			Plum				 = LCD_COLOR16_FROM_R_G_B(0xDD, 0xA0, 0xDD),
			PowderBlue			 = LCD_COLOR16_FROM_R_G_B(0xB0, 0xE0, 0xE6),
			Purple				 = LCD_COLOR16_FROM_R_G_B(0x80, 0x00, 0x80),
			Red					 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x00, 0x00),
			RosyBrown			 = LCD_COLOR16_FROM_R_G_B(0xBC, 0x8F, 0x8F),
			RoyalBlue			 = LCD_COLOR16_FROM_R_G_B(0x41, 0x69, 0xE1),
			SaddleBrown			 = LCD_COLOR16_FROM_R_G_B(0x8B, 0x45, 0x13),
			Salmon				 = LCD_COLOR16_FROM_R_G_B(0xFA, 0x80, 0x72),
			SandyBrown			 = LCD_COLOR16_FROM_R_G_B(0xF4, 0xA4, 0x60),
			SeaGreen			 = LCD_COLOR16_FROM_R_G_B(0x2E, 0x8B, 0x57),
			SeaShell			 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xF5, 0xEE),
			Sienna				 = LCD_COLOR16_FROM_R_G_B(0xA0, 0x52, 0x2D),
			Silver				 = LCD_COLOR16_FROM_R_G_B(0xC0, 0xC0, 0xC0),
			SkyBlue				 = LCD_COLOR16_FROM_R_G_B(0x87, 0xCE, 0xEB),
			SlateBlue			 = LCD_COLOR16_FROM_R_G_B(0x6A, 0x5A, 0xCD),
			SlateGray			 = LCD_COLOR16_FROM_R_G_B(0x70, 0x80, 0x90),
			Snow				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFA, 0xFA),
			SpringGreen			 = LCD_COLOR16_FROM_R_G_B(0x00, 0xFF, 0x7F),
			SteelBlue			 = LCD_COLOR16_FROM_R_G_B(0x46, 0x82, 0xB4),
			Tan					 = LCD_COLOR16_FROM_R_G_B(0xD2, 0xB4, 0x8C),
			Teal				 = LCD_COLOR16_FROM_R_G_B(0x00, 0x80, 0x80),
			Thistle				 = LCD_COLOR16_FROM_R_G_B(0xD8, 0xBF, 0xD8),
			Tomato				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0x63, 0x47),
			Turquoise			 = LCD_COLOR16_FROM_R_G_B(0x40, 0xE0, 0xD0),
			Violet				 = LCD_COLOR16_FROM_R_G_B(0xEE, 0x82, 0xEE),
			Wheat				 = LCD_COLOR16_FROM_R_G_B(0xF5, 0xDE, 0xB3),
			White				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFF, 0xFF),
			WhiteSmoke			 = LCD_COLOR16_FROM_R_G_B(0xF5, 0xF5, 0xF5),
			Yellow				 = LCD_COLOR16_FROM_R_G_B(0xFF, 0xFF, 0x00),
			YellowGreen			 = LCD_COLOR16_FROM_R_G_B(0x9A, 0xCD, 0x32),
		};
		/// *********************************************************************

		// *********************************************************************
		constexpr EColor ColorLow75(EColor clr) { return (EColor)((((uint16_t)clr >> 1) & 0xEBEF) + (((uint16_t)clr >> 2) & 0x39E7)); }
		constexpr EColor ColorLow50(EColor clr) { return (EColor)(((uint16_t)clr >> 1) & 0xEBEF); }
		constexpr EColor ColorLow25(EColor clr) { return (EColor)(((uint16_t)clr >> 2) & 0x39E7); }
		constexpr EColor ColorLow13(EColor clr) { return (EColor)(((uint16_t)clr >> 3) & 0x18E3); }
		constexpr EColor ColorDarken(EColor clr, uint16_t pers256) { return (EColor)(((LCD_CLRPART_R(clr) * pers256) >> 8) | (((LCD_CLRPART_G(clr) * pers256) >> 8) & 0x7E0) | (((LCD_CLRPART_B(clr) * pers256) >> 8) & 0xF800)); }
		constexpr EColor ColorTrans(EColor clr1, EColor clr2, uint16_t pos256) { return (EColor)((((LCD_CLRPART_R(clr1) * (256 - pos256)) >> 8) + ((LCD_CLRPART_R(clr2) * pos256) >> 8)) | ((((LCD_CLRPART_G(clr1) * (256 - pos256)) >> 8) + ((LCD_CLRPART_G(clr2) * pos256) >> 8)) & 0x7E0) | ((((LCD_CLRPART_B(clr1) * (256 - pos256)) >> 8) + ((LCD_CLRPART_B(clr2) * pos256) >> 8)) & 0xF800)); }
	}
		// *********************************************************************
}
