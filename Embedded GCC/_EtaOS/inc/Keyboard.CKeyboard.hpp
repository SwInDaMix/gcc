/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Keyboard.CKeyboard.hpp
***********************************************************************/

#pragma once

#define KEYBOARD_KEY_PRESSED_BEEP_CANCELED 5
#define KEYBOARD_KEY_PRESSED_BEEP_HANDLED 1

#include "Types.h"
#include "CPeriph.hpp"
#include "Frames.CFrameEvent.hpp"

namespace System {
	namespace Keyboard {
		/// *********************************************************************
		class CKeyboard {
		public:
			/// **********************************
			enum class EKeyApp : uint16_t {
				None = 0,

				Dialog_Cancel_No_Back,
				Dialog_OK_Yes_Next,
				Dialog_Menu_Options,
				Dialog_Frame_Prev,
				Dialog_Frame_Next,

				Oscilloscope_RunPause,							// Starts or holds signal capturing
				Oscilloscope_ChanelSwitch,						// Channel A-D
				Oscilloscope_VoltagePerGridIncrement,           // Sensitivity from 50mV to 10V
				Oscilloscope_VoltagePerGridDecrement,           // 50mV, 100mV, 200mV, 500mV, 1V, 2V, 5V, 10V
				Oscilloscope_AC_DC_Off_Togle,                   // Toggle between AC and DC coupling of channel A or B
				Oscilloscope_SecondsPerGridIncrement,           // Sampling rate from  to 1s
				Oscilloscope_SecondsPerGridDecrement,           // 100ns, 200ns, 500ns, 1us, 2us, 5us, 10us, 20us, 50us, 100us, 200us, 500us, 1ms, 2ms, 5ms, 10ms, 20ms, 50ms, 100ms, 200ms, 500ms, 1s
				Oscilloscope_SynchronizationModePrevious,       // Auto, Norm, Single, None, Scan
				Oscilloscope_SynchronizationModeNext,           //
				Oscilloscope_TrigerModePrevious,                // `|_, _|`, <Vt, >Vt, <TL, >TL, <TH, >TH
				Oscilloscope_TrigerModeNext,                    //
				Oscilloscope_TrigerThresholdIncrement,          // Threshold of triggering mode
				Oscilloscope_TrigerThresholdDecrement,          //
				Oscilloscope_TrigerThreshold50Precent,         	// Set threshold to 50% of Vpp
				Oscilloscope_ChanelPositionIncrement,           // Y position of a channel on the grid
				Oscilloscope_ChanelPositionDecrement,           //
				Oscilloscope_ScreenXPositionIncrement,          // X position of a screen over the buffer
				Oscilloscope_ScreenXPositionDecrement,          //

				Generator_WaveTypePrevious,                		// Wave type shape
				Generator_WaveTypeNext,                         // Sine, Triangle, Saw, Square
				Generator_FrequencyIncrement,                   // Frequency of generation wave
				Generator_FrequencyDecrement,                   // 10Hz, 20Hz, 50Hz, 100Hz, 200Hz, 500Hz, 1KHz, 2KHz, 5KHz, 10KHz, 20KHz - for all wave types, 50KHz, 100KHz, 200KHz, 500KHz, 1MHz, 2MHz, 4MHz, 6MHz, 8MHz only for square wave
				Generator_SquareDutyCycleIncrement,             // Square wave duty cycle
				Generator_SquareDutyCycleDecrement,             // from 10% to 90% with 10% step
			};
			/// **********************************

		private:
			CKeyboard() = delete;
			CKeyboard(CKeyboard const &other) = delete;
			CKeyboard &operator=(CKeyboard const &other) = delete;
			~CKeyboard() = delete;

		public:
			static void FireKeyDown(EKeyScan key_scan);
			static void FireKeyUp(EKeyScan key_scan);
			static void FireKeyCanceled();
			static void FireKeyPressed(EKey key, EKeyModifierFlags key_modifier);
		};
		/// **********************************
		ENUM_CLASS_OPERATORS(CKeyboard::EKeyApp, uint16_t)
		/// *********************************************************************
	}
}
