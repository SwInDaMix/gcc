/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : CPeriph.cpp
 ***********************************************************************/

#include "eta_dbg.h"

#include "CPeriph.hpp"

#include "Keyboard.CKeyboard.hpp"

using namespace System;
using namespace System::Drawing;
using namespace System::Keyboard;

#define KEY_REPEAT_DELAY_FIRST (500 / 20)
#define KEY_REPEAT_DELAY_NEXT (50 / 20)
#define KEY_REPEAT_DISABLE 255

/// *********************************************************************
/*static*/ SRect CPeriph::s_rc_clip;

/*static*/ SRect const &CPeriph::LCD_ClipRect_get() { return s_rc_clip; }
/*static*/ void CPeriph::LCD_ClipRect_set(SRect const &rc) { s_rc_clip = rc; }

uint32_t kbd_keys_down_counter, kbd_keys_down_counter_prev;
EKey kbd_key, kbd_key_prev;
EKeyModifierFlags kbd_key_modifier, kbd_key_modifier_prev;
uint8_t kbd_repeat_counter;

void _KBD_ProceedKeyDown(EKeyScan key_scan) {
	DBG_PutFormat("KeyDown: %ud\n", key_scan);
	CKeyboard::FireKeyDown(key_scan);
}
void _KBD_ProceedKeyUp(EKeyScan key_scan) {
	DBG_PutFormat("KeyUp: %ud\n", key_scan);
	CKeyboard::FireKeyUp(key_scan);
}
void _KBD_ProceedKeyCanceled() {
	DBG_PutString("KeyCanceled:\n");
	CKeyboard::FireKeyCanceled();
}
void _KBD_ProceedKeyPressed(EKey key, EKeyModifierFlags key_modifier) {
	DBG_PutFormat("KeyPressed: %ud (%ud)\n", key, key_modifier);
	CKeyboard::FireKeyPressed(key, key_modifier);
}
/*static*/ void CPeriph::KBD_20ms() {
	if(kbd_keys_down_counter) {
		if(kbd_keys_down_counter > 1) { if(kbd_repeat_counter != KEY_REPEAT_DISABLE) { kbd_repeat_counter = KEY_REPEAT_DISABLE; _KBD_ProceedKeyCanceled(); } }
		else {
			if(kbd_keys_down_counter_prev) {
				if(kbd_key == kbd_key_prev && kbd_key_modifier == kbd_key_modifier_prev) {
					if(!kbd_repeat_counter) { _KBD_ProceedKeyPressed(kbd_key, kbd_key_modifier); kbd_repeat_counter = KEY_REPEAT_DELAY_NEXT; }
					else if(kbd_repeat_counter != KEY_REPEAT_DISABLE) kbd_repeat_counter--;
				}
				else { if(kbd_repeat_counter != KEY_REPEAT_DISABLE) { kbd_repeat_counter = KEY_REPEAT_DISABLE; _KBD_ProceedKeyCanceled(); } }
			}
			else { _KBD_ProceedKeyPressed(kbd_key, kbd_key_modifier); kbd_repeat_counter = KEY_REPEAT_DELAY_FIRST; }
		}
	}
	kbd_key_prev = kbd_key; kbd_key_modifier_prev = kbd_key_modifier; kbd_keys_down_counter_prev = kbd_keys_down_counter;
}
/*static*/ void CPeriph::KBD_Reset() {
	kbd_keys_down_counter = kbd_keys_down_counter_prev = 0;
	kbd_key_modifier = (EKeyModifierFlags)0;
}
/*static*/ void CPeriph::KBD_KeyDown(EKeyScan key_scan) {
	_KBD_ProceedKeyDown(key_scan);
	EKeyModifierFlags _key_modifier = KBD_IsModifier(key_scan);
	if(flags(_key_modifier)) kbd_key_modifier |= _key_modifier;
	else {
		if(!kbd_keys_down_counter) kbd_key = (EKey)key_scan;
		else kbd_key = (EKey)0;
		kbd_keys_down_counter++;
	}
}
/*static*/ void CPeriph::KBD_KeyUp(EKeyScan key_scan) {
	_KBD_ProceedKeyUp(key_scan);
	EKeyModifierFlags _key_modifier = KBD_IsModifier(key_scan);
	if(flags(_key_modifier)) kbd_key_modifier &= ~_key_modifier;
	else {
		kbd_keys_down_counter--;
		if(!kbd_keys_down_counter) kbd_key = (EKey)0;
	}
}
/// *********************************************************************

void Periph_Init() { CPeriph::Init(); }
void Periph_TIM_20ms() { CPeriph::TIM_20ms(); }
void Periph_KBD_20ms() { CPeriph::KBD_20ms(); }
