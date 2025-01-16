
#include "parse.h"
#include "utils.h"
#include <string.h>
#include "../ui/utils.h"

uint64_t  GetBufferData(buffer_t *buf) {
    uint64_t  value = 0;
    uint8_t  amount = 0;
    if (!buffer_read_u8(buf, &amount)) {
        return 0;
    }
    if (amount >= OPCODE_VALUE) {
        value = amount - 80;
    } else {
        value = getBytesValueByLen(buf, amount);
    }
    return value;
}


parser_status_e parse_tx(buffer_t *buf,cfg_t* tx,size_t array_length,vm_operator_t vm_type,uint8_t *resultArray[MAX_RESULT_SIZE],uint8_t storage[][VALUE_SIZE]) {
    uint64_t result_length = 0;
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
            if(vm_type != OEP4_WASM_VM_OPERATOR) {
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
            if(vm_type != OEP4_WASM_VM_OPERATOR &&
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
                if (tx[i].data_len >= OPCODE_VALUE) {
                    tx[i].values[0] = tx[i].data_len - 80;
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
            uint64_t peer_pubkey_number = GetBufferData(buf);
            if (peer_pubkey_number == 0) {
                return VALUE_PARSING_ERROR;
            }
            tx[i].data_info.pos_start = result_length;
            tx[i].data_info.data_number = peer_pubkey_number;
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
            for(size_t len=0;len< peer_pubkey_number;len++) {
                uint8_t peer_pubkey_length =0;
                if (!buffer_read_u8(buf, &peer_pubkey_length)) {
                    return VALUE_PARSING_ERROR;
                }
                if (result_length < MAX_RESULT_SIZE) {
                    resultArray[(result_length)++] = (uint8_t *) (buf->ptr + buf->offset);
                    if (!buffer_seek_cur(buf, peer_pubkey_length)) {
                        return PEER_PUBKEY_PARSING_ERROR;
                    }
                } else {
                    return PEER_PUBKEY_PARSING_ERROR;
                }
                if (getBytesValueByLen(buf, 3) != 13139050) {  // 6a7cc8
                    return VALUE_PARSING_ERROR;
                }
            }
        }else if(tx[i].data_type == MULTIPLE_AMOUNT_DATA_TYPE) {
            uint64_t amount_number= GetBufferData(buf);
            if (amount_number == 0) {
                return VALUE_PARSING_ERROR;
            }
            tx[i].data_info.pos_start = result_length;
            tx[i].data_info.data_number = amount_number;
            if(getBytesValueByLen(buf,3) != 13139050) { //6a7cc8
                return VALUE_PARSING_ERROR;
            }
            for(size_t num=0; num<amount_number;num++) {
                uint64_t value = GetBufferData(buf);
                if (value == 0) {
                    return VALUE_PARSING_ERROR;
                }
                size_t byte_count = sizeof(value);
                if (result_length >= MAX_RESULT_SIZE) {
                    return DATA_PARSING_ERROR;
                }
                uint8_t *current_storage = storage[result_length];
                for (size_t j = 0; j < byte_count; j++) {
                    current_storage[j] = (value >> (j * 8)) & 0xFF;
                }
                resultArray[result_length] = current_storage;
                result_length++;
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