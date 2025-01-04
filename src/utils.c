#include "utils.h"
#include "base58.h"
#include "lcx_common.h"
#include "lcx_sha256.h"
#include "lcx_ripemd160.h"
#include "crypto_helpers.h"
#include "globals.h"


/** the length of a SHA256 hash */
#define SHA256_HASH_LEN 32

/** the current version of the address field */
#define ADDRESS_VERSION 23

/** length of tx.output.script_hash */
#define SCRIPT_HASH_LEN 20

/** length of the checksum used to convert a script_hash into an Address. */
#define SCRIPT_HASH_CHECKSUM_LEN 4

/** length of a tx.output Address, after Base58 encoding. */
#define ADDRESS_BASE58_LEN 34

/** length of a Address before encoding, which is the length of <address_version>+<script_hash>+<checksum> */
#define ADDRESS_LEN_PRE (1 + SCRIPT_HASH_LEN + SCRIPT_HASH_CHECKSUM_LEN)

#define VERIFICATION_SCRIPT_LENGTH 35

#define UINT160_LEN 20

int script_hash_to_address(char* out, size_t out_len, const unsigned char* script_hash) {

    static cx_sha256_t data_hash;
    unsigned char data_hash_1[SHA256_HASH_LEN];
    unsigned char data_hash_2[SHA256_HASH_LEN];
    unsigned char address[ADDRESS_LEN_PRE];

    address[0] = ADDRESS_VERSION;
    memcpy(&address[1], script_hash, SCRIPT_HASH_LEN);

    cx_sha256_init(&data_hash);
    CX_ASSERT(cx_hash_no_throw(&data_hash.header, CX_LAST, address, SCRIPT_HASH_LEN + 1, data_hash_1, 32));
    cx_sha256_init(&data_hash);
    CX_ASSERT(cx_hash_no_throw(&data_hash.header, CX_LAST, data_hash_1, SHA256_HASH_LEN, data_hash_2, 32));

    memcpy(&address[1 + SCRIPT_HASH_LEN], data_hash_2, SCRIPT_HASH_CHECKSUM_LEN);

    return base58_encode(address, sizeof(address), out, out_len);
}


size_t utf8_strlen(const uint8_t* str) {
    size_t len = 0;
    const uint8_t* ptr = str;
    while (*ptr) {
        if ((*ptr & 0x80) == 0) {
            ptr += 1;
        } else if ((*ptr & 0xE0) == 0xC0) {
            ptr += 2;
        } else if ((*ptr & 0xF0) == 0xE0) {
            ptr += 3;
        } else if ((*ptr & 0xF8) == 0xF0) {
            ptr += 4;
        } else {
            ptr += 1;
        }
        len++;
    }
    return len;
}

void process_precision(const char *input, int precision, char *output, size_t output_len) {
    size_t len = 0;
    while (input[len] != '\0') {
        len++;
    }
    size_t precision_unsigned = (size_t)(precision >= 0 ? precision : 0);

    if (precision_unsigned == 0 || precision_unsigned >= len) {
        size_t i = 0;
        while (i < len && i < output_len - 1) {
            output[i] = input[i];
            i++;
        }
        output[i] = '\0';
        return;
    }

    size_t integer_part_len = len - precision_unsigned;
    size_t i = 0, j = 0;

    for (; i < integer_part_len && i < output_len - 1; i++) {
        output[i] = input[j++];
    }
    output[i++] = '.';
    for (size_t k = 0; k < precision_unsigned && i < output_len - 1; k++) {
        output[i++] = input[j++];
    }
    output[i] = '\0';
    size_t end = i - 1;
    while (end > integer_part_len && output[end] == '0') {
        end--;
    }
    if (output[end] == '.') {
        output[end] = '\0';
    } else {
        output[end + 1] = '\0';
    }
}

bool create_signature_redeem_script(const uint8_t *uncompressed_key, uint8_t* out, size_t out_len) {
    if (out_len != VERIFICATION_SCRIPT_LENGTH) {
        return false;
    }
    const uint8_t *x = &uncompressed_key[1];
    const uint8_t *y = &uncompressed_key[33];
    uint8_t compressed_key[33];
    compressed_key[0] = (y[31] & 1) ? 0x03 : 0x02;
    memcpy(&compressed_key[1], x, 32);
    out[0] = 0x21;
    memcpy(&out[1], compressed_key, sizeof(compressed_key));
    out[34] = 0xac;
    return true;
}

void generate_address_from_public_key(const uint8_t *compressed_key, size_t key_len, uint8_t *output_hash) {
    struct {
        cx_ripemd160_t ripe;
    } u;
    uint8_t sha256_hash[SHA256_HASH_LEN];
    cx_hash_sha256(compressed_key, key_len, sha256_hash, sizeof(sha256_hash));
    cx_ripemd160_init(&u.ripe);
    CX_ASSERT(cx_hash_no_throw(&u.ripe.header, CX_LAST, sha256_hash, 32, output_hash, 20));
}

bool ont_address_from_pubkey(char* out, size_t out_len) {
    uint8_t uncompressed_key[65];  /// format (1), x-coordinate (32), y-coodinate (32)
    uint8_t chain_code[32];
    cx_err_t error = bip32_derive_get_pubkey_256(CX_CURVE_256R1,
                                                 G_context.bip32_path,
                                                 G_context.bip32_path_len,
                                                 uncompressed_key,
                                                 chain_code,
                                                 CX_SHA256);

    if (error != CX_OK) {
        return false;
    }
    uint8_t verification_script[VERIFICATION_SCRIPT_LENGTH] = {0};
    if(!create_signature_redeem_script(uncompressed_key, verification_script,sizeof verification_script)) {
        return false;
    }
    uint8_t ripemd160_hash[UINT160_LEN] = {0};
    generate_address_from_public_key(verification_script, sizeof verification_script, ripemd160_hash);
    script_hash_to_address(out, out_len,ripemd160_hash);
    return true;
}