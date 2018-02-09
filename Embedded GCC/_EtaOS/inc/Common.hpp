/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Common.hpp
***********************************************************************/

#pragma once

#include "Types.h"

namespace System {
	/// *********************************************************************
	enum class EAlignment : uint8_t {
		Start	= 0,
		Middle	= 1,
		End		= 2,
	};
	ENUM_CLASS_OPERATORS(EAlignment, uint8_t)
	/// *********************************************************************
	enum class EAlignmentEx : uint8_t {
		Start	= 0,
		Middle	= 1,
		End		= 2,
		Stretch	= 3,
	};
	ENUM_CLASS_OPERATORS(EAlignmentEx, uint8_t)
	/// *********************************************************************
	enum class EOrientation : uint8_t {
		Horizontal	= 0,
		Vertical	= 1,
	};
	/// *********************************************************************
	ENUM_CLASS_OPERATORS(EOrientation, uint8_t)
}
