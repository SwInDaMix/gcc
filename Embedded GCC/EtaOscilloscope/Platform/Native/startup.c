/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : Interrupt.h
***********************************************************************/

#include "config.h"
#include "eta_memman.h"
#include "eta_dbg.h"
#include "stm32f10x.h"
#include "Interrupt.h"
#include "IntBIOS.h"
#include "CPeriph.h"
#include "CConverter.h"

typedef void(*const DInterruptHandler)();

void *__dso_handle __attribute__ ((weak));
#pragma weak __dso_handle

extern uint8_t _sidata[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern uint8_t _sbss[];
extern uint8_t _ebss[];
extern uint8_t _estack[];
extern uint8_t _sheap[];
extern uint8_t _eheap[];
extern uint8_t _vectors[];

void DBG_Init() {
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
		.USART_BaudRate = DBG_USART_BAUDRATE,
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

	DBG_PutString("\n\n\n\n");
}

void _init() {

}

vu32 vu32Tick;
vu8 Cursor_Cnt, Key_Wait_Cnt, Key_Repeat_Cnt, Key_Buffer, Cnt_mS, Cnt_20mS;
vu8 Twink, Blink;
unsigned char Volume = 20, Light;
vu16 Delay_Cnt, Beep_mS, Key_Status_Last, Sec_Cnt, PD_Cnt;
vu32 Wait_Cnt;

static char _fault_linebuf[64];
static int _fault_pos;
static int _fault_y;
static void **_fault_sp;

static void _fault_putc(char c) {
	if (c == '\n' || _fault_pos == 63) {
		_fault_linebuf[_fault_pos] = 0;
		__Display_Str(10, _fault_y, 0xFFFF, 0, _fault_linebuf);
		_fault_pos = 0;
		_fault_y -= 14;
	}
	else {
		_fault_linebuf[_fault_pos++] = c;
	}
	DBG_PutChar(c);
}
static char _fault_hex_nibble(uint32_t n) {
	if (n < 10) {
		return '0' + n;
	}
	else {
		return 'A' - 10 + n;
	}
}
static void _fault_putstring(char const *p) {
	while (*p) {
		_fault_putc(*p++);
	}
}
static void _fault_puthex(char const *p, uint32_t count) {
	while (count--) {
		_fault_putc(_fault_hex_nibble(*(p + count) >> 4));
		_fault_putc(_fault_hex_nibble(*(p + count) & 0x0F));
	}
}

void HardFault_Handler() {
	__asm("mrs %0, msp" : "=r"(_fault_sp) : :);

	_fault_pos = 0; _fault_y = 220;
	__LCD_Initial();
	__Set(BEEP_VOLUME, 0);

	__Point_SCR(0, 0);
	unsigned short _color = 0x001F;
	__LCD_Fill(&_color, 240 * 200); __LCD_DMA_Ready();
	__LCD_Fill(&_color, 240 * 200); __LCD_DMA_Ready();

	_fault_putstring("                   HARDFAULT                 ");

	_fault_putstring("\nSP: ");
	_fault_puthex((char *) &_fault_sp, sizeof(void *));
	_fault_putstring("  PC: ");
	_fault_puthex((char *) (_fault_sp + 6), sizeof(void *));
	_fault_putstring("  LR: ");
	_fault_puthex((char *) (_fault_sp + 5), sizeof(void *));
	_fault_putstring("\nRegs: r0 ");
	_fault_puthex((char *) (_fault_sp + 0), sizeof(void *));
	_fault_putstring(", r1 ");
	_fault_puthex((char *) (_fault_sp + 1), sizeof(void *));
	_fault_putstring("\n      r2 ");
	_fault_puthex((char *) (_fault_sp + 2), sizeof(void *));
	_fault_putstring(", r3 ");
	_fault_puthex((char *) (_fault_sp + 3), sizeof(void *));
	_fault_putstring(", r12 ");
	_fault_puthex((char *) (_fault_sp + 4), sizeof(void *));

	_fault_putstring("\nSCB HFSR: ");
	_fault_puthex((char *) &(SCB->HFSR), sizeof(SCB->HFSR));

	_fault_putstring("\nSCB CFSR: ");
	_fault_puthex((char *) &(SCB->CFSR), sizeof(SCB->CFSR));

	_fault_putstring("\nStack dump:\n");
	for (int i = 0; i < 32; i++) {
		_fault_puthex((char *) (_fault_sp + i), sizeof(void *));
		if (i % 4 == 3) _fault_putstring("\n");
		else _fault_putstring(" ");
	}

	while (1);
}
void USB_HP_CAN1_TX_IRQHandler() { __CTR_HP(); }
void USB_LP_CAN1_RX0_IRQHandler() { __USB_Istr(); }
void TIM3_IRQHandler() {
//  static char tmp[2] = {'A', 0};
	vu32Tick++;
	__Set(KEY_IF_RST, 0);                      //Clear TIM3 interrupt flag
	if (Cnt_mS > 0) Cnt_mS--;
	else {                                     //Read keys per 20mS
		Cnt_mS = 20;
		Periph_TIM_20ms();
		Periph_KBD_20ms();
		if (Wait_Cnt > 0) Wait_Cnt--;
		if (Delay_Cnt > 20) Delay_Cnt -= 20;
		else Delay_Cnt = 0;
//		if (Beep_mS >= 20) Beep_mS -= 20;
//		else __Set(BEEP_VOLUME, 0); // Beep off
		if (Cnt_20mS < 50) Cnt_20mS++;
		else {                                   // Do it pre sec.
			Cnt_20mS = 0;
			__Set(BETTERY_DT, 1);                  //Battery Detect
			Sec_Cnt++;
			if (PD_Cnt > 0) PD_Cnt--;
		}
		Cursor_Cnt++;
		if (Cursor_Cnt >= 12) {                   //12*20mS=240mS
//	  __Display_Str(0, 0, 0x8888, 0, tmp);
//	  if (++tmp[0] > 'Z')
//		tmp[0] = 'A';

			Cursor_Cnt = 0;
			Twink = !Twink;
			Blink = 1;
		}
		if (Key_Wait_Cnt) Key_Wait_Cnt--;
		if (Key_Repeat_Cnt) Key_Repeat_Cnt--;

	}
}

void USART1_IRQHandler() {
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
		USART_PushCharFromISR((char)USART_ReceiveData(USART1));
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET) {
		char _c;
		if(USART_PopCharFromISR(&_c)) USART_SendData(USART1, _c);
		else USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}
}

void __attribute__ ((noreturn)) __attribute__ ((used)) Reset_Handler() {
	memcpy(_sdata, _sidata, _edata - _sdata);
	memset(_sbss, 0, _ebss - _sbss);
	__Set(BEEP_VOLUME, 0);
	DBG_Init(); Periph_Init(); Converter_Init();
	memman_initialize(_sheap, (uint16_t)(_eheap - _sheap));
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)_vectors);
	__USB_Init();
	extern void __libc_init_array(); __libc_init_array();

	bool _is_abort_requested = false, _is_inited = false;
	__extension__ void _yield() {  };
	extern void EtaOsciloscopeMainLoop(bool *is_abort_requested, volatile bool *is_inited, void(*yield)());
	EtaOsciloscopeMainLoop(&_is_abort_requested, &_is_inited, _yield);

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
