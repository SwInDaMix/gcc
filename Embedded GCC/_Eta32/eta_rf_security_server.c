/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf_security_server.c
 ***********************************************************************/

#include "eta_rf_security_server.h"

#ifdef SECURITY_SERVER

// RFM setting of Security Server point
static sRF_Settings const RF_SettingsSecurityServer = {
	.CONFIG = RFConfig_EN_CRC | RFConfig_CRCO,
	.SETUP_AW = RFSetupAW_5Bytes,
	.SETUP_RETR = RFSetupRetr_ARC_15Time | RFSetupRetr_ARD_500us,
	.RF_CH = RF_SECURITY_CHANNEL,
	.RF_SETUP = RFRFSetup_LNA_HCURR | RFRFSetup_RF_PWR_High | RFRFSetup_RF_DR_1Mbps,
	.FEATURE = RFFeature_EN_DYN_ACK | RFFeature_EN_DPL,
	.ENAA = RFPipe_0 | RFPipe_1 | RFPipe_2 | RFPipe_3 | RFPipe_4 | RFPipe_5,
	.EN_RX_ADDR = RFPipe_0 | RFPipe_1 | RFPipe_2 | RFPipe_3 | RFPipe_4 | RFPipe_5,
	.RX_ADDR_P0 = { RF_SECURITY_ADDR_REQUEST, RF_SECURITY_ADDRPREF },
	.RX_ADDR_P1 = { RF_SECURITY_ADDR_CLIENT_START, RF_SECURITY_ADDRPREF },
	.RX_ADDR_P2 = RF_SECURITY_ADDR_CLIENT_START + 1,
	.RX_ADDR_P3 = RF_SECURITY_ADDR_CLIENT_START + 2,
	.RX_ADDR_P4 = RF_SECURITY_ADDR_CLIENT_START + 3,
	.RX_ADDR_P5 = RF_SECURITY_ADDR_CLIENT_START + 4,
	.RX_PW_P0 = sizeof(sSecurityPackageRequest),
	.RX_PW_P1 = sizeof(sSecurityPackageCommand),
	.RX_PW_P2 = sizeof(sSecurityPackageCommand),
	.RX_PW_P3 = sizeof(sSecurityPackageCommand),
	.RX_PW_P4 = sizeof(sSecurityPackageCommand),
	.RX_PW_P5 = sizeof(sSecurityPackageCommand)
};

#define RF_PIPE_SERVER_REQUESTOR 0
#define RF_PIPE_CLIENT_1 1
#define RF_PIPE_CLIENT_2 2
#define RF_PIPE_CLIENT_3 3
#define RF_PIPE_CLIENT_4 4
#define RF_PIPE_CLIENT_5 5

static sSecurityServerContext ServerContext;
static uint8_t ClientAddr[5] = { 0, RF_SECURITY_ADDRPREF };

static void ClientContext_CheckChallengingTimeout(uint32_t volatile *timer_counter) {
	sSecurityClientSessionContext *_client_session_context_ptr = ServerContext.SecurityClientChallengeContextes; uint32_t _client_session_context_cnt = sizeof_array(ServerContext.SecurityClientChallengeContextes);
	while(_client_session_context_cnt--) {
		if(_client_session_context_ptr->ClientContextState != SecurityClientChallengeContextState_Free && ((int32_t)(_client_session_context_ptr->TimeOut - *timer_counter)) <= 0) {
			if(_client_session_context_ptr->ClientContextState == SecurityClientChallengeContextState_Challenge || _client_session_context_ptr->ClientContextState == SecurityClientChallengeContextState_Registration || _client_session_context_ptr->ClientContextState == SecurityClientChallengeContextState_RegistrationConfirmation) SecurityFail_SessionTimeout(&_client_session_context_ptr->ChallengerID, _client_session_context_ptr->ClientContextState);
			_client_session_context_ptr->ClientContextState = SecurityClientChallengeContextState_Free;
		}
		_client_session_context_ptr++;
	}
}
static uint8_t ClientContext_FindByIDOrAllocNew(uint32_t volatile *timer_counter, sSecurityID const *id, bool is_alloc_if_not_found) {
	uint8_t _pipe = 0, _free_pipe = (uint8_t)-1;
	sSecurityClientSessionContext *_client_session_context_ptr = ServerContext.SecurityClientChallengeContextes; uint32_t _client_session_context_cnt = sizeof_array(ServerContext.SecurityClientChallengeContextes);
	while(_client_session_context_cnt--) {
		if(is_alloc_if_not_found && _client_session_context_ptr->ClientContextState == SecurityClientChallengeContextState_Free && _free_pipe == (uint8_t)-1) _free_pipe = _pipe;
		if(_client_session_context_ptr->ClientContextState != SecurityClientChallengeContextState_Free && !memcmp(&_client_session_context_ptr->ChallengerID, id, sizeof(sSecurityID))) return _pipe;
		_client_session_context_ptr++; _pipe++;
	}
	if(_free_pipe != (uint8_t)-1) {
		sSecurityClientSessionContext *_client_session_context = &ServerContext.SecurityClientChallengeContextes[_free_pipe];
		_client_session_context->ClientContextState = SecurityClientChallengeContextState_Alloced;
		_client_session_context->TimeOut = *timer_counter + PERIOD_REQUEST_TIMEOUT;
		_client_session_context->Action = SecurityAction_None;
		memcpy(&_client_session_context->ChallengerID, id, sizeof(sSecurityID));
	}
	return _free_pipe;
}

static bool Security_ServerPackageSend(uint32_t volatile *timer_counter, uint8_t pipe, eSecurityCommand command, uint32_t parameter2, sSecurityPayload const *payload) {
	sSecurityClientSessionContext *_client_session_context = &ServerContext.SecurityClientChallengeContextes[pipe];
	DBG_PutFormat(" -> Sending package: ID: %Q, cmd: %X, governor: %uX, prm2: %uX", &_client_session_context->ChallengerID, sizeof(sSecurityID), command, ServerContext.UniqueID, parameter2); if(command & SecurityCommand__WithPayload) { DBG_PutFormat(", payload: %Z", payload, sizeof(sSecurityPayload)); }
	sSecurityPackageCommand _package_command = { .ID = _client_session_context->ChallengerID, .Command = command, .PayloadCRC = command & SecurityCommand__WithPayload ? crc32(0, payload, sizeof(sSecurityPayload)) : 0, .Parameter1 = ServerContext.UniqueID, .Parameter2 = parameter2 };
	RF_Mode_Transmit(&RFMSPIServer, true); ClientAddr[0] = RF_SECURITY_ADDR_CLIENT_START + pipe; RF_TransmitterAddress(&RFMSPIServer, ClientAddr, true);
	bool _res = RF_PackageTransmit(&RFMSPIServer, true, &_package_command, sizeof(sSecurityPackageCommand)) && (!(command & SecurityCommand__WithPayload) || RF_PackageTransmit(&RFMSPIServer, true, payload, sizeof(sSecurityPayload)));
	RF_Mode_Receive(&RFMSPIServer, true, RF_SettingsSecurityServer.RX_ADDR_P0); _client_session_context->PackageIsPayload = false;
	DBG_PutString(_res ? "...OK" NL : "...Failed" NL);
	if(!_res) {
		_client_session_context->ClientContextState = SecurityClientChallengeContextState_Failed;
		_client_session_context->TimeOut = *timer_counter + PERIOD_FAIL_TIMEOUT;
		SecurityFail_CommunicationFailed(&_client_session_context->ChallengerID);
	}
	return _res;
}
static uint8_t Security_ServerPackageReceive(uint32_t volatile *timer_counter) {
	uint32_t _package[8]; uint8_t _package_len; uint8_t _pipe_res = (uint8_t)-1; uint32_t _timeout = 1;
	uint8_t _pipe_received = RF_PackageReceive(&RFMSPIServer, _package, &_package_len, &_timeout);
	if(_pipe_received != (uint8_t)-1) { DBG_PutFormat(" <- Received some data for pipe %d: %Z" NL, _pipe_received, _package, _package_len); }

	// Request package received
	if(_pipe_received == RF_PIPE_SERVER_REQUESTOR && _package_len == sizeof(sSecurityPackageRequest)) {
		sSecurityPackageRequest *_package_request = (sSecurityPackageRequest *)_package;
		if(!_package_request->ServerID) {
			DBG_PutFormat(" <- Received request from: %Q, Action: %uX" NL, &_package_request->ClientID, sizeof(sSecurityID), _package_request->Parameter);
			uint8_t _pipe = ClientContext_FindByIDOrAllocNew(timer_counter, &_package_request->ClientID, true);
			if(_pipe == (uint8_t)-1) { SecuritySystemError_NoFreePipe(&_package_request->ClientID); }
			else {
				sSecurityClientSessionContext *_client_session_context = &ServerContext.SecurityClientChallengeContextes[_pipe];
				if(_client_session_context->ClientContextState == SecurityClientChallengeContextState_Failed) SecurityFail_SessionAlreadyFailed(&_client_session_context->ChallengerID, _pipe);
				else if(_client_session_context->ClientContextState != SecurityClientChallengeContextState_Free && _client_session_context->ClientContextState != SecurityClientChallengeContextState_Alloced && _client_session_context->ClientContextState != SecurityClientChallengeContextState_Requested) SecurityFail_PipeAlreadyRequested(&_package_request->ClientID);
				else {
					bool _do_send_response = false;
					// Action challenge
					if((eSecurityActionPressButtons)_package_request->Parameter != eSecurityActionPressButtons_None) {
						DBG_PutString("Action request" NL);
						sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_package_request->ClientID);
						if(!_client_register) SecurityFail_NoRegistrationForClient(&_client_session_context->ChallengerID);
						else {
							eSecurityAction _action = Security_ClientRegister_FindAction(_client_register, _package_request->Parameter);
							bool _is_active = Security_ClientRegister_IsActive(_client_register);
							if(_action == SecurityAction_None) {
								if(_client_session_context->Action != SecurityAction_Free) SecurityFail_NoProperActionForButtons(&_client_session_context->ChallengerID, _package_request->Parameter);
								_client_session_context->Action = SecurityAction_Free;
							}
							else if(!_is_active) SecurityFail_ClientIsNotActive(&_client_session_context->ChallengerID);
							else {
								_client_session_context->ChallengerClientRegister = _client_register;
								_client_session_context->Action = _action;
								_do_send_response = true;
							}
						}
					}
					// Registration request
					else {
						DBG_PutString("Registration request" NL);
						if(!ServerContext.IsAllowRegistration) SecurityFail_RegistrationIsNotAllowed(&_client_session_context->ChallengerID);
						else {
							sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_package_request->ClientID);

							// Registration for specified ID
							if(ServerContext.RegistrationForID && !_client_register) SecurityFail_NoRegistrationForClient(&_client_session_context->ChallengerID);
							else if(ServerContext.RegistrationForID && &_client_register->ID != ServerContext.RegistrationForID) SecurityFail_RegistrationFromWrongClient(ServerContext.RegistrationForID, &_client_session_context->ChallengerID);
							// New ID registration
							else if(!ServerContext.RegistrationForID && _client_register) SecurityFail_RegistrationAlreadyExists(&_client_session_context->ChallengerID);
							else {
								_client_session_context->ChallengerClientRegister = _client_register;
								_do_send_response = true;
							}
						}
					}

					// Sending response for pipe request
					if(_do_send_response) {
						_package_request->ServerID = ServerContext.UniqueID; _package_request->Parameter = _pipe;
						DBG_PutFormat(" <- Sending request response to: %Q, governor: %08uX, pipe: %uX" NL, &_package_request->ClientID, sizeof(sSecurityID), _package_request->ServerID, _package_request->Parameter);
						RF_Mode_Transmit(&RFMSPIServer, true); RF_TransmitterAddress(&RFMSPIServer, RF_SettingsSecurityServer.RX_ADDR_P0, false);
						RF_PackageTransmit(&RFMSPIServer, false, _package_request, sizeof(sSecurityPackageRequest));
						RF_Mode_Receive(&RFMSPIServer, true, nullptr);
						_client_session_context->ClientContextState = SecurityClientChallengeContextState_Requested;
						_client_session_context->TimeOut = *timer_counter + PERIOD_CHALLENGE_TIMEOUT;
						_client_session_context->PackageIsPayload = false;
					}
				}
			}
		}
	}
	// Generic package from client
	else if(_pipe_received >= RF_PIPE_CLIENT_1 && _pipe_received <= RF_PIPE_CLIENT_5) {
		uint8_t _pipe = _pipe_received - RF_PIPE_CLIENT_1;
		sSecurityClientSessionContext *_client_session_context = &ServerContext.SecurityClientChallengeContextes[_pipe];
		if(_client_session_context->ClientContextState == SecurityClientChallengeContextState_Free) SecurityFail_SessionIsNotInitiated(_pipe);
		else if(_client_session_context->ClientContextState == SecurityClientChallengeContextState_Failed) SecurityFail_SessionAlreadyFailed(&_client_session_context->ChallengerID, _pipe);
		else {
			// Command package
			if(!_client_session_context->PackageIsPayload && _package_len == sizeof(sSecurityPackageCommand)) {
				sSecurityPackageCommand const *_package_command = (sSecurityPackageCommand const *)_package;
				if(memcmp(&_package_command->ID, &_client_session_context->ChallengerID, sizeof(sSecurityID))) SecurityFail_SessionWrongClient(&_client_session_context->ChallengerID, _pipe);
				else {
					memcpy(&_client_session_context->PackageCommand, _package, sizeof(sSecurityPackageCommand));
					if(_package_command->Command & SecurityCommand__WithPayload) { _client_session_context->PackageIsPayload = true; } else { _pipe_res = _pipe; }
				}
			}
			// Additional payload for the package
			else if(_client_session_context->PackageIsPayload && _package_len == sizeof(sSecurityPayload) && crc32(0, _package, sizeof(sSecurityPayload)) == _client_session_context->PackageCommand.PayloadCRC) {
				memcpy(&_client_session_context->PackagePayload, _package, sizeof(sSecurityPayload));
				_client_session_context->PackageIsPayload = false; _pipe_res = _pipe;
			}

			if(_pipe_res != (uint8_t)-1) { _delay_ms(10); DBG_PutFormat(" <- Received package: ID: %Q, cmd: %X, prm1: %uX, prm2: %uX", &_client_session_context->PackageCommand.ID, sizeof(sSecurityID), _client_session_context->PackageCommand.Command, _client_session_context->PackageCommand.Parameter1, _client_session_context->PackageCommand.Parameter2); if(_client_session_context->PackageCommand.Command & SecurityCommand__WithPayload) { DBG_PutFormat(", payload: %Z", &_client_session_context->PackagePayload, sizeof(sSecurityPayload)); } DBG_PutString(NL); }
		}
	}
	return _pipe_res;
}

void Security_ClientRegister_NormalizeBeforeNewWrite(sSecurityClientRegister *client_register) {
	client_register->Flags |= SecurityClientRegisterFlags__Mask;
	eSecurityAction *_action_ptr = client_register->Actions;
	for(uint32_t _idx = 0; _idx < SECURITY_CLIENT_ACTION_ENTRIES_COUNT; _idx++) {
		if(*_action_ptr == SecurityAction_None) *_action_ptr = SecurityAction_Free;
		_action_ptr++;
	}
	char *_description_ptr = client_register->Description;
	for(uint32_t _idx = 0; _idx < SECURITY_CLIENT_DESCRIPTION_COUNT; _idx++) {
		if(*_description_ptr == 0) *_description_ptr = 0xFF;
		_description_ptr++;
	}
}
void Security_ClientRegister_NormalizeFlashArea() {
	uint8_t _flash_page[SECURITY_PAGE_SIZE];
	uint8_t const * _page_address = ((uint8_t const *)&SecurityServerDisk); uint32_t _pages_cnt = SECURITY_SERVER_DISK_PAGES;
	while(_pages_cnt--) {
		memcpy(_flash_page, _page_address, SECURITY_PAGE_SIZE);
		sSecurityClientRegister *_client_register_ptr = (sSecurityClientRegister *)_flash_page; uint32_t _client_register_cnt = SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT_PER_PAGE;
		while(_client_register_cnt--) {
			if(!memcmp(&_client_register_ptr->ID, &__id_clobbered, sizeof(sSecurityID)) || !memcmp(&_client_register_ptr->ID, &__id_free, sizeof(sSecurityID))) { memset(_client_register_ptr, 0xFF, sizeof(sSecurityClientRegister)); }
			else Security_ClientRegister_NormalizeBeforeNewWrite(_client_register_ptr);
			_client_register_ptr++;
		}
		if(memcmp(_page_address, _flash_page, SECURITY_PAGE_SIZE)) {
			bool _res;
			do { Flash_PageErase(_page_address); _res = Flash_Write(_page_address, _flash_page, SECURITY_PAGE_SIZE); } while(!_res);
		}
		_page_address += SECURITY_PAGE_SIZE;
	}
}
sSecurityClientRegister const *Security_ClientRegister_GetByID(sSecurityID const *id) {
	sSecurityClientRegister const *_client_register_ptr = SecurityServerDisk.ClientRegisters; uint32_t _client_register_cnt = SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT;
	while(_client_register_cnt--) {
		if(!memcmp(&_client_register_ptr->ID, id, sizeof(sSecurityID))) return _client_register_ptr;
		_client_register_ptr++;
	}
	return nullptr;
}
sSecurityClientRegister const *Security_ClientRegister_GetValidStartingFromIndex(uint32_t *index) {
	uint32_t _idx = *index; sSecurityClientRegister const *_client_register_ptr = &SecurityServerDisk.ClientRegisters[_idx];
	for(; _idx < SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT; _idx++) {
		if(memcmp(&_client_register_ptr->ID, &__id_free, sizeof(sSecurityID)) && memcmp(&_client_register_ptr->ID, &__id_clobbered, sizeof(sSecurityID))) { *index = _idx + 1; return _client_register_ptr; }
		_client_register_ptr++;
	}
	return nullptr;
}
sSecurityClientRegister const *Security_ClientRegister_GetFree(bool *is_normalized) {
	bool _is_normalized = false; if(is_normalized) *is_normalized = false;
	while(true) {
		sSecurityClientRegister const *_client_register_ptr = SecurityServerDisk.ClientRegisters; uint32_t _client_register_cnt = SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT;
		while(_client_register_cnt--) {
			if(!memcmp(&_client_register_ptr->ID, &__id_free, sizeof(sSecurityID))) return _client_register_ptr;
			_client_register_ptr++;
		}
		if(_is_normalized) return nullptr;
		Security_ClientRegister_NormalizeFlashArea(); _is_normalized = true; if(is_normalized) *is_normalized = true;
	}
}
eSecurityAction Security_ClientRegister_FindAction(sSecurityClientRegister const *client_register, eSecurityActionPressButtons action_press_buttons) {
	eSecurityAction _action = SecurityAction_None;
	if(action_press_buttons >= eSecurityActionPressButtons_1 && action_press_buttons < eSecurityActionPressButtons__Last) {
		_action = client_register->Actions[action_press_buttons];
		if(_action == SecurityAction_Free) _action = SecurityAction_None;
	}
	return _action;
}
void Security_ClientRegister_Clobber(sSecurityClientRegister const *client_register) { while(!Flash_Write(&client_register->ID, &__id_clobbered, sizeof(sSecurityID))); }
bool Security_ClientRegister_Modify(sSecurityClientRegister const **client_register_flash, sSecurityClientRegister *client_register_new) {
	//////////////////////////////////////////////////////////////////////////
	__extension__ bool rewrite() {
		while(true) {
			bool _is_overwrite = true;
			uint16_t const *_dst = (uint16_t const *)*client_register_flash; uint16_t const *_src = (uint16_t const *)client_register_new; uint32_t _cnt = sizeof(sSecurityClientRegister) / sizeof(uint16_t);
			while(_cnt--) {
				uint16_t _dst_data = *_dst++, _src_data = *_src++;
				if(_dst_data != _src_data && _dst_data != 0xFFFF && _src_data) { _is_overwrite = false; break; }
			}
			if(_is_overwrite) { if(Flash_Write(*client_register_flash, client_register_new, sizeof(sSecurityClientRegister))) return true; }
			Security_ClientRegister_NormalizeBeforeNewWrite(client_register_new); bool _is_normalized;
			Security_ClientRegister_Clobber(*client_register_flash);
			sSecurityClientRegister const *_client_register_flash_new = Security_ClientRegister_GetFree(&_is_normalized);
			if(_client_register_flash_new) {
				*client_register_flash = _client_register_flash_new;
				if(Flash_Write(_client_register_flash_new, client_register_new, sizeof(sSecurityClientRegister))) return true;
			}
			else return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	if(!memcmp(*client_register_flash, client_register_new, sizeof(sSecurityClientRegister))) return true;
	return rewrite();
}
bool Security_ClientRegister_SetSecondaryKey(sSecurityClientRegister const **client_register, sSecurityPayload const *secondary_key) {
	sSecurityClientRegister _register_to_flash = **client_register; memcpy(&_register_to_flash.SecondaryKey, secondary_key, sizeof(sSecurityPayload));
	return Security_ClientRegister_Modify(client_register, &_register_to_flash);
}
bool Security_ClientRegister_SetSecondaryKeyAsPrimary(sSecurityClientRegister const **client_register) {
	sSecurityClientRegister _client_register_new = **client_register;
	memcpy(&_client_register_new.PrimaryKey, &_client_register_new.SecondaryKey, sizeof(sSecurityPayload)); memcpy(&_client_register_new.SecondaryKey, &__key_free, sizeof(sSecurityPayload));
	return Security_ClientRegister_Modify(client_register, &_client_register_new);
}
sSecurityClientRegister const *Security_ClientRegister_CreateNewOrResetPrimaryKey(sSecurityID const *id, sSecurityPayload const *primary_key) {
	sSecurityClientRegister _register_to_flash;
	while(true) {
		sSecurityClientRegister const *_client_register_existing = Security_ClientRegister_GetByID(id);
		if(_client_register_existing) {
			_register_to_flash = *_client_register_existing; memcpy(&_register_to_flash.PrimaryKey, primary_key, sizeof(sSecurityPayload));
			if(Security_ClientRegister_Modify(&_client_register_existing, &_register_to_flash)) return _client_register_existing;
		}
		else {
			sSecurityClientRegister const *_client_register_new = Security_ClientRegister_GetFree(nullptr);
			if(!_client_register_new) return nullptr;
			_register_to_flash = *_client_register_new; memcpy(&_register_to_flash.ID, id, sizeof(sSecurityID)); memcpy(&_register_to_flash.PrimaryKey, primary_key, sizeof(sSecurityPayload));
			if(Flash_Write(_client_register_new, &_register_to_flash, sizeof(sSecurityClientRegister))) return _client_register_new;
		}
	}
}
bool Security_ClientRegister_IsActive(sSecurityClientRegister const *client_register) { return client_register->Flags & SecurityClientRegisterFlags_IsActive; }
bool Security_ClientRegister_ToggleActive(sSecurityClientRegister const **client_register) {
	sSecurityClientRegister _client_register_new = **client_register;
	_client_register_new.Flags ^= SecurityClientRegisterFlags_IsActive;
	return Security_ClientRegister_Modify(client_register, &_client_register_new);
}
bool Security_ClientRegister_SetDescription(sSecurityClientRegister const **client_register, char const *description) {
	sSecurityClientRegister _client_register_new = **client_register;
	uint32_t _description_len = min(strlen(description), SECURITY_CLIENT_DESCRIPTION_COUNT - 1);
	memcpy(_client_register_new.Description, description, _description_len); _client_register_new.Description[_description_len] = '\0';
	return Security_ClientRegister_Modify(client_register, &_client_register_new);
}

void Security_ServerInit(uint32_t governor_id) {
	ServerContext.UniqueID = governor_id; DBG_PutFormat("Governor ID: %08uX" NL, governor_id);
	RF_Mode_PowerDown(&RFMSPIServer, true);
	DBG_PutFormat("Is RF governor module present: %s" NL, RF_IsPresent(&RFMSPIServer) ? "yes" : "no");
	RF_ApplySettings(&RFMSPIServer, &RF_SettingsSecurityServer);
	RF_Mode_Standby(&RFMSPIServer); RF_Mode_Receive(&RFMSPIServer, true, nullptr);
}

void Security_ServerPoll(uint32_t volatile *timer_counter) {
	// Check for timeout of all pending challenges
	ClientContext_CheckChallengingTimeout(timer_counter);
	if(ServerContext.IsAllowRegistration && (*timer_counter - ServerContext.RegistrationIimeIn) > PERIOD_REGISTRATION_ALLOW_TIMEOUT) { ServerContext.IsAllowRegistration = false; SecurityFail_RegistrationAllowanceTimeout(); }

	// Process incoming packet if any
	uint8_t _received_pipe = Security_ServerPackageReceive(timer_counter);
	if(_received_pipe != (uint8_t)-1) {
		sSecurityClientSessionContext *_client_session_context = &ServerContext.SecurityClientChallengeContextes[_received_pipe];
		eSecurityClientChallengeContextState _state_this = _client_session_context->ClientContextState; _client_session_context->ClientContextState = SecurityClientChallengeContextState_Failed;
		if(_client_session_context->PackageCommand.Command == SecurityCommand_ChallengeRequest) {
			// Client requests a generic challenge
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if(_state_this != SecurityClientChallengeContextState_Requested) SecurityFail_UnexpectedResponseForSession(&_client_session_context->ChallengerID);
			else {
				Security_RandomFill(_client_session_context->Random.Data, sizeof_array(_client_session_context->Random.Data));
				SecurityOrdinalEvent_ChallengeInitiated(&_client_session_context->ChallengerID, _client_session_context->Action, &_client_session_context->Random);
				if(Security_ServerPackageSend(timer_counter, _received_pipe, SecurityCommand_ChallengeRandom, 0, &_client_session_context->Random)) {
					_client_session_context->ClientContextState = SecurityClientChallengeContextState_Challenge;
					_client_session_context->TimeOut = *timer_counter + PERIOD_CHALLENGE_TIMEOUT;
				}
			}
		}
		else if(_client_session_context->PackageCommand.Command == SecurityCommand_ChallengeResponse) {
			// Client responds for challenge for Server's random
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if(_state_this != SecurityClientChallengeContextState_Challenge) SecurityFail_UnexpectedResponseForSession(&_client_session_context->ChallengerID);
			else {
				sSecurityPayload _encoded_random; bool _is_won, _is_allow_key_change = true, _is_do_key_change = false;
				// Checking primary key
				_encoded_random = _client_session_context->Random; Security_CipherEncode(_client_session_context->ChallengerClientRegister->PrimaryKey.Data, _encoded_random.Data, 2);
				if((_is_won = !memcmp(&_encoded_random, &_client_session_context->PackagePayload, sizeof(sSecurityPayload)))) {
					SecurityOrdinalEvent_ChallengeWonWithPrimaryKey(&_client_session_context->ChallengerID, _client_session_context->Action);
					if(memcmp(&_client_session_context->ChallengerClientRegister->SecondaryKey, &__key_free, sizeof(sSecurityPayload))) {
						// Repeat key change
						_client_session_context->NewKey = _client_session_context->ChallengerClientRegister->SecondaryKey; _is_do_key_change = true;
						SecurityOrdinalEvent_ChallengeAnotherTryKeyReplacement(&_client_session_context->ChallengerID);
					}
				}
				// Checking secondary key if set
				else if(memcmp(&_client_session_context->ChallengerClientRegister->SecondaryKey, &__key_free, sizeof(sSecurityPayload))) {
					_encoded_random = _client_session_context->Random; Security_CipherEncode(_client_session_context->ChallengerClientRegister->SecondaryKey.Data, _encoded_random.Data, 2);
					if((_is_won = !memcmp(&_encoded_random, &_client_session_context->PackagePayload, sizeof(sSecurityPayload)))) {
						// make secondary key as primary
						_is_allow_key_change = Security_ClientRegister_SetSecondaryKeyAsPrimary(&_client_session_context->ChallengerClientRegister);
						SecurityOrdinalEvent_ChallengeWonWithSecondaryKey(&_client_session_context->ChallengerID, _client_session_context->Action);
					}
				}

				if(_is_won) {
					bool _action_success = Security_PerformAction(_client_session_context->Action);

					if(_action_success && _is_allow_key_change && !_is_do_key_change && Security_RandomGet() < SECURITY_KEY_REPLACE_PROBABILITY) {
						// Replace old key with a new one
						Security_RandomFill(_client_session_context->NewKey.Data, sizeof_array(_client_session_context->NewKey.Data));
						_is_do_key_change = Security_ClientRegister_SetSecondaryKey(&_client_session_context->ChallengerClientRegister, &_client_session_context->NewKey);
						SecurityOrdinalEvent_ChallengeKeyReplacement(&_client_session_context->ChallengerID);
					}

					if(_action_success && _is_do_key_change) {
						sSecurityPayload _new_key = _client_session_context->NewKey; Security_CipherEncode(_client_session_context->ChallengerClientRegister->PrimaryKey.Data, _new_key.Data, 2);
						DBG_PutFormat("Sending new key: %Z" NL, &_client_session_context->NewKey, sizeof(sSecurityPayload));
						if(Security_ServerPackageSend(timer_counter, _received_pipe, SecurityCommand_ChallengeNewKey, 0, &_new_key)) { _client_session_context->ClientContextState = SecurityClientChallengeContextState_ChallengeNewKeyRandom; }
					}
					else {
						// Sending won acknowledgment
						if(Security_ServerPackageSend(timer_counter, _received_pipe, SecurityCommand_ChallengeWonAcknowledgment, _action_success, nullptr)) { _client_session_context->ClientContextState = SecurityClientChallengeContextState_Free; }
					}
				}
				else SecurityFail_ChallengeLost(&_client_session_context->ChallengerID);
			}
		}
		else if(_client_session_context->PackageCommand.Command == SecurityCommand_ChallengeNewKeyRandom) {
			// Client requests a new key checking by challenging it with server
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if(_state_this != SecurityClientChallengeContextState_ChallengeNewKeyRandom) SecurityFail_UnexpectedResponseForSession(&_client_session_context->ChallengerID);
			else {
				Security_CipherEncode(_client_session_context->NewKey.Data, _client_session_context->PackagePayload.Data, 2);
				if(Security_ServerPackageSend(timer_counter, _received_pipe, SecurityCommand_ChallengeNewKeyResponse, 0, &_client_session_context->PackagePayload)) { _client_session_context->ClientContextState = SecurityClientChallengeContextState_Free; }
			}
		}
		else if(_client_session_context->PackageCommand.Command == SecurityCommand_RegistrationRequest) {
			// Client requests a registration challenge
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if(_state_this != SecurityClientChallengeContextState_Requested) SecurityFail_UnexpectedResponseForSession(&_client_session_context->ChallengerID);
			else {
				ServerContext.IsAllowRegistration = false; ServerContext.RegistrationForID = nullptr;
				uint32_t _sequence = Security_RandomGet();
				Security_RandomFill(_client_session_context->Random.Data, sizeof_array(_client_session_context->Random.Data));
				SecurityOrdinalEvent_RegistrationInitiated(&_client_session_context->ChallengerID, _sequence, &_client_session_context->Random);
				if(Security_ServerPackageSend(timer_counter, _received_pipe, SecurityCommand_RegistrationRandom, 0, &_client_session_context->Random)) {
					_client_session_context->ClientContextState = SecurityClientChallengeContextState_Registration;
					_client_session_context->TimeOut = *timer_counter + PERIOD_REGISTRATION_TIMEOUT;
					_client_session_context->RegistrationSequence = _sequence;
				}
			}
		}
		else if(_client_session_context->PackageCommand.Command == SecurityCommand_RegistrationResponse) {
			// Client responds the registration challenge for Server's random
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if(_state_this != SecurityClientChallengeContextState_Registration) SecurityFail_UnexpectedResponseForSession(&_client_session_context->ChallengerID);
			else {
				sSecurityPayload _registration_key;
				uint32_t *_registration_key_ptr = _registration_key.Data; uint32_t _registration_key_cnt = sizeof_array(_registration_key.Data);
				while(_registration_key_cnt--) { *_registration_key_ptr++ = _client_session_context->RegistrationSequence; }

				// Checking sequence key
				sSecurityPayload _encoded_random = _client_session_context->Random; Security_CipherEncode(_registration_key.Data, _encoded_random.Data, 2);
				if(!memcmp(&_encoded_random, &_client_session_context->PackagePayload, sizeof(sSecurityPayload))) {
					// generating new full key instead for temporary sequence key
					Security_RandomFill(_client_session_context->NewKey.Data, sizeof_array(_client_session_context->NewKey.Data));
					sSecurityPayload _new_key = _client_session_context->NewKey; Security_CipherEncode(_registration_key.Data, _new_key.Data, 2);
					DBG_PutFormat("Sending new key: %Z" NL, &_client_session_context->NewKey, sizeof(sSecurityPayload));
					SecurityOrdinalEvent_RegistrationWonKeyReplacement(&_client_session_context->ChallengerID);
					if(Security_ServerPackageSend(timer_counter, _received_pipe, SecurityCommand_RegistrationNewKey, 0, &_new_key)) { _client_session_context->ClientContextState = SecurityClientChallengeContextState_RegistrationConfirmation; }
				}
				else SecurityFail_RegistrationLost(&_client_session_context->ChallengerID);
			}
		}
		else if(_client_session_context->PackageCommand.Command == SecurityCommand_RegistrationConfirmNewKey) {
			// Client confirms the registration challenge for Server's new key
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if(_state_this != SecurityClientChallengeContextState_RegistrationConfirmation) SecurityFail_UnexpectedResponseForSession(&_client_session_context->ChallengerID);
			else {
				// Checking confirmation re-encoded key
				sSecurityPayload _encoded_key = _client_session_context->NewKey; Security_CipherEncode(_client_session_context->NewKey.Data, _encoded_key.Data, 2);
				if(!memcmp(&_encoded_key, &_client_session_context->PackagePayload, sizeof(sSecurityPayload))) {
					_client_session_context->ClientContextState = SecurityClientChallengeContextState_Free;
					// Create new registration
					Security_ClientRegister_CreateNewOrResetPrimaryKey(&_client_session_context->ChallengerID, &_client_session_context->NewKey);
					SecurityOrdinalEvent_RegistrationConfirmed(&_client_session_context->ChallengerID);
				}
				else SecurityFail_RegistrationDenied(&_client_session_context->ChallengerID);
			}
		}
		else SecurityFail_UnexpectedCommand(&_client_session_context->ChallengerID, _client_session_context->PackageCommand.Command);
	}
}
void Security_AllowRegistration(uint32_t volatile *timer_counter, sSecurityID const *id) {
	ServerContext.IsAllowRegistration = true; ServerContext.RegistrationForID = id;
	ServerContext.RegistrationIimeIn = *timer_counter;
}

#endif
