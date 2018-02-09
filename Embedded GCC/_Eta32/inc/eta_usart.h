/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_usart.h
 ***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// === PORT 0 ===
#if defined USART0_PERIPH && defined USART0_IRQ && (USART0_RX_BUFFER_SIZESHIFT > 0 || USART0_TX_BUFFER_SIZESHIFT > 0)
#define USART_PORTS 1
#if USART0_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART0_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 1 ===
#if defined USART1_PERIPH && defined USART1_IRQ && (USART1_RX_BUFFER_SIZESHIFT > 0 || USART1_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 2
#if USART1_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART1_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 2 ===
#if defined USART2_PERIPH && defined USART2_IRQ && (USART2_RX_BUFFER_SIZESHIFT > 0 || USART2_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 3
#if USART2_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART2_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 3 ===
#if defined USART3_PERIPH && defined USART3_IRQ && (USART3_RX_BUFFER_SIZESHIFT > 0 || USART3_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 4
#if USART3_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART3_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 4 ===
#if defined USART4_PERIPH && defined USART4_IRQ && (USART4_RX_BUFFER_SIZESHIFT > 0 || USART4_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 5
#if USART4_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART4_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 5 ===
#if defined USART5_PERIPH && defined USART5_IRQ && (USART5_RX_BUFFER_SIZESHIFT > 0 || USART5_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 6
#if USART5_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART5_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 6 ===
#if defined USART6_PERIPH && defined USART6_IRQ && (USART6_RX_BUFFER_SIZESHIFT > 0 || USART6_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 7
#if USART6_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART6_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

// === PORT 7 ===
#if defined USART7_PERIPH && defined USART7_IRQ && (USART7_RX_BUFFER_SIZESHIFT > 0 || USART7_TX_BUFFER_SIZESHIFT > 0)
#undef USART_PORTS
#define USART_PORTS 8
#if USART7_RX_BUFFER_SIZESHIFT > 0
#define USART_HAS_RECEIVER
#endif
#if USART7_TX_BUFFER_SIZESHIFT > 0
#define USART_HAS_TRANSMITTER
#endif

#endif /* defined USART7_PERIPH && defined USART7_IRQ && (USART7_RX_BUFFER_SIZESHIFT > 0 || USART7_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART6_PERIPH && defined USART6_IRQ && (USART6_RX_BUFFER_SIZESHIFT > 0 || USART6_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART5_PERIPH && defined USART5_IRQ && (USART5_RX_BUFFER_SIZESHIFT > 0 || USART5_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART4_PERIPH && defined USART4_IRQ && (USART4_RX_BUFFER_SIZESHIFT > 0 || USART4_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART3_PERIPH && defined USART3_IRQ && (USART3_RX_BUFFER_SIZESHIFT > 0 || USART3_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART2_PERIPH && defined USART2_IRQ && (USART2_RX_BUFFER_SIZESHIFT > 0 || USART2_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART1_PERIPH && defined USART1_IRQ && (USART1_RX_BUFFER_SIZESHIFT > 0 || USART1_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART0_PERIPH && defined USART0_IRQ && (USART0_RX_BUFFER_SIZESHIFT > 0 || USART0_TX_BUFFER_SIZESHIFT > 0) */

typedef enum {
	USART_RX_BUFFER_ERROR = 0x1,
	USART_RX_BUFFER_OVERFLOW = 0x2,
} eUSART_Flags;

#if FRAMES_PORTS > 1
#define USART_portdecllast USART_portdecllast
#define USART_portdecl USART_portdecllast,
#define USART_portassert
#else
#define USART_portdecllast
#define USART_portdecl
#define USART_portassert
#endif

#if defined USART_HAS_RECEIVER || defined USART_HAS_TRANSMITTER

void USART_SetFlagsFromISR(USART_portdecl eUSART_Flags flags);

bool USART_CheckAndResetFlags(USART_portdecl eUSART_Flags flags);

#endif /* defined USART_HAS_RECEIVER || defined USART_HAS_TRANSMITTER */

#ifdef USART_HAS_RECEIVER

// Put a character to the USART Receiver buffer from interrupt service routine
void USART_PushCharFromISR(USART_portdecl char c);

// Get a character from the USART Receiver buffer
char USART_GetChar(USART_portdecllast);

// Gets a value indicating whether there is a ready character in the USART Receiver buffer
bool USART_IsCharAvailable(USART_portdecllast);

#endif /* USART_HAS_RECEIVER */

#ifdef USART_HAS_TRANSMITTER

// Get a character from the USART Transmitter buffer from interrupt service routine
bool USART_PopCharFromISR(USART_portdecl char *c);

// Put a character to the USART Transmitter buffer
void USART_PutChar(USART_portdecl char c);

// Write a block to the USART Transmitter buffer
void USART_PutBlock(USART_portdecl void const *block, size_t len);

// Waits until all date gone from Transmitter buffer
void USART_Flush(USART_portdecllast);

#endif /* USART_HAS_TRANSMITTER */

#ifdef __cplusplus
}
#endif
