/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CFont.hpp
***********************************************************************/

#pragma once

#include "Types.h"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		class CFont {
			friend class CGraphics;

		private:
			static CFont const s_font_small;
			static CFont const s_font_large;

			uint8_t const *const d_fntbmp_regular;
			uint8_t const *const d_fntbmp_bold;
			uint32_t const d_height;
			uint32_t const d_chars;

		protected:
			CFont(uint8_t const *fntbmp_regular, uint8_t const *fntbmp_bold, uint32_t height, uint32_t chars);
			CFont(CFont const &other) = delete;
			CFont &operator=(CFont const &other) = delete;
			virtual ~CFont();

		public:
			static CFont const &FontSmall_get();
			static CFont const &FontLarge_get();

			uint8_t const *FntBmpRegular_get() const;
			uint8_t const *FntBmpBold_get() const;
			uint32_t Height_get() const;
			uint32_t Chars_get() const;
		};
		/// *********************************************************************

		// *********************************************************************
		inline /*static*/ CFont const &CFont::FontSmall_get() { return s_font_small; }
		inline /*static*/ CFont const &CFont::FontLarge_get() { return s_font_large; }

		inline uint8_t const *CFont::FntBmpRegular_get() const { return d_fntbmp_regular; }
		inline uint8_t const *CFont::FntBmpBold_get() const { return d_fntbmp_bold; }
		inline uint32_t CFont::Height_get() const { return d_height; }
		inline uint32_t CFont::Chars_get() const { return d_chars; }
		// *********************************************************************
	}
}
