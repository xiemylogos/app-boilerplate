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

parser_status_e transaction_deserialize_header(buffer_t *buf,transaction_header_t *tx) {
    if (buf->size > MAX_TRANSACTION_LEN) {
        return WRONG_LENGTH_ERROR;
    }
    //nonce
    if(!buffer_read_u32(buf,&tx->nonce,LE)) {
        return NONCE_PARSING_ERROR;
    }
    //gasPrice
    if(!buffer_read_u64(buf,&tx->gas_price,LE)) {
        return GASPRICE_PARSING_ERROR;
    }
    //gasLimit
    if(!buffer_read_u64(buf,&tx->gas_limit,LE)) {
        return GASLIMIT_PARSING_ERROR;
    }
    //payer
    tx->payer = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return PAYER_PARSING_ERROR;
    }
    return PARSING_OK;
}