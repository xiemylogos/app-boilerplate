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
    return check_govern_end_data(buf,RegisterCandidate);
}

parser_status_e withdraw_tx_deserialize(buffer_t *buf, withdraw_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e  status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    } else {
        if (op_code_value == 66) { //42
           tx->peer_pubkey_length = op_code_value;
            tx->peer_pubkey[0] = (uint8_t*)(buf->ptr+buf->offset);
            if (!buffer_seek_cur(buf,tx->peer_pubkey_length)) {
                return PEER_PUBKEY_PARSING_ERROR;
            }
        }
        if(getBytesValueByLen(buf,2) != 49489) { //51c1
            return VALUE_PARSING_ERROR;
        }
        if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
            return VALUE_PARSING_ERROR;
        }
        tx->withdraw_value = 0;
        for(int j=0;j<tx->peer_pubkey_number;j++) {
            uint8_t withdraw_len_opcode;
            if (!buffer_read_u8(buf, &withdraw_len_opcode)) {
                return VALUE_PARSING_ERROR;
            }
            if (tx->withdraw_list_len >= 81) {
                tx->withdraw_value += tx->withdraw_list_len - 80;
            } else {
                tx->withdraw_value += getBytesValueByLen(buf, tx->withdraw_list_len);
            }
        }
        //0x51 // The number 1 is pushed onto the stack
        if(getBytesValueByLen(buf,1) != 80 + tx->peer_pubkey_number) {
            return VALUE_PARSING_ERROR;
        }
        if(getBytesValueByLen(buf,1) != 193) { //c1
            return VALUE_PARSING_ERROR;
        }
    }
    return check_govern_end_data(buf,Withdraw);
}


parser_status_e quit_node_tx_deserialize(buffer_t *buf, quit_node_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    return check_govern_end_data(buf,QuitNode);
}


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf, add_init_pos_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    return check_govern_end_data(buf,AddInitPos);
}

parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf, reduce_init_pos_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    return check_govern_end_data(buf,ReduceInitPos);
}


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf, change_max_authorization_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    return check_govern_end_data(buf,ChangeMaxAuthorization);
}


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf, set_fee_percentage_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    return check_govern_end_data(buf,SetFeePercentage); 
}


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf, authorize_for_peer_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }  
    if(!buffer_read_varint(buf,&tx->peer_pubkey_length)) {
        return DATA_PARSING_ERROR;
    }
    if (tx->peer_pubkey_length >= 81) {
        tx->peer_pubkey_length = tx->peer_pubkey_length -80;
    }
   if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len*tx->peer_pubkey_length)) {
        return PEER_PUBKEY_PARSING_ERROR;
    } 
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    if(!buffer_read_u8(buf,&tx->pos_list_number)) {
        return DATA_PARSING_ERROR;
    }
    if (tx->pos_list_number >= 81) {
        tx->pos_list_number = tx->pos_list_number -80;
    } 
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_u8(buf,&tx->pos_list_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (tx->pos_list_len < 81) {
        tx->pos_list = (uint8_t *) (buf->ptr + buf->offset);
        if (!buffer_seek_cur(buf, tx->pos_list_len * tx->pos_list_number)) {
            return DATA_PARSING_ERROR;
        }
    }
    return check_govern_end_data(buf,AuthorizeForPeer);  
}


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf, un_authorize_for_peer_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }  
    if(!buffer_read_varint(buf,&tx->peer_pubkey_length)) {
        return DATA_PARSING_ERROR;
    }
    if (tx->peer_pubkey_length >= 81) {
        tx->peer_pubkey_length = tx->peer_pubkey_length -80;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    uint8_t pre_pub_len;
    if(!buffer_read_u8(buf,&pre_pub_len)) {
        return VALUE_PARSING_ERROR;
    }
    tx->peer_pubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, pre_pub_len*tx->peer_pubkey_length)) {
        return PEER_PUBKEY_PARSING_ERROR;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    } 
    if(!buffer_read_u8(buf,&tx->pos_list_number)) {
        return DATA_PARSING_ERROR;
    }
    if (tx->pos_list_number >= 81) {
        tx->pos_list_number = tx->pos_list_number -80;
    }
    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
        return VALUE_PARSING_ERROR;
    }
    if(!buffer_read_u8(buf,&tx->pos_list_len)) {
        return VALUE_PARSING_ERROR;
    }
    if (tx->pos_list_len < 81) {
        tx->pos_list = (uint8_t *) (buf->ptr + buf->offset);
        if (!buffer_seek_cur(buf, tx->pos_list_len * tx->pos_list_number)) {
            return DATA_PARSING_ERROR;
        }
    }
    return check_govern_end_data(buf,UnAuthorizeForPeer);  
}


parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf, withdraw_fee_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
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
    return check_govern_end_data(buf,WithdrawFee);  
}
