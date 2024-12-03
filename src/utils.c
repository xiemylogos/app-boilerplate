#include "utils.h"
#include "base58.h"
#include "lcx_common.h"
#include "lcx_sha256.h"


/** the length of a SHA256 hash */
#define SHA256_HASH_LEN 32

/** the current version of the address field */
#define ADDRESS_VERSION 0x35

/** length of tx.output.script_hash */
#define SCRIPT_HASH_LEN 20

#define UINT160_LEN 20
/** length of the checksum used to convert a script_hash into an Address. */
#define SCRIPT_HASH_CHECKSUM_LEN 4

/** length of a tx.output Address, after Base58 encoding. */
#define ADDRESS_BASE58_LEN 34

/** length of a Address before encoding, which is the length of <address_version>+<script_hash>+<checksum> */
#define ADDRESS_LEN_PRE (1 + SCRIPT_HASH_LEN + SCRIPT_HASH_CHECKSUM_LEN)


void script_hash_to_address(char* out, size_t out_len, const unsigned char* script_hash) {

    static cx_sha256_t data_hash;
    unsigned char data_hash_1[SHA256_HASH_LEN];
    unsigned char data_hash_2[SHA256_HASH_LEN];
    unsigned char address[ADDRESS_LEN_PRE];

    address[0] = ADDRESS_VERSION;
    memcpy(&address[1], script_hash, UINT160_LEN);

    cx_sha256_init(&data_hash);
    CX_ASSERT(cx_hash_no_throw(&data_hash.header, CX_LAST, address, UINT160_LEN + 1, data_hash_1, 32));
    cx_sha256_init(&data_hash);
    CX_ASSERT(cx_hash_no_throw(&data_hash.header, CX_LAST, data_hash_1, SHA256_HASH_LEN, data_hash_2, 32));

    memcpy(&address[1 + UINT160_LEN], data_hash_2, SCRIPT_HASH_CHECKSUM_LEN);

    base58_encode(address, sizeof(address), out, out_len);
}