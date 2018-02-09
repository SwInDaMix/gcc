/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_frames.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include "eta_usartinit.h"

#if defined FRAMES_8B || defined FRAMES_9B

#ifdef FRAMES_RECEIVER
// This flag is set on USART Receiver error
extern bit frames_rx_buffer_error;
// This flag is set on USART Receiver buffer overflow
extern bit frames_rx_buffer_overflow;
// This flag is set on USART Receiver frame collision
extern bit frames_rx_frame_collision;

// Process incomming data to the USART Receiver buffer from interrupt service routine
#if defined FRAMES_8B
void frames8b_push_data_from_isr(uint8_t data);
#elif defined FRAMES_9B
void frames9b_push_data_from_isr(uint16_t data);
#endif

// Get address of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_address();

// Get command of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_command();

// Get size of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_size();

// Get data of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_data(void *dst, uint8_t offset, uint8_t size);

// Remove the first available frame in the USART Receiver buffer
void frames_remove_frame();

// Gets a value indicating whether there is a ready frame in the USART Receiver buffer
bool frames_is_frame_available();
#endif

#ifdef FRAMES_TRANSMITTER
// Process outgoing data from the USART Transmitter buffer from interrupt service routine
#if defined FRAMES_8B
bool frames8b_pop_data_from_isr();
#elif defined FRAMES_9B
bool frames9b_pop_data_from_isr();
#endif

// Write a frame to the USART Transmitter buffer
bool frames_put_frame(uint8_t addr, uint8_t cmd, void const *data, uint8_t size);

// Write a frame to the USART Transmitter buffer (extended version)
bool frames_put_frame_ex(uint8_t addr, uint8_t cmd, uint8_t cnt, ...);
bool frames_put_frame_exva(uint8_t addr, uint8_t cmd, uint8_t datas_cnt, va_list ap);

// Ensures and waits until all date gone from Transmitter buffer
void frames_flush();

// Gets a value indicating whether USART Transmitter is free
bool frames_is_transmitter_free();
#endif

#if defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER
// Inits interrupt-driven USART port
void frames_init();
#endif

#endif /* defined FRAMES_8B || defined FRAMES_9B */
