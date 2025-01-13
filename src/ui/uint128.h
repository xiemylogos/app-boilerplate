#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"

static const char HEXDIGITS[] = "0123456789abcdef";

#define UPPER_P(x) x->elements[0]
#define LOWER_P(x) x->elements[1]
#define UPPER(x)   x.elements[0]
#define LOWER(x)   x.elements[1]

void reverseString(char *const str, uint32_t length);

typedef struct uint128_t {
    uint64_t elements[2];
} uint128_t;

void readu128BE(const uint8_t *const buffer, uint128_t *const target);
bool zero128(const uint128_t *const number);
void copy128(uint128_t *const target, const uint128_t *const number);
void clear128(uint128_t *const target);
void shiftl128(const uint128_t *const number, uint32_t value, uint128_t *const target);
void shiftr128(const uint128_t *const number, uint32_t value, uint128_t *const target);
uint32_t bits128(const uint128_t *const number);
bool equal128(const uint128_t *const number1, const uint128_t *const number2);
bool gt128(const uint128_t *const number1, const uint128_t *const number2);
bool gte128(const uint128_t *const number1, const uint128_t *const number2);
void add128(const uint128_t *const number1,
            const uint128_t *const number2,
            uint128_t *const target);
void sub128(const uint128_t *const number1,
            const uint128_t *const number2,
            uint128_t *const target);
void or128(const uint128_t *const number1, const uint128_t *const number2, uint128_t *const target);
void mul128(const uint128_t *const number1,
            const uint128_t *const number2,
            uint128_t *const target);
void divmod128(const uint128_t *const l,
               const uint128_t *const r,
               uint128_t *const div,
               uint128_t *const mod);
bool tostring128(const uint128_t *const number, uint32_t base, char *const out, uint32_t outLength);
bool tostring128_signed(const uint128_t *const number,
                        uint32_t base,
                        char *const out,
                        uint32_t out_length);

