#pragma once

#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdbool.h>  // bool

void script_hash_to_address(char* out, size_t out_len, const unsigned char* script_hash);

size_t utf8_strlen(const uint8_t* str);

void process_precision(const char *input, int precision, char *output, size_t output_len);


bool create_signature_redeem_script(const uint8_t *uncompressed_key, uint8_t* out, size_t out_len);
void generate_address_from_public_key(const uint8_t *compressed_key, size_t key_len, uint8_t *output_hash);

bool ont_address_from_pubkey(char* out, size_t out_len);
/**
 * Convert public key to ont address.
 * @param[in]  public_key
 *   Pointer to byte buffer with public key.
 *   The public key is represented as 65 bytes with 1 byte for format and 32 bytes for
 *   each coordinate.
 * @param[out] out
 *   Pointer to output byte buffer for address.
 * @param[in]  out_len
 *   Length of output byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool ont_address_by_pubkey(const uint8_t public_key[static 65],char* out, size_t out_len);

bool get_token_amount(const uint8_t value_len,const uint64_t value[2],const uint8_t decimals,char* amount,size_t amount_len);

uint8_t get_oep4_token_decimals(uint8_t *contract_addr);

void get_ong_fee(uint64_t gas_price,uint64_t gas_limit,char* out, size_t out_len);

bool get_native_token_amount(uint8_t *contract_addr,const uint8_t value_len,const uint64_t value[2],char* out, size_t out_len);

bool get_oep4_token_amount(uint8_t *contract_addr,const uint8_t value_len,const uint64_t value[2],char* out, size_t out_len);

bool convert_uint64_to_char(char* out, size_t out_len,uint64_t amount);