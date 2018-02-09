/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : interrupt.h
***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "eta_dbg.h"

extern uint8_t _estack[];

typedef void(*const DInterruptHandler)();

void __attribute__ ((naked, noreturn)) DefaultBlocking_Handler() { DBG_PutString(" <<< Blocking >>> "NL); while (true); }
void __attribute__ ((naked)) DefaultDummy_Handler() {  }

void Reset_Handler();

void __attribute__ ((weak)) NMI_Handler();
#pragma weak MMI_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) HardFault_Handler();
#pragma weak HardFault_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) SVC_Handler();
#pragma weak SVC_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) PendSV_Handler();
#pragma weak PendSV_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) SysTick_Handler();
#pragma weak SysTick_Handler = DefaultBlocking_Handler

void __attribute__ ((weak)) WWDG_IRQHandler();
#pragma weak WWDG_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) RTC_IRQHandler();
#pragma weak RTC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) FLASH_IRQHandler();
#pragma weak FLASH_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) RCC_IRQHandler();
#pragma weak RCC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI0_1_IRQHandler();
#pragma weak EXTI0_1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI2_3_IRQHandler();
#pragma weak EXTI2_3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI4_15_IRQHandler();
#pragma weak EXTI4_15_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel1_IRQHandler();
#pragma weak DMA1_Channel1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel2_3_IRQHandler();
#pragma weak DMA1_Channel2_3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel4_5_IRQHandler();
#pragma weak DMA1_Channel4_5_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) ADC1_IRQHandler();
#pragma weak ADC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM1_BRK_UP_TRG_COM_IRQHandler();
#pragma weak TIM1_BRK_UP_TRG_COM_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM1_CC_IRQHandler();
#pragma weak TIM1_CC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM3_IRQHandler();
#pragma weak TIM3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM6_IRQHandler();
#pragma weak TIM6_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM14_IRQHandler();
#pragma weak TIM14_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM15_IRQHandler();
#pragma weak TIM15_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM16_IRQHandler();
#pragma weak TIM16_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM17_IRQHandler();
#pragma weak TIM17_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) I2C1_IRQHandler();
#pragma weak I2C1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) SPI1_IRQHandler();
#pragma weak SPI1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USART1_IRQHandler();
#pragma weak USART1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USART2_IRQHandler();
#pragma weak USART2_IRQHandler = DefaultBlocking_Handler

__attribute__ ((section(".isr_vectors"))) __attribute__ ((used))
DInterruptHandler a_fn_vectors[] = {
	__extension__ (DInterruptHandler)_estack,	/* The stack pointer after relocation */
	Reset_Handler,                              /* Reset Handler */
	NMI_Handler,                                /* NMI Handler */
	HardFault_Handler,                          /* Hard Fault Handler */
	0,                         					/* Reserved */
	0,                           				/* Reserved */
	0,               	         				/* Reserved */
	0,                                          /* Reserved */
	0,                                          /* Reserved */
	0,                                          /* Reserved */
	0,                                          /* Reserved */
	SVC_Handler,                                /* SVCall Handler */
	0,                           				/* Reserved */
	0,                                          /* Reserved */
	PendSV_Handler,                             /* PendSV Handler */
	SysTick_Handler,                            /* SysTick Handler */

    /* External Interrupts */
	WWDG_IRQHandler,            		/*  0 Window Watchdog */
	0,             						/*  1 Reserved */
	RTC_IRQHandler,             		/*  2 RTC */
	FLASH_IRQHandler,           		/*  3 Flash */
	RCC_IRQHandler,             		/*  4 RCC */
	EXTI0_1_IRQHandler,         		/*  5 EXTI Line 0 and 1 */
	EXTI2_3_IRQHandler,         		/*  6 EXTI Line 2 and 3 */
	EXTI4_15_IRQHandler,        		/*  7 EXTI Line 4 to 15 */
	0,           						/*  8 Reserved */
	DMA1_Channel1_IRQHandler,   		/*  9 DMA1 Channel 1 */
	DMA1_Channel2_3_IRQHandler, 		/* 10 DMA1 Channel 2 and 3 */
	DMA1_Channel4_5_IRQHandler, 		/* 11 DMA1 Channel 4 and 5 */
	ADC1_IRQHandler,          			/* 12 ADC1 */
	TIM1_BRK_UP_TRG_COM_IRQHandler,     /* 13 TIM1 Break, Update, Trigger and Commutation */
    TIM1_CC_IRQHandler,         		/* 14 TIM1 Capture Compare */
	0,            						/* 15 Reserved */
    TIM3_IRQHandler,            		/* 16 TIM3 */
    TIM6_IRQHandler,         			/* 17 TIM6 */
    0,         							/* 18 Reserved */
	TIM14_IRQHandler,         			/* 19 TIM14 */
	TIM15_IRQHandler,         			/* 20 TIM15 */
	TIM16_IRQHandler,         			/* 21 TIM16 */
	TIM17_IRQHandler,         			/* 22 TIM17 */
	I2C1_IRQHandler,         			/* 23 I2C1 */
    0,         							/* 24 Reserved */
    SPI1_IRQHandler,            		/* 25 SPI1 */
    0,         							/* 26 Reserved */
    USART1_IRQHandler,          		/* 27 USART1 */
    USART2_IRQHandler,          		/* 28 USART2 */
    0,         							/* 29 Reserved */
    0,         							/* 30 Reserved */
    0,         							/* 31 Reserved */
};
