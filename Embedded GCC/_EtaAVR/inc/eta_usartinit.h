/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_usartinit.h
 ***********************************************************************/

#pragma once

#include "config.h"

#if defined USARTINIT_RECEIVER || defined USARTINIT_TRANSMITTER

#if	defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega48PA__) \
 || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega88PA__) \
 || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) \
 || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328A__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PA__)
	#define USARTINIT_TX_VECT		USART_TX_vect
	#define USARTINIT_RX_VECT		USART_RX_vect
	#define USARTINIT_UDRE_VECT		USART_UDRE_vect
	#define USARTINIT_UDR			UDR0
	#define USARTINIT_UCSRA			UCSR0A
	#define USARTINIT_UCSRB			UCSR0B
	#define USARTINIT_UCSRC			UCSR0C
	#define USARTINIT_RXCIE			RXCIE0
	#define USARTINIT_TXCIE			TXCIE0
	#define USARTINIT_UDRIE			UDRIE0
	#define USARTINIT_RXC			RXC0
	#define USARTINIT_TXC			TXC0
	#define USARTINIT_RXEN			RXEN0
	#define USARTINIT_TXEN			TXEN0
	#define USARTINIT_UBRR			UBRR0
	#define USARTINIT_UDRE			UDRE0
	#define USARTINIT_FE			FE0
	#define USARTINIT_DOR			DOR0
	#define USARTINIT_UPE			UPE0
	#define USARTINIT_URSELBIT		0
	#define USARTINIT_UPM0			UPM00
	#define USARTINIT_USBS			USBS0
	#define USARTINIT_UCSZ0			UCSZ00
	#define USARTINIT_UCSZ2			UCSZ02
	#define USARTINIT_RXEN			RXEN0
	#define USARTINIT_TXEN			TXEN0
	#define USARTINIT_ERROR_BITS	((1 << USARTINIT_FE) | (1 << USARTINIT_UPE) | (1 << USARTINIT_DOR))
#endif
#if	defined(__AVR_ATmega8__) || defined(__AVR_ATmega8A__)
	#define USARTINIT_TX_VECT		USART_TXC_vect
	#define USARTINIT_RX_VECT		USART_RXC_vect
	#define USARTINIT_UDRE_VECT		USART_UDRE_vect
	#define USARTINIT_UDR			UDR
	#define USARTINIT_UCSRA			UCSRA
	#define USARTINIT_UCSRB			UCSRB
	#define USARTINIT_UCSRC			UCSRC
	#define USARTINIT_RXCIE			RXCIE
	#define USARTINIT_TXCIE			TXCIE
	#define USARTINIT_UDRIE			UDRIE
	#define USARTINIT_RXC			RXC
	#define USARTINIT_TXC			TXC
	#define USARTINIT_RXEN			RXEN
	#define USARTINIT_TXEN			TXEN
	#define USARTINIT_UBRR			UBRRL
	#define USARTINIT_UDRE			UDRE
	#define USARTINIT_FE			FE
	#define USARTINIT_DOR			DOR
	#define USARTINIT_UPE			UPE
	#define USARTINIT_URSELBIT		(1 << URSEL)
	#define USARTINIT_UPM0			UPM0
	#define USARTINIT_USBS			USBS
	#define USARTINIT_UCSZ0			UCSZ0
	#define USARTINIT_UCSZ2			UCSZ2
	#define USARTINIT_RXEN			RXEN
	#define USARTINIT_TXEN			TXEN
	#define USARTINIT_ERROR_BITS	((1 << USARTINIT_FE) | (1 << USARTINIT_UPE) | (1 << USARTINIT_DOR))
#endif
#if	defined(__AVR_ATtiny2313__)
	#define USARTINIT_TX_VECT		USART_TX_vect
	#define USARTINIT_RX_VECT		USART_RX_vect
	#define USARTINIT_UDRE_VECT		USART_UDRE_vect
	#define USARTINIT_UDR			UDR
	#define USARTINIT_UCSRA			UCSRA
	#define USARTINIT_UCSRB			UCSRB
	#define USARTINIT_UCSRC			UCSRC
	#define USARTINIT_RXCIE			RXCIE
	#define USARTINIT_TXCIE			TXCIE
	#define USARTINIT_UDRIE			UDRIE
	#define USARTINIT_RXC			RXC
	#define USARTINIT_TXC			TXC
	#define USARTINIT_RXEN			RXEN
	#define USARTINIT_TXEN			TXEN
	#define USARTINIT_UBRR			UBRRL
	#define USARTINIT_UDRE			UDRE
	#define USARTINIT_FE			FE
	#define USARTINIT_DOR			DOR
	#define USARTINIT_UPE			UPE
	#define USARTINIT_URSELBIT		0
	#define USARTINIT_UPM0			UPM0
	#define USARTINIT_USBS			USBS
	#define USARTINIT_UCSZ0			UCSZ0
	#define USARTINIT_UCSZ2			UCSZ2
	#define USARTINIT_RXEN			RXEN
	#define USARTINIT_TXEN			TXEN
	#define USARTINIT_ERROR_BITS	((1 << USARTINIT_FE) | (1 << USARTINIT_UPE) | (1 << USARTINIT_DOR))
#endif
#if	defined(__AVR_AT90S2313__)
	#define USARTINIT_CLASSIC
	#define USARTINIT_TX_VECT		UART_TX_vect
	#define USARTINIT_RX_VECT		UART_RX_vect
	#define USARTINIT_UDRE_VECT		UART_UDRE_vect
	#define USARTINIT_UDR			UDR
	#define USARTINIT_UCSRA			USR
	#define USARTINIT_UCSRB			UCR
	#define USARTINIT_RXCIE			RXCIE
	#define USARTINIT_TXCIE			TXCIE
	#define USARTINIT_UDRIE			UDRIE
	#define USARTINIT_RXC			RXC
	#define USARTINIT_TXC			TXC
	#define USARTINIT_RXEN			RXEN
	#define USARTINIT_TXEN			TXEN
	#define USARTINIT_UBRR			UBRR
	#define USARTINIT_UDRE			UDRE
	#define USARTINIT_FE			FE
	#define USARTINIT_DOR			DOR
	#define USARTINIT_URSELBIT		0
	#define USARTINIT_RXEN			RXEN
	#define USARTINIT_TXEN			TXEN
	#define USARTINIT_ERROR_BITS	((1 << USARTINIT_FE) | (1 << USARTINIT_DOR))
#endif

#ifndef USARTINIT_CLASSIC
typedef enum {
	USARTINIT_BITS_5 = (0 << USARTINIT_UCSZ0),
	USARTINIT_BITS_6 = (1 << USARTINIT_UCSZ0),
	USARTINIT_BITS_7 = (2 << USARTINIT_UCSZ0),
	USARTINIT_BITS_8 = (3 << USARTINIT_UCSZ0),
	USARTINIT_BITS_9 = (7 << USARTINIT_UCSZ0),
	USARTINIT_PARITY_NONE = (0 << USARTINIT_UPM0),
	USARTINIT_PARITY_EVEN = (2 << USARTINIT_UPM0),
	USARTINIT_PARITY_ODD = (3 << USARTINIT_UPM0),
	USARTINIT_STOPBIT_1 = (0 << USARTINIT_USBS),
	USARTINIT_STOPBIT_2 = (1 << USARTINIT_USBS),
} eUsartConfig;
#endif

// Inits interrupt-driven USART port
void usartinit_init();

// Set baud rate at runtime
void usartinit_setbaudrate(uint16_t baudrate);

#endif /* defined USARTINIT_RECEIVER || defined USARTINIT_TRANSMITTER */

