
#include "parse.h"
#include "utils.h"
#include <string.h>
//add vm type check address
//add char array return values
parser_status_e parse_tx(buffer_t *buf,cfg_t* tx,size_t array_length,vm_operator_t vm_type) {
    for(size_t i=0;i<array_length;i++) {
        if (tx[i].data_type == OP_CODE_DATA_TYPE) {
            if (!buffer_can_read(buf, tx[i].data_len)) {
                return DATA_END_PARSING_ERROR;
            }
            if (memcmp(buf->ptr + buf->offset, tx[i].data, tx[i].data_len) != 0) {
                return DATA_END_PARSING_ERROR;
            }
            buffer_seek_cur(buf, tx[i].data_len);
        } else if (tx[i].data_type== ADDRESS_DATA_TYPE) {
            if(vm_type != OEP4_WSAM_VM_OPERATOR) {
                uint8_t address_len;
                if (!buffer_read_u8(buf, &address_len)) {
                    return OPCODE_PARSING_ERROR;
                }
                if (address_len != ADDRESS_LEN) {
                    return OPCODE_PARSING_ERROR;
                }
            }
            tx[i].data = (uint8_t*)(buf->ptr+buf->offset);
            if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
                return FROM_PARSING_ERROR;
            }
        } else if(tx[i].data_type== CONTRACT_ADDRESS_DATA_TYPE){
            if(vm_type != OEP4_WSAM_VM_OPERATOR &&
               vm_type != OEP4_NEO_VM_OPERATOR) {
                uint8_t address_len;
                if (!buffer_read_u8(buf, &address_len)) {
                    return OPCODE_PARSING_ERROR;
                }
                if (address_len != ADDRESS_LEN) {
                    return OPCODE_PARSING_ERROR;
                }
            }
            tx[i].data = (uint8_t*)(buf->ptr+buf->offset);
            if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
                return FROM_PARSING_ERROR;
            }
        }else if(tx[i].data_type == AMOUNT_DATA_TYPE){
            if (tx[i].data_len == 16) {
                if (!buffer_read_u64(buf, &tx[i].values[0], LE)) {
                    return OPCODE_PARSING_ERROR;
                }
                if (!buffer_read_u64(buf, &tx[i].values[1], LE)) {
                    return OPCODE_PARSING_ERROR;
                }
                if (tx[i].values[1] != 0 ) {
                    tx[i].data_len = 16;
                } else {
                    tx[i].data_len = 8;
                }
            } else {
                if (!buffer_read_u8(buf, &tx[i].data_len)) {
                    return OPCODE_PARSING_ERROR;
                }
                if (tx[i].data_len >= 81) {
                    tx[i].values[0] = tx[i].data_len - 81;
                    tx[i].values[1] = 0;
                } else {
                    if (tx[i].data_len <= 8) {
                        tx[i].values[0] = getBytesValueByLen(buf, tx[i].data_len);
                        tx[i].values[1] = 0;
                    } else {
                        if (!buffer_read_u64(buf, &tx[i].values[0], LE)) {
                            return OPCODE_PARSING_ERROR;
                        }
                        tx[i].values[1] = getBytesValueByLen(buf, tx[i].data_len - 8);
                    }
                }
            }
        } else if (tx[i].data_type == PUBKEY_DATA_TYPE){
            tx[i].data = (uint8_t*)(buf->ptr+buf->offset);
            if (!buffer_seek_cur(buf, PEER_PUBKEY_LEN)) {
                return FROM_PARSING_ERROR;
            }
        } else if(tx[i].data_type == MULTIPLE_PUBKEY_DATA_TYPE) {
            uint8_t op_code_value;
            if (!buffer_read_u8(buf, &op_code_value)) {
                return VALUE_PARSING_ERROR;
            }
            uint8_t  peer_pubkey_number = op_code_value;
            if(op_code_value >=81) {
                peer_pubkey_number = op_code_value -80;
            }
            //tx[i].data_len = op_code_value - 80;
            //todo
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
            for(size_t len=0;len< peer_pubkey_number;i++) {
                uint8_t peer_pubkey_length =0;
                if (!buffer_read_u8(buf, &peer_pubkey_length)) {
                    return VALUE_PARSING_ERROR;
                }
                if(i<3) {
                    tx[i].peer_pubkeys[len] = (uint8_t *) (buf->ptr + buf->offset);
                    if (!buffer_seek_cur(buf, peer_pubkey_length)) {
                        return PEER_PUBKEY_PARSING_ERROR;
                    }
                } else {
                    if (!buffer_seek_cur(buf,peer_pubkey_length)) {
                        return PEER_PUBKEY_PARSING_ERROR;
                    }
                }
                if (getBytesValueByLen(buf, 3) != 13139050) {  // 6a7cc8
                    return VALUE_PARSING_ERROR;
                }
            }
        }else if(tx[i].data_type == MULTIPLE_AMOUNT_DATA_TYPE) {
            uint8_t amount_number =0;
            if (!buffer_read_u8(buf, &amount_number)) {
                return VALUE_PARSING_ERROR;
            }
            if(amount_number >= 81) {
                amount_number = amount_number - 80;
            }
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
            for(size_t num=0; num<amount_number;num++) {
                uint8_t  amount_len = 0;
                if (!buffer_read_u8(buf, &amount_len)) {
                    return VALUE_PARSING_ERROR;
                }
                if (amount_len >= 81) {
                    tx[i].values[0] += amount_len - 80;
                } else {
                    tx[i].values[0] += getBytesValueByLen(buf, amount_len);
                }
                if (num+1 < amount_number) {
                    if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                        return VALUE_PARSING_ERROR;
                    }
                }
            }
        }else {
            return DATA_PARSING_ERROR;
        }
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}