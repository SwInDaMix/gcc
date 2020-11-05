/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_usart.c
 ***********************************************************************/

#include "stdperiph.h"
#include "eta_usart.h"

// === PORT 0 ===
#if defined USART0_PERIPH && defined USART0_IRQ && (USART0_RX_BUFFER_SIZESHIFT > 0 || USART0_TX_BUFFER_SIZESHIFT > 0)
#if USART0_RX_BUFFER_SIZESHIFT > 0
#define USART0_RX_BUFFER_SIZE (1 << USART0_RX_BUFFER_SIZESHIFT)
static char USART0_RxBuffer[USART0_RX_BUFFER_SIZE];
#else
#define USART0_RX_BUFFER_SIZE 0
#define USART0_RxBuffer nullptr
#endif
#if USART0_TX_BUFFER_SIZESHIFT > 0
#define USART0_TX_BUFFER_SIZE (1 << USART0_TX_BUFFER_SIZESHIFT)
static char USART0_TxBuffer[USART0_TX_BUFFER_SIZE];
#else
#define USART0_TX_BUFFER_SIZE 0
#define USART0_TxBuffer nullptr
#endif
#define USART_PERIPHS USART0_PERIPH
#define USART_IRQS USART0_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer

// === PORT 1 ===
#if defined USART1_PERIPH && defined USART1_IRQ && (USART1_RX_BUFFER_SIZESHIFT > 0 || USART1_TX_BUFFER_SIZESHIFT > 0)
#if USART1_RX_BUFFER_SIZESHIFT > 0
#define USART1_RX_BUFFER_SIZE (1 << USART1_RX_BUFFER_SIZESHIFT)
static char USART1_RxBuffer[USART1_RX_BUFFER_SIZE];
#else
#define USART1_RX_BUFFER_SIZE 0
#define USART1_RxBuffer nullptr
#endif
#if USART1_TX_BUFFER_SIZESHIFT > 0
#define USART1_TX_BUFFER_SIZE (1 << USART1_TX_BUFFER_SIZESHIFT)
static char USART1_TxBuffer[USART1_TX_BUFFER_SIZE];
#else
#define USART1_TX_BUFFER_SIZE 0
#define USART1_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer

// === PORT 2 ===
#if defined USART2_PERIPH && defined USART2_IRQ && (USART2_RX_BUFFER_SIZESHIFT > 0 || USART2_TX_BUFFER_SIZESHIFT > 0)
#if USART2_RX_BUFFER_SIZESHIFT > 0
#define USART2_RX_BUFFER_SIZE (1 << USART2_RX_BUFFER_SIZESHIFT)
static char USART2_RxBuffer[USART2_RX_BUFFER_SIZE];
#else
#define USART2_RX_BUFFER_SIZE 0
#define USART2_RxBuffer nullptr
#endif
#if USART2_TX_BUFFER_SIZESHIFT > 0
#define USART2_TX_BUFFER_SIZE (1 << USART2_TX_BUFFER_SIZESHIFT)
static char USART2_TxBuffer[USART2_TX_BUFFER_SIZE];
#else
#define USART2_TX_BUFFER_SIZE 0
#define USART2_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH, USART2_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ, USART2_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE, USART2_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE, USART2_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer, USART2_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer, USART2_TxBuffer

// === PORT 3 ===
#if defined USART3_PERIPH && defined USART3_IRQ && (USART3_RX_BUFFER_SIZESHIFT > 0 || USART3_TX_BUFFER_SIZESHIFT > 0)
#if USART3_RX_BUFFER_SIZESHIFT > 0
#define USART3_RX_BUFFER_SIZE (1 << USART3_RX_BUFFER_SIZESHIFT)
static char USART3_RxBuffer[USART3_RX_BUFFER_SIZE];
#else
#define USART3_RX_BUFFER_SIZE 0
#define USART3_RxBuffer nullptr
#endif
#if USART3_TX_BUFFER_SIZESHIFT > 0
#define USART3_TX_BUFFER_SIZE (1 << USART3_TX_BUFFER_SIZESHIFT)
static char USART3_TxBuffer[USART3_TX_BUFFER_SIZE];
#else
#define USART3_TX_BUFFER_SIZE 0
#define USART3_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH, USART2_PERIPH, USART3_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ, USART2_IRQ, USART3_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE, USART2_RX_BUFFER_SIZE, USART3_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE, USART2_TX_BUFFER_SIZE, USART3_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer, USART2_RxBuffer, USART3_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer, USART2_TxBuffer, USART3_TxBuffer

// === PORT 4 ===
#if defined USART4_PERIPH && defined USART4_IRQ && (USART4_RX_BUFFER_SIZESHIFT > 0 || USART4_TX_BUFFER_SIZESHIFT > 0)
#if USART4_RX_BUFFER_SIZESHIFT > 0
#define USART4_RX_BUFFER_SIZE (1 << USART4_RX_BUFFER_SIZESHIFT)
static char USART4_RxBuffer[USART4_RX_BUFFER_SIZE];
#else
#define USART4_RX_BUFFER_SIZE 0
#define USART4_RxBuffer nullptr
#endif
#if USART4_TX_BUFFER_SIZESHIFT > 0
#define USART4_TX_BUFFER_SIZE (1 << USART4_TX_BUFFER_SIZESHIFT)
static char USART4_TxBuffer[USART4_TX_BUFFER_SIZE];
#else
#define USART4_TX_BUFFER_SIZE 0
#define USART4_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH, USART2_PERIPH, USART3_PERIPH, USART4_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ, USART2_IRQ, USART3_IRQ, USART4_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE, USART2_RX_BUFFER_SIZE, USART3_RX_BUFFER_SIZE, USART4_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE, USART2_TX_BUFFER_SIZE, USART3_TX_BUFFER_SIZE, USART4_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer, USART2_RxBuffer, USART3_RxBuffer, USART4_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer, USART2_TxBuffer, USART3_TxBuffer, USART4_TxBuffer

// === PORT 5 ===
#if defined USART5_PERIPH && defined USART5_IRQ && (USART5_RX_BUFFER_SIZESHIFT > 0 || USART5_TX_BUFFER_SIZESHIFT > 0)
#if USART5_RX_BUFFER_SIZESHIFT > 0
#define USART5_RX_BUFFER_SIZE (1 << USART5_RX_BUFFER_SIZESHIFT)
static char USART5_RxBuffer[USART5_RX_BUFFER_SIZE];
#else
#define USART5_RX_BUFFER_SIZE 0
#define USART5_RxBuffer nullptr
#endif
#if USART5_TX_BUFFER_SIZESHIFT > 0
#define USART5_TX_BUFFER_SIZE (1 << USART5_TX_BUFFER_SIZESHIFT)
static char USART5_TxBuffer[USART5_TX_BUFFER_SIZE];
#else
#define USART5_TX_BUFFER_SIZE 0
#define USART5_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH, USART2_PERIPH, USART3_PERIPH, USART4_PERIPH, USART5_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ, USART2_IRQ, USART3_IRQ, USART4_IRQ, USART5_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE, USART2_RX_BUFFER_SIZE, USART3_RX_BUFFER_SIZE, USART4_RX_BUFFER_SIZE, USART5_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE, USART2_TX_BUFFER_SIZE, USART3_TX_BUFFER_SIZE, USART4_TX_BUFFER_SIZE, USART5_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer, USART2_RxBuffer, USART3_RxBuffer, USART4_RxBuffer, USART5_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer, USART2_TxBuffer, USART3_TxBuffer, USART4_TxBuffer, USART5_TxBuffer

// === PORT 6 ===
#if defined USART6_PERIPH && defined USART6_IRQ && (USART6_RX_BUFFER_SIZESHIFT > 0 || USART6_TX_BUFFER_SIZESHIFT > 0)
#if USART6_RX_BUFFER_SIZESHIFT > 0
#define USART6_RX_BUFFER_SIZE (1 << USART6_RX_BUFFER_SIZESHIFT)
static char USART6_RxBuffer[USART6_RX_BUFFER_SIZE];
#else
#define USART6_RX_BUFFER_SIZE 0
#define USART6_RxBuffer nullptr
#endif
#if USART6_TX_BUFFER_SIZESHIFT > 0
#define USART6_TX_BUFFER_SIZE (1 << USART6_TX_BUFFER_SIZESHIFT)
static char USART6_TxBuffer[USART6_TX_BUFFER_SIZE];
#else
#define USART6_TX_BUFFER_SIZE 0
#define USART6_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH, USART2_PERIPH, USART3_PERIPH, USART4_PERIPH, USART5_PERIPH, USART6_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ, USART2_IRQ, USART3_IRQ, USART4_IRQ, USART5_IRQ, USART6_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE, USART2_RX_BUFFER_SIZE, USART3_RX_BUFFER_SIZE, USART4_RX_BUFFER_SIZE, USART5_RX_BUFFER_SIZE, USART6_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE, USART2_TX_BUFFER_SIZE, USART3_TX_BUFFER_SIZE, USART4_TX_BUFFER_SIZE, USART5_TX_BUFFER_SIZE, USART6_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer, USART2_RxBuffer, USART3_RxBuffer, USART4_RxBuffer, USART5_RxBuffer, USART6_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer, USART2_TxBuffer, USART3_TxBuffer, USART4_TxBuffer, USART5_TxBuffer, USART6_TxBuffer

// === PORT 7 ===
#if defined USART7_PERIPH && defined USART7_IRQ && (USART7_RX_BUFFER_SIZESHIFT > 0 || USART7_TX_BUFFER_SIZESHIFT > 0)
#if USART7_RX_BUFFER_SIZESHIFT > 0
#define USART7_RX_BUFFER_SIZE (1 << USART7_RX_BUFFER_SIZESHIFT)
static char USART7_RxBuffer[USART7_RX_BUFFER_SIZE];
#else
#define USART7_RX_BUFFER_SIZE 0
#define USART7_RxBuffer nullptr
#endif
#if USART7_TX_BUFFER_SIZESHIFT > 0
#define USART7_TX_BUFFER_SIZE (1 << USART7_TX_BUFFER_SIZESHIFT)
static char USART7_TxBuffer[USART7_TX_BUFFER_SIZE];
#else
#define USART7_TX_BUFFER_SIZE 0
#define USART7_TxBuffer nullptr
#endif
#undef USART_PERIPHS
#undef USART_IRQS
#undef USART_RX_SIZES
#undef USART_TX_SIZES
#undef USART_RX_BUFFERS
#undef USART_TX_BUFFERS
#define USART_PERIPHS USART0_PERIPH, USART1_PERIPH, USART2_PERIPH, USART3_PERIPH, USART4_PERIPH, USART5_PERIPH, USART6_PERIPH, USART7_PERIPH
#define USART_IRQS USART0_IRQ, USART1_IRQ, USART2_IRQ, USART3_IRQ, USART4_IRQ, USART5_IRQ, USART6_IRQ, USART7_IRQ
#define USART_RX_SIZES USART0_RX_BUFFER_SIZE, USART1_RX_BUFFER_SIZE, USART2_RX_BUFFER_SIZE, USART3_RX_BUFFER_SIZE, USART4_RX_BUFFER_SIZE, USART5_RX_BUFFER_SIZE, USART6_RX_BUFFER_SIZE, USART7_RX_BUFFER_SIZE
#define USART_TX_SIZES USART0_TX_BUFFER_SIZE, USART1_TX_BUFFER_SIZE, USART2_TX_BUFFER_SIZE, USART3_TX_BUFFER_SIZE, USART4_TX_BUFFER_SIZE, USART5_TX_BUFFER_SIZE, USART6_TX_BUFFER_SIZE, USART7_TX_BUFFER_SIZE
#define USART_RX_BUFFERS USART0_RxBuffer, USART1_RxBuffer, USART2_RxBuffer, USART3_RxBuffer, USART4_RxBuffer, USART5_RxBuffer, USART6_RxBuffer, USART7_RxBuffer
#define USART_TX_BUFFERS USART0_TxBuffer, USART1_TxBuffer, USART2_TxBuffer, USART3_TxBuffer, USART4_TxBuffer, USART5_TxBuffer, USART6_TxBuffer, USART7_TxBuffer

#endif /* defined USART7_PERIPH && defined USART7_IRQ && (USART7_RX_BUFFER_SIZESHIFT > 0 || USART7_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART6_PERIPH && defined USART6_IRQ && (USART6_RX_BUFFER_SIZESHIFT > 0 || USART6_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART5_PERIPH && defined USART5_IRQ && (USART5_RX_BUFFER_SIZESHIFT > 0 || USART5_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART4_PERIPH && defined USART4_IRQ && (USART4_RX_BUFFER_SIZESHIFT > 0 || USART4_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART3_PERIPH && defined USART3_IRQ && (USART3_RX_BUFFER_SIZESHIFT > 0 || USART3_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART2_PERIPH && defined USART2_IRQ && (USART2_RX_BUFFER_SIZESHIFT > 0 || USART2_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART1_PERIPH && defined USART1_IRQ && (USART1_RX_BUFFER_SIZESHIFT > 0 || USART1_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined USART0_PERIPH && defined USART0_IRQ && (USART0_RX_BUFFER_SIZESHIFT > 0 || USART0_TX_BUFFER_SIZESHIFT > 0) */

#if USART_PORTS > 1
#define portparamlast port
#define portparam portparamlast,
#define portassert(ret) if(port >= FRAMES_PORTS) return ret
#else
#define port 0
#define portparamlast
#define portparam
#define portassert(ret)
#endif

#if defined USART_HAS_RECEIVER || defined USART_HAS_TRANSMITTER

static USART_TypeDef* const USART_Periphs[USART_PORTS] = { USART_PERIPHS };
static IRQn_Type const USART_IRQs[USART_PORTS] = { USART_IRQS };
static eUSART_Flags USART_Flags[USART_PORTS];

void USART_SetFlagsFromISR(USART_portdecl eUSART_Flags flags) {
	portassert();
	USART_Flags[port] |= flags;
}

bool USART_CheckAndResetFlags(USART_portdecl eUSART_Flags flags) {
	portassert();
	bool _res;
	NVIC_DisableIRQ(USART_IRQs[port]); _res = USART_Flags[port] & flags; USART_Flags[port] &= ~flags; NVIC_EnableIRQ(USART_IRQs[port]);
	return _res;
}

#endif

/// ********************************************************************
/// USART Receiver
/// ********************************************************************
#ifdef USART_HAS_RECEIVER

static uint16_t const USART_RxSize[USART_PORTS] = { USART_RX_SIZES };
static uint16_t USART_RxWriteIndex[USART_PORTS];
static uint16_t USART_RxReadIndex[USART_PORTS];
static uint16_t volatile USART_RxCounter[USART_PORTS];
static char *const USART_RxBuffer[USART_PORTS] = { USART_RX_BUFFERS };

// Put a character to the USART Receiver buffer from interrupt service routine
void USART_PushCharFromISR(USART_portdecl char c) {
	portassert();
	if(USART_RxCounter[port] < USART_RxSize[port]) {
		USART_RxCounter[port]++; USART_RxBuffer[port][USART_RxWriteIndex[port]++] = c;
		USART_RxWriteIndex[port] &= (USART_RxSize[port] - 1);
	}
	else USART_SetFlagsFromISR(portparam USART_RX_BUFFER_OVERFLOW);
}

// Get a character from the USART Receiver buffer
char USART_GetChar(USART_portdecllast) {
	portassert(0);
	while (!USART_RxCounter[port]) YIELD();
	char _c = USART_RxBuffer[port][USART_RxReadIndex[port]++];
	USART_RxReadIndex[port] &= (USART_RxSize[port] - 1);
	NVIC_DisableIRQ(USART_IRQs[port]); USART_RxCounter[port]--; NVIC_EnableIRQ(USART_IRQs[port]);
	return _c;
}

// Gets a value indicating whether there is a ready character in the USART Receiver buffer
bool USARTIsCharAvailable(USART_portdecllast) {
	portassert(false);
	return USART_RxCounter[port];
}

#endif /* USART_HAS_RECEIVER */

/// ********************************************************************
/// USART Transmitter
/// ********************************************************************
#ifdef USART_HAS_TRANSMITTER

static uint16_t const USART_TxSize[USART_PORTS] = { USART_TX_SIZES };
static uint16_t USART_TxWriteIndex[USART_PORTS];
static uint16_t USART_TxReadIndex[USART_PORTS];
static uint16_t volatile USART_TxCounter[USART_PORTS];
static char *const USART_TxBuffer[USART_PORTS] = { USART_TX_BUFFERS };

// Get a character from the USART Transmitter buffer from interrupt service routine
bool USART_PopCharFromISR(USART_portdecl char *c) {
	portassert(false);
	if (!USART_TxCounter[port]) return false;
	USART_TxCounter[port]--;
	*c = USART_TxBuffer[port][USART_TxReadIndex[port]++];
	USART_TxReadIndex[port] &= (USART_TxSize[port] - 1);
	return true;
}

// Put a character to the USART Transmitter buffer
void USART_PutChar(USART_portdecl char c) {
	portassert();
	while (USART_TxCounter[port] >= USART_TxSize[port]) USART_YIELD();
	USART_TxBuffer[port][USART_TxWriteIndex[port]++] = c;
	USART_TxWriteIndex[port] &= (USART_TxSize[port] - 1);
	NVIC_DisableIRQ(USART_IRQs[port]);
	USART_TxCounter[port]++;
	NVIC_EnableIRQ(USART_IRQs[port]);
	USART_ITConfig(USART_Periphs[port], USART_IT_TXE, ENABLE);
}

// Write a block to the USART Transmitter buffer
void USART_PutBlock(USART_portdecl void const *block, size_t len) {
	portassert();
	char const *_block = (char const *)block;
	while(len--) { USART_PutChar(portparam *_block++); }
}

// Waits until all date gone from Transmitter buffer
void USART_Flush(USART_portdecllast) {
	portassert();
	while(USART_TxCounter[port] > 0) USART_YIELD();
}

#endif /* USART_HAS_TRANSMITTER */
