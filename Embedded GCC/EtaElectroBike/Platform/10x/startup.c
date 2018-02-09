/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Interrupt.h
***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "eta_memman.h"
#include "eta_usart.h"
#include "eta_dbg.h"

void *__dso_handle __attribute__ ((weak));
#pragma weak __dso_handle

extern uint8_t _sidata[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern uint8_t _sbss[];
extern uint8_t _ebss[];
extern uint8_t _sheap[];
extern uint8_t _eheap[];
extern uint8_t _vectors[];

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

// Inits interrupt-driven USART port
void USART_Dbg_Config() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef _gpio_init_9 = {
		.GPIO_Pin = GPIO_Pin_9,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_Mode = GPIO_Mode_AF_PP,
	};
	GPIO_InitTypeDef _gpio_init_10 = {
		.GPIO_Pin = GPIO_Pin_10,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_Mode = GPIO_Mode_IN_FLOATING,
	};
	USART_InitTypeDef _usart_init = {
		.USART_BaudRate = 115200,
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
	};

	GPIO_Init(GPIOA, &_gpio_init_9);
	GPIO_Init(GPIOA, &_gpio_init_10);
	USART_Init(USART1, &_usart_init);
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 14);
}

void __attribute__ ((noreturn)) Reset_Handler() {
	memcpy(_sdata, _sidata, _edata - _sdata);
	memset(_sbss, 0, _ebss - _sbss);
	USART_Dbg_Config();
	memman_initialize(_sheap, (uint16_t)(_eheap - _sheap));
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)_vectors);
	extern void __libc_init_array(); __libc_init_array();

	while(1);
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
