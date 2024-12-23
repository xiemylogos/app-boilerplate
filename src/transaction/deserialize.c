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

#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e transaction_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

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
    uint8_t  op_code;
    if(!buffer_read_u8(buf,&op_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (op_code != 0x7b) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size-buf->offset > PAYLOAD_TRANSFER_V2_LEN) {
        if(memcmp(buf->ptr+buf->size - 46 - 10 - 1, "transferV2", 10) != 0) {
            return PARSE_STRING_MATCH_ERROR;
        }
        if(getThreeBytesValue(buf) != 7063040) { //00c66b
            return VALUE_PARSING_ERROR;
        }
        uint8_t  from_op_code;
        if(!buffer_read_u8(buf,&from_op_code)) {
            return OPCODE_PARSING_ERROR;
        }
        if (from_op_code != 0x14) {
            return OPCODE_PARSING_ERROR;
        }
        tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
        if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
            return FROM_PARSING_ERROR;
        }
        if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
            return VALUE_PARSING_ERROR;
        }
        uint8_t  to_op_code;
        if(!buffer_read_u8(buf,&to_op_code)) {
            return OPCODE_PARSING_ERROR;
        }
        if (to_op_code != 0x14) {
            return OPCODE_PARSING_ERROR;
        }
        tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
        if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
            return TO_PARSING_ERROR;
        }
        if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
            return VALUE_PARSING_ERROR;
        }
        uint8_t  value_len;
        if(!buffer_read_u8(buf,&value_len)) {
            return OPCODE_PARSING_ERROR;
        }
        tx->payload.value = (uint8_t*)(buf->ptr+buf->offset);
        if (!buffer_seek_cur(buf, value_len)) {
            return VALUE_PARSING_ERROR;
        }
        if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
            return VALUE_PARSING_ERROR;
        }
        if(getBytesValueByLen(buf,4) != 180441452) { //6c51c10a
            return VALUE_PARSING_ERROR;
        }
        if(memcmp(buf->ptr+buf->offset, "transferV2", 10) != 0) {
            return PARSE_STRING_MATCH_ERROR;
        }
        if (!buffer_seek_cur(buf,10)) {
            return BUFFER_OFFSET_MOVE_ERROR;
        }
        uint8_t  contract_op_code;
        if(!buffer_read_u8(buf,&contract_op_code)) {
            return OPCODE_PARSING_ERROR;
        }
        if (contract_op_code != 0x14) {
            return OPCODE_PARSING_ERROR;
        }
        tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
        if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
            return CONTRACT_ADDR_PARSING_ERROR;
        }
        if (memcmp(tx->payload.contract_addr,ONG_ADDR,20) != 0 &&
            memcmp(tx->payload.contract_addr,ONT_ADDR,20) != 0) {
            return CONTRACT_ADDR_PARSING_ERROR;
        }
        uint8_t end_data[] = {
            0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
            0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
            0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
            0x65, 0x00
        };
        if(!buffer_can_read(buf,sizeof(end_data))) {
            return DATA_END_PARSING_ERROR;
        }
        if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
            return DATA_END_PARSING_ERROR;
        }
        if (!buffer_seek_cur(buf,26)) {
            return DATA_END_PARSING_ERROR;
        }
    } else if (buf->size-buf->offset > PAYLOAD_TRANSFER_FROM_V2_LEN) {
        if(memcmp(buf->ptr + buf->size-buf->offset - 46 - 14, "transferFromV2", 10) != 0) {
            return PARSE_STRING_MATCH_ERROR;
        }
        return PARSE_STRING_MATCH_ERROR;
    } else {
        return TO_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}

parser_status_e state_info_deserialize(buffer_t *buf,size_t length, state_info_v2 *tx) {
    if(!buffer_can_read(buf,length)) {
        return WRONG_LENGTH_ERROR;
    }
    if (length <= PAYLOAD_MIN_LENGTH_LIMIT) {
        return WRONG_LENGTH_ERROR;
    }
    if (memcmp(buf->ptr +buf->offset+ length - 22 - 1, "Ontology.Native.Invoke", 22) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (length > PAYLOAD_TRANSFER_V2_LEN) {
        if(memcmp(buf->ptr+buf->offset + length - 46 - 10 - 1, "transferV2", 10) != 0) {
            return PARSE_STRING_MATCH_ERROR;
        }
        if (!buffer_seek_cur(buf,4)) {
            return BUFFER_OFFSET_MOVE_ERROR;
        }
        tx->from = (uint8_t*)(buf->ptr+buf->offset);
        if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
            return FROM_PARSING_ERROR;
        }
        if (!buffer_seek_cur(buf,4)) {
            return BUFFER_OFFSET_MOVE_ERROR;
        }
        tx->to = (uint8_t*)(buf->ptr+buf->offset);
        if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
            return TO_PARSING_ERROR;
        }
        if (!buffer_seek_cur(buf,4)) {
            return BUFFER_OFFSET_MOVE_ERROR;
        }
        if (!buffer_read_u64(buf, &tx->value, LE)) {
            return VALUE_PARSING_ERROR;
        }
        if (!buffer_seek_cur(buf,18)) {
            return BUFFER_OFFSET_MOVE_ERROR;
        }
        tx->contract_addr = (uint8_t *) (buf->ptr + buf->offset);
        if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
            return CONTRACT_ADDR_PARSING_ERROR;
        }
    } else if (length > PAYLOAD_TRANSFER_FROM_V2_LEN) {
        if(memcmp(buf->ptr + length - 46 - 14, "transferFromV2", 10) != 0) {
            return PARSE_STRING_MATCH_ERROR;
        }
        return PARSE_STRING_MATCH_ERROR;
    } else {
        return TO_PARSING_ERROR;
    }
    return PARSING_OK;
}
