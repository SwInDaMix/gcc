/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : meolody.h
 ***********************************************************************/

#pragma once

#include "config.h"

typedef enum {
	BuzzerNote_Silence,
	BuzzerNote_C,
	BuzzerNote_Cd,
	BuzzerNote_D,
	BuzzerNote_Dd,
	BuzzerNote_E,
	BuzzerNote_F,
	BuzzerNote_Fd,
	BuzzerNote_G,
	BuzzerNote_Gd,
	BuzzerNote_A,
	BuzzerNote_Ad,
	BuzzerNote_H,
} eBuzzerNote;

typedef enum {
	BuzzerOcatva_SubContr,
	BuzzerOcatva_Contr,
	BuzzerOcatva_Big,
	BuzzerOcatva_Small,
	BuzzerOcatva_1,
	BuzzerOcatva_2,
	BuzzerOcatva_3,
	BuzzerOcatva_4,
	BuzzerOcatva_5,
	BuzzerOcatva_6,
	BuzzerOcatva_7,
} eBuzzerOctave;

typedef struct {
	uint16_t Duration:8;
	uint16_t Octave:4;
	uint16_t Note:4;
} sBuzzerQuant;

typedef void (*d_buzzer_block)();
typedef void (*d_buzzer_irq_safe)(d_buzzer_block block);
typedef void (*d_buzzer_set_wave)(eBuzzerNote note, eBuzzerOctave octave);
typedef void (*d_buzzer_set_silence)();

typedef struct {
    d_buzzer_irq_safe irq_safe;
    d_buzzer_set_wave set_wave;
    d_buzzer_set_silence set_silence;
} sBuzzer_Interface;

void Buzzer_Periodic();
void Buzzer_Init(sBuzzer_Interface const *buzzer_interface);

extern sBuzzerQuant const BuzzerMelody_Wisp[];
extern sBuzzerQuant const BuzzerMelody_Error[];
extern sBuzzerQuant const BuzzerMelody_Success[];
extern sBuzzerQuant const BuzzerMelody_LowTank[];
extern sBuzzerQuant const BuzzerMelody_CleanMembrane[];
extern sBuzzerQuant const BuzzerMelody_FatalError[];
extern sBuzzerQuant const BuzzerMelody_NewYearTree[];
extern sBuzzerQuant const BuzzerMelody_MoonlightSonata14[];
extern sBuzzerQuant const BuzzerMelody_BaskervillDog[];
extern sBuzzerQuant const BuzzerMelody_Trepak[];
extern sBuzzerQuant const BuzzerMelody_TurkishMarch[];
extern sBuzzerQuant const BuzzerMelody_Badinerie[];
extern sBuzzerQuant const BuzzerMelody_Kan_Kan[];
extern sBuzzerQuant const BuzzerMelody_Mozart40[];
