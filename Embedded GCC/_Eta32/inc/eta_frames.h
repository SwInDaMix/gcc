/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_frames.h
 ***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined FRAMES_8B || defined FRAMES_9B

// === PORT 0 ===
#if defined FRAMES0_PERIPH && defined FRAMES0_IRQ && (FRAMES0_RX_BUFFER_SIZESHIFT > 0 || FRAMES0_TX_BUFFER_SIZESHIFT > 0)
#define FRAMES_PORTS 1
#if FRAMES0_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES0_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX0_PACKET_SIZE (1 << FRAMES0_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 1 ===
#if defined FRAMES1_PERIPH && defined FRAMES1_IRQ && (FRAMES1_RX_BUFFER_SIZESHIFT > 0 || FRAMES1_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 2
#if FRAMES1_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES1_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX1_PACKET_SIZE (1 << FRAMES1_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 2 ===
#if defined FRAMES2_PERIPH && defined FRAMES2_IRQ && (FRAMES2_RX_BUFFER_SIZESHIFT > 0 || FRAMES2_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 3
#if FRAMES2_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES2_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX2_PACKET_SIZE (1 << FRAMES2_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 3 ===
#if defined FRAMES3_PERIPH && defined FRAMES3_IRQ && (FRAMES3_RX_BUFFER_SIZESHIFT > 0 || FRAMES3_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 4
#if FRAMES3_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES3_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX3_PACKET_SIZE (1 << FRAMES3_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 4 ===
#if defined FRAMES4_PERIPH && defined FRAMES4_IRQ && (FRAMES4_RX_BUFFER_SIZESHIFT > 0 || FRAMES4_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 5
#if FRAMES4_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES4_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX4_PACKET_SIZE (1 << FRAMES4_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 5 ===
#if defined FRAMES5_PERIPH && defined FRAMES5_IRQ && (FRAMES5_RX_BUFFER_SIZESHIFT > 0 || FRAMES5_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 6
#if FRAMES5_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES5_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX5_PACKET_SIZE (1 << FRAMES5_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 6 ===
#if defined FRAMES6_PERIPH && defined FRAMES6_IRQ && (FRAMES6_RX_BUFFER_SIZESHIFT > 0 || FRAMES6_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 7
#if FRAMES6_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES6_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX6_PACKET_SIZE (1 << FRAMES6_TX_BUFFER_SIZESHIFT) - 2
#endif

// === PORT 7 ===
#if defined FRAMES7_PERIPH && defined FRAMES7_IRQ && (FRAMES7_RX_BUFFER_SIZESHIFT > 0 || FRAMES7_TX_BUFFER_SIZESHIFT > 0)
#undef FRAMES_PORTS
#define FRAMES_PORTS 8
#if FRAMES7_RX_BUFFER_SIZESHIFT > 0
#define FRAMES_RECEIVER
#endif
#if FRAMES7_TX_BUFFER_SIZESHIFT > 0
#define FRAMES_TRANSMITTER
#define FRAMES_MAX_TX7_PACKET_SIZE (1 << FRAMES7_TX_BUFFER_SIZESHIFT) - 2
#endif

#endif /* defined FRAMES7_PERIPH && defined FRAMES7_IRQ && (FRAMES7_RX_BUFFER_SIZESHIFT > 0 || FRAMES7_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES6_PERIPH && defined FRAMES6_IRQ && (FRAMES6_RX_BUFFER_SIZESHIFT > 0 || FRAMES6_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES5_PERIPH && defined FRAMES5_IRQ && (FRAMES5_RX_BUFFER_SIZESHIFT > 0 || FRAMES5_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES4_PERIPH && defined FRAMES4_IRQ && (FRAMES4_RX_BUFFER_SIZESHIFT > 0 || FRAMES4_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES3_PERIPH && defined FRAMES3_IRQ && (FRAMES3_RX_BUFFER_SIZESHIFT > 0 || FRAMES3_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES2_PERIPH && defined FRAMES2_IRQ && (FRAMES2_RX_BUFFER_SIZESHIFT > 0 || FRAMES2_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES1_PERIPH && defined FRAMES1_IRQ && (FRAMES1_RX_BUFFER_SIZESHIFT > 0 || FRAMES1_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES0_PERIPH && defined FRAMES0_IRQ && (FRAMES0_RX_BUFFER_SIZESHIFT > 0 || FRAMES0_TX_BUFFER_SIZESHIFT > 0) */

typedef enum {
	FRAMES_RX_BUFFER_ERROR = 0x1,
	FRAMES_RX_BUFFER_OVERFLOW = 0x2,
	FRAMES_RX_FRAME_COLLISION = 0x4,
} eFrames_Flags;

#if FRAMES_PORTS > 1
#define FRAMES_portdecllast uint8_t port
#define FRAMES_portdecl FRAMES_portdecllast,
#define FRAMES_portassert
#else
#define FRAMES_portdecllast
#define FRAMES_portdecl
#define FRAMES_portassert
#endif

#if defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER

void Frames_SetFlagsFromISR(FRAMES_portdecl eFrames_Flags flags);

bool Frames_CheckAndResetFlags(FRAMES_portdecl eFrames_Flags flags);

#endif /* defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER */

#ifdef FRAMES_RECEIVER

// Process incomming data to the USART Receiver buffer from interrupt service routine
#ifdef FRAMES_8B
void Frames8b_PushDataFromISR(FRAMES_portdecl uint8_t data);
#endif
#ifdef FRAMES_9B
void Frames9b_PushDataFromISR(FRAMES_portdecl uint16_t data);
#endif

// Get address of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameAddress(FRAMES_portdecllast);

// Get command of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameCommand(FRAMES_portdecllast);

// Get size of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameSize(FRAMES_portdecllast);

// Get data of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameData(FRAMES_portdecl void *dst, uint8_t offset, uint8_t size);

// Remove the first available frame from the USART Receiver buffer
void Frames_RemoveFrame(FRAMES_portdecllast);

// Gets a value indicating whether there is a ready frame in the USART Receiver buffer
bool Frames_IsFrameAvailable(FRAMES_portdecllast);

#endif /* FRAMES_RECEIVER */

#ifdef FRAMES_TRANSMITTER

// Process outgoing data from the USART Transmitter buffer from interrupt service routine
#ifdef FRAMES_8B
bool Frames8b_PopDataFromISR(FRAMES_portdecl uint8_t *data);
#endif
#ifdef FRAMES_9B
bool Frames9b_PopDataFromISR(FRAMES_portdecl uint16_t *data);
#endif

// Write a frame to the USART Transmitter buffer
bool Frames_PutFrame(FRAMES_portdecl uint8_t addr, uint8_t cmd, void const *data, uint8_t size);

#ifdef FRAMES_EXTENDED

// Write a frame to the USART Transmitter buffer (extended version)
bool Frames_PutFrameEx(FRAMES_portdecl uint8_t addr, uint8_t cmd, uint32_t datas_cnt, ...);
bool Frames_PutFrameExVA(FRAMES_portdecl uint8_t addr, uint8_t cmd, uint32_t datas_cnt, va_list ap);

#endif /* FRAMES_EXTENDED */

// Waits until all date gone from Transmitter buffer
void Frames_Flush(FRAMES_portdecllast);

// Gets a value indicating whether USART Transmitter is free
bool Frames_IsTransmitterFree(FRAMES_portdecllast);

#endif /* FRAMES_TRANSMITTER */

#if defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER

typedef struct {
	uint32_t TimeOut;
	uint8_t Address;
	uint8_t Command;
} sFrames_Request;

// Creates and send request frame
bool Frames_RequestInit(FRAMES_portdecl sFrames_Request *request, uint32_t time_out, uint8_t addr, uint8_t cmd, void const *data, uint8_t size);

// Send response and waits for response
bool Frames_RequestResponse(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void const *data, uint8_t size);

// Send response and waits for response with constant size
bool Frames_RequestResponseWithConstSize(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void *data_resp, uint8_t resp_size, void const *data, uint8_t size);

#ifdef FRAMES_EXTENDED

// Creates and send request frame (extended version)
bool Frames_RequestInitEx(FRAMES_portdecl sFrames_Request *request, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, ...);
bool Frames_RequestInitExVA(FRAMES_portdecl sFrames_Request *request, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, va_list ap);

// Send response and waits for response (extended version)
bool Frames_RequestResponseEx(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, ...);
bool Frames_RequestResponseExVA(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, va_list ap);

// Send response and waits for response with constant size (extended version)
bool Frames_RequestResponseWithConstSizeEx(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void *data_resp, uint8_t resp_size, uint32_t datas_cnt, ...);
bool Frames_RequestResponseWithConstSizeExVA(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void *data_resp, uint8_t resp_size, uint32_t datas_cnt, va_list ap);

#endif /* FRAMES_EXTENDED */

// Polls for the response for specified request
bool Frames_RequestPollForResponse(FRAMES_portdecl sFrames_Request const *request);

// Polls for the response for specified request
bool Frames_RequestCheckForTimeout(FRAMES_portdecl sFrames_Request const *request, uint32_t timer);

#endif /* defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER */

#endif /* defined FRAMES_8B || defined FRAMES_9B */

#ifdef __cplusplus
}
#endif
