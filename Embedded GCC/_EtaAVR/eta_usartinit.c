/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_usartinit.c
 ***********************************************************************/

#include <avr/interrupt.h>
#include "eta_usartinit.h"

#ifdef USARTINIT_RECEIVER
// USART Receiver interrupt service routine
ISR(USARTINIT_RX_VECT) { USARTINIT_RECEIVER(USARTINIT_UDR); }
#endif

#ifdef USARTINIT_TRANSMITTER
// USART Receiver interrupt service routine
ISR(USARTINIT_UDRE_VECT) { if(!USARTINIT_TRANSMITTER()) { USARTINIT_UCSRB &= ~(1 << USARTINIT_UDRIE); } }
#endif

#if defined USARTINIT_RECEIVER || defined USARTINIT_TRANSMITTER
// Inits interrupt-driven USART port
void usartinit_init() {
	// baud rate
	USARTINIT_UBRR = ((F_CPU / 16 / USARTINIT_BAUDRATE) - 1);
	#ifndef USARTINIT_CLASSIC
	//8 bits, 1 stop bit, no parity control
	USARTINIT_UCSRC = USARTINIT_URSELBIT | USARTINIT_CONFIG;
	#endif

	// Enable interrupt-driven receiver and transmitter
	#if defined USARTINIT_RECEIVER && defined USARTINIT_TRANSMITTER
	USARTINIT_UCSRB = (1 << USARTINIT_RXEN) | (1 << USARTINIT_RXCIE) | (1 << USARTINIT_TXEN);
	#elif defined USARTINIT_RECEIVER
	USARTINIT_UCSRB = (1 << USARTINIT_RXEN) | (1 << USARTINIT_RXCIE);
	#elif defined USARTINIT_TRANSMITTER
	USARTINIT_UCSRB = (1 << USARTINIT_TXEN);
	#endif
}

// Set baud rate at runtime
void usartinit_setbaudrate(uint16_t baudrate) { USARTINIT_UBRR = ((F_CPU / 16 / baudrate) - 1); }
#endif
