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
#include "buffer.h"

#include "oep4_deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e oep4_transaction_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL oep4 tx");

    if (buf->size > MAX_TRANSACTION_LEN) {
        return WRONG_LENGTH_ERROR;
    }
    //version
    if(!buffer_read_u8(buf,&tx->version)) {
        return VERSION_PARSING_ERROR;
    }
    //txType
    if(!buffer_read_u8(buf,&tx->tx_type)) {
        return TXTYPE_PARSING_ERROR;
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
    if (!buffer_seek_cur(buf,1)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    return PARSING_OK;
    //return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}

parser_status_e oep4_state_info_deserialize(buffer_t *buf,size_t length, state_info_v2 *tx) {
    if(!buffer_can_read(buf,length)) {
        return WRONG_LENGTH_ERROR;
    }
    if (length <= PAYLOAD_MIN_LENGTH_LIMIT) {
        return WRONG_LENGTH_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset + length - 56 - 8, "transfer", 8) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,length - 56)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    tx->to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    /*
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
     */
    if (!buffer_read_u64(buf, &tx->value, LE)) {
        return VALUE_PARSING_ERROR;
    }
    /*
    if (!buffer_seek_cur(buf,18)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    */
    return PARSING_OK;
}
