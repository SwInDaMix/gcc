#pragma once

#include <stdio.h>

//#define DEBUG

#define FCPU 16000000ULL

#define BLDC_VOLTAGE_TO_ERPS_FACTOR 0.00775
//#define BLDC_VOLTAGE_TO_ERPS_FACTOR 0.0090
#define BLDC_INTERPOLATION_60_ERPS 15U
#define BLDC_INTERPOLATION_360_ERPS 80U

// samokat
#define BLDC_IS_PURE_SINE true
#define BLDC_IS_REVERSE true
#define BLDC_IS_INVERT_HALLS true
#define BLDC_CORRECTION_ANGLE (-12)

// evel
//#define BLDC_IS_PURE_SINE true
//#define BLDC_IS_REVERSE false
//#define BLDC_IS_INVERT_HALLS false
//#define BLDC_CORRECTION_ANGLE (-16)

#define ADC_THUMB_MIN_VALUE 50U
#define ADC_THUMB_MAX_VALUE 175U

#define DRV_MAX_CURRENT_025A (30 * 4)

#ifdef DEBUG
#define UART_BAUDRATE 115200U
#define DBG(fmt) puts(fmt)
#define DBGF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define UART_BAUDRATE 9600U
#define DBG(fmt)
#define DBGF(fmt, ...)
#endif

#define P_FACTOR_DIVIDENT 2
#define P_FACTOR_DIVISOR 5
#define I_FACTOR_DIVIDENT 11
#define I_FACTOR_DIVISOR 7

#define BATTERY_CELLS_NUMBER 8

#ifdef __CLION_IDE__
#define __interrupt(h)
#endif
