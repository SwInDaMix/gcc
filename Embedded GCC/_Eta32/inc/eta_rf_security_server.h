/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf_security_server.h
 ***********************************************************************/

#pragma once

#include "eta_rf_security_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SECURITY_SERVER

__extension__ typedef enum {
	SecurityClientRegisterFlags_IsActive = (1 << 0),
	SecurityClientRegisterFlags__Free = (uint32_t)0xFFFFFFFF,
	SecurityClientRegisterFlags__Mask = (uint32_t)0xFFFFFFFE,
} eSecurityClientRegisterFlags;

typedef enum {
	SecurityClientChallengeContextState_Free,
	SecurityClientChallengeContextState_Alloced,
	SecurityClientChallengeContextState_Requested,
	SecurityClientChallengeContextState_Challenge,
	SecurityClientChallengeContextState_ChallengeNewKeyRandom,
	SecurityClientChallengeContextState_Registration,
	SecurityClientChallengeContextState_RegistrationConfirmation,
	SecurityClientChallengeContextState_Failed,
	SecurityClientChallengeContextState_NoProperAction,
} eSecurityClientChallengeContextState;

#define SECURITY_CLIENT_ACTION_ENTRIES_COUNT eSecurityActionPressButtons__Last
#define SECURITY_CLIENT_DESCRIPTION_COUNT (44 - eSecurityActionPressButtons__Last)
typedef struct {
	sSecurityID ID;
	sSecurityPayload PrimaryKey;
	sSecurityPayload SecondaryKey;
	eSecurityClientRegisterFlags Flags;
	struct __attribute__((packed)) {
		eSecurityAction Actions[SECURITY_CLIENT_ACTION_ENTRIES_COUNT];
		char Description[SECURITY_CLIENT_DESCRIPTION_COUNT];
	};
} sSecurityClientRegister;

typedef struct {
	sSecurityClientRegister ClientRegisters[1];
} sSecurityServerDisk;

typedef struct {
	eSecurityClientChallengeContextState ClientContextState;
	bool PackageIsPayload;
	sSecurityPackageCommand PackageCommand;
	sSecurityPayload PackagePayload;
	uint32_t TimeOut;
	sSecurityID ChallengerID;
	sSecurityClientRegister const *ChallengerClientRegister;
	eSecurityAction Action;
	uint32_t RegistrationSequence;
	sSecurityPayload Random;
	sSecurityPayload NewKey;
} sSecurityClientSessionContext;

typedef struct {
	sSecurityClientSessionContext SecurityClientChallengeContextes[5];
	uint32_t UniqueID;
	bool IsAllowRegistration;
	uint32_t RegistrationIimeIn;
	sSecurityID const *RegistrationForID;
} sSecurityServerContext;

extern sRF_SPI const RFMSPIServer;

extern sSecurityServerDisk const SecurityServerDisk;
extern uint8_t const SecurityServerDisk_End[];
#define SECURITY_SERVER_DISK_SIZE (SecurityServerDisk_End - ((uint8_t const *)&SecurityServerDisk))
#define SECURITY_SERVER_DISK_PAGES (SECURITY_SERVER_DISK_SIZE >> SECURITY_PAGE_SHIFT)
#define SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT (SECURITY_SERVER_DISK_SIZE / sizeof(sSecurityClientRegister))
#define SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT_PER_PAGE (SECURITY_PAGE_SIZE / sizeof(sSecurityClientRegister))

void Security_ClientRegister_NormalizeBeforeNewWrite(sSecurityClientRegister *client_register);
void Security_ClientRegister_NormalizeFlashArea();
sSecurityClientRegister const *Security_ClientRegister_GetByID(sSecurityID const *id);
sSecurityClientRegister const *Security_ClientRegister_GetValidStartingFromIndex(uint32_t *index);
sSecurityClientRegister const *Security_ClientRegister_GetFree(bool *is_normalized);
eSecurityAction Security_ClientRegister_FindAction(sSecurityClientRegister const *client_register, eSecurityActionPressButtons action_press_buttons);
void Security_ClientRegister_Clobber(sSecurityClientRegister const *client_register);
bool Security_ClientRegister_Modify(sSecurityClientRegister const **client_register_flash, sSecurityClientRegister *client_register_new);
bool Security_ClientRegister_SetSecondaryKey(sSecurityClientRegister const **client_register, sSecurityPayload const *secondary_key);
bool Security_ClientRegister_SetSecondaryKeyAsPrimary(sSecurityClientRegister const **client_register);
sSecurityClientRegister const *Security_ClientRegister_CreateNewOrResetPrimaryKey(sSecurityID const *id, sSecurityPayload const *primary_key);
bool Security_ClientRegister_IsActive(sSecurityClientRegister const *client_register);
bool Security_ClientRegister_ToggleActive(sSecurityClientRegister const **client_register);
bool Security_ClientRegister_SetDescription(sSecurityClientRegister const **client_register, char const *description);

void Security_ServerInit(uint32_t governor_id);
void Security_ServerPoll(uint32_t volatile *timer_counter);
void Security_AllowRegistration(uint32_t volatile *timer_counter, sSecurityID const *id);

void SecuritySystemError_NoFreePipe(sSecurityID const *id);

void SecurityOrdinalEvent_ChallengeInitiated(sSecurityID const *id, eSecurityAction action, sSecurityPayload const *random);
void SecurityOrdinalEvent_ChallengeWonWithPrimaryKey(sSecurityID const *id, eSecurityAction action);
void SecurityOrdinalEvent_ChallengeWonWithSecondaryKey(sSecurityID const *id, eSecurityAction action);
void SecurityOrdinalEvent_ChallengeKeyReplacement(sSecurityID const *id);
void SecurityOrdinalEvent_ChallengeAnotherTryKeyReplacement(sSecurityID const *id);
void SecurityOrdinalEvent_RegistrationInitiated(sSecurityID const *id, uint32_t sequence, sSecurityPayload const *random);
void SecurityOrdinalEvent_RegistrationWonKeyReplacement(sSecurityID const *id);
void SecurityOrdinalEvent_RegistrationConfirmed(sSecurityID const *id);

void SecurityFail_PipeAlreadyRequested(sSecurityID const *id);
void SecurityFail_CommunicationFailed(sSecurityID const *id);
void SecurityFail_SessionIsNotInitiated(uint8_t pipe);
void SecurityFail_SessionWrongClient(sSecurityID const *id, uint8_t pipe);
void SecurityFail_SessionAlreadyFailed(sSecurityID const *id, uint8_t pipe);
void SecurityFail_SessionTimeout(sSecurityID const *id, eSecurityClientChallengeContextState state);
void SecurityFail_UnexpectedResponseForSession(sSecurityID const *id);
void SecurityFail_NoRegistrationForClient(sSecurityID const *id);
void SecurityFail_NoProperActionForButtons(sSecurityID const *id, eSecurityActionButtons buttons);
void SecurityFail_ClientIsNotActive(sSecurityID const *id);
void SecurityFail_ChallengeLost(sSecurityID const *id);
void SecurityFail_RegistrationIsNotAllowed(sSecurityID const *id);
void SecurityFail_RegistrationFromWrongClient(sSecurityID const *id1, sSecurityID const *id2);
void SecurityFail_RegistrationAlreadyExists(sSecurityID const *id);
void SecurityFail_RegistrationLost(sSecurityID const *id);
void SecurityFail_RegistrationDenied(sSecurityID const *id);
void SecurityFail_RegistrationAllowanceTimeout();
void SecurityFail_UnexpectedCommand(sSecurityID const *id, eSecurityCommand command);

bool Security_PerformAction(eSecurityAction action);

#endif

#ifdef __cplusplus
}
#endif
