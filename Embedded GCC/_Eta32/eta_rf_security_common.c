/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf_security_common.c
 ***********************************************************************/

#include "eta_rf_security_common.h"

#if defined SECURITY_SERVER || defined SECURITY_CLIENT
 
sSecurityPayload const __key_free = { .Data = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF } };
sSecurityID const __id_free = { .ID = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF } };
sSecurityID const __id_clobbered = { .ID = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 } };

void Security_RandomFill(uint32_t *dst, uint32_t cnt) { while(cnt--) { *dst++ = Security_RandomGet(); } }

void Security_CipherEncode(uint32_t const key[8], void *data, uint32_t blockcnt) {
	aes256_ctx_t _aes_ctx;
	uint32_t _iv[] = { SECURITY_INITIALIZATION_VECTOR };
	aes256_init(key, &_aes_ctx); uint8_t *_data_ptr = data;
	while(blockcnt--) { memxor(_data_ptr, _iv, 16); aes256_enc(_data_ptr, &_aes_ctx); memcpy(_iv, _data_ptr, 16); _data_ptr += 16; }
}
void Security_CipherDecode(uint32_t const key[8], void *data, uint32_t blockcnt) {
	aes256_ctx_t _aes_ctx;
	uint32_t _iv[] = { SECURITY_INITIALIZATION_VECTOR }; uint8_t _tmp[16];
	aes256_init(key, &_aes_ctx); uint8_t *_data_ptr = data;
	while(blockcnt--) { memcpy(_tmp, _data_ptr, 16); aes256_dec(_data_ptr, &_aes_ctx); memxor(_data_ptr, _iv, 16); memcpy(_iv, _tmp, 16); _data_ptr += 16; }
}

char const *Security_GetUserFriendlySequence(uint32_t sequence) { static char _buf[20]; char *_buf_ptr = _buf; for(uint32_t _i = 16; _i; _i--) { *_buf_ptr++ = '1' + ((sequence >> ((_i - 1) << 1)) & 0x03); } *_buf_ptr = '\0'; return _buf; }

#endif
