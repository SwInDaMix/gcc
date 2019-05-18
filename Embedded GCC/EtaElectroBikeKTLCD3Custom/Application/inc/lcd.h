#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LCDBit_Cruise = 1,
    LCDBit_Assist = 15,
    LCDBit_Vol = 23,
    LCDBit_ODO = 31,
    LCDBit_Km = 39,
    LCDBit_Mil = 47,
    LCDBit_ODOVolts_Dot = 55,
    LCDBit_Temp_One = 63,
    LCDBit_Temp_Minus = 71,
    LCDBit_Temp_C = 72,
    LCDBit_Temp_F = 73,
    LCDBit_PowerTemp_C = 74,
    LCDBit_PowerTemp_W = 75,
    LCDBit_PowerTemp_F = 76,
    LCDBit_PowerTemp_Motor = 77,
    LCDBit_PowerTemp_Minus = 103,
    LCDBit_AVS = 104,
    LCDBit_MXS = 105,
    LCDBit_Walking = 106,
    LCDBit_Mph = 107,
    LCDBit_Kmh = 108,
    LCDBit_Speed_Dot = 111,
    LCDBit_DST = 135,
    LCDBit_TM = 136,
    LCDBit_TTM = 137,
    LCDBit_Light = 189,
    LCDBit_Brake = 190,
    LCDBit_TravelTime_Colon = 191,
} eLCDBit;

typedef enum {
    LCDDigitOffset__Twisted        = 0x80,
    LCDDigitOffset_AssistLevel    = 1,
    LCDDigitOffset_ODOVolts_1      = 2,
    LCDDigitOffset_ODOVolts_2      = 3,
    LCDDigitOffset_ODOVolts_3      = 4,
    LCDDigitOffset_ODOVolts_4      = 5,
    LCDDigitOffset_ODOVolts_5      = 6,
    LCDDigitOffset_Temp_1          = 7,
    LCDDigitOffset_Temp_2          = 8,
    LCDDigitOffset_PowerTemp_3     = 10 | LCDDigitOffset__Twisted,
    LCDDigitOffset_PowerTemp_2     = 11 | LCDDigitOffset__Twisted,
    LCDDigitOffset_PowerTemp_1     = 12 | LCDDigitOffset__Twisted,
    LCDDigitOffset_Speed_3         = 14 | LCDDigitOffset__Twisted,
    LCDDigitOffset_Speed_2         = 15 | LCDDigitOffset__Twisted,
    LCDDigitOffset_Speed_1         = 16 | LCDDigitOffset__Twisted,
    LCDDigitOffset_TravelTime_5    = 18 | LCDDigitOffset__Twisted,
    LCDDigitOffset_TravelTime_4    = 19 | LCDDigitOffset__Twisted,
    LCDDigitOffset_TravelTime_3    = 20 | LCDDigitOffset__Twisted,
    LCDDigitOffset_TravelTime_2    = 21 | LCDDigitOffset__Twisted,
    LCDDigitOffset_TravelTime_1    = 22 | LCDDigitOffset__Twisted,
} eLCDDigitOffset;

typedef enum {
    //                          654 3210
    LCDDigit_0      = 0x77, // 0111 0111
    LCDDigit_1      = 0x24, // 0010 0100
    LCDDigit_2      = 0x6B, // 0110 1011
    LCDDigit_3      = 0x6D, // 0110 1101
    LCDDigit_4      = 0x3C, // 0011 1100
    LCDDigit_5      = 0x5D, // 0101 1101
    LCDDigit_6      = 0x5F, // 0101 1111
    LCDDigit_7      = 0x74, // 0111 0100
    LCDDigit_8      = 0x7F, // 0111 1111
    LCDDigit_9      = 0x7D, // 0111 1101
    LCDDigit_A      = 0x7E, // 0111 1110
    LCDDigit_b      = 0x1F, // 0001 1111
    LCDDigit_c      = 0x0B, // 0000 1011
    LCDDigit_d      = 0x2F, // 0010 1111
    LCDDigit_E      = 0x5B, // 0101 1011
    LCDDigit_F      = 0x5A, // 0101 1010
    LCDDigit_p      = 0x7A, // 0111 1010
    LCDDigit_ROT0   = 0x60, // 0110 0000
    LCDDigit_ROT1   = 0x30, // 0010 0100
    LCDDigit_ROT2   = 0x18, // 0000 0101
    LCDDigit_ROT3   = 0x0C, // 0000 0011
    LCDDigit_ROT4   = 0x06, // 0001 0010
    LCDDigit_ROT5   = 0x03, // 0101 0000
} eLCDDigit;

typedef enum {
    LCDBatterySoC_Body  = 0x01,
    LCDBatterySoC_SoC_1 = 0x02,
    LCDBatterySoC_SoC_2 = 0x04,
    LCDBatterySoC_SoC_3 = 0x10,
    LCDBatterySoC_SoC_4 = 0x08,
    LCDBatterySoC__All  = LCDBatterySoC_Body | LCDBatterySoC_SoC_1 | LCDBatterySoC_SoC_2 | LCDBatterySoC_SoC_3 | LCDBatterySoC_SoC_4,
} eLCDBatterySocBits;

void lcd_flush();
void lcd_clear();
void lcd_set_bit(eLCDBit bit, bool value);
void lcd_set_digit(eLCDDigitOffset digit_offset, eLCDDigit digit);
void lcd_set_battery_soc_bits(eLCDBatterySocBits battery_soc_bits);
