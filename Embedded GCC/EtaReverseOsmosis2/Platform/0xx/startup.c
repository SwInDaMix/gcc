/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Interrupt.h
***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "eta_usart.h"
#include "eta_frames.h"
#include "eta_dbg.h"
#include "periphs.h"
#include "entry.h"

//void *__dso_handle __attribute__ ((weak));
//#pragma weak __dso_handle

extern uint8_t _sidata[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern uint8_t _sbss[];
extern uint8_t _ebss[];

void _init() {

}

#define USART_MAIN_portparamlast
#define USART_MAIN_portparam
void USART_MAIN_IRQHandler() {
	if(USART_GetFlagStatus(USART_MAIN, USART_FLAG_RXNE) == SET) {
		//Frames8b_PushDataFromISR(USART_MAIN_portparam (uint8_t)USART_ReceiveData(USART_MAIN));
		USART_PushCharFromISR(USART_MAIN_portparam (uint8_t)USART_ReceiveData(USART_MAIN));
	}
	if(USART_GetFlagStatus(USART_MAIN, USART_FLAG_TXE) == SET) {
		char _c;
		if(USART_PopCharFromISR(USART_MAIN_portparam &_c)) USART_SendData(USART_MAIN, _c);
		//if(Frames8b_PopDataFromISR(USART_MAIN_portparam (uint8_t *)&_c)) USART_SendData(USART_MAIN, _c);
		else USART_ITConfig(USART_MAIN, USART_IT_TXE, DISABLE);
	}
}

void TIM_LCD_BL_IRQHandler() {
	if(TIM_GetITStatus(TIM_LCD_BL, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM_LCD_BL, TIM_IT_Update);
		Periph_Periodic();
	}
}

// Configure main clock and PLL
static void CLK_Config() {
	RCC_HSICmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); RCC_PLLCmd(ENABLE); while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL);
}

// Config independent watchdog
static void IWDG_Config() {
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetReload(F_LSI * 1000 / 32 / 750);						    // 750 ms
	IWDG_ReloadCounter();
	IWDG_Enable();
}

// Init System specific features
static void SYS_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);				// Enable SYSCFG clock
}

// Configure GPIO peripheral
static void GPIO_Config() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);					// Enable GPIOA clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);					// Enable GPIOB clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);					// Enable GPIOB clock
}

// Configure CRC clock
static void CRC_Config() {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

// Configure timer for delays
static void TIM_Delay_Config() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
}

// Configure interrupt-driven USART port
static void USART_Dbg_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);				// Enable USART_MAIN clock
	GPIO_PinAFConfig(USART_MAIN_TX_PORT, USART_MAIN_TX_PIN, USART_MAIN_TX_AF);
	GPIO_PinAFConfig(USART_MAIN_RX_PORT, USART_MAIN_RX_PIN, USART_MAIN_RX_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_usart_port_a = {
		.GPIO_Pin = USART_MAIN_PINCFG_A,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_10MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_pin_usart_port_a);
	static USART_InitTypeDef const _usart_init = {
		.USART_BaudRate = USART_BAUDRATE,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
	};
	USART_Init(USART_MAIN, (USART_InitTypeDef*)&_usart_init);				// USART_MAIN configuration as 115200 baud 8n1
	USART_ITConfig(USART_MAIN, USART_IT_RXNE, ENABLE);						// Enable USART_MAIN receiver interrupt
	NVIC_SetPriority(USART_MAIN_IRQn, 14); NVIC_EnableIRQ(USART_MAIN_IRQn);	// Enable USART_MAIN interrupts with priority 14
	USART_Cmd(USART_MAIN, ENABLE);											// Enable USART_MAIN
}

// Configure SPI for both LCD and Flash which sit on the same bus
static void SPI_LCD_Flash_Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    GPIO_PinAFConfig(SPI_LCD_FLASH_SCK_PORT, SPI_LCD_FLASH_SCK_PIN, SPI_LCD_FLASH_SCK_AF);
    GPIO_PinAFConfig(SPI_LCD_FLASH_MOSI_PORT, SPI_LCD_FLASH_MOSI_PIN, SPI_LCD_FLASH_MOSI_AF);
    GPIO_PinAFConfig(SPI_LCD_FLASH_MISO_PORT, SPI_LCD_FLASH_MISO_PIN, SPI_LCD_FLASH_MISO_AF);
    static GPIO_InitTypeDef const _gpio_init_pin_spi_port_b = {
        .GPIO_Pin = SPI_LCD_FLASH_PINCFG_B,
        .GPIO_Mode = GPIO_Mode_AF,
        .GPIO_Speed = GPIO_Speed_10MHz,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_PuPd = GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_spi_port_b);
    static SPI_InitTypeDef const _spi_init = {
        .SPI_Direction = SPI_Direction_2Lines_FullDuplex,
        .SPI_Mode = SPI_Mode_Master,
        .SPI_DataSize = SPI_DataSize_8b,
        .SPI_CPOL = SPI_CPOL_High,
        .SPI_CPHA = SPI_CPHA_2Edge,
        .SPI_NSS = SPI_NSS_Soft,
        .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16,
        .SPI_FirstBit = SPI_FirstBit_MSB,
        .SPI_CRCPolynomial = 0
    };
    SPI_Init(SPI_LCD_FLASH, (SPI_InitTypeDef *)&_spi_init);
    SPI_RxFIFOThresholdConfig(SPI_LCD_FLASH, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI_LCD_FLASH, ENABLE);
}

// Configure LCD
static void LCD_Config() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
    GPIO_PinAFConfig(TIM_LCD_BL_PORT, TIM_LCD_BL_PIN, TIM_LCD_BL_AF);
    static GPIO_InitTypeDef const _gpio_init_output_lcd_port_a = {
            .GPIO_Pin = LCD_PINCFG_A,
            .GPIO_Mode = GPIO_Mode_OUT,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_output_lcd_port_a);
    static GPIO_InitTypeDef const _gpio_init_output_lcd_port_b = {
            .GPIO_Pin = LCD_PINCFG_B,
            .GPIO_Mode = GPIO_Mode_OUT,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_output_lcd_port_b);
    GPIO_SetBits(LCD_CS_PORT, (1 << LCD_CS_PIN));

    static GPIO_InitTypeDef const _gpio_init_pin_tim_port_b = {
            .GPIO_Pin = TIM_LCD_PINCFG_B,
            .GPIO_Mode = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_port_b);
    static TIM_TimeBaseInitTypeDef const _tim_base_init = {
            .TIM_Prescaler = TIM_LCD_BL_PRESCALER - 1,
            .TIM_CounterMode = TIM_CounterMode_Up,
            .TIM_Period = TIM_LCD_BL_PERIOD,
            .TIM_ClockDivision = 0,
            .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM_LCD_BL, (TIM_TimeBaseInitTypeDef*)&_tim_base_init);
    static TIM_OCInitTypeDef const _tim_oc_init = {
            .TIM_OCMode = TIM_OCMode_Timing,
            .TIM_OutputState = TIM_OutputState_Disable,
            .TIM_OutputNState = TIM_OutputNState_Enable,
            .TIM_Pulse = TIM_LCD_BL_PERIOD,
            .TIM_OCPolarity = TIM_OCPolarity_High,
            .TIM_OCNPolarity = TIM_OCNPolarity_High,
            .TIM_OCIdleState = TIM_OCIdleState_Set,
            .TIM_OCNIdleState = TIM_OCNIdleState_Set
    };
    TIM_OC1Init(TIM_LCD_BL, (TIM_OCInitTypeDef*)&_tim_oc_init);
    TIM_OC1PreloadConfig(TIM_LCD_BL, TIM_OCPreload_Enable); TIM_SelectOCxM(TIM_LCD_BL, TIM_Channel_1, TIM_OCMode_PWM1); TIM_CCxCmd(TIM_LCD_BL, TIM_Channel_1, TIM_CCx_Enable);
    TIM_CtrlPWMOutputs(TIM_LCD_BL, ENABLE);
    TIM_SelectOnePulseMode(TIM_LCD_BL, TIM_OPMode_Repetitive);
    TIM_ITConfig(TIM_LCD_BL, TIM_IT_Update, ENABLE);
    NVIC_SetPriority(TIM_LCD_BL_IRQn, 0); NVIC_EnableIRQ(TIM_LCD_BL_IRQn);
    TIM_Cmd(TIM_LCD_BL, ENABLE);
}

static void Flash_Init() {
    static GPIO_InitTypeDef const _gpio_init_output_flash_port_a = {
        .GPIO_Pin = FLASH_PINCFG_A,
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_PuPd = GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_output_flash_port_a);
    GPIO_SetBits(FLASH_CS_PORT, (1 << FLASH_CS_PIN));
}

// Configure Buzzer
static void Buzzer_Config() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	GPIO_PinAFConfig(BUZZER_PORT, BUZZER_PIN, BUZZER_AF);
	static GPIO_InitTypeDef const _gpio_init_pin_tim_port_b = {
		.GPIO_Pin = BUZZER_PINCFG_B,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL
	};
	GPIO_Init(GPIOB, (GPIO_InitTypeDef*)&_gpio_init_pin_tim_port_b);
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

// Configure Sensors GPIO and ADC
static void Sensors_Config() {
    static GPIO_InitTypeDef const _gpio_init_input_sens_port_a = {
            .GPIO_Pin = SENS_PINCFG_A,
            .GPIO_Mode = GPIO_Mode_IN,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_DOWN,
    };
    GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_input_sens_port_a);
    static GPIO_InitTypeDef const _gpio_init_adc_sens_port_a = {
            .GPIO_Pin = SENS_ADC_PINCFG_A,
            .GPIO_Mode = GPIO_Mode_AN,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_NOPULL,
    };
    GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_adc_sens_port_a);
}

// Configure GPIO pins for switches
static void Switches_Config() {
    static GPIO_InitTypeDef const _gpio_init_output_sw_port_a = {
            .GPIO_Pin = VALVE_PINCFG_A,
            .GPIO_Mode = GPIO_Mode_OUT,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_output_sw_port_a);
}

// Configure GPIO pins for button
static void Buttons_Config() {
    static GPIO_InitTypeDef const _gpio_init_output_btn_port_a = {
            .GPIO_Pin = BTN_PINCFG_A,
            .GPIO_Mode = GPIO_Mode_IN,
            .GPIO_Speed = GPIO_Speed_2MHz,
            .GPIO_OType = GPIO_OType_PP,
            .GPIO_PuPd = GPIO_PuPd_UP
    };
    GPIO_Init(GPIOA, (GPIO_InitTypeDef*)&_gpio_init_output_btn_port_a);
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
	//IWDG_Config();
	memcpy(_sdata, _sidata, _edata - _sdata);
	memset(_sbss, 0, _ebss - _sbss);
	SYS_Config();
	GPIO_Config();
    CRC_Config();

    TIM_Delay_Config();
	USART_Dbg_Config();
    SPI_LCD_Flash_Init();
	LCD_Config();
    Flash_Init();
	Buzzer_Config();
    Sensors_Config();
    Switches_Config();
	Buttons_Config();
	extern void __libc_init_array(); __libc_init_array();

    Periph_Init();

	bool _is_abort_requested = false, _is_inited = false;

	__extension__ void yield_func() {  };
	EtaReverseOsmosisMainLoop(&_is_abort_requested, &_is_inited, yield_func);
	while(true);
}

void __attribute__((noreturn)) HardFault_Handler() {
	while (1);
}
