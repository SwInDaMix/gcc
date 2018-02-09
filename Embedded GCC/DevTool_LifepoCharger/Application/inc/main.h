/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.h
 ***********************************************************************/

#pragma once

#include "config.h"

#define PERIOD_1S_SHIFT 5
#define PERIOD_1S (1 << PERIOD_1S_SHIFT)
#define PERIOD_1S_HALF (1 << (PERIOD_1S_SHIFT - 1))
#define PERIOD_2S (1 << (PERIOD_1S_SHIFT + 1))
#define PERIOD_RELIEVE 60

//#define ADC_CURRENT_adc(adc0, adc_act) ((int16_t)(adc_act - adc0))
#define ADC_CURRENT_adc(adc0, adc_act) ((int16_t)(adc0 - adc_act))
#define ADC_CURRENT_Value(v, cal, shl) ((int32_t)((((int32_t)v) * (ADC_AREF * 2500UL / cal))) >> (ADC_BITS + shl - 2))
#define ADC_VOLTAGE_Value(v, cal, shl) ((((uint32_t)v) * cal) >> (ADC_BITS + shl))
#define ADC_VOLTAGE_DivThresholdOn (1023 - 52)
#define ADC_VOLTAGE_DivThresholdOff ((1023 - 104) / 3)

typedef enum {
	CCCellType_LiFePo4,
	CCCellType_LiFePo4_Full,
	CCCellType_LiIon,
	CCCellType_LiPo,
	CCCellType_PbAcid6,
	CCCellType_PbAcid12,
	CCCellType__Last,
} eCC_CellType;

typedef enum {
	CCButton_Escape = 0x01,
	CCButton_Prev = 0x02,
	CCButton_Next = 0x04,
	CCButton_Enter = 0x08,
} eCC_Button;

typedef enum {
	CCScreen_Program,
	CCScreen_MemoryInfo,
	CCScreen_MemoryValues,
	CCScreen_StatePower,
	CCScreen_StatePowerAdditional,
	CCScreen_Calibration,
	CCScreen__Last,
} eCC_Screen;

typedef enum {
	CCProgram_Charge1,
	CCProgram_Charge2,
	CCProgram_Charge3,
	CCProgram_Charge4,
	CCProgram_Disharge1,
	CCProgram_Disharge2,
	CCProgram_Disharge3,
	CCProgram_Disharge4,
	CCProgram__Last,
	CCProgram__MaskDischarge = 0x04,
	CCProgram__MaskCount = 0x03,
} eCC_Program;

typedef enum {
	CCState_Idle,
	CCState_Discharge,
	CCState_Relieve,
	CCState_Charge,
} eCC_State;

typedef enum {
	CCCalibrationValue_Voltage,
	CCCalibrationValue_Current,
	CCCalibrationValue_CurrentOffset,
	CCCalibrationValue__Last,
} eCC_CalibrationValue;

typedef enum {
	ACPMCommand_DebugString,
	ACPMCommand_NewState,
	ACPMCommand_ValuesUpdated,
} eCC_Command;

typedef struct {
	uint24_t cell_current;
	uint16_t cell_voltage;
	uint16_t sensed_voltage;
} sCC_Values;

#define SDCARD_SD_SIGNATURE 0x3CF18EDA
#define SDCARD_ENTRY_SIGNATURE 0x2C483AC7		// new 16 bit voltage value version
//#define SDCARD_ENTRY_SIGNATURE 0xD236FAC4		// old 24 bit voltage value version
#define CC_CALIBRATION_SIGNATURE 0x482C
#define CC_CALIBRATION_DEF_voltage_aref (ADC_AREF + 30)
#define CC_CALIBRATION_DEF_current_mv_per_amp (ADC_MV_PER_AMP + 4)
#define CC_CALIBRATION_DEF_current_offset 0
#define CC_CALIBRATION_DEF_wire_mohm 5
typedef struct {
	uint16_t signature;
	uint16_t voltage_aref;
	uint16_t current_mv_per_amp;
	int8_t current_offset;
	uint8_t wire_mohm;
} sCC_Calibration;

#define CC_MEMORY_CYCLES ((E2END + 1 - sizeof(sCC_Calibration) - sizeof(sCC_MemoryCountIndex) - sizeof(uint32_t)) / sizeof(sCC_Cycle))
#define CC_MEMORY_SIGNATURE 0x5AD1
typedef struct {
	uint24_t seconds;
	uint16_t cap_ah;
	uint16_t cap_power;
} sCC_CycleValue;
typedef union {
	struct{
		sCC_CycleValue discharge;
		sCC_CycleValue charge;
	};
} sCC_Cycle;
typedef union {
	struct {
		uint16_t count;
		uint16_t index;
	};
	uint32_t value;
} sCC_MemoryCountIndex;
typedef struct {
	uint16_t signature;
	sCC_MemoryCountIndex countindex;
	sCC_Cycle cycles[CC_MEMORY_CYCLES];
} sCC_Memory;
