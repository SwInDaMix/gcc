/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriphNative.hpp
 ***********************************************************************/

#pragma once

#include "config.h"
#include "CPeriphNative.h"
#include "CPeriph.hpp"
#include "stm32f10x.h"
#include "stm32f10x_dma.h"

namespace System {
	/// *********************************************************************
	enum class EKeyScan : uint16_t {
		RunPause = 1,
		Stop,
		Record,

		Navigator1_Left,
		Navigator1_Right,
		Navigator1_Push,
		Navigator2_Left,
		Navigator2_Right,
		Navigator2_Push,

		Shift,
	};
	/// *********************************************************************
	enum class EKey : uint16_t {
		RunPause = 1,
		Stop,
		Record,

		Navigator1_Left,
		Navigator1_Right,
		Navigator1_Push,
		Navigator2_Left,
		Navigator2_Right,
		Navigator2_Push,
	};
	/// *********************************************************************
	enum class EKeyModifierFlags : uint16_t {
		Shift = 0x1,
	};
	/// *********************************************************************
	class CPeriphNative : CPeriph {
	public:
		/// **********************************
		enum class eLCD_ILI9488Cmd : uint8_t {
			SetColumnAddress = 0x2A,
			SetPageAddress = 0x2B,
			WriteMemoryStart = 0x2C,
			ReadMemoryStart = 0x2E,
			MemoryAccessControl = 0x36,
			DeviceCodeRead = 0xEF,
		};
		/// **********************************

	public:
		CPeriphNative() = delete;
		CPeriphNative(CPeriphNative const &other) = delete;
		CPeriphNative &operator=(CPeriphNative const &other) = delete;
		virtual ~CPeriphNative() = delete;
	};
	/// *********************************************************************
}
