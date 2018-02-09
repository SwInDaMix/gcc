/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CEdge.hpp
***********************************************************************/

#pragma once

#include "Types.h"
#include "Drawing.SPoint.hpp"
#include "Drawing.SSize.hpp"
#include "Drawing.SSizeSigned.hpp"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		struct SRect;
		struct SEdge {
			friend struct SRect;

		private:
			uint32_t d_left, d_top, d_right, d_bottom;

		public:
			SEdge();
			SEdge(uint32_t edge);
			SEdge(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
			SEdge(SPoint const &location, SSize const &size);
			SEdge(SRect const &rc);

			uint32_t Left_get() const;
			void Left_set(uint32_t value);
			uint32_t Top_get() const;
			void Top_set(uint32_t value);
			uint32_t Right_get() const;
			void Right_set(uint32_t value);
			uint32_t Bottom_get() const;
			void Bottom_set(uint32_t value);

			void Invalidate();

			static SEdge Empty_get();

			bool operator ==(SEdge const &other) const;
			bool operator !=(SEdge const &other) const;

			operator SRect();
		};
		/// *********************************************************************

		// *********************************************************************
		inline uint32_t SEdge::Left_get() const { return d_left; }
		inline void SEdge::Left_set(uint32_t value) { d_left = min(value, d_right); }
		inline uint32_t SEdge::Top_get() const { return d_top; }
		inline void SEdge::Top_set(uint32_t value) { d_top = min(value, d_bottom); }
		inline uint32_t SEdge::Right_get() const { return d_right; }
		inline void SEdge::Right_set(uint32_t value) { d_right = max(value, d_left); }
		inline uint32_t SEdge::Bottom_get() const { return d_bottom; }
		inline void SEdge::Bottom_set(uint32_t value) { d_bottom = max(value, d_top); }

		inline void SEdge::Invalidate() { d_left = d_top = d_right = d_bottom = 0; }

		inline /*static*/ SEdge SEdge::Empty_get() { return SEdge(); }
		// *********************************************************************
	}
}
