/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : entry.cpp
 ***********************************************************************/

#include "stdperiph.h"
#include "entry.hpp"
#include "eta_dbg.h"
#include "eta_frames.h"

#include "BLDC.h"

extern "C" {
	void EtaElectroBikeMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void (*yield)());
}

void EtaElectroBikeMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void (*yield)()) {
	DBG_PutString(	"\nEta ElectroBike Firmware" NL
					" - Platform v " VER_PLATFORM "" NL
					" - Application v " VER_APP "" NL);

	while(true) {
		if(Frames_IsFrameAvailable()) {
			sFrameCmd _frame_cmd = (sFrameCmd)Frames_GetFrameCommand();
			uint8_t _frame_size = Frames_GetFrameSize();
			uint8_t _frame_data[_frame_size];
			Frames_GetFrameData(_frame_data, 0, _frame_size);
			Frames_RemoveFrame();

			if(_frame_cmd == FRAME_CMD_TEST_COMMAND && _frame_size > 0) {
				uint8_t _cmd = _frame_data[0];
				if(_cmd == 0) { TIM_GenerateEvent(TIM_3PHASE, TIM_EventSource_COM); DBG_PutString("TIM_GenerateEvent" NL); }
				else if(_cmd == 1) { BLDC_ProcessStep(0); DBG_PutString("BLDC_ProcessStep" NL); }
			}
			else if(_frame_cmd == FRAME_CMD_NEW_PARAMS && _frame_size == sizeof(sNewParams)) {
				sNewParams *_new_params = (sNewParams*)_frame_data;
				if(BLDC_HallPrescaler == _new_params->HallPrescaler) BLDC_UpdateHallPeriod(_new_params->HallPeriod);
				if(BLDC_PWMPeriod != _new_params->PWMPower) BLDC_RebuildSinusTable(_new_params->PWMPower);
			}
		}
		else if(Frames_IsTransmitterFree()) {
			sStatus _status;
			_status.HallPosition = BLDC_HallPositionLast;
			_status.HallPrescaler = BLDC_HallPrescaler;
			_status.HallPeriod = BLDC_HallPeriod;
			_status.PWMPower = BLDC_PWMPeriod;
			Frames_PutFrame(FRAME_ADDRESS, (uint8_t)FRAME_CMD_STATUS, &_status, sizeof(sStatus));
		}
		else yield();
	}
}
