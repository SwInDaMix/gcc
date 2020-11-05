/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : entry.h
 ***********************************************************************/

#pragma once

#include "config.h"

typedef enum {
	FRAME_CMD_DEBUG_STRING,
	FRAME_CMD_STATUS,
	FRAME_CMD_NEW_PARAMS,
	FRAME_CMD_TEST_COMMAND
} eFrameCmd;

void Int_Update();
void Valve_WaterInlet(bool state);
void Valve_CleanMembrane(bool state);

void EtaReverseOsmosisMainLoop(bool volatile *is_abort_requested, bool volatile *is_inited, void (*yield)());
