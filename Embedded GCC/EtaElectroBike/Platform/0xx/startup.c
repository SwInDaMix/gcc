/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Interrupt.h
***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "eta_memman.h"
#include "eta_usart.h"
#include "eta_frames.h"
#include "eta_dbg.h"

#include "BLDC.h"

void *__dso_handle __attribute__ ((weak));
#pragma weak __dso_handle

extern uint8_t _sidata[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern uint8_t _sbss[];
extern uint8_t _ebss[];
extern uint8_t _sheap[];
extern uint8_t _eheap[];

void _init() {

}

#define USART_DBG_portparamlast
#define USART_DBG_portparam
void USART_DBG_IRQHandler() {
	if(USART_GetFlagStatus(USART_DBG, USART_FLAG_RXNE) == SET) {
		Frames8b_PushDataFromISR(USART_DBG_portparam (uint8_t)USART_ReceiveData(USART_DBG));
	}
	if(USART_GetFlagStatus(USART_DBG, USART_FLAG_TXE) == SET) {
		char _c;
		if(Frames8b_PopDataFromISR(USART_DBG_portparam (uint8_t *)&_c)) USART_SendData(USART_DBG, _c);
		else USART_ITConfig(USART_DBG, USART_IT_TXE, DISABLE);
	}
}

void TIM_3PHASE_BRK_UP_TRG_COM_IRQHandler() {
	if(TIM_GetITStatus(TIM_3PHASE, TIM_IT_COM) == SET) {
		TIM_ClearITPendingBit(TIM_3PHASE, TIM_IT_COM);
		// commutationCount++;
	}
}

// this handles TIM_HALL IRQs (from hall sensors)
void TIM_HALL_IRQHandler() {
	#define HALL_PERIOD_OVER 0xC000
	#define HALL_PERIOD_UNDER 0x6000
	#define HALL_PRESCALER_MAX 4

	static uint8_t __prescaler_prev;
	static uint8_t __prescaler;

	if(TIM_GetITStatus(TIM_HALL, TIM_IT_CC1) == SET) {
		TIM_ClearITPendingBit(TIM_HALL, TIM_IT_CC1);
		// calculate motor speed or else with CCR1 values
		uint16_t _hall_period_new = TIM_HALL->CCR1;

		__prescaler_prev = __prescaler;
		if(_hall_period_new < HALL_PERIOD_UNDER && __prescaler < HALL_PRESCALER_MAX) { __prescaler++; }
		else if(_hall_period_new > HALL_PERIOD_OVER && __prescaler) { __prescaler--; }

	}
	else if (TIM_GetITStatus(TIM_HALL, TIM_IT_CC2) == SET) {
		TIM_ClearITPendingBit(TIM_HALL, TIM_IT_CC2);
		// this interrupt handler is called AFTER the motor commutaton event
		// is done after commutation the next motor step must be prepared

		static uint8_t _step;

		BLDC_ProcessStep(_step);
		_step++; if(_step > 5) _step = 0;

		//uint8_t _newhallpos = ((GPIO_ReadInputData(GPIOA) >> 6) & 0x3) | ((GPIO_ReadInputData(GPIOB) >> 0) & 0x1);
	}
}

// Configure main clock and PLL
void CLK_Config() {
	RCC_HSICmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); RCC_PLLCmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL);
}

// Init System specific features
void SYS_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);				// Enable SYSCFG clock
	SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_TIM16, ENABLE);
}

// Configure GPIO peripheral
void GPIO_Config() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);					// Enable GPIOA clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);					// Enable GPIOB clock
}

// Configure interrupt-driven USART port
void USART_Dbg_Config() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);				// Enable USART_DBG clock
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);				// Connect PA2 to USART_DBG_Tx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);				// Connect PA3 to USART_DBG_Rx
	static GPIO_InitTypeDef const _gpio_init_pin_usart_tx = {
		.GPIO_Pin = GPIO_Pin_2,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_usart_tx);		// Configure USART_DBG Tx as alternate function push-pull on PA2
	static GPIO_InitTypeDef const _gpio_init_pin_usart_rx = {
		.GPIO_Pin = GPIO_Pin_3,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_usart_rx);		// Configure USART_DBG Rx as alternate function push-pull on PA3
	static USART_InitTypeDef const _usart_init = {
		.USART_BaudRate = 115200,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
	};
	USART_Init(USART_DBG, (USART_InitTypeDef*)&_usart_init);				// USART_DBG configuration as 115200 baud 8n1
	USART_ITConfig(USART_DBG, USART_IT_RXNE, ENABLE);						// Enable USART_DBG receiver interrupt
	NVIC_SetPriority(USART_DBG_IRQn, 14); NVIC_EnableIRQ(USART_DBG_IRQn);	// Enable USART_DBG interrupts with priority 14
	USART_Cmd(USART_DBG, ENABLE);											// Enable USART_DBG
}

// Configure Timer for 3 phase PWM
void TIM_3Phase_Config() {
	// define the 6 output pins for the bridge
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);				// Enable TIM_3PHASE clock
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);				// Connect PA8 to TIM_3PHASE Ch1 positive PWM output
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);				// Connect PA9 to TIM_3PHASE Ch2 positive PWM output
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_2);				// Connect PA10 to TIM_3PHASE Ch3 positive PWM output
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_2);				// Connect PB13 to TIM_3PHASE Ch1 negative PWM output
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_2);				// Connect PB14 to TIM_3PHASE Ch2 negative PWM output
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_2);				// Connect PB15 to TIM_3PHASE Ch3 negative PWM output
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch1 = {
		.GPIO_Pin = GPIO_Pin_8,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch1);		// Configure TIM_3PHASE Ch1 positive PWM output as alternate function push-pull on PA8
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch2 = {
		.GPIO_Pin = GPIO_Pin_9,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch2);		// Configure TIM_3PHASE Ch2 positive PWM output as alternate function push-pull on PA9
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch3 = {
		.GPIO_Pin = GPIO_Pin_10,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch3);		// Configure TIM_3PHASE Ch3 positive PWM output as alternate function push-pull on PA10
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch1n = {
		.GPIO_Pin = GPIO_Pin_13,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch1n);		// Configure TIM_3PHASE Ch1 negative PWM output as alternate function push-pull on PB13
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch2n = {
		.GPIO_Pin = GPIO_Pin_14,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch2n);		// Configure TIM_3PHASE Ch2 negative PWM output as alternate function push-pull on PB14
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch3n = {
		.GPIO_Pin = GPIO_Pin_15,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch3n);		// Configure TIM_3PHASE Ch3 negative PWM output as alternate function push-pull on PB15

	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = 0,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = BLDC_PWM_PERIOD,									// Chopper Frequency (PWM for the FETs) 18kHz was good in imperic tests
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(TIM_3PHASE, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);	// TIM_3PHASE Channel 1, 2, 3 set to PWM mode - all 6 outputs per channel on output of FETs
	static TIM_OCInitTypeDef const _tim_oc_init = {
		.TIM_OCMode = TIM_OCMode_Timing,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Enable,
		.TIM_Pulse = 0, // BLDC_ccr_val
		.TIM_OCPolarity = TIM_OCPolarity_High,
		.TIM_OCNPolarity = TIM_OCNPolarity_High,
		.TIM_OCIdleState = TIM_OCIdleState_Set,
		.TIM_OCNIdleState = TIM_OCNIdleState_Set
	};
	TIM_OC1Init(TIM_3PHASE, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC2Init(TIM_3PHASE, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC3Init(TIM_3PHASE, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC1PreloadConfig(TIM_3PHASE, TIM_OCPreload_Enable);				// Activate preloading of the CCR register
	TIM_OC2PreloadConfig(TIM_3PHASE, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM_3PHASE, TIM_OCPreload_Enable);
	static TIM_BDTRInitTypeDef const _tim_bdtr_init = {
		.TIM_OSSRState = TIM_OSSRState_Enable,
		.TIM_OSSIState = TIM_OSSIState_Enable,
		.TIM_LOCKLevel = TIM_LOCKLevel_OFF,
		.TIM_DeadTime = 7,												// DeadTime[ns] = value * (1/SystemCoreFreq) (on 48MHz: 7 is 146ns)
		.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable,
		.TIM_Break = TIM_Break_Disable,									// Enabel this if you use emergency stop signal
		.TIM_BreakPolarity = TIM_BreakPolarity_High
	};
	TIM_BDTRConfig(TIM_3PHASE, (TIM_BDTRInitTypeDef*)&_tim_bdtr_init);		// Automatic output enable, break off, dead time ca. 200ns and no lock of configuration
	TIM_CCPreloadControl(TIM_3PHASE, ENABLE);								// Preload ARR register
	TIM_SelectInputTrigger(TIM_3PHASE, TIM_TS_ITR2);						// MotorTimer = TIM_3PHASE, HallTimer = TIM_HALL
	TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_1, TIM_OCMode_PWM1); TIM_CCxCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCx_Enable); TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_1, TIM_CCxN_Enable);
	TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_2, TIM_OCMode_PWM1); TIM_CCxCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCx_Enable); TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_2, TIM_CCxN_Enable);
	TIM_SelectOCxM(TIM_3PHASE, TIM_Channel_3, TIM_OCMode_PWM1); TIM_CCxCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCx_Enable); TIM_CCxNCmd(TIM_3PHASE, TIM_Channel_3, TIM_CCxN_Enable);
	TIM_SelectCOM(TIM_3PHASE, ENABLE);										// Activate COM (Commutation) Event from Slave (HallSensor timer) through TRGI

	// --------- activate the bldc bridge ctrl. ----------
	// in a project this will be done late after complete
	// configuration of other peripherie
	TIM_ITConfig(TIM_3PHASE, TIM_IT_COM, ENABLE);							// Enable COM (commutation) IRQ
	NVIC_SetPriority(TIM_3PHASE_BRK_UP_TRG_COM_IRQn, 0); NVIC_EnableIRQ(TIM_3PHASE_BRK_UP_TRG_COM_IRQn);	// Enable interrupt, motor commutation has the highest priority
	TIM_Cmd(TIM_3PHASE, ENABLE);											// Enable motor timer
	TIM_CtrlPWMOutputs(TIM_3PHASE, ENABLE);									// Enable motor timer main output (the bridge signals)
}

// Configure Timer for hall sensors
void TIM_Hall_Config() {
	// define timer clock between two changes on the hall sensor lines on the lowest rotation speed (eg. 1/100 from max. speed) the timer must not overflow
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);				// Enable TIM_HALL clock
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);				// Connect PA6 to TIM_HALL Ch1 hall sensor input
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);				// Connect PA7 to TIM_HALL Ch2 hall sensor input
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_1);				// Connect PB0 to TIM_HALL Ch3 hall sensor input
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch1 = {
		.GPIO_Pin = GPIO_Pin_6,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_2MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch1);		// Configure TIM_HALL Ch1 hall sensor input as alternate function pull-down on PA6
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch2 = {
		.GPIO_Pin = GPIO_Pin_7,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_2MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch2);		// Configure TIM_HALL Ch2 hall sensor input as alternate function pull-down on PA7
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch3 = {
		.GPIO_Pin = GPIO_Pin_0,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_2MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch3);		// Configure TIM_HALL Ch3 hall sensor input as alternate function pull-down on PB0

	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = BLDC_SINUS_STEP_COUNT - 1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = 25000,
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(TIM_HALL, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);	// TIM_HALL base configuration: 126 => 3,5s till overflow ; 285,714kHz TimerClock [36MHz/Prescaler]
	TIM_SelectHallSensor(TIM_HALL, ENABLE);									// Enable hall sensor: T1F_ED will be connected to  HallSensoren Inputs TIM3_CH1, TIM3_CH2, TIM3_CH3
	TIM_SelectInputTrigger(TIM_HALL, TIM_TS_TI1F_ED);						// HallSensor event is delivered with signal TI1F_ED (this is XOR of the three hall sensor lines). Signal TI1F_ED: falling and rising edge of the inputs is used
	TIM_SelectSlaveMode(TIM_HALL, TIM_SlaveMode_Reset);						// On every TI1F_ED event the counter is reseted and update is triggered
	static TIM_ICInitTypeDef const _tim_ic_init = {
		.TIM_Channel = TIM_Channel_1,
		.TIM_ICPolarity = TIM_ICPolarity_Rising,
		.TIM_ICSelection = TIM_ICSelection_TRC,							// listen to T1, the  HallSensorEvent
		.TIM_ICPrescaler = TIM_ICPSC_DIV1,								// Div:1, every edge
		.TIM_ICFilter = 0xF												// Noise filter: 1111 => 72000kHz / factor (==1) / 32 / 8 -> 281kHz input noise filter (reference manual page 322)
	};
	TIM_ICInit(TIM_HALL, (TIM_ICInitTypeDef*)&_tim_ic_init);			// Channel 1 in input capture mode on every TCR edge (build from TI1F_ED which is a HallSensor edge) the timer value is copied into CCR register and a CCR1 Interrupt TIM_IT_CC1 is fired
	TIM_ClearFlag(TIM_HALL, TIM_FLAG_CC2);								// Clear interrupt flag
	TIM_SelectOutputTrigger(TIM_HALL, TIM_TRGOSource_OC2Ref);			// Chanel 2 output compare signal is connected to TRIGO
	TIM_ITConfig(TIM_HALL, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);			// Enable channel 2 compare interrupt request TIM_IT_CC1 | TIM_IT_CC2
	NVIC_SetPriority(TIM_HALL_IRQn, 1); NVIC_EnableIRQ(TIM_HALL_IRQn);	// We use preemption interrupts here, BLDC Bridge switching and Hall has highest priority after BLDC Bridge timer
	TIM_Cmd(TIM_HALL, ENABLE);
}

// Configure Timers for DMA triggering
void TIM_DMAs_Config() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);				// Enable TIM_DMA1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);				// Enable TIM_DMA2 clock

	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = 0,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = 25000,
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(TIM_DMA1, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);	// TIM_DMA1 base configuration
	TIM_TimeBaseInit(TIM_DMA2, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);	// TIM_DMA2 base configuration
	TIM_DMACmd(TIM_DMA1, TIM_DMA_Update, ENABLE); TIM_DMACmd(TIM_DMA2, TIM_DMA_Update, ENABLE);
	TIM_Cmd(TIM_DMA1, ENABLE); TIM_Cmd(TIM_DMA2, ENABLE);
}

// Configure Timers for recuparator booster
void TIM_RecuperatorBooster_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);				// Enable TIM_RECUPERATOR_BOOSTER clock
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_2);				// Connect PB9 to TIM_RECUPERATOR_BOOSTER Ch1 positive PWM output
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_2);				// Connect PB7 to TIM_RECUPERATOR_BOOSTER Ch1 negative PWM output
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch1 = {
		.GPIO_Pin = GPIO_Pin_9,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch1);		// Configure TIM_RECUPERATOR_BOOSTER Ch1 positive PWM output as alternate function push-pull on PB9
	static GPIO_InitTypeDef const _gpio_init_pin_tim_ch1n = {
		.GPIO_Pin = GPIO_Pin_7,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_ch1n);		// Configure TIM_RECUPERATOR_BOOSTER Ch1 negative PWM output as alternate function push-pull on PB7

	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = 0,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = 960,
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(TIM_RECUPERATOR_BOOSTER, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);	// TIM_RECUPERATOR_BOOSTER base configuration
	static TIM_OCInitTypeDef const _tim_oc_init = {
		.TIM_OCMode = TIM_OCMode_Timing,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Enable,
		.TIM_Pulse = 0, // BLDC_ccr_val
		.TIM_OCPolarity = TIM_OCPolarity_High,
		.TIM_OCNPolarity = TIM_OCNPolarity_High,
		.TIM_OCIdleState = TIM_OCIdleState_Set,
		.TIM_OCNIdleState = TIM_OCNIdleState_Set
	};
	TIM_OC1Init(TIM_RECUPERATOR_BOOSTER, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC1PreloadConfig(TIM_RECUPERATOR_BOOSTER, TIM_OCPreload_Enable);// Activate preloading of the CCR register
	static TIM_BDTRInitTypeDef const _tim_bdtr_init = {
		.TIM_OSSRState = TIM_OSSRState_Enable,
		.TIM_OSSIState = TIM_OSSIState_Enable,
		.TIM_LOCKLevel = TIM_LOCKLevel_OFF,
		.TIM_DeadTime = 7,												// DeadTime[ns] = value * (1/SystemCoreFreq) (on 48MHz: 7 is 146ns)
		.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable,
		.TIM_Break = TIM_Break_Disable,									// Enabel this if you use emergency stop signal
		.TIM_BreakPolarity = TIM_BreakPolarity_High
	};
	TIM_BDTRConfig(TIM_RECUPERATOR_BOOSTER, (TIM_BDTRInitTypeDef*)&_tim_bdtr_init);		// Automatic output enable, break off, dead time 146ns and no lock of configuration
	TIM_SelectOCxM(TIM_RECUPERATOR_BOOSTER, TIM_Channel_1, TIM_OCMode_PWM1); TIM_CCxCmd(TIM_RECUPERATOR_BOOSTER, TIM_Channel_1, TIM_CCx_Enable); TIM_CCxNCmd(TIM_RECUPERATOR_BOOSTER, TIM_Channel_1, TIM_CCxN_Enable);
	TIM_Cmd(TIM_RECUPERATOR_BOOSTER, ENABLE);
	TIM_CtrlPWMOutputs(TIM_RECUPERATOR_BOOSTER, ENABLE);				// Enable motor timer main output (the bridge signals)

	TIM_RECUPERATOR_BOOSTER->CCR1 = 200;
}

// Configure DMA to form sine wave
void DMA_Config() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);					// Configure DMA for 16 bit transfer of BLDC_SINUS_STEP_COUNT

	static DMA_InitTypeDef const _dma_init = {
		.DMA_PeripheralBaseAddr = 0,
		.DMA_MemoryBaseAddr = 0,
		.DMA_DIR = DMA_DIR_PeripheralDST,
		.DMA_BufferSize = BLDC_SINUS_STEP_COUNT,
		.DMA_PeripheralInc = DMA_PeripheralInc_Disable,
		.DMA_MemoryInc = DMA_MemoryInc_Enable,
		.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
		.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
		.DMA_Mode = DMA_Mode_Normal,
		.DMA_Priority = DMA_Priority_VeryHigh,
		.DMA_M2M = DMA_M2M_Disable
	};
	DMA_Init(DMA_Channel_Sin1, (DMA_InitTypeDef*)&_dma_init);
	DMA_Init(DMA_Channel_Sin2, (DMA_InitTypeDef*)&_dma_init);
}

void __attribute__ ((noreturn)) __attribute__ ((used)) Reset_Handler() {
	CLK_Config();
	memcpy(_sdata, _sidata, _edata - _sdata);
	memset(_sbss, 0, _ebss - _sbss);
	SYS_Config();
	GPIO_Config();
	USART_Dbg_Config();
	TIM_3Phase_Config(); TIM_Hall_Config(); TIM_DMAs_Config(); TIM_RecuperatorBooster_Config();
	DMA_Config();
	BLDC_RebuildSinusTable(BLDC_PWM_PERIOD - 1); BLDC_UpdateHallPeriod(25000);
	memman_initialize(_sheap, (uint16_t)(_eheap - _sheap));
	extern void __libc_init_array(); __libc_init_array();

	bool _is_abort_requested = false, _is_inited = false;

	__extension__ void yield_func() {  };
	extern void EtaElectroBikeMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void (*yield)());
	EtaElectroBikeMainLoop(&_is_abort_requested, &_is_inited, yield_func);
	while(true);
}

void _exit(int exitcode) {
	DBG_PutFormat("_exit function called with code %d"NL, exitcode);
	while(1);
}
int _kill(int pid, int sig) {
	DBG_PutFormat("_kill function called with sig %d"NL, sig);
	return 0;
}
int _getpid() {
	return 0;
}

void __attribute__((noreturn)) HardFault_Handler() {
	while (1);
}
