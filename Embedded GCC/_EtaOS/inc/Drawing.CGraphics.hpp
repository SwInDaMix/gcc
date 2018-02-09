/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CGraphics.hpp
***********************************************************************/

#pragma once

#include "Types.h"
#include "CPeriph.hpp"
#include "Common.hpp"
#include "Drawing.CFont.hpp"
#include "Drawing.CImage.hpp"
#include "Drawing.EColor.hpp"
#include "Drawing.SPoint.hpp"
#include "Drawing.SRect.hpp"
#include "Drawing.SSize.hpp"
#include "Drawing.SSizeSigned.hpp"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		class CGraphics {
		public:

		private:
			uint32_t const d_active_graphics_index;
			SRect const d_rc_screen;

		public:
			CGraphics(SRect const &rc_screen);
			CGraphics(CGraphics const &other) = delete;
			CGraphics &operator=(CGraphics const &other) = delete;
			virtual ~CGraphics();

			void Clear(EColor clr);

			bool RectDraw(SRect const &rc, bool is_round, EColor clr);
			bool RectFill(SRect const &rc, EColor clr);
			bool RectDrawFill(SRect const &rc, bool is_round, EColor clr_draw, EColor clr_fill);
			bool RectFocus(SRect const &rc, EColor clr);
			bool RectFrame3D(SRect const &rc, bool is_bold, bool is_inside, EColor clr1, EColor clr2);
			bool RectFrameStatic(SRect const &rc, bool is_inside, EColor clr1, EColor clr2);
			bool RectGradient(SRect const &rc, EOrientation orientation, EColor clr_from, EColor clr_to);
			bool ElipseDraw(SRect const &rc, EColor clr);
			bool ElipseFill(SRect const &rc, EColor clr);
			bool ElipseDrawFill(SRect const &rc, EColor clr_draw, EColor clr_fill);
			bool LineDraw(SPoint const &pt_start, SSizeSigned const &szs, EColor clr);
			bool LineStatic(SPoint const &pt_start, EOrientation orientation, uint32_t length, bool is_inside, EColor clr1, EColor clr2);
			bool ImageDraw(SPoint const &pt_origin, CImage const &image);
			bool ImageDraw(SPoint const &pt_origin, CImage const &image, EColor clr_transparent);
			bool ImageDraw1(SPoint const &pt_origin, CImage1 const &image1, EColor clr_fore, EColor clr_back);
			bool ImageDraw1(SPoint const &pt_origin, CImage1 const &image1, EColor clr_fore);

			static SSize StringMeasure(CFont const &font, bool is_bold, uint32_t *lines_len, uint32_t *lines_len_count, char const *str);
			static SSize StringMeasureFmt(CFont const &font, bool is_bold, uint32_t *lines_len, uint32_t *lines_len_count, char const *fmt, ...);
			static SSize StringMeasureFmt(CFont const &font, bool is_bold, uint32_t *lines_len, uint32_t *lines_len_count, char const *fmt, va_list ap);
			SSize StringDrawEx(SPoint const &pt_location, EColor clr_fore, EColor clr_back, bool is_transparent_back, bool is_shadow, CFont const &font, bool is_bold, EAlignment alignment, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max, char const *str);
			SSize StringDrawExFmt(SPoint const &pt_location, EColor clr_fore, EColor clr_back, bool is_transparent_back, bool is_shadow, CFont const &font, bool is_bold, EAlignment alignment, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max, char const *fmt, ...);
			SSize StringDrawExFmt(SPoint const &pt_location, EColor clr_fore, EColor clr_back, bool is_transparent_back, bool is_shadow, CFont const &font, bool is_bold, EAlignment alignment, uint32_t *lines_len, uint32_t lines_len_count, uint32_t lines_len_max, char const *fmt, va_list ap);

		private:
			void _EnsureIndex();
		};
		/// *********************************************************************

		// *********************************************************************
		// *********************************************************************
	}
}
