#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LCDBit_Gear_Cruise,
    LCDBit_Gear_Assist,
    LCDBit_ODOVolts_Vol,
    LCDBit_ODOVolts_ODO,
    LCDBit_ODOVolts_Km,
    LCDBit_ODOVolts_Mil,
    LCDBit_ODOVolts_Dot,
    LCDBit_Temp_One,
    LCDBit_Temp_Minus,
    LCDBit_Temp_C,
    LCDBit_Temp_F,
    LCDBit_PowerTemp_C,
    LCDBit_PowerTemp_W,
    LCDBit_PowerTemp_F,
    LCDBit_PowerTemp_Motor,
    LCDBit_PowerTemp_One,
    LCDBit_Speed_AVS,
    LCDBit_Speed_MXS,
    LCDBit_Speed_Walking,
    LCDBit_Speed_Mph,
    LCDBit_Speed_Kmh,
    LCDBit_Speed_Dot,
    LCDBit_ODOVolts_DST,
    LCDBit_TravelTime_TM,
    LCDBit_TravelTime_TTM,
    LCDBit_Light,
    LCDBit_Brake,
    LCDBit_TravelTime_Colon,
    LCDBit__Max
} eLCDBit;

typedef enum {
    LCDDigitOffset_Gear,
    LCDDigitOffset_ODOVolts_1,
    LCDDigitOffset_ODOVolts_2,
    LCDDigitOffset_ODOVolts_3,
    LCDDigitOffset_ODOVolts_4,
    LCDDigitOffset_ODOVolts_5,
    LCDDigitOffset_Temp_1,
    LCDDigitOffset_Temp_2,
    LCDDigitOffset_PowerTemp_3,
    LCDDigitOffset_PowerTemp_2,
    LCDDigitOffset_PowerTemp_1,
    LCDDigitOffset_Speed_3,
    LCDDigitOffset_Speed_2,
    LCDDigitOffset_Speed_1,
    LCDDigitOffset_TravelTime_5,
    LCDDigitOffset_TravelTime_4,
    LCDDigitOffset_TravelTime_3,
    LCDDigitOffset_TravelTime_2,
    LCDDigitOffset_TravelTime_1,
    LCDDigitOffset__Max
} eLCDDigitOffset;

typedef enum {
    LCDDigit_None,
    LCDDigit_0,
    LCDDigit_1,
    LCDDigit_2,
    LCDDigit_3,
    LCDDigit_4,
    LCDDigit_5,
    LCDDigit_6,
    LCDDigit_7,
    LCDDigit_8,
    LCDDigit_9,
    LCDDigit_A,
    LCDDigit_b,
    LCDDigit_C,
    LCDDigit_d,
    LCDDigit_E,
    LCDDigit_F,
    LCDDigit_Minus,
    LCDDigit_Minus1,
    LCDDigit_c,
    LCDDigit_n,
    LCDDigit_P,
    LCDDigit_o,
    LCDDigit_r,
    LCDDigit_u,
    LCDDigit_ROT0,
    LCDDigit_ROT1,
    LCDDigit_ROT2,
    LCDDigit_ROT3,
    LCDDigit_ROT4,
    LCDDigit_ROT5,
    LCDDigit_ROTD0,
    LCDDigit_ROTD1,
    LCDDigit_ROTD2,
    LCDDigit_ROTD3,
    LCDDigit_ROTD4,
    LCDDigit_ROTD5,
    LCDDigit__Max
} eLCDDigit;

typedef enum {
    LCDBatterySoC_Body = 0x01,
    LCDBatterySoC_SoC_1 = 0x08,
    LCDBatterySoC_SoC_2 = 0x10,
    LCDBatterySoC_SoC_3 = 0x04,
    LCDBatterySoC_SoC_4 = 0x02,
    LCDBatterySoC__All = LCDBatterySoC_Body | LCDBatterySoC_SoC_1 | LCDBatterySoC_SoC_2 | LCDBatterySoC_SoC_3 | LCDBatterySoC_SoC_4,
} eLCDBatterySocBits;

void lcd_flush();
void lcd_clear();
void lcd_set_bit(eLCDBit bit, bool value);
void lcd_set_digit(eLCDDigitOffset digit_offset, eLCDDigit digit);
void lcd_set_battery_soc_bits(eLCDBatterySocBits battery_soc_bits);
