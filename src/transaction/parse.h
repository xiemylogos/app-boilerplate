#pragma once

#include <stdint.h>
#include "buffer.h"
#include "../types.h"

typedef enum {
    OP_CODE_DATA_TYPE = 0x01,
    ADDRESS_DATA_TYPE = 0x02,
    AMOUNT_DATA_TYPE  = 0x03,
    PUBKEY_DATA_TYPE = 0x04
}cfg_param_e;


typedef struct {
    uint8_t data_type;
    uint8_t *data;
    uint8_t data_len;
}cfg_t;

parser_status_e parse_tx(buffer_t *buf,cfg_t* txArray,size_t array_length);







