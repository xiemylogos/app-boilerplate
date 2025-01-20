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
#include "oep4_deserialize.h"
#include "types.h"
#include "constants.h"
#include "utils.h"
#include "../globals.h"
#include "parse.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e oep4_neo_vm_transaction_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(NeoVmTransferTx) / sizeof(NeoVmTransferTx[0]);
    parser_status_e status_tx = parse_tx(buf,NeoVmTransferTx,numElements,OEP4_NEO_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           NeoVmTransferTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           NeoVmTransferTx[1].data);
    if(!get_oep4_token_amount(NeoVmTransferTx[4].data,
                                NeoVmTransferTx[0].data_len,
                                NeoVmTransferTx[0].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e oep4_wasm_vm_transaction_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(WasmVmTransferTx) / sizeof(WasmVmTransferTx[0]);
    parser_status_e status_tx = parse_tx(buf,WasmVmTransferTx,numElements,OEP4_WASM_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           WasmVmTransferTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           WasmVmTransferTx[3].data);
                           
                     
    if(!get_oep4_token_amount(WasmVmTransferTx[0].data,
                              WasmVmTransferTx[4].data_len,
                              WasmVmTransferTx[4].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    
    return PARSING_OK;
}

parser_status_e oep4_neo_vm_approve_transaction_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(NeoVmApproveTx) / sizeof(NeoVmApproveTx[0]);
    parser_status_e status_tx = parse_tx(buf,NeoVmApproveTx,numElements,OEP4_NEO_VM_OPERATOR,resultArray, storage);
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
}

parser_status_e oep4_wasm_vm_approve_transaction_deserialize(buffer_t *buf) {

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
    size_t numElements = sizeof(WasmApproveTx) / sizeof(WasmApproveTx[0]);
    parser_status_e status_tx = parse_tx(buf,WasmApproveTx,numElements,OEP4_WASM_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           WasmApproveTx[2].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           WasmApproveTx[3].data);
    if(!get_oep4_token_amount(WasmApproveTx[0].data,
                              WasmApproveTx[4].data_len,
                              WasmApproveTx[4].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e oep4_neo_vm_transfer_from_transaction_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(NeoVmTransferFromTx) / sizeof(NeoVmTransferFromTx[0]);
    parser_status_e status_tx = parse_tx(buf,NeoVmTransferFromTx,numElements,OEP4_NEO_VM_OPERATOR,resultArray, storage);
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

    if(!get_oep4_token_amount(NeoVmTransferFromTx[5].data,
                              NeoVmTransferFromTx[0].data_len,
                              NeoVmTransferFromTx[0].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e oep4_wasm_vm_transfer_from_transaction_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(WasmVmTransferFromTx) / sizeof(WasmVmTransferFromTx[0]);
    parser_status_e status_tx = parse_tx(buf,WasmVmTransferFromTx,numElements,OEP4_WASM_VM_OPERATOR,resultArray, storage);
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

    if(!get_oep4_token_amount(WasmVmTransferFromTx[0].data,
                              WasmVmTransferFromTx[5].data_len,
                              WasmVmTransferFromTx[5].values,
                              G_context.display_data.amount,
                              sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}
