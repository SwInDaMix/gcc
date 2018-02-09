/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : BLDC.c
 ***********************************************************************/

#include "BLDC.h"
#include "stdperiph.h"
#include "eta_dbg.h"

typedef struct {
	bool Ph1H;
	bool Ph1L;
	bool Ph2H;
	bool Ph2L;
	bool Ph3H;
	bool Ph3L;
} sBLDCBridgeState;

static uint16_t const BLDC_SinusoidalTable[BLDC_SINUS_TABLE_COUNT] = {
	0x0000, 0x0860, 0x10BE, 0x1917, 0x216A, 0x29B3, 0x31F1, 0x3A21, 0x4241, 0x4A4F, 0x5249, 0x5A2C, 0x61F7, 0x69A6, 0x7139, 0x78AC,
	0x7FFF, 0x872E, 0x8E39, 0x951C, 0x9BD7, 0xA266, 0xA8CA, 0xAEFF, 0xB504, 0xBAD7, 0xC077, 0xC5E3, 0xCB18, 0xD015, 0xD4DA, 0xD964,
	0xDDB2, 0xE1C4, 0xE598, 0xE92D, 0xEC82, 0xEF96, 0xF269, 0xF4F9, 0xF745, 0xF94F, 0xFB13, 0xFC93, 0xFDCE, 0xFEC3, 0xFF72, 0xFFDB,
	0xFFFF, 0xFFDB, 0xFF72, 0xFEC3, 0xFDCE, 0xFC93, 0xFB13, 0xF94F, 0xF745, 0xF4F9, 0xF269, 0xEF96, 0xEC82, 0xE92D, 0xE598, 0xE1C4,
};

static uint16_t BLDC_SinusoidalTable_Forward[BLDC_SINUS_TABLE_COUNT];
static uint16_t BLDC_SinusoidalTable_Reverse[BLDC_SINUS_TABLE_COUNT];
static uint8_t BLDC_HallPrescalerNew;

uint8_t BLDC_HallPositionLast;
uint8_t BLDC_HallPrescaler;
uint16_t BLDC_HallPeriod;
uint16_t BLDC_PWMPeriod;

#define HALL_PERIOD_OVER 0xC000
#define HALL_PERIOD_UNDER 0x6000
#define HALL_PRESCALER_MAX 4

void BLDC_HallFault() {
	BLDC_HallPrescaler = HALL_PRESCALER_MAX;
}

void BLDC_UpdateHallPeriod(uint16_t hall_period) {

	__extension__ void _update_prescaler() {
		TIM_PrescalerConfig(TIM_HALL, (BLDC_SINUS_STEP_COUNT >> BLDC_HallPrescaler) - 1, TIM_PSCReloadMode_Update);
	};

	BLDC_HallPrescaler = BLDC_HallPrescalerNew;
	BLDC_HallPeriod = hall_period;
	TIM_SetAutoreload(TIM_HALL, BLDC_HallPeriod >> BLDC_HallPrescaler);
//	if(hall_period < HALL_PERIOD_UNDER && BLDC_HallPrescalerNew < HALL_PRESCALER_MAX) { BLDC_HallPrescalerNew++; _update_prescaler(); }
//	else if(hall_period > HALL_PERIOD_OVER && BLDC_HallPrescalerNew) { BLDC_HallPrescalerNew--; _update_prescaler(); }
}

void BLDC_RebuildSinusTable(uint16_t pwm_period) {
	if(BLDC_PWMPeriod != pwm_period) {
		BLDC_PWMPeriod = pwm_period;
		uint32_t _cnt = BLDC_SINUS_TABLE_COUNT;
		uint16_t const *_src = BLDC_SinusoidalTable;
		uint16_t *_dst_f = BLDC_SinusoidalTable_Forward, *_dst_r = BLDC_SinusoidalTable_Reverse + BLDC_SINUS_TABLE_COUNT;
		while(_cnt--) {
			uint16_t _val = (uint16_t)(((uint32_t)*_src++) * pwm_period / 0xFFFF);
			*_dst_f++ = *(--_dst_r) = _val;
		}
	}
}

void BLDC_ProcessStep(uint8_t step) {
	typedef struct {
		uint32_t volatile *Dst1;
		uint32_t volatile *Dst2;
		uint32_t volatile *CCRTo0;
	} sDMAStep;
	static sDMAStep const _dma_step_sources[3] = {
		{ .Dst1 = &TIM_3PHASE->CCR1, .Dst2 = &TIM_3PHASE->CCR3, .CCRTo0 = &TIM_3PHASE->CCR2, },
		{ .Dst1 = &TIM_3PHASE->CCR2, .Dst2 = &TIM_3PHASE->CCR1, .CCRTo0 = &TIM_3PHASE->CCR3, },
		{ .Dst1 = &TIM_3PHASE->CCR3, .Dst2 = &TIM_3PHASE->CCR2, .CCRTo0 = &TIM_3PHASE->CCR1, },
	};

	DMA_Cmd(DMA_Channel_Sin1, DISABLE); DMA_Cmd(DMA_Channel_Sin2, DISABLE);
	uint32_t _off = step & 1 ? 32 : 0;
	sDMAStep const *_dma_step = &_dma_step_sources[(step >> 1)];
	*_dma_step->CCRTo0 = 0;

	TIM_SetAutoreload(TIM_DMA1, BLDC_HallPeriod >> BLDC_HallPrescaler); TIM_SetAutoreload(TIM_DMA2, BLDC_HallPeriod >> BLDC_HallPrescaler);
	DMA_QuickConfig(DMA_Channel_Sin1, (uint32_t)_dma_step->Dst1, (uint32_t)(&BLDC_SinusoidalTable_Forward[_off]), BLDC_SINUS_STEP_COUNT);
	DMA_QuickConfig(DMA_Channel_Sin2, (uint32_t)_dma_step->Dst2, (uint32_t)(&BLDC_SinusoidalTable_Reverse[_off]), BLDC_SINUS_STEP_COUNT);

	DMA_Cmd(DMA_Channel_Sin1, ENABLE); DMA_Cmd(DMA_Channel_Sin2, ENABLE);
}

//void BLDCMotorPrepareCommutation(uint8_t hall_position) {
//	// next bridge step calculated by HallSensor inputs
//	// if there was an hall event without changing the hall position,
//	// do nothing.
//	//
//	// In principle, on every hall event you can go to the next motor
//	// step but i had sometimes problems that the motor was running
//	// on an harmonic wave (??) when the motor was without load
//
////	if (__lasthallpos == hallpos) return;
////	__lasthallpos = hallpos;
//
//	BLDC_HallPositionLast++;
//	if(BLDC_HallPositionLast == 7) BLDC_HallPositionLast = 1;
//	hall_position = BLDC_HallPositionLast;
//
//	// this is only for motor direction forward
//	static const sBLDCBridgeState __bldc_bridge_state[8] = {
//		{ .Ph1H = false, .Ph1L = false,      .Ph2H = false, .Ph2L = false,      .Ph3H = false, .Ph3L = false },  // 0 (invalid state)
//		{ .Ph1H = true , .Ph1L = false,      .Ph2H = false, .Ph2L = true ,      .Ph3H = false, .Ph3L = false },  // 1
//		{ .Ph1H = true , .Ph1L = false,      .Ph2H = false, .Ph2L = false,      .Ph3H = false, .Ph3L = true  },  // 2
//		{ .Ph1H = false, .Ph1L = false,      .Ph2H = true , .Ph2L = false,      .Ph3H = false, .Ph3L = true  },  // 3
//		{ .Ph1H = false, .Ph1L = true ,      .Ph2H = true , .Ph2L = false,      .Ph3H = false, .Ph3L = false },  // 4
//		{ .Ph1H = false, .Ph1L = true ,      .Ph2H = false, .Ph2L = false,      .Ph3H = true , .Ph3L = false },  // 5
//		{ .Ph1H = false, .Ph1L = false,      .Ph2H = false, .Ph2L = true ,      .Ph3H = true , .Ph3L = false },  // 6
//		{ .Ph1H = false, .Ph1L = false,      .Ph2H = false, .Ph2L = false,      .Ph3H = false, .Ph3L = false }   // 7 (invalid state)
//	};
//
//	// **** this is with active freewheeling ****
//
//	// Bridge FETs for Motor Phase 1
//	if(__bldc_bridge_state[hall_position].Ph1H) {
//		TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_1, TIM_OCMode_PWM1);
//		TIM_CCxCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCx_Enable);
//		TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCxN_Enable);
//	}
//	else {
//		TIM_CCxCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCx_Disable);
//		if(__bldc_bridge_state[hall_position].Ph1L) {
//			TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_1, TIM_ForcedAction_Active);
//			TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCxN_Enable);
//		}
//		else {
//			TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCxN_Disable);
//		}
//	}
//
//	// Bridge FETs for Motor Phase 2
//	if(__bldc_bridge_state[hall_position].Ph2H) {
//		TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_2, TIM_OCMode_PWM1);
//		TIM_CCxCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCx_Enable);
//		TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCxN_Enable);
//	}
//	else {
//		TIM_CCxCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCx_Disable);
//		if (__bldc_bridge_state[hall_position].Ph2L) {
//			TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_2, TIM_ForcedAction_Active);
//			TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCxN_Enable);
//		}
//		else {
//			TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCxN_Disable);
//		}
//	}
//
//	// Bridge FETs for Motor Phase 3
//	if(__bldc_bridge_state[hall_position].Ph3H) {
//		TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_3, TIM_OCMode_PWM1);
//		TIM_CCxCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCx_Enable);
//		TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCxN_Enable);
//	}
//	else {
//		TIM_CCxCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCx_Disable);
//		if(__bldc_bridge_state[hall_position].Ph3L) {
//			TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_3, TIM_ForcedAction_Active);
//			TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCxN_Enable);
//		}
//		else {
//			TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCxN_Disable);
//		}
//	}
//}
