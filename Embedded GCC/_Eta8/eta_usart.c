/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_usart.c
 ***********************************************************************/

#include <avr/interrupt.h>
#include "eta_usart.h"

/// ********************************************************************
/// USART Receiver Capability
/// ********************************************************************

#ifdef USART_RECEIVER
// This flag is set on USART Receiver error
bit usart_rx_buffer_error;
// This flag is set on USART Receiver buffer overflow
bit usart_rx_buffer_overflow;

#define USART_RX_BUFFER_SIZE (1 << USART_RX_BUFFER_SIZESHIFT)
#define USART_RX_BUFFER_SIZEMASK (USART_RX_BUFFER_SIZE - 1)

typedef struct {
	#if USART_RX_BUFFER_SIZESHIFT <= 8
	uint8_t wr_index;
	uint8_t rd_index;
	#else
	uint16_t wr_index;
	uint16_t rd_index;
	#endif
	#if USART_RX_BUFFER_SIZESHIFT <= 7
	uint8_t volatile counter;
	#else
	uint16_t volatile counter;
	#endif
} sUsart_RxContext;

static char usart_rx_buffer[USART_RX_BUFFER_SIZE];
static sUsart_RxContext usart_rx_context;

// USART Receiver interrupt service routine
void usart_push_char_from_isr(char c) {
	sUsart_RxContext *_rx_ctx = &usart_rx_context; force_ptr(_rx_ctx);

	if ((USARTINIT_UCSRA & USARTINIT_ERROR_BITS) == 0) {
		if(_rx_ctx->counter < USART_RX_BUFFER_SIZE) {
			_rx_ctx->counter++; usart_rx_buffer[_rx_ctx->wr_index++] = c;
			#if USART_RX_BUFFER_SIZESHIFT != 8
			_rx_ctx->wr_index &= USART_RX_BUFFER_SIZEMASK;
			#endif
		}
		else usart_rx_buffer_overflow = true;
	}
	else usart_rx_buffer_error = true;
}

// Get a character from the USART Receiver buffer
char usart_get_char() {
	sUsart_RxContext *_rx_ctx = &usart_rx_context; force_ptr(_rx_ctx);

	char _c;
	while (!_rx_ctx->counter) USART_YIELD();
	_c = usart_rx_buffer[_rx_ctx->rd_index++];
	#if USART_RX_BUFFER_SIZESHIFT != 8
	_rx_ctx->rd_index &= USART_RX_BUFFER_SIZEMASK;
	#endif
	cli(); _rx_ctx->counter--; sei();
	return _c;
}

// Gets a value indicating whether there is a ready character in the USART Receiver buffer
bool usart_is_char_available() { return usart_rx_context.counter; }
#endif

/// ********************************************************************
/// USART Transmitter Capability
/// ********************************************************************

#ifdef USART_TRANSMITTER
#define USART_TX_BUFFER_SIZE (1 << USART_TX_BUFFER_SIZESHIFT)
#define USART_TX_BUFFER_SIZEMASK (USART_TX_BUFFER_SIZE - 1)

typedef struct {
	#if USART_TX_BUFFER_SIZESHIFT <= 8
	uint8_t wr_index;
	uint8_t rd_index;
	#else
	uint16_t wr_index;
	uint16_t rd_index;
	#endif
	#if USART_TX_BUFFER_SIZESHIFT <= 7
	uint8_t volatile counter;
	#else
	uint16_t volatile counter;
	#endif
} sUsart_TxContext;

static char usart_tx_buffer[USART_TX_BUFFER_SIZE];
static sUsart_TxContext usart_tx_context;

// USART Transmitter interrupt service routine
bool usart_pop_char_from_isr() {
	sUsart_TxContext *_tx_ctx = &usart_tx_context; force_ptr(_tx_ctx);

	if(_tx_ctx->counter) {
		_tx_ctx->counter--;
		USARTINIT_WRITE_DATA(usart_tx_buffer[_tx_ctx->rd_index++]);
		#if USART_TX_BUFFER_SIZESHIFT != 8
		_tx_ctx->rd_index &= USART_TX_BUFFER_SIZEMASK;
		#endif
		return true;
	}
	return false;
}

// Write a character to the USART Transmitter buffer
void usart_put_char(char c) {
	sUsart_TxContext *_tx_ctx = &usart_tx_context; force_ptr(_tx_ctx);

	while (_tx_ctx->counter >= USART_TX_BUFFER_SIZE) USART_YIELD();
	usart_tx_buffer[_tx_ctx->wr_index++] = c;
	#if USART_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= USART_TX_BUFFER_SIZEMASK;
	#endif
	cli(); _tx_ctx->counter++; sei();
	USARTINIT_ENABLE_TX_INTERRUPT();
}

// Write a block to the USART Transmitter buffer
void usart_put_block(void const *block, size_t len) {
	char const *_block = (char const *)block;
	while(len--) { usart_put_char(*_block++); }
}

// Waits until all date gone from Transmitter buffer
void usart_flush() { while(usart_tx_context.counter > 0) USART_YIELD(); }
#endif
