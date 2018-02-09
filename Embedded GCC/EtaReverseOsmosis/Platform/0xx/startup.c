/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Interrupt.h
***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "eta_usart.h"
#include "eta_frames.h"
#include "eta_dbg.h"
#include "entry.h"

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

void LED_IRQHandler() {
	if(TIM_GetITStatus(LED_TIM1, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(LED_TIM1, TIM_IT_Update);
		Int_Update();
	}
}

// Configure main clock and PLL
void CLK_Config() {
	RCC_HSICmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); RCC_PLLCmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL);
}

// Config independent watchdog
void IWDG_Config() {
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetReload(F_LSI / 32 * 1000 / 750);						// 750 ms
	IWDG_ReloadCounter();
	IWDG_Enable();
}

// Init System specific features
void SYS_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);				// Enable SYSCFG clock
}

// Configure GPIO peripheral
void GPIO_Config() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);					// Enable GPIOA clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);					// Enable GPIOB clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);					// Enable GPIOB clock
}

// Configure interrupt-driven USART port
void USART_Dbg_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);				// Enable USART_DBG clock
	GPIO_PinAFConfig(USART_DBG_TX_PORT, USART_DBG_TX_PIN, USART_DBG_TX_AF);
	GPIO_PinAFConfig(USART_DBG_RX_PORT, USART_DBG_RX_PIN, USART_DBG_RX_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_usart_port_b = {
		.GPIO_Pin = USART_DBG_PINCFG_B,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_usart_port_b);
	static USART_InitTypeDef const _usart_init = {
		.USART_BaudRate = USART_BAUDRATE,
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

// Configure Timers for 7-Segment and LEDs indicators
void TIM_LED_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	GPIO_PinAFConfig(LED_SEGMENT_A_PORT, LED_SEGMENT_A_PIN, LED_SEGMENT_A_AF);
	GPIO_PinAFConfig(LED_SEGMENT_B_PORT, LED_SEGMENT_B_PIN, LED_SEGMENT_B_AF);
	GPIO_PinAFConfig(LED_SEGMENT_C_PORT, LED_SEGMENT_C_PIN, LED_SEGMENT_C_AF);
	GPIO_PinAFConfig(LED_SEGMENT_D_PORT, LED_SEGMENT_D_PIN, LED_SEGMENT_D_AF);
	GPIO_PinAFConfig(LED_SEGMENT_E_PORT, LED_SEGMENT_E_PIN, LED_SEGMENT_E_AF);
	GPIO_PinAFConfig(LED_SEGMENT_F_PORT, LED_SEGMENT_F_PIN, LED_SEGMENT_F_AF);
	GPIO_PinAFConfig(LED_SEGMENT_G_PORT, LED_SEGMENT_G_PIN, LED_SEGMENT_G_AF);
	GPIO_PinAFConfig(LED_SEGMENT_DP_PORT, LED_SEGMENT_DP_PIN, LED_SEGMENT_DP_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_tim_port_a = {
		.GPIO_Pin = LED_SEGMENT_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_port_a);
	static GPIO_InitTypeDef const _gpio_init_pin_tim_port_b = {
		.GPIO_Pin = LED_SEGMENT_PINCFG_B,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_port_b);

	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = LED_PRESCALER - 1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = LED_PERIOD,
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(LED_TIM1, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);
	TIM_TimeBaseInit(LED_TIM2, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);
	static TIM_OCInitTypeDef const _tim_oc_init = {
		.TIM_OCMode = TIM_OCMode_Timing,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Disable,
		.TIM_Pulse = 0,
		.TIM_OCPolarity = TIM_OCPolarity_High,
		.TIM_OCNPolarity = TIM_OCNPolarity_High,
		.TIM_OCIdleState = TIM_OCIdleState_Set,
		.TIM_OCNIdleState = TIM_OCNIdleState_Set
	};
	TIM_OC1Init(LED_TIM1, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC2Init(LED_TIM1, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC3Init(LED_TIM1, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC4Init(LED_TIM1, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC1Init(LED_TIM2, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC2Init(LED_TIM2, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC3Init(LED_TIM2, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC4Init(LED_TIM2, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC1PreloadConfig(LED_TIM1, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM1, TIM_Channel_1, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM1, TIM_Channel_1, TIM_CCx_Enable);
	TIM_OC2PreloadConfig(LED_TIM1, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM1, TIM_Channel_2, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM1, TIM_Channel_2, TIM_CCx_Enable);
	TIM_OC3PreloadConfig(LED_TIM1, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM1, TIM_Channel_3, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM1, TIM_Channel_3, TIM_CCx_Enable);
	TIM_OC4PreloadConfig(LED_TIM1, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM1, TIM_Channel_4, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM1, TIM_Channel_4, TIM_CCx_Enable);
	TIM_OC1PreloadConfig(LED_TIM2, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM2, TIM_Channel_1, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM2, TIM_Channel_1, TIM_CCx_Enable);
	TIM_OC2PreloadConfig(LED_TIM2, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM2, TIM_Channel_2, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM2, TIM_Channel_2, TIM_CCx_Enable);
	TIM_OC3PreloadConfig(LED_TIM2, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM2, TIM_Channel_3, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM2, TIM_Channel_3, TIM_CCx_Enable);
	TIM_OC4PreloadConfig(LED_TIM2, TIM_OCPreload_Enable); TIM_SelectOCxM(LED_TIM2, TIM_Channel_4, TIM_OCMode_PWM1); TIM_CCxCmd(LED_TIM2, TIM_Channel_4, TIM_CCx_Enable);

	TIM_CtrlPWMOutputs(LED_TIM1, ENABLE);
	TIM_SelectOnePulseMode(LED_TIM1, TIM_OPMode_Single);
	TIM_SelectOnePulseMode(LED_TIM2, TIM_OPMode_Single);

	TIM_SelectOutputTrigger(LED_TIM1, TIM_TRGOSource_Enable);
	TIM_SelectMasterSlaveMode(LED_TIM1, TIM_MasterSlaveMode_Enable);

	TIM_SelectInputTrigger(LED_TIM2, TIM_TS_ITR0);
	TIM_SelectSlaveMode(LED_TIM2, TIM_SlaveMode_Trigger);

	TIM_ITConfig(LED_TIM1, TIM_IT_Update, ENABLE);
	NVIC_SetPriority(LED_IRQn, 0); NVIC_EnableIRQ(LED_IRQn);
	TIM_Cmd(LED_TIM1, ENABLE);// TIM_Cmd(LED_TIM2, ENABLE);
}

// Configure Timer for Buzzer
void TIM_Buzzer_Config() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	GPIO_PinAFConfig(BUZZER_PORT, BUZZER_PIN, BUZZER_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_tim_port_a = {
		.GPIO_Pin = BUZZER_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_port_a);
	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = 0,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = 20000,
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(BUZZER_TIM, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);
	static TIM_OCInitTypeDef const _tim_oc_init = {
		.TIM_OCMode = TIM_OCMode_Timing,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Disable,
		.TIM_Pulse = 0,
		.TIM_OCPolarity = TIM_OCPolarity_High,
		.TIM_OCNPolarity = TIM_OCPolarity_High,
		.TIM_OCIdleState = TIM_OCIdleState_Set,
		.TIM_OCNIdleState = TIM_OCNIdleState_Set
	};
	TIM_OC1Init(BUZZER_TIM, (TIM_OCInitTypeDef*)&_tim_oc_init);
	TIM_OC1PreloadConfig(BUZZER_TIM, TIM_OCPreload_Enable);
	TIM_CCPreloadControl(BUZZER_TIM, ENABLE);
	TIM_SelectOCxM(BUZZER_TIM, TIM_Channel_1, TIM_OCMode_PWM1); TIM_CCxCmd(BUZZER_TIM, TIM_Channel_1, TIM_CCx_Enable);
	TIM_Cmd(BUZZER_TIM, ENABLE);
}

// Configure GPIO pins for direct usage
void Pins_Config() {
	// for LEDS digits
	static GPIO_InitTypeDef const _gpio_init_output_dig_port_a = {
		.GPIO_Pin = LED_DIG_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_output_dig_port_a);
	static GPIO_InitTypeDef const _gpio_init_output_dig_port_f = {
		.GPIO_Pin = LED_DIG_PINCFG_F,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOF, (GPIO_InitTypeDef*)&_gpio_init_output_dig_port_f);

	// for switches
	static GPIO_InitTypeDef const _gpio_init_output_sw_port_a = {
		.GPIO_Pin = VALVE_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_Speed = GPIO_Speed_2MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_output_sw_port_a);

	// for sensors
	#ifdef REVOSM_LOWINLET_SWITCH_HIGHTANK_SWITCH
	static GPIO_InitTypeDef const _gpio_init_input_sens_port_a = {
		.GPIO_Pin = SENS_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_IN,
		.GPIO_Speed = GPIO_Speed_2MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN,
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_input_sens_port_a);
	#endif

	// for buttons
	static GPIO_InitTypeDef const _gpio_init_output_btn_port_b = {
		.GPIO_Pin = BTN_PINCFG_B,
		.GPIO_Mode = GPIO_Mode_IN,
		.GPIO_Speed = GPIO_Speed_2MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_output_btn_port_b);
}

void __attribute__ ((noreturn)) __attribute__ ((used)) Reset_Handler() {
	CLK_Config();
	IWDG_Config();
	memcpy(_sdata, _sidata, _edata - _sdata);
	memset(_sbss, 0, _ebss - _sbss);
	SYS_Config();
	GPIO_Config();
	USART_Dbg_Config();
	TIM_LED_Config();
	TIM_Buzzer_Config();
	Pins_Config();
	extern void __libc_init_array(); __libc_init_array();

	bool _is_abort_requested = false, _is_inited = false;

	__extension__ void yield_func() {  };
	EtaReverseOsmosisMainLoop(&_is_abort_requested, &_is_inited, yield_func);
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
