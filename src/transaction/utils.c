/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include <stdio.h>
#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove
#include "buffer.h"
#include <inttypes.h>
#include <string.h>

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

#include "types.h"

bool transaction_utils_check_encoding(const uint8_t *memo, uint64_t memo_len) {
    LEDGER_ASSERT(memo != NULL, "NULL memo");

    for (uint64_t i = 0; i < memo_len; i++) {
        if (memo[i] > 0x7F) {
            return false;
        }
    }

    return true;
}

bool transaction_utils_format_memo(const uint8_t *memo,
                                   uint64_t memo_len,
                                   char *dst,
                                   uint64_t dst_len) {
    LEDGER_ASSERT(memo != NULL, "NULL memo");
    LEDGER_ASSERT(dst != NULL, "NULL dst");

    if (memo_len > MAX_MEMO_LEN || dst_len < memo_len + 1) {
        return false;
    }

    memmove(dst, memo, memo_len);
    dst[memo_len] = '\0';

    return true;
}

//6a7cc8 or 00c66b
uint64_t getThreeBytesValue(buffer_t *buf) {
    uint8_t *value;
    value = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, 3)) {
        return 0;
    }
    uint64_t pre_value =0;
    for (int i = 0; i < 3; i++) {
        pre_value |= ((int64_t)value[i] << (8 * i));
    }
    return pre_value;
}

uint64_t getBytesValueByLen(buffer_t *buf,uint8_t len) {
    uint8_t *value;
    value = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, len)) {
        return 0;
    }
    uint64_t pre_value =0;
    for (int i = 0; i < len; i++) {
        pre_value |= ((int64_t)value[i] << (8 * i));
    }
    return pre_value;
}

void uint128_to_string(uint64_t value[2], char* output) {
    snprintf(output, 40, "%" PRIu64 "%" PRIu64, value[1], value[0]);
}

typedef struct {
    uint64_t high;  // high 64
    uint64_t low;   // low 64
} uint128_t;

void uint128_to_string_with_precision(uint64_t values[2], uint64_t precision, char* output) {
    uint128_t value;
    value.high = values[1];
    value.low = values[0];
    unsigned long long intPart = (unsigned long long)(value.high);
    unsigned long long fracPart = value.low;

    char intStr[128];
    snprintf(intStr, sizeof(intStr), "%llu", intPart);

    double fraction = (double)fracPart / (1ULL << 64);
    for (uint64_t i = 0; i < precision; i++) {
        fraction *= 10;
    }

    unsigned long long fracPartPrecision = (unsigned long long)fraction;

    snprintf(output, 128, "%s", intStr);

    if (precision > 0) {
        char fracStr[128];
        snprintf(fracStr, sizeof(fracStr), ".%0*llu", (int)precision, fracPartPrecision);
        strcat(output, fracStr);
    }
}