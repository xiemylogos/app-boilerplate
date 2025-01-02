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

bool create_signature_redeem_script(const uint8_t* public_key, uint8_t* out, size_t out_len) {
    if (out_len != VERIFICATION_SCRIPT_LENGTH) {
        return false;
    }
    // we first have to compress the public key
    uint8_t compressed_key[33];
    compressed_key[0] = ((public_key[63] & 1) ? 0x03 : 0x02);
    memcpy(&compressed_key[1], public_key, 32);

    out[0] = 0xc;   // OpCode.PUSHDATA1;
    out[1] = 0x21;  // data size, 33 bytes for compressed public key
    memcpy(&out[2], compressed_key, sizeof(compressed_key));

    out[35] = 0x41;                  // OpCode.SYSCALL
    uint32_t checksig = 0x27B3E756;  // Syscall "System.Crypto.CheckSig"
    memcpy(&out[36], &checksig, 4);

    return true;
}

void public_key_hash160(const unsigned char* in, unsigned short inlen, unsigned char* out) {
    union {
        cx_sha256_t shasha;
        cx_ripemd160_t riprip;
    } u;
    unsigned char buffer[32];

    cx_sha256_init(&u.shasha);
    CX_ASSERT(cx_hash_no_throw(&u.shasha.header, CX_LAST, in, inlen, buffer, 32));
    cx_ripemd160_init(&u.riprip);
    CX_ASSERT(cx_hash_no_throw(&u.riprip.header, CX_LAST, buffer, 32, out, 20));
}

bool ont_address_from_pubkey(char* out, size_t out_len) {
    uint8_t public_key[65];  /// format (1), x-coordinate (32), y-coodinate (32)
    uint8_t chain_code[32];
    cx_err_t error = bip32_derive_get_pubkey_256(CX_CURVE_256R1,
                                                 G_context.bip32_path,
                                                 G_context.bip32_path_len,
                                                 public_key,
                                                 chain_code,
                                                 CX_SHA256);

    if (error != CX_OK) {
        return false;
    }
    // we need to go through 3 steps
    // 1. create a verification script with the public key
    // 2. create a script hash of the verification script (using sha256 + ripemd160)
    // 3. base58check encode the NEO account version + script hash to get the address
    unsigned char verification_script[VERIFICATION_SCRIPT_LENGTH];
    unsigned char script_hash[UINT160_LEN];

    // step 1
    if (!create_signature_redeem_script(public_key + 1, verification_script, sizeof(verification_script))) {
        return false;
    }
    // step 2
    public_key_hash160(verification_script, sizeof(verification_script), script_hash);
    // step 3
    script_hash_to_address(out, out_len, script_hash);
    return true;
}