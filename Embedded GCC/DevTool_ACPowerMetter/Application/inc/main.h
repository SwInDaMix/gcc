/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include "eta_ade7753.h"

#define PERIOD_1S_SHIFT 5
#define PERIOD_1S (1 << PERIOD_1S_SHIFT)
#define PERIOD_1S_HALF (1 << (PERIOD_1S_SHIFT - 1))

#define ACPM_MAX_WAVEFORM_SAMPLES_PAGES_SHIFT 3
#define ACPM_MAX_WAVEFORM_SAMPLES_PAGES (1 << ACPM_MAX_WAVEFORM_SAMPLES_PAGES_SHIFT)
#define ACPM_MAX_WAVEFORM_SAMPLES_SHIFT 9
#define ACPM_MAX_WAVEFORM_SAMPLES (1 << ACPM_MAX_WAVEFORM_SAMPLES_SHIFT)
#define ACPM_MAX_WAVEFORM_SAMPLES_PART_SHIFT (ACPM_MAX_WAVEFORM_SAMPLES_SHIFT - ACPM_MAX_WAVEFORM_SAMPLES_PAGES_SHIFT)
#define ACPM_MAX_WAVEFORM_SAMPLES_PART (1 << ACPM_MAX_WAVEFORM_SAMPLES_PART_SHIFT)

#define ACPM_SIN45 0.70710678118654752440084436210485
#define ACPM_INPUT_DIV 100

#define ACPM_CURRENT_GAIN 2
#define ACPM_CURRENT_RATIO 600
#define ACPM_CURRENT_RESISTOR 5

#define ACPM_VOLTAGE_GAIN 1
#define ACPM_VOLTAGE_RATIO 1000

#define ACPM_I_INPUT (500. / ACPM_CURRENT_GAIN / ACPM_CURRENT_RESISTOR * ACPM_CURRENT_RATIO)
#define ACPM_V_INPUT (500. / ACPM_VOLTAGE_GAIN * ACPM_VOLTAGE_RATIO / 1000)

#define ACPM_ADE_PERIOD_NOMINATOR ((ADE_F_CPU * 10) >> 3)
#define ACPM_ADE_IRMS_DIVIDOR ((uint32_t)((ADE7753_FULLSACLE_IRMS * ACPM_INPUT_DIV) / ACPM_I_INPUT / ACPM_SIN45))
#define ACPM_ADE_VRMS_DIVIDOR ((uint32_t)((ADE7753_FULLSACLE_VRMS * ACPM_INPUT_DIV) / ACPM_V_INPUT / ACPM_SIN45))

typedef enum {
	ACPMScreen_Generic,
	ACPMScreen_PFC,
	ACPMScreen_ApparentReactive,
	ACPMScreen_Counter,
	ACPMScreen__Last,
} eACPM_Screen;

typedef enum {
	ACPMCommand_DebugString,
	ACPMCommand_ValuesUpdated,

	ACPMCommand_ReadValues = 0x80,
	ACPMCommand_WaveformRequest,
	ACPMCommand_WaveformPage,
	ACPMCommand_ReadCalibration,
	ACPMCommand_WriteCalibration,
	ACPMCommand_SaveCalibration,
	ACPMCommand_Reset,
} eACPM_Command;

typedef struct {
	uint32_t GlobalTicks;
	int32_t EnergyActive;
	uint32_t EnergyApparent;
	int24_t LinePowerActive;
	uint24_t LinePowerApparent;
	int24_t LinePowerReactive;
	uint24_t CurrentRMS;
	uint24_t VoltageRMS;
	uint24_t CurrentPeak;
	uint24_t VoltagePeak;
	uint16_t Period;
	int8_t Temp;
} sACPM_ADEValues;
