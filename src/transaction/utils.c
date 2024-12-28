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
#include "utils.h"
#include <stdio.h>
#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove
#include "buffer.h"


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

uint64_t getBytesValueByLen(buffer_t *buf,uint8_t len) {
    uint8_t *value;
    value = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, len)) {
        return 0;
    }
    return getValueByLen(value,len); 
}

uint64_t getValueByLen(uint8_t *value,uint8_t len) {
    uint64_t pre_value =0;
    for (int i = 0; i < len; i++) {
        pre_value |= ((int64_t)value[i] << (8 * i));
    }
    return pre_value;
}