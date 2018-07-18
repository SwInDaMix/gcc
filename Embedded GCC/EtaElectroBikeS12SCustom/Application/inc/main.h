#pragma once

// Considering the follow voltage values for each li-ion battery cell
// State of charge 		| voltage

#define ADC_BATTERY_VOLTAGE_K 73 // 0.272 << 8

// Phase current: max of +-15.5 amps
// 512 --> 15.5 amps
// 1 ADC increment --> 0.030A
// 1 ADC RMS --> 0.03 * 0.707 -- > 0.021A
#define ADC_PHASE_B_CURRENT_STEP 21 // 0.021 * 1000 = 21
#define ADC_PHASE_B_CURRENT_FACTOR_MA 21 // 0,030273437 * 1000 * 0.707


extern uint16_t ui16_log1;
extern uint16_t ui16_log2;
extern uint8_t ui8_log;
extern uint8_t ui8_adc_read_throttle_busy;
extern uint16_t ui16_SPEED_Counter;    //Counter for bike speed
extern uint16_t ui16_SPEED;        //Speed duration of one wheel revolution (tics * 64us)
extern uint16_t ui16_PAS_Counter;    //Counter for cadence
extern uint16_t ui16_PAS_High_Counter;
extern uint8_t ui8_PAS_Flag;        //Flag for PAS Interrupt detected
extern uint8_t ui8_SPEED_Flag;        //Flag for PAS Interrupt detected
extern uint8_t ui8_assistlevel_global;
extern uint8_t ui8_msg_received;
extern uint8_t PAS_dir;            //flag for pasdirection

uint8_t update_advance_angle();
