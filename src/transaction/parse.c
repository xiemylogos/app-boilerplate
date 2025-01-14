
#include "parse.h"

parser_status_e parse_tx(buffer_t *buf,cfg_t* tx,size_t array_length) {
    for(size_t i=0;i<array_length;i++) {
        if (tx[i].data_type == OP_CODE_DATA_TYPE) {
            if (!buffer_can_read(buf, tx[i].data_len)) {
                return DATA_END_PARSING_ERROR;
            }
            if (memcmp(buf->ptr + buf->offset, tx[i].data, tx[i].data_len) != 0) {
                return DATA_END_PARSING_ERROR;
            }
            if (!buffer_seek_cur(buf, tx[i].data_len)) {
                return DATA_END_PARSING_ERROR;
            }
        } else if (tx[i].data_type== ADDRESS_DATA_TYPE) {
            tx[i].data = (uint8_t*)(buf->ptr+buf->offset);
            if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
                return FROM_PARSING_ERROR;
            }
        } else if(tx[i].data_type == AMOUNT_DATA_TYPE){
            uint8_t  value_len;
            if (!buffer_read_u8(buf, &value_len)) {
                return OPCODE_PARSING_ERROR;
            }
            tx[i].data_len = value_len;
            if(value_len < 81){
               tx[i].data = (uint8_t*)(buf->ptr+buf->offset);
                if (!buffer_seek_cur(buf, value_len)) {
                    return FROM_PARSING_ERROR;
                }
            }
        } else if (tx[i].data_type == PUBKEY_DATA_TYPE){

        }else {
            return DATA_PARSING_ERROR;
        }
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}