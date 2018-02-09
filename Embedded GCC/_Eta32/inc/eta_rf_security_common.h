/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf_security_common.h
 ***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined SECURITY_SERVER || defined SECURITY_CLIENT

#include "aes.h"
#include "memxor.h"
#include "eta_dbg.h"
#include "eta_delay.h"
#include "eta_crc.h"
#include "eta_rf.h"

typedef enum {
	SecurityActionButton_0 = 0x01,
	SecurityActionButton_1 = 0x02,
	SecurityActionButton_2 = 0x04,
	SecurityActionButton_3 = 0x08,
	SecurityActionButton_Long = 0x10,
} eSecurityActionButtons;

typedef enum {
	eSecurityActionPressButtons_1,
	eSecurityActionPressButtons_2,
	eSecurityActionPressButtons_3,
	eSecurityActionPressButtons_4,
	eSecurityActionPressButtons_Long1,
	eSecurityActionPressButtons_Long2,
	eSecurityActionPressButtons_Long12,
	eSecurityActionPressButtons_Long3,
	eSecurityActionPressButtons_Long13,
	eSecurityActionPressButtons_Long23,
	eSecurityActionPressButtons_Long123,
	eSecurityActionPressButtons_Long4,
	eSecurityActionPressButtons_Long14,
	eSecurityActionPressButtons_Long24,
	eSecurityActionPressButtons_Long124,
	eSecurityActionPressButtons_Long34,
	eSecurityActionPressButtons_Long134,
	eSecurityActionPressButtons_Long234,
	eSecurityActionPressButtons_Long1234,
	eSecurityActionPressButtons__Last,
	eSecurityActionPressButtons_None = -1,
} eSecurityActionPressButtons;

typedef enum {
	SecurityAction_None,
	SecurityAction_Free = 0xFF,
} eSecurityAction;

typedef enum {
	SecurityCommand__WithPayload = 0x80,

	SecurityCommand_ChallengeRequest = 0x00,
	SecurityCommand_ChallengeRandom = 0x01 | SecurityCommand__WithPayload,
	SecurityCommand_ChallengeResponse = 0x02 | SecurityCommand__WithPayload,
	SecurityCommand_ChallengeNewKey = 0x03 | SecurityCommand__WithPayload,
	SecurityCommand_ChallengeNewKeyRandom = 0x04 | SecurityCommand__WithPayload,
	SecurityCommand_ChallengeNewKeyResponse = 0x05 | SecurityCommand__WithPayload,
	SecurityCommand_ChallengeWonAcknowledgment = 0x06,
	SecurityCommand_RegistrationRequest = 0x20,
	SecurityCommand_RegistrationRandom = 0x21 | SecurityCommand__WithPayload,
	SecurityCommand_RegistrationResponse = 0x22 | SecurityCommand__WithPayload,
	SecurityCommand_RegistrationNewKey = 0x23 | SecurityCommand__WithPayload,
	SecurityCommand_RegistrationConfirmNewKey = 0x24 | SecurityCommand__WithPayload,

	SecurityCommand__ForceDword = 0x7FFFFFFF,
} eSecurityCommand;

typedef struct {
	uint32_t ID[4];
} sSecurityID;

typedef struct {
	uint32_t Data[8];
} sSecurityPayload;

typedef struct {
	sSecurityID ClientID;
	uint32_t ServerID;
	uint32_t Parameter;
} sSecurityPackageRequest;

typedef struct {
	sSecurityID ID;
	eSecurityCommand Command;
	uint32_t PayloadCRC;
	uint32_t Parameter1;
	uint32_t Parameter2;
} sSecurityPackageCommand;

extern sSecurityPayload const __key_free;
extern sSecurityID const __id_free;
extern sSecurityID const __id_clobbered;

void Flash_PageErase(void const *addr);
bool Flash_Write(void const *addr, void const *data, size_t len);

uint32_t Security_RandomGet();
void Security_RandomFill(uint32_t *dst, uint32_t cnt);

void Security_CipherEncode(uint32_t const key[8], void *data, uint32_t blockcnt);
void Security_CipherDecode(uint32_t const key[8], void *data, uint32_t blockcnt);

char const *Security_GetUserFriendlySequence(uint32_t sequence);

#endif

#ifdef __cplusplus
}
#endif
