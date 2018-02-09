/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : interrupt.h
***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "eta_memman.h"
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
void __attribute__ ((weak)) MemManage_Handler();
#pragma weak MemManage_Handler = DefaultDummy_Handler
void __attribute__ ((weak)) BusFault_Handler();
#pragma weak BusFault_Handler = DefaultDummy_Handler
void __attribute__ ((weak)) UsageFault_Handler();
#pragma weak UsageFault_Handler = DefaultDummy_Handler
void __attribute__ ((weak)) SVC_Handler();
#pragma weak SVC_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) DebugMon_Handler();
#pragma weak DebugMon_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) PendSV_Handler();
#pragma weak PendSV_Handler = DefaultBlocking_Handler
void __attribute__ ((weak)) SysTick_Handler();
#pragma weak SysTick_Handler = DefaultBlocking_Handler

void __attribute__ ((weak)) WWDG_IRQHandler();
#pragma weak WWDG_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) PVD_IRQHandler();
#pragma weak PVD_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TAMPER_IRQHandler();
#pragma weak TAMPER_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) RTC_IRQHandler();
#pragma weak RTC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) FLASH_IRQHandler();
#pragma weak FLASH_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) RCC_IRQHandler();
#pragma weak RCC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI0_IRQHandler();
#pragma weak EXTI0_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI1_IRQHandler();
#pragma weak EXTI1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI2_IRQHandler();
#pragma weak EXTI2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI3_IRQHandler();
#pragma weak EXTI3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI4_IRQHandler();
#pragma weak EXTI4_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel1_IRQHandler();
#pragma weak DMA1_Channel1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel2_IRQHandler();
#pragma weak DMA1_Channel2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel3_IRQHandler();
#pragma weak DMA1_Channel3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel4_IRQHandler();
#pragma weak DMA1_Channel4_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel5_IRQHandler();
#pragma weak DMA1_Channel5_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel6_IRQHandler();
#pragma weak DMA1_Channel6_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA1_Channel7_IRQHandler();
#pragma weak DMA1_Channel7_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) ADC1_2_IRQHandler();
#pragma weak ADC1_2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USB_HP_CAN1_TX_IRQHandler();
#pragma weak USB_HP_CAN1_TX_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USB_LP_CAN1_RX0_IRQHandler();
#pragma weak USB_LP_CAN1_RX0_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) CAN1_RX1_IRQHandler();
#pragma weak CAN1_RX1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) CAN1_SCE_IRQHandler();
#pragma weak CAN1_SCE_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI9_5_IRQHandler();
#pragma weak EXTI9_5_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM1_BRK_IRQHandler();
#pragma weak TIM1_BRK_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM1_UP_IRQHandler();
#pragma weak TIM1_UP_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM1_TRG_COM_IRQHandler();
#pragma weak TIM1_TRG_COM_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM1_CC_IRQHandler();
#pragma weak TIM1_CC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM2_IRQHandler();
#pragma weak TIM2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM3_IRQHandler();
#pragma weak TIM3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM4_IRQHandler();
#pragma weak TIM4_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) I2C1_EV_IRQHandler();
#pragma weak I2C1_EV_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) I2C1_ER_IRQHandler();
#pragma weak I2C1_ER_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) I2C2_EV_IRQHandler();
#pragma weak I2C2_EV_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) I2C2_ER_IRQHandler();
#pragma weak I2C2_ER_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) SPI1_IRQHandler();
#pragma weak SPI1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) SPI2_IRQHandler();
#pragma weak SPI2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USART1_IRQHandler();
#pragma weak USART1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USART2_IRQHandler();
#pragma weak USART2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USART3_IRQHandler();
#pragma weak USART3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) EXTI15_10_IRQHandler();
#pragma weak EXTI15_10_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) RTCAlarm_IRQHandler();
#pragma weak RTCAlarm_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) USBWakeUp_IRQHandler();
#pragma weak USBWakeUp_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM8_BRK_IRQHandler();
#pragma weak TIM8_BRK_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM8_UP_IRQHandler();
#pragma weak TIM8_UP_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM8_TRG_COM_IRQHandler();
#pragma weak TIM8_TRG_COM_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM8_CC_IRQHandler();
#pragma weak TIM8_CC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) ADC3_IRQHandler();
#pragma weak ADC3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) FSMC_IRQHandler();
#pragma weak FSMC_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) SDIO_IRQHandler();
#pragma weak SDIO_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM5_IRQHandler();
#pragma weak TIM5_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) SPI3_IRQHandler();
#pragma weak SPI3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) UART4_IRQHandler();
#pragma weak UART4_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) UART5_IRQHandler();
#pragma weak UART5_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM6_IRQHandler();
#pragma weak TIM6_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) TIM7_IRQHandler();
#pragma weak TIM7_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA2_Channel1_IRQHandler();
#pragma weak DMA2_Channel1_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA2_Channel2_IRQHandler();
#pragma weak DMA2_Channel2_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA2_Channel3_IRQHandler();
#pragma weak DMA2_Channel3_IRQHandler = DefaultBlocking_Handler
void __attribute__ ((weak)) DMA2_Channel4_5_IRQHandler();
#pragma weak DMA2_Channel4_5_IRQHandler = DefaultBlocking_Handler

__attribute__ ((section(".isr_vectors"))) __attribute__ ((used))
DInterruptHandler a_fn_vectors[] = {
	__extension__ (DInterruptHandler)_estack,	/* The stack pointer after relocation */
	Reset_Handler,                              /* Reset Handler */
	NMI_Handler,                                /* NMI Handler */
	HardFault_Handler,                          /* Hard Fault Handler */
	MemManage_Handler,                          /* MPU Fault Handler */
	BusFault_Handler,                           /* Bus Fault Handler */
	UsageFault_Handler,                         /* Usage Fault Handler */
	0,                                          /* Reserved */
	0,                                          /* Reserved */
	0,                                          /* Reserved */
	0,                                          /* Reserved */
	SVC_Handler,                                /* SVCall Handler */
	DebugMon_Handler,                           /* Debug Monitor Handler */
	0,                                          /* Reserved */
	PendSV_Handler,                             /* PendSV Handler */
	SysTick_Handler,                            /* SysTick Handler */

    /* External Interrupts */
    WWDG_IRQHandler,            /* Window Watchdog */
    PVD_IRQHandler,             /* PVD through EXTI Line detect */
    TAMPER_IRQHandler,          /* Tamper */
    RTC_IRQHandler,             /* RTC */
    FLASH_IRQHandler,           /* Flash */
    RCC_IRQHandler,             /* RCC */
    EXTI0_IRQHandler,           /* EXTI Line 0 */
    EXTI1_IRQHandler,           /* EXTI Line 1 */
    EXTI2_IRQHandler,           /* EXTI Line 2 */
    EXTI3_IRQHandler,           /* EXTI Line 3 */
    EXTI4_IRQHandler,           /* EXTI Line 4 */
    DMA1_Channel1_IRQHandler,   /* DMA1 Channel 1 */
    DMA1_Channel2_IRQHandler,   /* DMA1 Channel 2 */
    DMA1_Channel3_IRQHandler,   /* DMA1 Channel 3 */
    DMA1_Channel4_IRQHandler,   /* DMA1 Channel 4 */
    DMA1_Channel5_IRQHandler,   /* DMA1 Channel 5 */
    DMA1_Channel6_IRQHandler,   /* DMA1 Channel 6 */
    DMA1_Channel7_IRQHandler,   /* DMA1 Channel 7 */
    ADC1_2_IRQHandler,          /* ADC1 & ADC2 */
    USB_HP_CAN1_TX_IRQHandler,  /* USB High Priority or CAN1 TX */
    USB_LP_CAN1_RX0_IRQHandler, /* USB Low  Priority or CAN1 RX0 */
    CAN1_RX1_IRQHandler,        /* CAN1 RX1 */
    CAN1_SCE_IRQHandler,        /* CAN1 SCE */
    EXTI9_5_IRQHandler,         /* EXTI Line 9..5 */
    TIM1_BRK_IRQHandler,        /* TIM1 Break */
    TIM1_UP_IRQHandler,         /* TIM1 Update */
    TIM1_TRG_COM_IRQHandler,    /* TIM1 Trigger and Commutation */
    TIM1_CC_IRQHandler,         /* TIM1 Capture Compare */
    TIM2_IRQHandler,            /* TIM2 */
    TIM3_IRQHandler,            /* TIM3 */
    TIM4_IRQHandler,            /* TIM4 */
    I2C1_EV_IRQHandler,         /* I2C1 Event */
    I2C1_ER_IRQHandler,         /* I2C1 Error */
    I2C2_EV_IRQHandler,         /* I2C2 Event */
    I2C2_ER_IRQHandler,         /* I2C2 Error */
    SPI1_IRQHandler,            /* SPI1 */
    SPI2_IRQHandler,            /* SPI2 */
    USART1_IRQHandler,          /* USART1 */
    USART2_IRQHandler,          /* USART2 */
    USART3_IRQHandler,          /* USART3 */
    EXTI15_10_IRQHandler,       /* EXTI Line 15..10 */
    RTCAlarm_IRQHandler,        /* RTC Alarm through EXTI Line */
    USBWakeUp_IRQHandler,       /* USB Wakeup from suspend */
    TIM8_BRK_IRQHandler,		/*  */
    TIM8_UP_IRQHandler,			/*  */
    TIM8_TRG_COM_IRQHandler,	/*  */
    TIM8_CC_IRQHandler,			/*  */
    ADC3_IRQHandler,			/*  */
    FSMC_IRQHandler,			/*  */
    SDIO_IRQHandler,			/*  */
    TIM5_IRQHandler,			/*  */
    SPI3_IRQHandler,			/*  */
    UART4_IRQHandler,			/*  */
    UART5_IRQHandler,			/*  */
    TIM6_IRQHandler,			/*  */
    TIM7_IRQHandler,			/*  */
    DMA2_Channel1_IRQHandler,	/*  */
    DMA2_Channel2_IRQHandler,	/*  */
    DMA2_Channel3_IRQHandler,	/*  */
    DMA2_Channel4_5_IRQHandler,	/*  */
};
