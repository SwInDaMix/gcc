/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf_security_client.c
 ***********************************************************************/

#include "eta_rf_security_client.h"

#ifdef SECURITY_CLIENT

// RFM setting of Security Client point
static sRF_Settings const RF_SettingsSecurityClient = {
	.CONFIG = RFConfig_EN_CRC | RFConfig_CRCO,
	.SETUP_AW = RFSetupAW_5Bytes,
	.SETUP_RETR = RFSetupRetr_ARC_15Time | RFSetupRetr_ARD_500us,
	.RF_CH = RF_SECURITY_CHANNEL,
	.RF_SETUP = RFRFSetup_LNA_HCURR | RFRFSetup_RF_PWR_High | RFRFSetup_RF_DR_1Mbps,
	.FEATURE = RFFeature_EN_DYN_ACK | RFFeature_EN_DPL,
	.ENAA = RFPipe_0 | RFPipe_1,
	.EN_RX_ADDR = RFPipe_0 | RFPipe_1,
	.RX_ADDR_P0 = { RF_SECURITY_ADDR_CLIENT_START, RF_SECURITY_ADDRPREF },
	.RX_ADDR_P1 = { RF_SECURITY_ADDR_REQUEST, RF_SECURITY_ADDRPREF },
	.RX_ADDR_P2 = 0,
	.RX_ADDR_P3 = 0,
	.RX_ADDR_P4 = 0,
	.RX_ADDR_P5 = 0,
	.RX_PW_P0 = sizeof(sSecurityPackageCommand),
	.RX_PW_P1 = sizeof(sSecurityPackageRequest),
	.RX_PW_P2 = 0,
	.RX_PW_P3 = 0,
	.RX_PW_P4 = 0,
	.RX_PW_P5 = 0
};

#define RF_PIPE_SERVER 0
#define RF_PIPE_CLIENT_REQUESTOR 1

static sSecurityClientContext ClientContext;
static uint8_t ServerAddr[5] = { 0, RF_SECURITY_ADDRPREF };

static bool ClientPackageReqeust(eSecurityActionPressButtons press_buttons) {
	DBG_PutFormat(" -> Sending request package from %Q" NL, &ClientContext.CurrentID, sizeof(sSecurityID));
	sSecurityPackageRequest _package_req = { .ClientID = ClientContext.CurrentID, .ServerID = 0, .Parameter = press_buttons };

	uint32_t _repeat_cnt = 15;
	while(_repeat_cnt--) {
		RF_Mode_Transmit(&RFMSPIClient, true); RF_TransmitterAddress(&RFMSPIClient, RF_SettingsSecurityClient.RX_ADDR_P1, false); RF_PackageTransmit(&RFMSPIClient, false, &_package_req, sizeof(sSecurityPackageRequest));
		RF_Mode_Receive(&RFMSPIClient, true, nullptr);
		uint32_t _resp[8];
		sSecurityPackageRequest *_package_resp = (sSecurityPackageRequest *)_resp; uint8_t _package_resp_len; uint32_t _timeout = 1000;
		while(_timeout) {
			uint8_t _pipe_received = RF_PackageReceive(&RFMSPIClient, _package_resp, &_package_resp_len, &_timeout);
			if(_pipe_received != (uint8_t)-1) { DBG_PutFormat(" <- Received some data for pipe %d: %Z" NL, _pipe_received, _package_resp, _package_resp_len); }
			if(_pipe_received == RF_PIPE_CLIENT_REQUESTOR && _package_resp_len == sizeof(sSecurityPackageRequest) && !memcmp(&_package_resp->ClientID, &ClientContext.CurrentID, sizeof(sSecurityID)) && _package_resp->ServerID) {
				ClientContext.ServerID = _package_resp->ServerID; ServerAddr[0] = RF_SECURITY_ADDR_CLIENT_START + ((uint8_t)_package_resp->Parameter & 0xF);
				return true;
			}
		}
	}
	return false;
}
static bool ClientPackageSend(eSecurityCommand command, uint32_t parameter1, uint32_t parameter2, sSecurityPayload const *payload) {
	DBG_PutFormat(" -> Sending package: ID: %Q, cmd: %X, prm1: %uX, prm2: %uX", &ClientContext.CurrentID, sizeof(sSecurityID), command, parameter1, parameter2); if(command & SecurityCommand__WithPayload) { DBG_PutFormat(", payload: %Z", payload, sizeof(sSecurityPayload)); }
	RF_Mode_Transmit(&RFMSPIClient, true); RF_TransmitterAddress(&RFMSPIClient, ServerAddr, true);
	sSecurityPackageCommand _package_command = { .ID = ClientContext.CurrentID, .Command = command, .PayloadCRC = command & SecurityCommand__WithPayload ? crc32(0, payload, sizeof(sSecurityPayload)) : 0, .Parameter1 = parameter1, .Parameter2 = parameter2 };
	bool _res = RF_PackageTransmit(&RFMSPIClient, true, &_package_command, sizeof(sSecurityPackageCommand)) && (!(command & SecurityCommand__WithPayload) || RF_PackageTransmit(&RFMSPIClient, true, payload, sizeof(sSecurityPayload)));
	RF_Mode_Receive(&RFMSPIClient, true, nullptr);
	DBG_PutString(_res ? "...OK" NL : "...Failed" NL);
	return _res;
}
static bool ClientPackageReceive(bool *is_payload, sSecurityPackageCommand *package_command, sSecurityPayload *payload) {
	uint32_t _package[8]; uint8_t _package_len; bool _res = false; uint32_t _timeout = 1;
	uint8_t _pipe_received = RF_PackageReceive(&RFMSPIClient, _package, &_package_len, &_timeout);
	if(_pipe_received != (uint8_t)-1) { DBG_PutFormat(" <- Received some data for pipe %d: %Z" NL, _pipe_received, _package, _package_len); }

	if(_pipe_received == RF_PIPE_SERVER) {
		// Command package
		if(!*is_payload && _package_len == sizeof(sSecurityPackageCommand)) {
			sSecurityPackageCommand const *_package_command = (sSecurityPackageCommand const *)_package;
			if(memcmp(&_package_command->ID, &ClientContext.CurrentID, sizeof(sSecurityID))) SecurityFail_SessionWrongClient(&_package_command->ID);
			else if(_package_command->Parameter1 != ClientContext.ServerID) SecurityFail_SessionWrongServer(_package_command->Parameter1);
			else {
				memcpy(package_command, _package, sizeof(sSecurityPackageCommand));
				if(package_command->Command & SecurityCommand__WithPayload) { *is_payload = true; } else { _res = true; }
			}
		}
		// Additional payload for the package
		else if(*is_payload && _package_len == sizeof(sSecurityPayload) && crc32(0, _package, sizeof(sSecurityPayload)) == package_command->PayloadCRC) {
			memcpy(payload, _package, sizeof(sSecurityPayload));
			*is_payload = false; _res = true;
		}
	}
	if(_res) { _delay_ms(10); DBG_PutFormat(" <- Received package: ID: %Q, cmd: %X, governor: %uX, prm2: %uX", &package_command->ID, sizeof(sSecurityID), package_command->Command, package_command->Parameter1, package_command->Parameter2); if(package_command->Command & SecurityCommand__WithPayload) { DBG_PutFormat(", payload: %Z", payload, sizeof(sSecurityPayload)); } DBG_PutString(NL); }
	return _res;
}

void Security_ClientInit() {
	ClientContext.ClientState = SecurityClientState_Initial;
	RF_Mode_PowerDown(&RFMSPIClient, true);
	DBG_PutFormat("Is RF accessor module present: %s" NL, RF_IsPresent(&RFMSPIClient) ? "yes" : "no");
	RF_ApplySettings(&RFMSPIClient, &RF_SettingsSecurityClient);
	RF_Mode_Standby(&RFMSPIClient);
	DBG_PutFormat("Accessor ID: %Q" NL, &SecurityClientDisk.ClientInfos[0].ClientID, sizeof(sSecurityID));
	sSecurityClientInfo const *_client_info_ptr = &SecurityClientDisk.ClientInfos[1]; uint32_t _client_info_cnt = SECURITY_CLIENT_DISK_CLIENT_INFOS_COUNT - 1;
	while(_client_info_cnt--) {
		if(_client_info_ptr->ServerID != __id_clobbered.ID[0] && _client_info_ptr->ServerID != __id_free.ID[0]) {
			DBG_PutFormat("Governor ID: %08uX, Key: %Z" NL, _client_info_ptr->ServerID, &_client_info_ptr->Key, sizeof(sSecurityPayload));
		}
		_client_info_ptr++;
	}
}

void Security_ClientPoll(uint32_t volatile *timer_counter, eSecurityActionPressButtons *action_buttons) {
	// Inactivity timeout to power off
	if((*timer_counter - ClientContext.TimeIn) >= (ClientContext.ClientState == SecurityClientState_RegisterSequence ? PERIOD_REGISTER_SEQUENCE_INACTIVITY : PERIOD_INACTIVITY)) {
		if(ClientContext.ClientState == SecurityClientState_RegisterSequence) { DBG_PutString(NL); }
		ClientContext.ClientState = SecurityClientState_PowerOff;
	}

	if(ClientContext.ClientState != SecurityClientState_RegisterSequence) {
		if(*action_buttons != eSecurityActionPressButtons_None) {
			SecurityOrdinalEvent_ButtonsPressed(*action_buttons);
			ClientContext.CurrentID = SecurityClientDisk.ClientInfos[0].ClientID;
			ClientContext.TimeIn = *timer_counter;
			if(*action_buttons == SECURITY_KEY_FULL_RESET_BUTTONS) {
				// Full client reseting
				// ~~~~~~~~~~~~~~~~~~~~
				SecurityOrdinalEvent_FullResetInitiated(&ClientContext.CurrentID);
				ClientContext.IsFullReset = true;
				ClientContext.KeypressSequence = ClientContext.KeypressSequenceCount = 0;
				ClientContext.ClientState = SecurityClientState_RegisterSequence;
			}
			else if(*action_buttons == SECURITY_KEY_REGISTER_BUTTONS) {
				// Trying to register in server
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(!memcmp(&ClientContext.CurrentID, &__id_free, sizeof(sSecurityID))) {
					Security_RandomFill(ClientContext.CurrentID.ID, sizeof_array(ClientContext.CurrentID.ID));
					SecurityOrdinalEvent_GeneratedNewID(&ClientContext.CurrentID);
				}
				if(!ClientPackageReqeust(eSecurityActionPressButtons_None) || !ClientPackageSend(SecurityCommand_RegistrationRequest, 0, 0, nullptr)) ClientContext.ClientState = SecurityClientState_Initial;
				else {
					SecurityOrdinalEvent_RegistrationInitiated(&ClientContext.CurrentID);
					ClientContext.ClientState = SecurityClientState_Receiving; ClientContext.ReceivingType = SecurityClientReceivingType_RegistrationRandom;
				}
			}
			else {
				// Initiating challenge
				// ~~~~~~~~~~~~~~~~~~~~
				if(memcmp(&ClientContext.CurrentID, &__id_free, sizeof(sSecurityID))) {
					if(!ClientPackageReqeust(*action_buttons)) ClientContext.ClientState = SecurityClientState_Initial;
					else {
						sSecurityClientInfo const *_client_info_valid = ClientInfo_GetValid(ClientContext.ServerID);
						if(!_client_info_valid) SecurityFail_NoRegistrationForServer(ClientContext.ServerID);
						else {
							ClientContext.ClientInfo = *_client_info_valid;
							SecurityOrdinalEvent_ChallengeInitiated(&ClientContext.CurrentID, ClientContext.ServerID);
							if(!ClientPackageSend(SecurityCommand_ChallengeRequest, 0, 0, nullptr)) ClientContext.ClientState = SecurityClientState_Initial;
							else { ClientContext.ClientState = SecurityClientState_Receiving; ClientContext.ReceivingType = SecurityClientReceivingType_ChallengeRandom; }
						}
					}
				}
				else SecurityFail_NeverRegistered();
				ClientContext.ClientState = SecurityClientState_Receiving; ClientContext.ReceivingType = SecurityClientReceivingType_ChallengeRandom;
			}
			*action_buttons = eSecurityActionPressButtons_None;
		}
	}

	if(ClientContext.ClientState == SecurityClientState_Receiving) {
		if(ClientPackageReceive(&ClientContext.PackageIsPayload, &ClientContext.PackageCommand, &ClientContext.PackagePayload)) {
			ClientContext.ClientState = SecurityClientState_Initial;
			if(ClientContext.PackageCommand.Command == SecurityCommand_ChallengeRandom) {
				// Server accepted a generic challenge with random
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(ClientContext.ReceivingType != SecurityClientReceivingType_ChallengeRandom) SecurityFail_UnexpectedResponseForSession(ClientContext.ServerID);
				else {
					Security_CipherEncode(ClientContext.ClientInfo.Key.Data, ClientContext.PackagePayload.Data, 2);
					SecurityOrdinalEvent_ChallengeAttempt(ClientContext.ServerID);
					if(!ClientPackageSend(SecurityCommand_ChallengeResponse, 0, 0, &ClientContext.PackagePayload)) ClientContext.ClientState = SecurityClientState_Initial;
					else { ClientContext.ClientState = SecurityClientState_Receiving; ClientContext.ReceivingType = SecurityClientReceivingType_ChallengeNewKeyOrWonAcknowledgment; }
				}
			}
			else if(ClientContext.PackageCommand.Command == SecurityCommand_ChallengeNewKey) {
				// Server accepted a won challenge and initiates key replacement
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(ClientContext.ReceivingType != SecurityClientReceivingType_ChallengeNewKeyOrWonAcknowledgment) SecurityFail_UnexpectedResponseForSession(ClientContext.ServerID);
				else {
					Security_CipherDecode(ClientContext.ClientInfo.Key.Data, ClientContext.PackagePayload.Data, 2); memcpy(&ClientContext.ClientInfo.Key, &ClientContext.PackagePayload, sizeof(sSecurityPayload));
					SecurityOrdinalEvent_ChallengeNewKey(ClientContext.ServerID, &ClientContext.ClientInfo.Key);
					Security_RandomFill(ClientContext.RegistrationRandom.Data, sizeof_array(ClientContext.RegistrationRandom.Data));
					if(!ClientPackageSend(SecurityCommand_ChallengeNewKeyRandom, 0, 0, &ClientContext.RegistrationRandom)) ClientContext.ClientState = SecurityClientState_Initial;
					else { ClientContext.ClientState = SecurityClientState_Receiving; ClientContext.ReceivingType = SecurityClientReceivingType_ChallengeNewKeyResponse; }
				}
			}
			else if(ClientContext.PackageCommand.Command == SecurityCommand_ChallengeNewKeyResponse) {
				// Server sent encrypted random with a new key
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(ClientContext.ReceivingType != SecurityClientReceivingType_ChallengeNewKeyResponse) SecurityFail_UnexpectedResponseForSession(ClientContext.ServerID);
				else {
					Security_CipherEncode(ClientContext.ClientInfo.Key.Data, ClientContext.RegistrationRandom.Data, 2);
					if(!memcmp(&ClientContext.RegistrationRandom, &ClientContext.PackagePayload, sizeof(sSecurityPayload))) {
						ClientInfo_SetNew(&ClientContext.CurrentID, &ClientContext.ClientInfo);
						SecurityOrdinalEvent_ChallengeNewKeyApproved(ClientContext.ServerID, &ClientContext.ClientInfo.Key);
					}
					else SecurityFail_ChallengeNewKeyFailed(ClientContext.ServerID, &ClientContext.ClientInfo.Key);
					ClientContext.ClientState = SecurityClientState_Initial;
				}
			}
			else if(ClientContext.PackageCommand.Command == SecurityCommand_ChallengeWonAcknowledgment) {
				// Server accepted a won challenge
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(ClientContext.ReceivingType != SecurityClientReceivingType_ChallengeNewKeyOrWonAcknowledgment) SecurityFail_UnexpectedResponseForSession(ClientContext.ServerID);
				else {
					SecurityOrdinalEvent_ChallengeWon(ClientContext.ServerID, (bool)ClientContext.PackageCommand.Parameter2);
					ClientContext.ClientState = SecurityClientState_Initial;
				}
			}
			else if(ClientContext.PackageCommand.Command == SecurityCommand_RegistrationRandom) {
				// Server accepted a registration challenge with random
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(ClientContext.ReceivingType != SecurityClientReceivingType_RegistrationRandom) SecurityFail_UnexpectedResponseForSession(ClientContext.ServerID);
				else {
					ClientContext.ClientInfo.ServerID = ClientContext.ServerID;
					memcpy(&ClientContext.RegistrationRandom, &ClientContext.PackagePayload, sizeof(sSecurityPayload));
					ClientContext.IsFullReset = false;
					ClientContext.KeypressSequence = ClientContext.KeypressSequenceCount = 0;
					ClientContext.ClientState = SecurityClientState_RegisterSequence;
					SecurityOrdinalEvent_RegistrationSequence(ClientContext.ServerID);
				}
			}
			else if(ClientContext.PackageCommand.Command == SecurityCommand_RegistrationNewKey) {
				// Server accepted a won registration challenge and initiates sequence key replacement
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if(ClientContext.ReceivingType != SecurityClientReceivingType_RegistrationNewKey) SecurityFail_UnexpectedResponseForSession(ClientContext.ServerID);
				else {
					Security_CipherDecode(ClientContext.ClientInfo.Key.Data, ClientContext.PackagePayload.Data, 2); memcpy(&ClientContext.ClientInfo.Key, &ClientContext.PackagePayload, sizeof(sSecurityPayload));
					ClientInfo_SetNew(&ClientContext.CurrentID, &ClientContext.ClientInfo);
					SecurityOrdinalEvent_RegistrationNewKey(ClientContext.ServerID, &ClientContext.ClientInfo.Key);
					Security_CipherEncode(ClientContext.ClientInfo.Key.Data, ClientContext.PackagePayload.Data, 2);
					if(!ClientPackageSend(SecurityCommand_RegistrationConfirmNewKey, 0, 0, &ClientContext.PackagePayload)) ClientContext.ClientState = SecurityClientState_Initial;
					else { ClientContext.ClientState = SecurityClientState_Initial; }
				}
			}
			else SecurityFail_UnexpectedCommand(ClientContext.ServerID, ClientContext.PackageCommand.Command);
		}
	}
	else if(ClientContext.ClientState == SecurityClientState_RegisterSequence) {
		if(*action_buttons != eSecurityActionPressButtons_None) {
			if(*action_buttons >= eSecurityActionPressButtons_1 && *action_buttons <= eSecurityActionPressButtons_4) {
				ClientContext.TimeIn = *timer_counter;
				uint32_t _val = *action_buttons - eSecurityActionPressButtons_1;
				ClientContext.KeypressSequence = (ClientContext.KeypressSequence << 2) | _val; ClientContext.KeypressSequenceCount++;
				DBG_PutChar('1' + _val); DBG_Flush();
				if(ClientContext.KeypressSequenceCount >= (sizeof(uint32_t) << 2)) {
					DBG_PutString(NL);
					if(ClientContext.IsFullReset) {
						if(ClientContext.KeypressSequence == SECURITY_FULL_RESET_SEQUENCE) {
							ClientInfo_FullReset();
							SecurityOrdinalEvent_FullResetConfirmed();
						}
						else SecurityOrdinalEvent_FullResetFailed();
						ClientContext.ClientState = SecurityClientState_Initial;
					}
					else {
						uint32_t *_sequence_key_ptr = ClientContext.ClientInfo.Key.Data; uint32_t _sequence_key_cnt = sizeof_array(ClientContext.ClientInfo.Key.Data);
						while(_sequence_key_cnt--) { *_sequence_key_ptr++ = ClientContext.KeypressSequence; }
						Security_CipherEncode(ClientContext.ClientInfo.Key.Data, ClientContext.RegistrationRandom.Data, 2);
						if(!ClientPackageSend(SecurityCommand_RegistrationResponse, 0, 0, &ClientContext.RegistrationRandom)) ClientContext.ClientState = SecurityClientState_Initial;
						else { ClientContext.ClientState = SecurityClientState_Receiving; ClientContext.ReceivingType = SecurityClientReceivingType_RegistrationNewKey; }
					}
				}
			}
			else { DBG_PutString(NL); ClientContext.ClientState = SecurityClientState_Initial; }
			*action_buttons = eSecurityActionPressButtons_None;
		}
	}
	else if(ClientContext.ClientState == SecurityClientState_PowerOff) {
		SecurityOrdinalEvent_AllDone();
		ClientContext.ClientState = SecurityClientState_Initial;
	}
}

sSecurityClientInfo const *ClientInfo_GetValid(uint32_t server_id) {
	sSecurityClientInfo const *_client_info_ptr = &SecurityClientDisk.ClientInfos[1]; uint32_t _client_info_cnt = SECURITY_CLIENT_DISK_CLIENT_INFOS_COUNT - 1;
	while(_client_info_cnt--) {
		if(_client_info_ptr->ServerID == server_id) return _client_info_ptr;
		_client_info_ptr++;
	}
	return nullptr;
}
void ClientInfo_FullReset() {
	uint8_t const * _page_address = ((uint8_t const *)&SecurityClientDisk); uint32_t _pages_cnt = SECURITY_CLIENT_DISK_PAGES;
	while(_pages_cnt--) { Flash_PageErase(_page_address); _page_address += SECURITY_PAGE_SIZE; }
}
void ClientInfo_Clobber(sSecurityClientInfo const *client_info) { while(!Flash_Write(&client_info->ServerID, &__id_clobbered.ID[0], sizeof(uint32_t))); }
void ClientInfo_NormalizeBeforeNewWrite(sSecurityClientInfo *client_info) {
	memset(client_info->_reserved2, 0xFF, sizeof(client_info->_reserved2));
}
void ClientInfo_NormalizeFlashArea(sSecurityID const *client_id) {
	uint8_t _flash_page[SECURITY_PAGE_SIZE];
	uint8_t const * _page_address = ((uint8_t const *)&SecurityClientDisk); uint32_t _pages_cnt = SECURITY_CLIENT_DISK_PAGES; bool _is_first = true;
	while(_pages_cnt--) {
		memcpy(_flash_page, _page_address, SECURITY_PAGE_SIZE);
		sSecurityClientInfo *_client_info_ptr = (sSecurityClientInfo *)_flash_page; uint32_t _client_info_cnt = SECURITY_CLIENT_DISK_CLIENT_INFOS_COUNT_PER_PAGE;
		while(_client_info_cnt--) {
			if(_is_first) { _is_first = false; _client_info_ptr->ClientID = *client_id; memset(_client_info_ptr->_reserved1, 0xFF, sizeof(_client_info_ptr->_reserved1)); }
			else if(_client_info_ptr->ServerID == __id_clobbered.ID[0] || _client_info_ptr->ServerID == __id_free.ID[0]) { memset(_client_info_ptr, 0xFF, sizeof(sSecurityClientInfo)); }
			else ClientInfo_NormalizeBeforeNewWrite(_client_info_ptr);
			_client_info_ptr++;
		}
		if(memcmp(_page_address, _flash_page, SECURITY_PAGE_SIZE)) {
			bool _res;
			do { Flash_PageErase(_page_address); _res = Flash_Write(_page_address, _flash_page, SECURITY_PAGE_SIZE); } while(!_res);
		}
		_page_address += SECURITY_PAGE_SIZE;
	}
	DBG_PutFormat("New Client ID: %Q" NL, &SecurityClientDisk.ClientInfos[0].ClientID, sizeof(sSecurityID));
}
void ClientInfo_SetNew(sSecurityID const *client_id, sSecurityClientInfo *client_info_new) {
	sSecurityClientInfo const *_client_info_valid = ClientInfo_GetValid(client_info_new->ServerID);
	if(_client_info_valid) { ClientInfo_Clobber(_client_info_valid); }
	ClientInfo_NormalizeBeforeNewWrite(client_info_new);
	while(true) {
		if(memcmp(&SecurityClientDisk.ClientInfos[0].ClientID, client_id, sizeof(sSecurityID))) { ClientInfo_NormalizeFlashArea(client_id); }
		sSecurityClientInfo const *_client_info_ptr = &SecurityClientDisk.ClientInfos[1]; uint32_t _client_info_cnt = SECURITY_CLIENT_DISK_CLIENT_INFOS_COUNT - 1;
		while(_client_info_cnt--) {
			if(_client_info_ptr->ServerID == __id_free.ID[0]) {
				if(Flash_Write(_client_info_ptr, client_info_new, sizeof(sSecurityClientInfo))) {
					DBG_PutFormat("New Client Server ID: %08uX" NL, _client_info_ptr->ServerID);
					DBG_PutFormat("New Client Key: %Z" NL, &_client_info_ptr->Key, sizeof(sSecurityPayload));
					return;
				}
				ClientInfo_Clobber(_client_info_ptr);
			}
			_client_info_ptr++;
		}
		ClientInfo_NormalizeFlashArea(client_id);
	}
}

#endif
