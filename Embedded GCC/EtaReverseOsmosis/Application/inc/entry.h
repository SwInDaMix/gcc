/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : entry.h
 ***********************************************************************/

#pragma once

#include "config.h"

typedef enum {
	Button_Prev = 0x01,
	Button_PrevLong = 0x02,
	Button_Next = 0x04,
	Button_NextLong = 0x08,
	Button_Enter = 0x10,
	Button_EnterLong = 0x20,
	Button_All = Button_Prev | Button_PrevLong | Button_Next | Button_NextLong | Button_Enter | Button_EnterLong,
} eButton;

typedef enum {
	LCD_DadeOut_None = 0,
	LCD_DadeOut_Fast = 16,
	LCD_DadeOut_Normal = 8,
	LCD_DadeOut_Slow = 2,
} eLCD_TransitionStep;

typedef enum {
	LCD_Mode_Transition = 0x00,
	LCD_Mode_Rotation = 0x10,
	LCD_Mode_Blinking = 0x20,
	LCD_Mode__Mask = 0xF0,
} eLCD_Mode;

typedef enum {
	LCD_Symbol_None,
	LCD_Symbol_0,
	LCD_Symbol_1,
	LCD_Symbol_2,
	LCD_Symbol_3,
	LCD_Symbol_4,
	LCD_Symbol_5,
	LCD_Symbol_6,
	LCD_Symbol_7,
	LCD_Symbol_8,
	LCD_Symbol_9,
	LCD_Symbol_A,
	LCD_Symbol_b,
	LCD_Symbol_C,
	LCD_Symbol_d,
	LCD_Symbol_E,
	LCD_Symbol_F,
	LCD_Symbol_MINUS,
	LCD_Symbol_u,
	LCD_Symbol_c,
	LCD_Symbol_n,
	LCD_Symbol_cr,
	LCD_Symbol_o,
	LCD_Symbol_Rotary,
	LCD_Symbol__Last,
} eLCD_Symbol;

typedef enum {
	LCD_LED_WaterInlet = 0x01,
	LCD_LED_CleanMembrane = 0x02,
	LCD_LED_LowTank = 0x04,
	LCD_LED_NoWaterSource = 0x08,
} eLCD_LEDs;

typedef enum {
	FRAME_CMD_DEBUG_STRING,
	FRAME_CMD_STATUS,
	FRAME_CMD_NEW_PARAMS,
	FRAME_CMD_TEST_COMMAND
} eFrameCmd;

typedef struct __attribute__((packed)) {
	uint8_t HallPosition;
	uint8_t HallPrescaler;
	uint16_t HallPeriod;
	uint16_t PWMPower;
} sStatus;
typedef struct __attribute__((packed)) {
	uint8_t HallPrescaler;
	uint16_t HallPeriod;
	uint16_t PWMPower;
} sNewParams;

void Int_BTN_Set(eButton button);
eButton BTN_CheckTrigered(eButton button);

void Int_Update();
void LCD_DigitSetSymbol(uint8_t digit, eLCD_Symbol symbol, bool dot_point, eLCD_TransitionStep transition_step);
void LCD_DigitSetRotation(uint8_t digit, eLCD_TransitionStep transition_step);
void LCD_LEDSetValue(eLCD_LEDs leds, bool value, eLCD_TransitionStep transition_step);
void LCD_LEDSetBlinking(eLCD_LEDs leds, eLCD_TransitionStep transition_step);

void LCD_DigitSetOp(eLCD_TransitionStep rotation_speed, eLCD_Symbol symbol, bool dot);
void LCD_DigitSetNumberOrTime(uint16_t number_or_time, bool is_time, bool is_leadzeros, bool dots);

void Valve_WaterInlet(bool state);
void Valve_CleanMembrane(bool state);

void Indicator_LowTank(bool state, bool is_post);

bool Sensor_NoWaterSource();
bool Sensor_LowTankPressure();

void EtaReverseOsmosisMainLoop(bool volatile *is_abort_requested, bool volatile *is_inited, void (*yield)());
