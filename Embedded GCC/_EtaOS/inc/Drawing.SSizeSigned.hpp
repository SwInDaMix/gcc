/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Drawing.CSizeSigned.hpp
***********************************************************************/

#pragma once

#include "Types.h"

namespace System {
	namespace Drawing {
		/// *********************************************************************
		struct SSize;
		struct SSizeSigned {
			friend struct SPoint;
			friend struct SSize;
			friend struct SRect;
			friend struct SEdge;

		private:
			int32_t d_width, d_height;

		public:
			SSizeSigned();
			SSizeSigned(int32_t width, int32_t height);
			SSizeSigned(SSize const &size);

			int32_t Width_get() const;
			void Width_set(int32_t value);
			int32_t Height_get() const;
			void Height_set(int32_t value);
			static SSizeSigned Empty_get();

			void Invalidate();

			SSizeSigned operator +(SSizeSigned const &size) const;
			SSizeSigned const &operator +=(SSizeSigned const &size);
			SSizeSigned operator -(SSizeSigned const &size) const;
			SSizeSigned const &operator -=(SSizeSigned const &size);
			SSizeSigned operator +(SSize const &size) const;
			SSizeSigned const &operator +=(SSize const &size);
			SSizeSigned operator -(SSize const &size) const;
			SSizeSigned const &operator -=(SSize const &size);

			bool operator ==(SSizeSigned const &other) const;
			bool operator !=(SSizeSigned const &other) const;
		};
		/// *********************************************************************

		// *********************************************************************
		inline int32_t SSizeSigned::Width_get() const { return d_width; }
		inline void SSizeSigned::Width_set(int32_t value) { d_width = value; }
		inline int32_t SSizeSigned::Height_get() const { return d_height; }
		inline void SSizeSigned::Height_set(int32_t value) { d_height = value; }
		inline /*static*/ SSizeSigned SSizeSigned::Empty_get() { return SSizeSigned(); }

		inline void SSizeSigned::Invalidate() { d_width = d_height = 0; }
		// *********************************************************************
	}
}
