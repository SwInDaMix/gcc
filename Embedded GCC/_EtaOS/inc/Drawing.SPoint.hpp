/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CPoint.hpp
***********************************************************************/

#pragma once

#include "Types.h"
#include "Drawing.SSize.hpp"
#include "Drawing.SSizeSigned.hpp"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		struct SPoint {
			friend struct SSize;
			friend struct SRect;
			friend struct SEdge;

		private:
			uint32_t d_x, d_y;

		public:
			SPoint();
			SPoint(uint32_t x, uint32_t y);

			uint32_t X_get() const;
			void X_set(uint32_t value);
			uint32_t Y_get() const;
			void Y_set(uint32_t value);
			static SPoint Empty_get();

			void Invalidate();

			SPoint operator +(SSize const &size) const;
			SPoint const &operator +=(SSize const &size);
			SPoint operator -(SSize const &size) const;
			SPoint const &operator -=(SSize const &size);
			SPoint operator +(SSizeSigned const &size) const;
			SPoint const &operator +=(SSizeSigned const &size);
			SPoint operator -(SSizeSigned const &size) const;
			SPoint const &operator -=(SSizeSigned const &size);
			SSizeSigned operator -(SPoint const &point) const;

			bool operator ==(SPoint const &other) const;
			bool operator !=(SPoint const &other) const;
		};
		/// *********************************************************************

		// *********************************************************************
		inline uint32_t SPoint::X_get() const { return d_x; }
		inline void SPoint::X_set(uint32_t value) { d_x = value; }
		inline uint32_t SPoint::Y_get() const { return d_y; }
		inline void SPoint::Y_set(uint32_t value) { d_y = value; }
		inline /*static*/ SPoint SPoint::Empty_get() { return SPoint(); }

		inline void SPoint::Invalidate() { d_x = d_y = 0; }
		// *********************************************************************
	}
}
