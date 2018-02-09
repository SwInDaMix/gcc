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
#include "entry.h"

void *__dso_handle __attribute__ ((weak));
#pragma weak __dso_handle

extern uint8_t _sidata[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern uint8_t _sbss[];
extern uint8_t _ebss[];
//extern uint8_t _sheap[];
//extern uint8_t _eheap[];

void _init() {

}

#define USART_DBG_portparamlast
#define USART_DBG_portparam
void USART_DBG_IRQHandler() {
	if(USART_GetFlagStatus(USART_DBG, USART_FLAG_RXNE) == SET) {
		Frames8b_PushDataFromISR(USART_DBG_portparam (uint8_t)USART_ReceiveData(USART_DBG));
	}
	if(USART_GetFlagStatus(USART_DBG, USART_FLAG_TXE) == SET) {
		uint8_t _c;
		if(Frames8b_PopDataFromISR(USART_DBG_portparam &_c)) USART_SendData(USART_DBG, _c);
		else USART_ITConfig(USART_DBG, USART_IT_TXE, DISABLE);
	}
}

void TIM_PERIODIC_IRQHandler() {
	if(TIM_GetITStatus(TIM_PERIODIC, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM_PERIODIC, TIM_IT_Update);
		PeriodicHandler();
	}
}

// Configure main clock and PLL
void CLK_Config() {
//	RCC_HSICmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
//	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); RCC_PLLCmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
//	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
}

// Config independent watchdog
void IWDG_Config() {
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetReload(F_LSI / 32 * 1000 / PERIOD_IWDG_PERIOD);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

// Init System specific features
void SYS_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);				// Enable SYSCFG clock
}

// Init ADC and CRC unit for RNG entropy
void ADC_RNGEntropy() {
	// Enable ADC1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// Initialize ADC 14MHz RC
	RCC_ADCCLKConfig(RCC_ADCCLK_HSI14);
	RCC_HSI14Cmd(ENABLE);
	while (!RCC_GetFlagStatus(RCC_FLAG_HSI14RDY));
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_AsynClk);

	// Init ADC for Temp channel
	ADC_DeInit(ADC1);
	static ADC_InitTypeDef const _adc_init = {
		.ADC_ContinuousConvMode = DISABLE,
		.ADC_DataAlign = ADC_DataAlign_Right,
		.ADC_Resolution = ADC_Resolution_12b,
		.ADC_ScanDirection = ADC_ScanDirection_Backward,
		.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None,
		.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO
	};
	ADC_Init(ADC1, (ADC_InitTypeDef *)&_adc_init);
	ADC_TempSensorCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	// Enable CRC clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

// Configure GPIO peripheral
void GPIO_Config() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);					// Enable GPIOA clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);					// Enable GPIOB clock

	static GPIO_InitTypeDef const _gpio_init_pin_output_pp_port_a = {
		.GPIO_Pin = GPIO_OUTPUT_PP_PIN_A,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef *)&_gpio_init_pin_output_pp_port_a);
	static GPIO_InitTypeDef const _gpio_init_pin_output_pp_port_b = {
		.GPIO_Pin = GPIO_OUTPUT_PP_PIN_B,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef *)&_gpio_init_pin_output_pp_port_b);
//	static GPIO_InitTypeDef const _gpio_init_pin_input_up_port_a = {
//		.GPIO_Pin = GPIO_INPUT_UP_PIN_A,
//		.GPIO_Mode = GPIO_Mode_IN,
//		.GPIO_Speed = GPIO_Speed_50MHz,
//		.GPIO_OType = GPIO_OType_PP,
//		.GPIO_PuPd = GPIO_PuPd_UP
//	};
//	GPIO_Init(GPIOA, (GPIO_InitTypeDef *)&_gpio_init_pin_input_up_port_a);
	static GPIO_InitTypeDef const _gpio_init_pin_input_up_port_b = {
		.GPIO_Pin = GPIO_INPUT_UP_PIN_B,
		.GPIO_Mode = GPIO_Mode_IN,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef *)&_gpio_init_pin_input_up_port_b);
}

// Configure interrupt-driven USART port
void USART_Dbg_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);				// Enable USART_DBG clock
	GPIO_PinAFConfig(USART_DBG_TX_PORT, USART_DBG_TX_PIN, USART_DBG_TX_AF);
	GPIO_PinAFConfig(USART_DBG_RX_PORT, USART_DBG_RX_PIN, USART_DBG_RX_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_usart_port_a = {
		.GPIO_Pin = USART_DBG_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef *)&_gpio_init_pin_usart_port_a);
	static USART_InitTypeDef const _usart_init = {
		.USART_BaudRate = USART_BAUDRATE,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
	};
	USART_Init(USART_DBG, (USART_InitTypeDef *)&_usart_init);				// USART_DBG configuration as 115200 baud 8n1
	USART_OverrunDetectionConfig(USART_DBG, USART_OVRDetection_Disable);	// Disable Overrun
	USART_ITConfig(USART_DBG, USART_IT_RXNE, ENABLE);						// Enable USART_DBG receiver interrupt
	NVIC_SetPriority(USART_DBG_IRQn, 14); NVIC_EnableIRQ(USART_DBG_IRQn);	// Enable USART_DBG interrupts with priority 14
	USART_Cmd(USART_DBG, ENABLE);											// Enable USART_DBG
}

// Configure SPI for RFM module
void SPI_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	GPIO_PinAFConfig(SPI_RF_CLIENT_SCK_PORT, SPI_RF_CLIENT_SCK_PIN, SPI_RF_CLIENT_SCK_AF);
	GPIO_PinAFConfig(SPI_RF_CLIENT_MISO_PORT, SPI_RF_CLIENT_MISO_PIN, SPI_RF_CLIENT_MISO_AF);
	GPIO_PinAFConfig(SPI_RF_CLIENT_MOSI_PORT, SPI_RF_CLIENT_MOSI_PIN, SPI_RF_CLIENT_MOSI_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_spi_port_a = {
		.GPIO_Pin = SPI_RF_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef *)&_gpio_init_pin_spi_port_a);
	static SPI_InitTypeDef const _spi_init = {
		.SPI_Direction = SPI_Direction_2Lines_FullDuplex,
		.SPI_Mode = SPI_Mode_Master,
		.SPI_DataSize = SPI_DataSize_8b,
		.SPI_CPOL = SPI_CPOL_Low,
		.SPI_CPHA = SPI_CPHA_1Edge,
		.SPI_NSS = SPI_NSS_Soft,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8,
		.SPI_FirstBit = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial = 0
	};
	SPI_Init(SPI_RF_CLIENT, (SPI_InitTypeDef *)&_spi_init);
	SPI_RxFIFOThresholdConfig(SPI_RF_CLIENT, SPI_RxFIFOThreshold_QF);
	GPIO_WriteBit(SPI_RF_CLIENT_NSS_PORT, (1 << SPI_RF_CLIENT_NSS_PIN), Bit_SET);
	SPI_Cmd(SPI_RF_CLIENT, ENABLE);
}

// Configure Timer for delays and timeouts
void TIM_PeriodicConfig() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	static TIM_TimeBaseInitTypeDef const _tim_base_init = {
		.TIM_Prescaler = 0,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_Period = (F_CPU / PERIODIC_FREQ),
		.TIM_ClockDivision = 0,
		.TIM_RepetitionCounter = 0
	};
	TIM_TimeBaseInit(TIM_PERIODIC, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);
	TIM_ITConfig(TIM_PERIODIC, TIM_IT_Update, ENABLE);
	NVIC_SetPriority(TIM_PERIODIC_IRQn, 0); NVIC_EnableIRQ(TIM_PERIODIC_IRQn);
	TIM_Cmd(TIM_PERIODIC, ENABLE);
}

void __attribute__ ((noreturn, used)) Reset_Handler() {
	CLK_Config();
	//IWDG_Config();
	memset(_sdata, 0, 2048);
	memcpy(_sdata, _sidata, _edata - _sdata);
	memset(_sbss, 0, _ebss - _sbss);
	SYS_Config();
	ADC_RNGEntropy(); GPIO_Config();
	USART_Dbg_Config(); SPI_Config(); TIM_PeriodicConfig();
	extern void __libc_init_array(); __libc_init_array();

	bool _is_abort_requested = false, _is_inited = false;

	__extension__ void yield_func() {  };
	EtaRemoteControlKeyChainMainLoop(&_is_abort_requested, &_is_inited, yield_func);
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
