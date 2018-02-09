/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : melody.c
 ***********************************************************************/

#include "stdperiph.h"
#include "melody.h"

typedef enum {
	Temp_largo = 44,            		// Очень медленно
	Temp_adagio = 52,           		// Медленно, спокойно
	Temp_lento = 56,            		// Медленно, слабо, тихо, скорее, чем largo
	Temp_larghetto = 60,        		// Довольно широко
	Temp_andante = 66,          		// Умеренный темп, в характере шага
	Temp_andantino = 74,        		// Скорее, чем andante, но медленнее, чем allegretto
	Temp_moderato = 88,         		// Умеренно, сдержанно
	Temp_allegretto = 104,      		// Медленнее, чем allegro, но скорее, чем andante
	Temp_animato = 120,         		// Оживлённо
	Temp_allegro = 132,         		// Скорый темп (буквально: «весело»)
	Temp_allegro_assai = 144,   		// Весьма быстро
	Temp_allegro_vivace = 152,  		// Значительно быстро
	Temp_vivace = 168,          		// Быстро, живо, быстрее, чем allegro, медленнее, чем presto
	Temp_molto_vivace = 170,    		// Быстрее, чем vivace
	Temp_presto = 184,          		// Быстро
	Temp_prestissimo = 208,     		// В высшей степени быстро. Очень быстро
	Temp_animato2 = Temp_animato * 2,	// Оживлённо * 2
	Temp_vivace2 = Temp_vivace * 2,		// Быстро * 2
} eBuzzer_Temp;

//Пересчет темпа (по метроному) в длительность такта прерывания
#define BUZZER_GET_TEMP_TICKS(x) (60.0 * GLOBAL_TICKS_PER_SECOND * 1 / x)

//Длительности звучания (от длительности такта прерывания x в милисекундах)

#define BUZZER_DRT_1(x)     ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)))
#define BUZZER_DRT_7D8(x)   ((uint8_t)((BUZZER_GET_TEMP_TICKS(x)*7)/8))
#define BUZZER_DRT_1D2(x)   ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/2))
#define BUZZER_DRT_1D3(x)   ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/3))
#define BUZZER_DRT_1D4(x)   ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/4))
#define BUZZER_DRT_1D6(x)   ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/6))
#define BUZZER_DRT_3D4(x)   ((uint8_t)((BUZZER_GET_TEMP_TICKS(x)*3)/4))
#define BUZZER_DRT_1D8(x)   ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/8))
#define BUZZER_DRT_3D8(x)   ((uint8_t)((BUZZER_GET_TEMP_TICKS(x)*3)/8))
#define BUZZER_DRT_3D16(x)  ((uint8_t)((BUZZER_GET_TEMP_TICKS(x)*3)/16))
#define BUZZER_DRT_1D12(x)  ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/12))
#define BUZZER_DRT_1D24(x)  ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/24))
#define BUZZER_DRT_1D16(x)  ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/16))
#define BUZZER_DRT_1D32(x)  ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/32))
#define BUZZER_DRT_1D64(x)  ((uint8_t)(BUZZER_GET_TEMP_TICKS(x)/64))

// Wisp
sBuzzerQuant const BuzzerMelody_Wisp[] = {
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Dd },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_E },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_F },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Fd },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_G },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Gd },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_A },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Ad },
	{ .Duration = 1, .Octave = BuzzerOcatva_3, .Note = BuzzerNote_H },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

// Error
sBuzzerQuant const BuzzerMelody_Error[] = {
	{ .Duration = BUZZER_DRT_1D12(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_allegro), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_allegro), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D6(Temp_allegro), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_C },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

// Success
sBuzzerQuant const BuzzerMelody_Success[] = {
	{ .Duration = BUZZER_DRT_1D12(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D6(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

// Low tank
sBuzzerQuant const BuzzerMelody_LowTank[] = {
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

// Clean membrane
sBuzzerQuant const BuzzerMelody_CleanMembrane[] = {
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

// Fatal error
sBuzzerQuant const BuzzerMelody_FatalError[] = {
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D2(Temp_allegro), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_C },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

//NewYearTree
sBuzzerQuant const BuzzerMelody_NewYearTree[] = {
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_3D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_3D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

//Moonlight Sonata (#14)
sBuzzerQuant const BuzzerMelody_MoonlightSonata14[] = {
	//1
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },

	//2
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },

	//3
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },

	//4
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//5
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//6
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//7
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },

	//8
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//9
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },

	//10
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//11
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//12
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },

	//13
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },

	//14
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },

	//15
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },

	//16
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//17
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },

	//18
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//19
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//20
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },

	//21
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//22
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },

	//23
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },

	//24
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },

	//25
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//26
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//27
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },

	//28
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//29
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//30
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//31
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },

	//32
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//33
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//34
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },

	//35
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Dd },

	//36
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//37
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//38
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//39
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//40
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },

	//41
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },

	//42
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//43
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//44
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },

	//45
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	// { .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//46
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },

	//47
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },

	//48
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//49
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },

	//50
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//51
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//52
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//53
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//54
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//55
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//56
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },

	//57
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },

	//58
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//59
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },

	//60
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },


	//61
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },

	//62
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },

	//63
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },

	//64
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },

	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },

	//65
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D24(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },

	//66
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },

	//67
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D12(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_E },

	//68
	{ .Duration = BUZZER_DRT_1D4(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D64(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D64(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D4(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },

	//69
	{ .Duration = BUZZER_DRT_1D64(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D64(Temp_adagio), .Octave = BuzzerOcatva_Big, .Note = BuzzerNote_Gd },
	{ .Duration = BUZZER_DRT_1D64(Temp_adagio), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D4(Temp_adagio), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Cd },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

//Baskervill Dog
sBuzzerQuant const BuzzerMelody_BaskervillDog[] = {
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//1
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//2
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//3
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//4
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//5
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//6
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//7
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//8
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D16(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_Small, .Note = BuzzerNote_A },

	//9
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D2(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//10
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_3D4(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },

	//11
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_3D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//12
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D2(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//13
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D2(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//14
	{ .Duration = BUZZER_DRT_1D4(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D2(Temp_allegro), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

//П. Чайковский, "Русский марш" ("Трепак")
//из сюиты на музыку балета "Щелкунчик"
sBuzzerQuant const BuzzerMelody_Trepak[] = {
    { .Duration = BUZZER_DRT_1D32(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D32(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D32(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },

	//1
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//2
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//3
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//4
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//5
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//6
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//7
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//8
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },

	//9
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//10
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//11
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//12
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//13
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//14
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//15
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_prestissimo), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//16
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_prestissimo), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

//В.А. Моцарт "Турецкий Марш"
//T=6s
sBuzzerQuant const BuzzerMelody_TurkishMarch[] = {
	//0
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//1
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//2
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//3
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Gd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Gd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//4
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },

	//5
    { .Duration = BUZZER_DRT_1D64(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D64(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//6
    { .Duration = BUZZER_DRT_1D64(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D64(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//7
    { .Duration = BUZZER_DRT_1D64(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D64(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },

	//8
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

//И.С. Бах "Шутка"
//T=12s
sBuzzerQuant const BuzzerMelody_Badinerie[] = {
	//0
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//1
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//2
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//3
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Gd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },

	//4
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//5
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//6
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//7
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//8
    { .Duration = BUZZER_DRT_1D32(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D32(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Cd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//9
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//10
    { .Duration = BUZZER_DRT_1D32(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D32(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//11
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },

	//12
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	//13
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	//14
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//15
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D32(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D32(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D16(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },

	//16
    { .Duration = BUZZER_DRT_1D8(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_allegro_vivace), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};


// T:41s
sBuzzerQuant const BuzzerMelody_Kan_Kan[] = {
	//1
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//2
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//3
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//4
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//5
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//6
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//7
    { .Duration = BUZZER_DRT_1D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_3D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_3D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//8
    { .Duration = BUZZER_DRT_1D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_3D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_3D16(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//9
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//10
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//11
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//12
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//13
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//14
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//15
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//16
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//17
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//18
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//19
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//20
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//21
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//22
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//23
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },

	//24
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//25
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//26
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//27
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//28
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//29
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//30
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//31
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Fd },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },

	//32
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//33
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_H },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//34
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//35
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
    { .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//36
	{ .Duration = BUZZER_DRT_1(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//37
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//38
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//39
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },

	//40
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//41
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//42
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//43
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//44
	{ .Duration = BUZZER_DRT_1(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//45
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//46
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//47
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },

	//48
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//49
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//50
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//51
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//52
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//53
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//54
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//55
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D2(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//56
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//57
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },

	//58
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//59
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//60
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//61
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_A },

	//62
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//63
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	//64
	{ .Duration = BUZZER_DRT_1(Temp_vivace2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};

sBuzzerQuant const BuzzerMelody_Mozart40[] = {
	//1
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//2
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//3
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//4
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//5
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//6
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//7
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//8
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//9
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//10
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//11
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//12
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//13
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	//14
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//15
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//16
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Fd },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },

	//17
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },

	//18
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },

	//19
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },

	//20
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D16(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//21
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//22
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//23
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//24
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	//25
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	//26
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },

	//27
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },

	//28
	{ .Duration = BUZZER_DRT_1(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },

	//29
	{ .Duration = BUZZER_DRT_7D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//30
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },

	//31
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_3D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },

	//32
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Dd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_C },

	//33
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_D },
	{ .Duration = BUZZER_DRT_3D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//34
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//35
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//36
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },

	//37
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },

	//38
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//39
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D4(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//40
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_C },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_A },

	//41
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_3, .Note = BuzzerNote_Cd },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_Ad },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_E },
	{ .Duration = BUZZER_DRT_1D8(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_G },

	//42
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_2, .Note = BuzzerNote_F },
	{ .Duration = BUZZER_DRT_1D2(Temp_animato2), .Octave = BuzzerOcatva_1, .Note = BuzzerNote_F },

	{ .Duration = 0, .Octave = 0, .Note = 0 },
};
