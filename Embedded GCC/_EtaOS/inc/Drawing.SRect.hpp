/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CRect.hpp
***********************************************************************/

#pragma once

#include "Types.h"
#include "Drawing.SSize.hpp"
#include "Drawing.SPoint.hpp"
#include "Drawing.SSizeSigned.hpp"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		struct SEdge;
		struct SRect {
			friend struct SEdge;

		private:
			SPoint d_location;
			SSize d_size;

		public:
			SRect();
			SRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			SRect(SPoint const &location, SSize const &size);

			uint32_t X_get() const;
			void X_set(uint32_t value);
			uint32_t Y_get() const;
			void Y_set(uint32_t value);
			uint32_t Width_get() const;
			void Width_set(uint32_t value);
			uint32_t Height_get() const;
			void Height_set(uint32_t value);
			uint32_t Right_get() const;
			bool Right_set(uint32_t value);
			uint32_t Bottom_get() const;
			bool Bottom_set(uint32_t value);
			SPoint const &Location_get() const;
			void Location_set(SPoint const &value);
			SSize const &Size_get() const;
			void Size_set(SSize const &value);
			uint32_t CenterX_get() const;
			uint32_t CenterY_get() const;
			SPoint Center_get() const;

			bool IsInside(uint32_t x, uint32_t y) const;
			bool IsInside(SPoint const &pt) const;
			bool IsInside(SRect const &rc) const;
			bool IsIntersectsWith(SRect const &rc) const;
			void Invalidate();

			static SRect Empty_get();

			SRect operator +(SSize const &size) const;
			SRect const &operator +=(SSize const &size);
			SRect operator -(SSize const &size) const;
			SRect const &operator -=(SSize const &size);
			SRect operator +(SSizeSigned const &size) const;
			SRect const &operator +=(SSizeSigned const &size);
			SRect operator -(SSizeSigned const &size) const;
			SRect const &operator -=(SSizeSigned const &size);
			SRect operator |(SRect const &rc) const;
			SRect const &operator |=(SRect const &rc);
			SRect operator &(SRect const &rc) const;
			SRect const &operator &=(SRect const &rc);

			SRect operator +(SEdge const &edg) const;
			SRect const &operator +=(SEdge const &edg);
			SRect operator -(SEdge const &edg) const;
			SRect const &operator -=(SEdge const &edg);

			bool operator ==(SRect const &other) const;
			bool operator !=(SRect const &other) const;

			operator SEdge();
		};
		/// *********************************************************************

		// *********************************************************************
		inline uint32_t SRect::X_get() const { return d_location.d_x; }
		inline void SRect::X_set(uint32_t value) { d_location.d_x = value; }
		inline uint32_t SRect::Y_get() const { return d_location.d_y; }
		inline void SRect::Y_set(uint32_t value) { d_location.d_y = value; }
		inline uint32_t SRect::Width_get() const { return d_size.d_width; }
		inline void SRect::Width_set(uint32_t value) { d_size.d_width = value; }
		inline uint32_t SRect::Height_get() const { return d_size.d_height; }
		inline void SRect::Height_set(uint32_t value) { d_size.d_height = value; }
		inline uint32_t SRect::Right_get() const { return d_location.d_x + d_size.d_width; }
		inline bool SRect::Right_set(uint32_t value) {
			if(value < d_location.d_x) return false;
			d_size.d_width = value - d_location.d_x; return true;
		}
		inline uint32_t SRect::Bottom_get() const { return d_location.d_y + d_size.d_height; }
		inline bool SRect::Bottom_set(uint32_t value) {
			if(value < d_location.d_y) return false;
			d_size.d_height = value - d_location.d_y; return true;
		}
		inline SPoint const &SRect::Location_get() const { return d_location; }
		inline void SRect::Location_set(SPoint const &value) { d_location = value; }
		inline SSize const &SRect::Size_get() const { return d_size; }
		inline void SRect::Size_set(SSize const &value) { d_size = value; }

		inline void SRect::Invalidate() { d_location.Invalidate(); d_size.Invalidate(); }

		inline /*static*/ SRect SRect::Empty_get() { return SRect(); }
		// *********************************************************************
	}
}
