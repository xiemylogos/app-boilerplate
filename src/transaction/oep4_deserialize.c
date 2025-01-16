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
#include "../ui/utils.h"
#include "../globals.h"
#include "parse.h"
#include "utils.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e oep4_neo_vm_transaction_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL oep4 tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t NeoVmTransferTx[] = {
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x53, 0xC1, 0x08, 0x74, 0x72, 0x61,
                                0x6E, 0x73, 0x66, 0x65, 0x72, 0x67},
            .data_len = 12
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00},
            .data_len = 1
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(NeoVmTransferTx) / sizeof(NeoVmTransferTx[0]);
    parser_status_e status_tx = parse_tx(buf,NeoVmTransferTx,numElements,OEP4_NEO_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           NeoVmTransferTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           NeoVmTransferTx[1].data);
    //oep4 contract addr
    memcpy(G_context.display_data.content, NeoVmTransferTx[4].data,ADDRESS_LEN);
    if(!get_oep4_token_amount(NeoVmTransferTx[4].data,
                                NeoVmTransferTx[0].data_len,
                                NeoVmTransferTx[0].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;

/*
    uint8_t  payload_size;
    if(!buffer_read_u8(buf,&payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size - buf->offset != payload_size +1) {
        return OPCODE_PARSING_ERROR;
    }
    //payload
    if(!buffer_read_u8(buf,&tx->payload.value_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tx->payload.value_len >= 81) {
        tx->payload.value[0] = tx->payload.value_len - 80;
        tx->payload.value[1] = 0;
    } else {
        if (tx->payload.value_len <= 8) {
            tx->payload.value[0] = getBytesValueByLen(buf, tx->payload.value_len);
            tx->payload.value[1] = 0;
        } else {
            if (!buffer_read_u64(buf, &tx->payload.value[0], LE)) {
                return OPCODE_PARSING_ERROR;
            }
            tx->payload.value[1] = getBytesValueByLen(buf, tx->payload.value_len - 8);
        }
    }
    uint8_t pre_to;
    if(!buffer_read_u8(buf,&pre_to)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_to != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    uint8_t pre_from;
    if(!buffer_read_u8(buf,&pre_from)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_from != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    uint8_t opcode_data[] = {0x53, 0xC1, 0x08, 0x74, 0x72, 0x61, 0x6E, 0x73, 0x66, 0x65, 0x72, 0x67};
    if(!buffer_can_read(buf,sizeof(opcode_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, opcode_data, sizeof(opcode_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (opcode_data))) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    uint8_t end_data[] = {
        0x00
    };
    if(memcmp(buf->ptr+buf->offset,end_data, sizeof (end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               tx->payload.from);
    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               tx->payload.to);

    if(!get_oep4_token_amount(tx->payload.value_len,tx->payload.value,
                               G_context.display_data.amount,
                               sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
    */
}

parser_status_e oep4_wasm_vm_transaction_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t WasmVmTransferTx[] = {
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x41,0x08,0x74,0x72,0x61,0x6e,0x73,0x66,0x65,0x72},
            .data_len = 10
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
            .data_len = 16
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00},
            .data_len = 1
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(WasmVmTransferTx) / sizeof(WasmVmTransferTx[0]);
    parser_status_e status_tx = parse_tx(buf,WasmVmTransferTx,numElements,OEP4_WSAM_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           WasmVmTransferTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           WasmVmTransferTx[3].data);
                           
                     
    //oep4 contract addr
    memcpy(G_context.display_data.content, WasmVmTransferTx[0].data,ADDRESS_LEN);
    if(!get_oep4_token_amount(WasmVmTransferTx[0].data,
                              WasmVmTransferTx[4].data_len,
                              WasmVmTransferTx[4].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    
    return PARSING_OK;
/*
    uint8_t  payload_size;
    if(!buffer_read_u8(buf,&payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size - buf->offset != payload_size +1) {
        return OPCODE_PARSING_ERROR;
    }
    tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 2113) { //4108
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, Transfer, 8) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,8)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,17)) {
        return DATA_END_PARSING_ERROR;
    }
    if(!buffer_read_u64(buf,&tx->payload.value[0],LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_read_u64(buf,&tx->payload.value[1],LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tx->payload.value[1] != 0 ) {
        tx->payload.value_len = 16;
    } else {
        tx->payload.value_len = 8;
    }
    uint8_t end_data[] = {
        0x00
    };
    if(memcmp(buf->ptr+buf->offset,end_data, sizeof (end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               tx->payload.from);
    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               tx->payload.to);

    if(!get_oep4_token_amount(tx->payload.value_len,tx->payload.value,
                              G_context.display_data.amount,
                               sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
    */
}

parser_status_e oep4_neo_vm_approve_transaction_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL oep4 tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t NeoVmApproveTx[] = {
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x53, 0xC1, 0x07, 0x61, 0x70, 0x70,
                                  0x72, 0x6F, 0x76, 0x65, 0x67},
            .data_len = 11
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00},
            .data_len = 1
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(NeoVmApproveTx) / sizeof(NeoVmApproveTx[0]);
    parser_status_e status_tx = parse_tx(buf,NeoVmApproveTx,numElements,OEP4_NEO_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           NeoVmApproveTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           NeoVmApproveTx[1].data);
    //oep4 contract addr
    memcpy(G_context.display_data.content, NeoVmApproveTx[4].data,ADDRESS_LEN);
    if(!get_oep4_token_amount(NeoVmApproveTx[4].data,
                              NeoVmApproveTx[0].data_len,
                              NeoVmApproveTx[0].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
    /*
    uint8_t  payload_size;
    if(!buffer_read_u8(buf,&payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size - buf->offset != payload_size +1) {
        return OPCODE_PARSING_ERROR;
    }
    //payload
    if(!buffer_read_u8(buf,&tx->payload.value_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tx->payload.value_len >= 81) {
        tx->payload.value[0] = tx->payload.value_len - 80;
        tx->payload.value[1] = 0;
    } else {
        if (tx->payload.value_len <= 8) {
            tx->payload.value[0] = getBytesValueByLen(buf, tx->payload.value_len);
            tx->payload.value[1] = 0;
        } else {
            if (!buffer_read_u64(buf, &tx->payload.value[0], LE)) {
                return OPCODE_PARSING_ERROR;
            }
            tx->payload.value[1] = getBytesValueByLen(buf, tx->payload.value_len - 8);
        }
    }
    uint8_t pre_to;
    if(!buffer_read_u8(buf,&pre_to)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_to != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    uint8_t pre_from;
    if(!buffer_read_u8(buf,&pre_from)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_from != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    //53 c1 07 61 70 70 72 6f 76 65 67
    uint8_t opcode_data[] = {0x53, 0xC1, 0x07, 0x61, 0x70, 0x70, 0x72, 0x6F, 0x76, 0x65,0x67};
    if(!buffer_can_read(buf,sizeof(opcode_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, opcode_data, sizeof(opcode_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (opcode_data))) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    uint8_t end_data[] = {
        0x00
    };
    if(memcmp(buf->ptr+buf->offset,end_data, sizeof (end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               tx->payload.from);
    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               tx->payload.to);
    if(!get_oep4_token_amount(tx->payload.value_len,tx->payload.value,
                              G_context.display_data.amount,
                               sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
     */
}

parser_status_e oep4_wasm_vm_approve_transaction_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL oep4 tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t WasmApproveTx[] = {
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x40,0x07,0x61,0x70,0x70,0x72,0x6f,0x76,0x65},
            .data_len = 9
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
            .data_len = 16
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00},
            .data_len = 1
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(WasmApproveTx) / sizeof(WasmApproveTx[0]);
    parser_status_e status_tx = parse_tx(buf,WasmApproveTx,numElements,OEP4_WSAM_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           WasmApproveTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           WasmApproveTx[3].data);
    //oep4 contract addr
    memcpy(G_context.display_data.content, WasmApproveTx[0].data,ADDRESS_LEN);
    if(!get_oep4_token_amount(WasmApproveTx[0].data,
                              WasmApproveTx[4].data_len,
                              WasmApproveTx[4].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
/*

    uint8_t  payload_size;
    if(!buffer_read_u8(buf,&payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size - buf->offset != payload_size +1) {
        return OPCODE_PARSING_ERROR;
    }
    tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 1856) { //4007
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, Approve, 7) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,7)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,17)) {
        return DATA_END_PARSING_ERROR;
    }
    if(!buffer_read_u64(buf,&tx->payload.value[0],LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_read_u64(buf,&tx->payload.value[1],LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tx->payload.value[1] != 0 ) {
        tx->payload.value_len = 16;
    } else {
        tx->payload.value_len = 8;
    }
    uint8_t end_data[] = {
        0x00
    };
    if(memcmp(buf->ptr+buf->offset,end_data, sizeof (end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               tx->payload.from);
    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               tx->payload.to);
    if(!get_oep4_token_amount(tx->payload.value_len,tx->payload.value,
                              G_context.display_data.amount,
                               sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
    */
}

parser_status_e oep4_neo_vm_transfer_from_transaction_deserialize(buffer_t *buf, ont_transaction_from_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL oep4 tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }
    cfg_t NeoVmTransferFromTx[] = {
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x54, 0xC1, 0x0C, 0x74, 0x72, 0x61,
                                  0x6e, 0x73, 0x66, 0x65,0x72,0x46,0x72,
                                  0x6f, 0x6d, 0x67},
            .data_len = 16
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00},
            .data_len = 1
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(NeoVmTransferFromTx) / sizeof(NeoVmTransferFromTx[0]);
    parser_status_e status_tx = parse_tx(buf,NeoVmTransferFromTx,numElements,OEP4_NEO_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           NeoVmTransferFromTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           NeoVmTransferFromTx[1].data);
    //sender
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           NeoVmTransferFromTx[3].data);
    //oep4 contract addr
    memcpy(G_context.display_data.content, NeoVmTransferFromTx[5].data,ADDRESS_LEN);
    if(!get_oep4_token_amount(NeoVmTransferFromTx[5].data,
                              NeoVmTransferFromTx[0].data_len,
                              NeoVmTransferFromTx[0].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;

   /*

    uint8_t  payload_size;
    if(!buffer_read_u8(buf,&payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size - buf->offset != payload_size +1) {
        return OPCODE_PARSING_ERROR;
    }
    //payload
    if(!buffer_read_u8(buf,&tx->payload.value_len)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tx->payload.value_len >= 81) {
        tx->payload.value[0] = tx->payload.value_len - 80;
        tx->payload.value[1] = 0;
    } else {
        if (tx->payload.value_len <= 8) {
            tx->payload.value[0] = getBytesValueByLen(buf, tx->payload.value_len);
            tx->payload.value[1] = 0;
        } else {
            if (!buffer_read_u64(buf, &tx->payload.value[0], LE)) {
                return OPCODE_PARSING_ERROR;
            }
            tx->payload.value[1] = getBytesValueByLen(buf, tx->payload.value_len - 8);
        }
    }
    uint8_t pre_to;
    if(!buffer_read_u8(buf,&pre_to)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_to != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    uint8_t pre_from;
    if(!buffer_read_u8(buf,&pre_from)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_from != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }

    uint8_t pre_sender;
    if(!buffer_read_u8(buf,&pre_sender)) {
        return VALUE_PARSING_ERROR;
    }
    if (pre_sender != ADDRESS_LEN) { //14
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.sender = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    //54 c1 0c 74 72 61 6e 73 66 65 72 46 72 6f 6d 67
    uint8_t opcode_data[] = {0x54, 0xC1, 0x0C, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x66, 0x65,0x72,0x46,0x72,0x6f,0x6d,0x67};
    if(!buffer_can_read(buf,sizeof(opcode_data))) {
        return DATA_END_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, opcode_data, sizeof(opcode_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (opcode_data))) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    uint8_t end_data[] = {
        0x00
    };
    if(memcmp(buf->ptr+buf->offset,end_data, sizeof (end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->payload.sender);
    script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               tx->payload.from);
    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               tx->payload.to);
    if(!get_oep4_token_amount(tx->payload.value_len,tx->payload.value,
                              G_context.display_data.amount,
                               sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
    */
}

parser_status_e oep4_wasm_vm_transfer_from_transaction_deserialize(buffer_t *buf, ont_transaction_from_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL oep4 tx");

    parser_status_e status = transaction_deserialize_header(buf,&tx->header);
    if (status != PARSING_OK) {
        return status;
    }

    cfg_t WasmVmTransferFromTx[] = {
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x59,0x0c,0x74,0x72,0x61,0x6e,0x73,
                                 0x66,0x65,0x72,0x46,0x72,0x6f,0x6d},
            .data_len = 14
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .values = {},
            .data_len = 16
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00},
            .data_len = 1
        },
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t resultLength = 0;
    size_t numElements = sizeof(WasmVmTransferFromTx) / sizeof(WasmVmTransferFromTx[0]);
    parser_status_e status_tx = parse_tx(buf,WasmVmTransferFromTx,numElements,OEP4_WSAM_VM_OPERATOR,resultArray, &resultLength,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    //sender
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           WasmVmTransferFromTx[2].data);
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           WasmVmTransferFromTx[3].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           WasmVmTransferFromTx[4].data);
    //oep4 contract addr
    memcpy(G_context.display_data.content, WasmVmTransferFromTx[0].data,ADDRESS_LEN);
    if(!get_oep4_token_amount(WasmVmTransferFromTx[0].data,
                              WasmVmTransferFromTx[5].data_len,
                              WasmVmTransferFromTx[5].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
    /*
    uint8_t  payload_size;
    if(!buffer_read_u8(buf,&payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,payload_size)) {
        return OPCODE_PARSING_ERROR;
    }
    if (buf->size - buf->offset != payload_size +1) {
        return OPCODE_PARSING_ERROR;
    }
    tx->payload.contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    uint16_t  pre_code;
    if(!buffer_read_u16(buf,&pre_code,LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (pre_code != 3161) { //590c
        return OPCODE_PARSING_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset, TransferFrom, 12) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,12)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->payload.sender = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    tx->payload.from = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    tx->payload.to = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return TO_PARSING_ERROR;
    }
    if(!buffer_can_read(buf,17)) {
        return DATA_END_PARSING_ERROR;
    }
    if(!buffer_read_u64(buf,&tx->payload.value[0],LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if(!buffer_read_u64(buf,&tx->payload.value[1],LE)) {
        return OPCODE_PARSING_ERROR;
    }
    if (tx->payload.value[1] != 0 ) {
        tx->payload.value_len = 16;
    } else {
        tx->payload.value_len = 8;
    }
    uint8_t end_data[] = {
        0x00
    };
    if(memcmp(buf->ptr+buf->offset,end_data, sizeof (end_data)) != 0) {
        return DATA_END_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,sizeof (end_data))) {
        return DATA_END_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.content,
                               sizeof(G_context.display_data.content),
                               tx->payload.sender);
   script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               tx->payload.from);
    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               tx->payload.to);
    if(!get_oep4_token_amount(tx->payload.value_len,tx->payload.value,
                              G_context.display_data.amount,
                               sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
     */
}
