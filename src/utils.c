#include "utils.h"
#include "base58.h"
#include "lcx_common.h"
#include "lcx_sha256.h"


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
    if (precision == 0 || precision >= len) {
        size_t i = 0;
        while (i < len && i < output_len - 1) {
            output[i] = input[i];
            i++;
        }
        output[i] = '\0';
        return;
    }
    size_t integer_part_len = len - precision;
    size_t i = 0, j = 0;

    for (; i < integer_part_len && i < output_len - 1; i++) {
        output[i] = input[j++];
    }
    output[i++] = '.';
    for (size_t k = 0; k < precision && i < output_len - 1; k++) {
        output[i++] = input[j++];
    }
    output[i] = '\0';
    size_t dot_pos = integer_part_len;
    bool all_zero = true;
    for (size_t k = dot_pos + 1; k < i - 1; k++) {
        if (output[k] != '0') {
            all_zero = false;
            break;
        }
    }
    if (all_zero) {
        output[dot_pos] = '\0';
    }
}