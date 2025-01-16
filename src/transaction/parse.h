#pragma once

#include <stdint.h>
#include "buffer.h"
#include "../types.h"

#define MAX_RESULT_SIZE 20
#define VALUE_SIZE 8
#define OPCODE_VALUE  81

typedef enum {
    OP_CODE_DATA_TYPE = 0x01,
    ADDRESS_DATA_TYPE = 0x02,
    AMOUNT_DATA_TYPE  = 0x03,
    PUBKEY_DATA_TYPE = 0x04,
    MULTIPLE_PUBKEY_DATA_TYPE = 0x05,
    MULTIPLE_AMOUNT_DATA_TYPE = 0x06,
    CONTRACT_ADDRESS_DATA_TYPE = 0x07
}cfg_param_e;


typedef enum {
    NATIVE_VM_OPERATOR = 0x01,
    OEP4_NEO_VM_OPERATOR = 0x02,
    OEP4_WASM_VM_OPERATOR = 0x03
}vm_operator_t;

typedef struct {
    uint64_t pos_start;
    uint64_t data_number;
}data_t;

typedef struct {
    uint8_t data_type;
    union {
        uint8_t *data;
        uint64_t values[2];
        data_t  data_info;
    };
    uint8_t data_len;
}cfg_t;



uint64_t  GetBufferData(buffer_t *buf);
parser_status_e parse_tx(buffer_t *buf,cfg_t* txArray,size_t array_length,vm_operator_t vm_type,uint8_t *resultArray[MAX_RESULT_SIZE],uint8_t storage[][VALUE_SIZE]);







