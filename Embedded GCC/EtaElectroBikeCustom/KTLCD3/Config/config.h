#pragma once

#include <stdio.h>

#define DEBUG

#define FCPU 16000000ULL

#ifdef DEBUG
#define UART_BAUDRATE 115200U
#define DBG(fmt) printf(fmt)
#define DBGF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define UART_BAUDRATE 9600U
#define DBG(fmt)
#define DBGF(fmt, ...)
#endif

#ifndef __SDCC
#define __interrupt(h) //
#endif
