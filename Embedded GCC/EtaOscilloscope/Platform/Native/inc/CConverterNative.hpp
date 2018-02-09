/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CConverterNative.hpp
 ***********************************************************************/

#pragma once

#include "config.h"
#include "CPeriphNative.hpp"
#include "CConverter.hpp"

namespace System {
	/// *********************************************************************
	class CConverterNative : CConverter {
	public:
		CConverterNative() = delete;
		CConverterNative(CConverterNative const &other) = delete;
		CConverterNative &operator=(CConverterNative const &other) = delete;
		virtual ~CConverterNative() = delete;
	};
	/// *********************************************************************
}
