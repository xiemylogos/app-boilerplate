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
#include "format.h"
#include "../ui/utils.h"
#include "../globals.h"
#include "parse.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif


parser_status_e check_govern_end_data(buffer_t *buf,const char* param_name ) {
    if (getBytesValueByLen(buf,3) != 13139050) {  // 6a7cc8
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
    if (!buffer_read_u8(buf, &tag_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tag_len == 0) {
        return OPCODE_PARSING_ERROR;
    }
    if (memcmp(buf->ptr + buf->offset, param_name, tag_len) != 0) {
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

parser_status_e register_candidate_tx_deserialize(buffer_t *buf, register_candidate_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
     cfg_t RegisterCandidateTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b,0x42},
            .data_len = 4
        },
        {
            .data_type = PUBKEY_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },

        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x00,0x6a,0x7c,0xc8},
            .data_len = 7
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x11,0x72,0x65,0x67,0x69,0x73,0x74,0x65,
                                 0x72,0x43,0x61,0x6e,0x64,0x69,0x64,0x61,0x74,0x65,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 69
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(RegisterCandidateTx) / sizeof(RegisterCandidateTx[0]);
    parser_status_e status_tx = parse_tx(buf,RegisterCandidateTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, RegisterCandidateTx[1].data, 66);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           RegisterCandidateTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               RegisterCandidateTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;

/*
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
        return ACCOUNT_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_u8(buf,&tx->ont_id_len)) {
        return DATA_PARSING_ERROR;
    }
    tx->ont_id = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, tx->ont_id_len)) {
        return DATA_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_u8(buf,&tx->key_no_len)) {
        return DATA_PARSING_ERROR;
    }
    if (tx->key_no_len >= 81) {
        tx->key_no = tx->key_no_len - 80;
    } else {
        tx->key_no = getBytesValueByLen(buf,tx->key_no_len);
    }
    memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->init_pos)) {
        return DATA_PARSING_ERROR;
    }



    return check_govern_end_data(buf,RegisterCandidate);
    */
}

parser_status_e withdraw_tx_deserialize(buffer_t *buf, withdraw_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e  status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }

    cfg_t WithDrawTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = MULTIPLE_PUBKEY_DATA_TYPE
        },
        {
            .data_type = MULTIPLE_AMOUNT_DATA_TYPE
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x08,0x77,0x69,0x74,0x68,0x64,0x72,0x61,0x77,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 60
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(WithDrawTx) / sizeof(WithDrawTx[0]);
    parser_status_e status_tx = parse_tx(buf,WithDrawTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    uint64_t  total_value = 0;
    for(size_t j=resultLength/2;j<resultLength;j++) {
        total_value += getValueByLen(resultArray[j], 8);
    }
    if(resultLength >0) {
        memcpy(G_context.display_data.peer_pubkey, resultArray[0], PEER_PUBKEY_LEN);
    }
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           WithDrawTx[1].data);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               total_value)) {
        return DATA_PARSING_ERROR;
    }
//For other devices, only 3 pubkeys are displayed at most.
#if !defined(TARGET_NANOS)
    G_context.display_data.pubkey_number = resultLength/2;
    if(resultLength == 4) {
        memcpy(G_context.display_data.content_three, resultArray[1], PEER_PUBKEY_LEN);
    }
    if(resultLength == 6) {
        memcpy(G_context.display_data.content_four, resultArray[2], PEER_PUBKEY_LEN);
    }
#endif
    return PARSING_OK;

/*
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if (op_code_size >= 253) {//fd
        uint16_t code_size;
        if (!buffer_read_u16(buf, &code_size,LE)) {
            return OPCODE_PARSING_ERROR;
        }
        if (!buffer_can_read(buf, code_size)) {
            return DATA_END_PARSING_ERROR;
        }
    }
    */
    /*
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
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
        return ACCOUNT_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t op_code_value;
    if (!buffer_read_u8(buf, &op_code_value)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey_number = op_code_value - 80;
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    for(int i=0;i<tx->peer_pubkey_number;i++) {
        if (!buffer_read_u8(buf, &tx->peer_pubkey_length)) {
            return VALUE_PARSING_ERROR;
        }
        if(i<3) {
            tx->peer_pubkey[i] = (uint8_t *) (buf->ptr + buf->offset);
            if (!buffer_seek_cur(buf, tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        } else {
            if (!buffer_seek_cur(buf, tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        }
        if (getBytesValueByLen(buf, 3) != 13139050) {  // 6a7cc8
            return VALUE_PARSING_ERROR;
        }
    }
    if (!buffer_read_u8(buf, &tx->withdraw_number)) {
        return VALUE_PARSING_ERROR;
    }
    if (tx->withdraw_number >= 81) {
        tx->withdraw_number =  tx->withdraw_number-80;
    }
    if (tx->peer_pubkey_number != tx->withdraw_number) {
        return VALUE_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    tx->withdraw_value = 0;
    for(int j=0;j<tx->withdraw_number;j++) {
        if (!buffer_read_u8(buf, &tx->withdraw_list_len)) {
            return VALUE_PARSING_ERROR;
        }
        if (tx->withdraw_list_len >= 81) {
            tx->withdraw_value += tx->withdraw_list_len - 80;
        } else {
            tx->withdraw_value += getBytesValueByLen(buf, tx->withdraw_list_len);
        }
        if (j+1 < tx->withdraw_number) {
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
        }
    }
    if (tx->peer_pubkey_number>0) {
        memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey[0], 66);
    }
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->withdraw_value)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,Withdraw);
*/
}


parser_status_e quit_node_tx_deserialize(buffer_t *buf, quit_node_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t QuitNodeTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b,0x42},
            .data_len = 4
        },
        {
            .data_type = PUBKEY_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x08,0x71,0x75,0x69,0x74,0x4e,0x6f,0x64,0x65,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 60
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(QuitNodeTx) / sizeof(QuitNodeTx[0]);
    parser_status_e status_tx = parse_tx(buf,QuitNodeTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, QuitNodeTx[1].data, 66);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           QuitNodeTx[3].data);

    return PARSING_OK;
    /*
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if (!buffer_read_u8(buf, &pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
        return ACCOUNT_PARSING_ERROR;
    }
    memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    return check_govern_end_data(buf,QuitNode);
     */
}


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf, add_init_pos_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t AddInitPosTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b,0x42},
            .data_len = 4
        },
        {
            .data_type = PUBKEY_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },

        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x0a,0x61,0x64,0x64,0x49,0x6e,0x69,0x74,0x50,0x6f,0x73,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 62
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(AddInitPosTx) / sizeof(AddInitPosTx[0]);
    parser_status_e status_tx = parse_tx(buf,AddInitPosTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, AddInitPosTx[1].data, 66);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           AddInitPosTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               AddInitPosTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
/*
    uint8_t op_code_size;
    if (!buffer_read_u8(buf, &op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (!buffer_can_read(buf, op_code_size)) {
        return DATA_END_PARSING_ERROR;
    }
    if (getBytesValueByLen(buf,3) != 7063040) {  // 00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if (!buffer_read_u8(buf, &pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if (getBytesValueByLen(buf,3) != 13139050) {  // 6a7cc8
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
        return ACCOUNT_PARSING_ERROR;
    }
    if (getBytesValueByLen(buf,3) != 13139050) {  // 6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pos_len;
    if (!buffer_read_u8(buf, &pos_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (pos_len >= 81) {
        tx->pos = pos_len - 80;
    } else {
        tx->pos = getBytesValueByLen(buf, pos_len);
    }
    memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->pos)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,AddInitPos);
    */
}

parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf, reduce_init_pos_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t ReduceInitPosTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b,0x42},
            .data_len = 4
        },
        {
            .data_type = PUBKEY_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },

        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x0d,0x72,0x65,0x64,0x75,0x63,0x65,0x49,0x6e,0x69,0x74,0x50,0x6f,0x73,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 65
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(ReduceInitPosTx) / sizeof(ReduceInitPosTx[0]);
    parser_status_e status_tx = parse_tx(buf,ReduceInitPosTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, ReduceInitPosTx[1].data, 66);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           ReduceInitPosTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               ReduceInitPosTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
    /*
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return OPCODE_PARSING_ERROR;
    } 
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    } 
     uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    } 
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
        return ACCOUNT_PARSING_ERROR;
    }
   if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pos_len;
    if(!buffer_read_u8(buf,&pos_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (pos_len >= 81) {
        tx->pos = pos_len - 80;
    } else {
        tx->pos = getBytesValueByLen(buf, pos_len);
    }
    memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->pos)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,ReduceInitPos);
     */
}


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf, change_max_authorization_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t ChangeMaxAuthorizationTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b,0x42},
            .data_len = 4
        },
        {
            .data_type = PUBKEY_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },

        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x16,0x63,0x68,0x61,0x6e,0x67,0x65,0x4d,0x61,0x78,0x41,0x75,0x74,
                                  0x68,0x6f,0x72,0x69,0x7a,0x61,0x74,0x69,0x6f,0x6e,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 74
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(ChangeMaxAuthorizationTx) / sizeof(ChangeMaxAuthorizationTx[0]);
    parser_status_e status_tx = parse_tx(buf,ChangeMaxAuthorizationTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, ChangeMaxAuthorizationTx[1].data, 66);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           ChangeMaxAuthorizationTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               ChangeMaxAuthorizationTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
/*
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return OPCODE_PARSING_ERROR;
    } 
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
        return ACCOUNT_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
    memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->max_authorize)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,ChangeMaxAuthorization);
    */
}


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf, set_fee_percentage_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t SetFeePercentageTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b,0x42},
            .data_len = 4
        },
        {
            .data_type = PUBKEY_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },

        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },

        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x10,0x73,0x65,0x74,0x46,0x65,0x65,0x50,0x65,0x72,0x63,0x65,0x6e,0x74,0x61,0x67,0x65,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 68
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(SetFeePercentageTx) / sizeof(SetFeePercentageTx[0]);
    parser_status_e status_tx = parse_tx(buf,SetFeePercentageTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, SetFeePercentageTx[1].data, 66);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           SetFeePercentageTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               SetFeePercentageTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    if(!convert_uint64_to_char(G_context.display_data.content_two,
                               sizeof (G_context.display_data.content_two),
                               SetFeePercentageTx[7].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;

/*

    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }  
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf,pre_pub_len)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
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
        return ACCOUNT_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t cost_len;
    if(!buffer_read_u8(buf,&cost_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_cost = getBytesValueByLen(buf,cost_len);

    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t stake_cost_len;
    if(!buffer_read_u8(buf,&stake_cost_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->stake_cost = getBytesValueByLen(buf,stake_cost_len);
    memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->peer_cost)) {
        return DATA_PARSING_ERROR;
    }
    if(!convert_uint64_to_char(G_context.display_data.content_two,
                               sizeof (G_context.display_data.content_two),
                               tx->stake_cost)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,SetFeePercentage);
    */
}


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf, authorize_for_peer_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t AuthorizeForPeerTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = MULTIPLE_PUBKEY_DATA_TYPE
        },
        {
            .data_type = MULTIPLE_AMOUNT_DATA_TYPE,

        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x10,0x61,0x75,0x74,0x68,0x6f,0x72,0x69,0x7a,0x65,0x46,0x6f,0x72,0x50,0x65,0x65,0x72,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 68
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(AuthorizeForPeerTx) / sizeof(AuthorizeForPeerTx[0]);
    parser_status_e status_tx = parse_tx(buf,AuthorizeForPeerTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    uint64_t  total_value = 0;
    for(size_t j=resultLength/2;j<resultLength;j++) {
        total_value += getValueByLen(resultArray[j],8);
    }
    if(resultLength >0) {
        memcpy(G_context.display_data.peer_pubkey, &resultArray[0], PEER_PUBKEY_LEN);
    }
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           AuthorizeForPeerTx[1].data);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               total_value)) {
        return DATA_PARSING_ERROR;
    }
//For other devices, only 3 pubkeys are displayed at most.
#if !defined(TARGET_NANOS)
    G_context.display_data.pubkey_number = resultLength/2;
    if(resultLength == 4) {
        memcpy(G_context.display_data.content_three, resultArray[1], PEER_PUBKEY_LEN);
    }
    if(resultLength == 6) {
        memcpy(G_context.display_data.content_four, resultArray[2], PEER_PUBKEY_LEN);
    }
#endif
    return PARSING_OK;
/*

    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (op_code_size >= 253) {//fd
        uint16_t code_size;
        if (!buffer_read_u16(buf, &code_size,LE)) {
            return OPCODE_PARSING_ERROR;
        }
        if (!buffer_can_read(buf, code_size)) {
            return DATA_END_PARSING_ERROR;
        }
    }
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
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
        return ACCOUNT_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t op_code_value;
    if (!buffer_read_u8(buf, &op_code_value)) {
        return VALUE_PARSING_ERROR;
    }
    if (op_code_value >= 81) {
        tx->peer_pubkey_number = op_code_value - 80;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    for(int i=0;i<tx->peer_pubkey_number;i++) {
        if (!buffer_read_u8(buf, &tx->peer_pubkey_length)) {
            return VALUE_PARSING_ERROR;
        }
        if(i<3) {
            tx->peer_pubkey[i] = (uint8_t *) (buf->ptr + buf->offset);
            if (!buffer_seek_cur(buf, tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        } else {
            if (!buffer_seek_cur(buf, tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        }
        if (getBytesValueByLen(buf, 3) != 13139050) {  // 6a7cc8
            return VALUE_PARSING_ERROR;
        }
    }
    if (!buffer_read_u8(buf, &tx->pos_list_number)) {
        return VALUE_PARSING_ERROR;
    }
    if (tx->pos_list_number >= 81) {
        tx->pos_list_number =  tx->pos_list_number-80;
    }
    if (tx->peer_pubkey_number != tx->pos_list_number) {
        return VALUE_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    tx->pos_list_value = 0;
    for(int j=0;j<tx->pos_list_number;j++) {
        if (!buffer_read_u8(buf, &tx->pos_list_len)) {
            return VALUE_PARSING_ERROR;
        }
        if (tx->pos_list_len >= 81) {
            tx->pos_list_value += tx->pos_list_len - 80;
        } else {
            tx->pos_list_value += getBytesValueByLen(buf, tx->pos_list_len);
        }
        if (j+1 < tx->pos_list_number) {
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
        }
    }
    if(tx->peer_pubkey_number >0) {
        memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    }
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->pos_list_value)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,AuthorizeForPeer);
    */
}


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf, un_authorize_for_peer_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t UnAuthorizeForPeerTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = MULTIPLE_PUBKEY_DATA_TYPE
        },
        {
            .data_type = MULTIPLE_AMOUNT_DATA_TYPE
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x12,0x75,0x6e,0x41,0x75,0x74,0x68,0x6f,0x72,0x69,0x7a,0x65,0x46,
                                  0x6f,0x72,0x50,0x65,0x65,0x72,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 70
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(UnAuthorizeForPeerTx) / sizeof(UnAuthorizeForPeerTx[0]);
    parser_status_e status_tx = parse_tx(buf,UnAuthorizeForPeerTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    uint64_t  total_value = 0;
    for(size_t j=resultLength/2;j<resultLength;j++) {
        total_value += getValueByLen(resultArray[j],8);
    }
    if(resultLength > 0) {
        memcpy(G_context.display_data.peer_pubkey, resultArray[0], PEER_PUBKEY_LEN);
    }
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           UnAuthorizeForPeerTx[1].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               total_value)) {
        return DATA_PARSING_ERROR;
    }
//For other devices, only 3 pubkeys are displayed at most.
#if !defined(TARGET_NANOS)
    G_context.display_data.pubkey_number = resultLength/2;
    if(resultLength == 4) {
        memcpy(G_context.display_data.content_three, resultArray[1], PEER_PUBKEY_LEN);
    }
    if(resultLength == 6) {
        memcpy(G_context.display_data.content_four, resultArray[2], PEER_PUBKEY_LEN);
    }
#endif
    return PARSING_OK;
/*
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (op_code_size >= 253) {//fd
        uint16_t code_size;
        if (!buffer_read_u16(buf, &code_size,LE)) {
            return OPCODE_PARSING_ERROR;
        }
        if (!buffer_can_read(buf, code_size)) {
            return DATA_END_PARSING_ERROR;
        }
    }
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
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
        return ACCOUNT_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t op_code_value;
    if (!buffer_read_u8(buf, &op_code_value)) {
        return VALUE_PARSING_ERROR;
    }
    if (op_code_value >= 81) {
        tx->peer_pubkey_number = op_code_value - 80;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    for(int i=0;i<tx->peer_pubkey_number;i++) {
        if (!buffer_read_u8(buf, &tx->peer_pubkey_length)) {
            return VALUE_PARSING_ERROR;
        }
        if(i<3) {
            tx->peer_pubkey[i] = (uint8_t *) (buf->ptr + buf->offset);
            if (!buffer_seek_cur(buf, tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        } else {
            if (!buffer_seek_cur(buf, tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        }
        if (getBytesValueByLen(buf, 3) != 13139050) {  // 6a7cc8
            return VALUE_PARSING_ERROR;
        }
    }
    if (!buffer_read_u8(buf, &tx->pos_list_number)) {
        return VALUE_PARSING_ERROR;
    }
    if (tx->pos_list_number >= 81) {
        tx->pos_list_number =  tx->pos_list_number-80;
    }
    if (tx->peer_pubkey_number != tx->pos_list_number) {
        return VALUE_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    tx->pos_list_value = 0;
    for(int j=0;j<tx->pos_list_number;j++) {
        if (!buffer_read_u8(buf, &tx->pos_list_len)) {
            return VALUE_PARSING_ERROR;
        }
        if (tx->pos_list_len >= 81) {
            tx->pos_list_value += tx->pos_list_len - 80;
        } else {
            tx->pos_list_value += getBytesValueByLen(buf, tx->pos_list_len);
        }
        if (j+1 < tx->pos_list_number) {
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
        }
    }
    if(tx->peer_pubkey_number >0) {
        memcpy(G_context.display_data.peer_pubkey, tx->peer_pubkey, 66);
    }
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               tx->pos_list_value)) {
        return DATA_PARSING_ERROR;
    }
    return check_govern_end_data(buf,UnAuthorizeForPeer);
    */
}


parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf, withdraw_fee_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t WithdrawFeeTx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,
                                  0x0b,0x77,0x69,0x74,0x68,0x64,0x72,0x61,0x77,0x46,0x65,0x65,
                                  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x68, 0x16,
                                  0x4F, 0x6E, 0x74, 0x6F, 0x6C, 0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74,
                                  0x69, 0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B, 0x65, 0x00},
            .data_len = 63
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(WithdrawFeeTx) / sizeof(WithdrawFeeTx[0]);
    parser_status_e status_tx = parse_tx(buf,WithdrawFeeTx,numElements,NATIVE_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           WithdrawFeeTx[1].data);

    return PARSING_OK;
/*
    uint8_t  op_code_size;
    if(!buffer_read_u8(buf,&op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,op_code_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 7063040) { //00c66b
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
        return ACCOUNT_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->account);
    return check_govern_end_data(buf,WithdrawFee);
    */
}
