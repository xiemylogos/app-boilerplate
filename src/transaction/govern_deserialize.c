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

#include "govern_deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e register_candidate_tx_deserialize(buffer_t *buf, register_candidate_t *tx) {
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
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t addr_len;
    if (!buffer_read_u8(buf, &addr_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (addr_len != ADDRESS_LEN) {  // 14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t init_pos_len;
    if (!buffer_read_u8(buf, &init_pos_len)) {
        return VALUE_PARSING_ERROR;
    }
    uint64_t value = getBytesValueByLen(buf, init_pos_len);
    if (value == 0) {
        return VALUE_PARSING_ERROR;
    }
    tx->init_pos = value;
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_u8(buf,&tx->ont_id_len)) {
        return VERSION_PARSING_ERROR;
    }
    tx->ont_id = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, tx->ont_id_len)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_varint(buf,&tx->key_no)) {
        return GASLIMIT_PARSING_ERROR;
    }
    if (tx->key_no >= 81) {
        tx->key_no = tx->key_no -80;
    }
    if (getThreeBytesValue(buf) != 13139050) {  // 6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_code;
    if (!buffer_read_u8(buf, &pre_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 108) {  // 6c
        return OPCODE_PARSING_ERROR;
    }
    uint8_t tag_len;
    if (!buffer_read_u8(buf, &tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, "registerCandidate", tag_len) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf, tag_len)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                          0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                          0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00};
    if (!buffer_can_read(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}

parser_status_e withdraw_tx_deserialize(buffer_t *buf, withdraw_t *tx) {
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
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t addr_len;
    if (!buffer_read_u8(buf, &addr_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (addr_len != ADDRESS_LEN) {  // 14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_varint(buf,&tx->peer_pubkey_length)) {
        return GASPRICE_PARSING_ERROR;
    }
    if (tx->peer_pubkey_length >= 81) {
        tx->peer_pubkey_length = tx->peer_pubkey_length -80;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len*tx->peer_pubkey_length)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_varint(buf,&tx->withdraw_list_number)) {
        return GASPRICE_PARSING_ERROR;
    }
    if (tx->withdraw_list_number >= 81) {
        tx->withdraw_list_number = tx->withdraw_list_number - 80;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_u8(buf,&tx->withdraw_list_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->withdraw_list = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,tx->withdraw_list_len*tx->withdraw_list_number)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_code;
    if (!buffer_read_u8(buf, &pre_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 108) {  // 6c
        return OPCODE_PARSING_ERROR;
    }
    uint8_t tag_len;
    if (!buffer_read_u8(buf, &tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, "withdraw", tag_len) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf, tag_len)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                          0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                          0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00};
    if (!buffer_can_read(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e quit_node_tx_deserialize(buffer_t *buf, quit_node_t *tx) {
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
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if (!buffer_read_u8(buf, &pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t addr_len;
    if (!buffer_read_u8(buf, &addr_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (addr_len != ADDRESS_LEN) {  // 14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_code;
    if (!buffer_read_u8(buf, &pre_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 108) {  // 6c
        return OPCODE_PARSING_ERROR;
    }
    uint8_t tag_len;
    if (!buffer_read_u8(buf, &tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, "quitNode", tag_len) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf, tag_len)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                          0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                          0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00};
    if (!buffer_can_read(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf, add_init_pos_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    if (buf->size > MAX_TRANSACTION_LEN) {
        return WRONG_LENGTH_ERROR;
    }
    // nonce
    if (!buffer_read_u32(buf, &tx->nonce, LE)) {
        return NONCE_PARSING_ERROR;
    }
    // gasPrice
    if (!buffer_read_u64(buf, &tx->gas_price, LE)) {
        return GASPRICE_PARSING_ERROR;
    }
    // gasLimit
    if (!buffer_read_u64(buf, &tx->gas_limit, LE)) {
        return GASLIMIT_PARSING_ERROR;
    }
    // payer
    tx->payer = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return PAYER_PARSING_ERROR;
    }
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if (getThreeBytesValue(buf) != 7063040) {  // 00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if (!buffer_read_u8(buf, &pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len)) {
        return FROM_PARSING_ERROR;
    }
    if (getThreeBytesValue(buf) != 13139050) {  // 6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t addr_len;
    if (!buffer_read_u8(buf, &addr_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (addr_len != ADDRESS_LEN) {  // 14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if (getThreeBytesValue(buf) != 13139050) {  // 6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pos_len;
    if (!buffer_read_u8(buf, &pos_len)) {
        return VALUE_PARSING_ERROR;
    }
    uint64_t value = getBytesValueByLen(buf, pos_len);
    if (value == 0) {
        return VALUE_PARSING_ERROR;
    }
    tx->pos = value;
    if (getThreeBytesValue(buf) != 13139050) {  // 6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_code;
    if (!buffer_read_u8(buf, &pre_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 108) {  // 6c
        return OPCODE_PARSING_ERROR;
    }
    uint8_t tag_len;
    if (!buffer_read_u8(buf, &tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, "addInitPos", tag_len) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf, tag_len)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                          0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                          0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00};
    if (!buffer_can_read(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf, sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf, reduce_init_pos_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    } 
     uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    } 
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }   
    uint8_t addr_len;
    if(!buffer_read_u8(buf,&addr_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (addr_len != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
   if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pos_len;
    if(!buffer_read_u8(buf,&pos_len)) {
        return VALUE_PARSING_ERROR;
    }
    uint64_t  value = getBytesValueByLen(buf,pos_len);
    if (value == 0) {
        return VALUE_PARSING_ERROR;
    }
    tx->pos = value;
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t  pre_code;
    if(!buffer_read_u8(buf,&pre_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 108) { //6c
        return OPCODE_PARSING_ERROR;
    }
    uint8_t tag_len;
    if(!buffer_read_u8(buf,&tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;   
    } 
    if(memcmp(buf->ptr+buf->offset, "reduceInitPos", tag_len) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,tag_len)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf, change_max_authorization_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    uint8_t pre_pub;
    if(!buffer_read_u8(buf,&pre_pub)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_pub != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t max_authorize_len;
    if(!buffer_read_u8(buf,&max_authorize_len)) {
        return VALUE_PARSING_ERROR;
    }
    uint64_t  value = getBytesValueByLen(buf,max_authorize_len);
    if (value == 0) {
        return VALUE_PARSING_ERROR;
    }
    tx->max_authorize = value;
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t  pre_code;
    if(!buffer_read_u8(buf,&pre_code)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 108) { //6c
        return OPCODE_PARSING_ERROR;
    }
    uint8_t tag_len;
    if(!buffer_read_u8(buf,&tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;   
    } 
    if(memcmp(buf->ptr+buf->offset, "changeMaxAuthorization", tag_len) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,tag_len)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf, set_fee_percentage_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }  
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub;
    if(!buffer_read_u8(buf,&pre_pub)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_pub != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t cost_len;
    if(!buffer_read_u8(buf,&cost_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_cost = getBytesValueByLen(buf,cost_len);

    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t stake_cost_len;
    if(!buffer_read_u8(buf,&stake_cost_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->stake_cost = getBytesValueByLen(buf,stake_cost_len);
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 4204) { //6c10
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, "setFeePercentage", 16) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,16)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf, authorize_for_peer_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub;
    if(!buffer_read_u8(buf,&pre_pub)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_pub != 20) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    } 

    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }  
    if(!buffer_read_varint(buf,&tx->peer_pubkey_length)) {
        return GASPRICE_PARSING_ERROR;
    }
    if (tx->peer_pubkey_length >= 81) {
        tx->peer_pubkey_length = tx->peer_pubkey_length -80;
    }
   if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len*tx->peer_pubkey_length)) {
        return FROM_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    if(!buffer_read_varint(buf,&tx->pos_list_length)) {
        return GASPRICE_PARSING_ERROR;
    }
    if (tx->pos_list_length >= 81) {
        tx->pos_list_length = tx->pos_list_length -80;
    } 
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }  
    tx->pos_list = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, 1*tx->pos_list_length)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 4204) { //6c10
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, "authorizeForPeer", 16) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,16)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }

    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf, un_authorize_for_peer_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub;
    if(!buffer_read_u8(buf,&pre_pub)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_pub != 20) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    } 
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }  
    if(!buffer_read_varint(buf,&tx->peer_pubkey_length)) {
        return GASPRICE_PARSING_ERROR;
    }
    if (tx->peer_pubkey_length >= 81) {
        tx->peer_pubkey_length = tx->peer_pubkey_length -80;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len*tx->peer_pubkey_length)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    if(!buffer_read_varint(buf,&tx->pos_list_length)) {
        return GASPRICE_PARSING_ERROR;
    }
    if (tx->pos_list_length >= 81) {
        tx->pos_list_length = tx->pos_list_length -80;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    tx->pos_list = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, 1*tx->pos_list_length)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 4716) { //6c12
        return OPCODE_PARSING_ERROR;
    }
    //todo 
    if(memcmp(buf->ptr+buf->offset, "unAuthorizeForPeer", 18) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,18)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }

    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}


parser_status_e withdraw_ong_tx_deserialize(buffer_t *buf, withdraw_ong_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    } 
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub;
    if(!buffer_read_u8(buf,&pre_pub)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_pub != 20) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 2924) { //6c0b
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, "withdrawOng", 11) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,11)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}

parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf, withdraw_fee_t *tx) {
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
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub;
    if(!buffer_read_u8(buf,&pre_pub)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_pub != 20) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if(getThreeBytesValue(buf) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 2924) { //6c0b
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, "withdrawFee", 11) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,11)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }

    uint8_t end_data[] = {
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
        0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79,
        0x2E, 0x4E, 0x61, 0x74, 0x69, 0x76, 0x65, 0x2E,
        0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00
    };
    if(!buffer_can_read(buf,sizeof(end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, end_data, sizeof(end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
