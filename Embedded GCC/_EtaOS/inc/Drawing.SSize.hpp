/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CSize.hpp
***********************************************************************/

#pragma once

#include "Types.h"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		struct SSizeSigned;
		struct SSize {
			friend struct SPoint;
			friend struct SSizeSigned;
			friend struct SRect;
			friend struct SEdge;

		private:
			uint32_t d_width, d_height;

		public:
			SSize();
			SSize(uint32_t width, uint32_t height);
			SSize(SSizeSigned const &size);

			uint32_t Width_get() const;
			void Width_set(uint32_t value);
			uint32_t Height_get() const;
			void Height_set(uint32_t value);
			static SSize Empty_get();

			void Invalidate();

			SSize operator +(SSize const &size) const;
			SSize const &operator +=(SSize const &size);
			SSize operator -(SSize const &size) const;
			SSize const &operator -=(SSize const &size);
			SSize operator |(SSize const &size) const;
			SSize const &operator |=(SSize const &size);
			SSize operator &(SSize const &size) const;
			SSize const &operator &=(SSize const &size);
			SSize operator +(SSizeSigned const &size) const;
			SSize const &operator +=(SSizeSigned const &size);
			SSize operator -(SSizeSigned const &size) const;
			SSize const &operator -=(SSizeSigned const &size);

			bool operator ==(SSize const &other) const;
			bool operator !=(SSize const &other) const;
		};
		/// *********************************************************************

		// *********************************************************************
		inline uint32_t SSize::Width_get() const { return d_width; }
		inline void SSize::Width_set(uint32_t value) { d_width = value; }
		inline uint32_t SSize::Height_get() const { return d_height; }
		inline void SSize::Height_set(uint32_t value) { d_height = value; }
		inline /*static*/ SSize SSize::Empty_get() { return SSize(); }

		inline void SSize::Invalidate() { d_width = d_height = 0; }
		// *********************************************************************
	}
}
