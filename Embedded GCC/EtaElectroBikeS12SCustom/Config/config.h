#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>

#define FCPU 16000000UL

#define BLDC_INTERPOLATION_60_ERPS 20U
#define BLDC_INTERPOLATION_360_ERPS 80U
#define BLDC_ANGLE_CORRECTION 128

#define ADC_THUMB_MIN_VALUE 60U
#define ADC_THUMB_MAX_VALUE 182U

#define UART_BAUDRATE 115200U

#define DBG(fmt) puts(fmt)
#define DBGF(fmt, ...) printf(fmt, __VA_ARGS__)

#define limit 50
#define timeout 3125
#define wheel_circumference 798L
#define fummelfaktor 32L
#define BATTERY_CURRENT_MAX_VALUE 371L
#define REGEN_CURRENT_MAX_VALUE 251L
#define MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT 234
#define current_cal_a 10
#define LEVEL_1 20
#define LEVEL_2 40
#define LEVEL_3 60
#define LEVEL_4 80
#define LEVEL_5 100
#define P_FACTOR_DIVIDENT 3
#define P_FACTOR_DIVISOR 8
#define I_FACTOR_DIVIDENT 11
#define I_FACTOR_DIVISOR 5
#define GEAR_RATIO 15
#define BATTERY_LI_ION_CELLS_NUMBER 9

#define SPEEDSENSOR_INTERNAL

#ifdef __CLION_IDE__
#define __interrupt(h)
#endif

#endif /* CONFIG_H_ */
