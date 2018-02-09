/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf_security_client.h
 ***********************************************************************/

#pragma once

#include "eta_rf_security_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SECURITY_CLIENT

typedef enum {
	SecurityClientState_Initial,
	SecurityClientState_Receiving,
	SecurityClientState_RegisterSequence,
	SecurityClientState_PowerOff,
} eSecurityClientState;

typedef enum {
	SecurityClientReceivingType_ChallengeRandom,
	SecurityClientReceivingType_ChallengeNewKeyOrWonAcknowledgment,
	SecurityClientReceivingType_ChallengeNewKeyResponse,
	SecurityClientReceivingType_RegistrationRandom,
	SecurityClientReceivingType_RegistrationNewKey,
} eSecurityClientReceivingType;

typedef union {
	struct {
		sSecurityID ClientID;
		uint32_t _reserved1[12];
	};
	struct {
		uint32_t ServerID;
		sSecurityPayload Key;
		uint32_t _reserved2[7];
	};
} sSecurityClientInfo;

typedef struct {
	sSecurityClientInfo ClientInfos[1];
} sSecurityClientDisk;

typedef struct {
	uint32_t ServerID;
	sSecurityID CurrentID;
	sSecurityClientInfo ClientInfo;
	bool PackageIsPayload;
	sSecurityPackageCommand PackageCommand;
	sSecurityPayload PackagePayload;
	eSecurityClientState ClientState;
	eSecurityClientReceivingType ReceivingType;
	uint32_t TimeIn;
	sSecurityPayload RegistrationRandom;
	uint32_t KeypressSequence;
	uint32_t KeypressSequenceCount;
	bool IsFullReset;
} sSecurityClientContext;

extern sRF_SPI const RFMSPIClient;

extern sSecurityClientDisk const SecurityClientDisk;
extern uint8_t const SecurityClientDisk_End[];
#define SECURITY_CLIENT_DISK_SIZE (SecurityClientDisk_End - ((uint8_t const *)&SecurityClientDisk))
#define SECURITY_CLIENT_DISK_PAGES (SECURITY_CLIENT_DISK_SIZE >> SECURITY_PAGE_SHIFT)
#define SECURITY_CLIENT_DISK_CLIENT_INFOS_COUNT (SECURITY_CLIENT_DISK_SIZE / sizeof(sSecurityClientInfo))
#define SECURITY_CLIENT_DISK_CLIENT_INFOS_COUNT_PER_PAGE (SECURITY_PAGE_SIZE / sizeof(sSecurityClientInfo))

void Security_ClientInit();
void Security_ClientPoll(uint32_t volatile *timer_counter, eSecurityActionPressButtons *action_buttons);

sSecurityClientInfo const *ClientInfo_GetValid(uint32_t server_id);
void ClientInfo_FullReset();
void ClientInfo_Clobber(sSecurityClientInfo const *client_info);
void ClientInfo_NormalizeBeforeNewWrite(sSecurityClientInfo *client_info);
void ClientInfo_NormalizeFlashArea(sSecurityID const *client_id);
void ClientInfo_SetNew(sSecurityID const *client_id, sSecurityClientInfo *client_info_new);

void SecurityOrdinalEvent_ButtonsPressed(eSecurityActionButtons buttons);
void SecurityOrdinalEvent_GeneratedNewID(sSecurityID const *id);
void SecurityOrdinalEvent_FullResetInitiated(sSecurityID const *id);
void SecurityOrdinalEvent_FullResetFailed();
void SecurityOrdinalEvent_FullResetConfirmed();
void SecurityOrdinalEvent_RegistrationInitiated(sSecurityID const *id);
void SecurityOrdinalEvent_RegistrationSequence(uint32_t id);
void SecurityOrdinalEvent_RegistrationNewKey(uint32_t id, sSecurityPayload const *new_key);
void SecurityOrdinalEvent_ChallengeInitiated(sSecurityID const *accessor_id, uint32_t governor_id);
void SecurityOrdinalEvent_ChallengeAttempt(uint32_t id);
void SecurityOrdinalEvent_ChallengeNewKey(uint32_t id, sSecurityPayload const *new_key);
void SecurityOrdinalEvent_ChallengeNewKeyApproved(uint32_t id, sSecurityPayload const *new_key);
void SecurityOrdinalEvent_ChallengeWon(uint32_t id, bool action_success);
void SecurityOrdinalEvent_AllDone();

void SecurityFail_NeverRegistered();
void SecurityFail_SessionWrongClient(sSecurityID const *id);
void SecurityFail_SessionWrongServer(uint32_t id);
void SecurityFail_NoRegistrationForServer(uint32_t id);
void SecurityFail_ChallengeNewKeyFailed(uint32_t id, sSecurityPayload const *new_key);
void SecurityFail_UnexpectedResponseForSession(uint32_t id);
void SecurityFail_UnexpectedCommand(uint32_t id, eSecurityCommand command);

#endif

#ifdef __cplusplus
}
#endif
