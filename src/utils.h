#pragma once

#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <stdbool.h>  // bool

#define VERIFICATION_SCRIPT_LENGTH 40
#define UINT160_LEN 20

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define BAIL_IF(x)           \
    do {                     \
        int err = x;         \
        if (err) return err; \
    } while (0)

#define assert_string_equal(actual, expected) assert(strcmp(actual, expected) == 0)

#define assert_pubkey_equal(actual, expected) assert(memcmp(actual, expected, 32) == 0)

int script_hash_to_address(char* out, size_t out_len, const unsigned char* script_hash);

size_t utf8_strlen(const uint8_t* str);

void process_precision(const char *input, int precision, char *output, size_t output_len);



bool ont_address_from_pubkey(char* out, size_t out_len);
