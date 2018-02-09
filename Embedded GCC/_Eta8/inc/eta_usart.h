/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_usart.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include "eta_usartinit.h"

#ifdef USART_RECEIVER
// This flag is set on USART Receiver error
extern bit usart_rx_buffer_error;
// This flag is set on USART Receiver buffer overflow
extern bit usart_rx_buffer_overflow;

// USART Receiver interrupt service routine
void usart_push_char_from_isr(char c);

// Get a character from the USART Receiver buffer
char usart_get_char();

// Gets a value indicating whether there is a ready character in the USART Receiver buffer
bool usart_is_char_available();
#endif

#ifdef USART_TRANSMITTER
// USART Transmitter interrupt service routine
bool usart_pop_char_from_isr();

// Write a character to the USART Transmitter buffer
void usart_put_char(char c);

// Write a block to the USART Transmitter buffer
void usart_put_block(void const *block, size_t len);

// Ensures and waits until all date gone from Transmitter buffer
void usart_flush();
#endif
