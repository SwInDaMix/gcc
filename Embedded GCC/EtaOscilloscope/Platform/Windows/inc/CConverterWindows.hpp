/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CConverterWindows.hpp
 ***********************************************************************/

#pragma once

#include "config.h"
#include "CPeriphWindows.hpp"
#include "CConverter.hpp"

namespace System {
	/// *********************************************************************
	class CConverterWindows : CConverter {
	public:
		CConverterWindows() = delete;
		CConverterWindows(CConverterWindows const &other) = delete;
		CConverterWindows &operator=(CConverterWindows const &other) = delete;
		virtual ~CConverterWindows() = delete;
	};
	/// *********************************************************************
}
