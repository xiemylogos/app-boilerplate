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
#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"
#include "../globals.h"
#include "parse.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif


parser_status_e transaction_native_transfer_deserialize(buffer_t *buf) {
    cfg_t TransferTx[] = {
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x51,0xc1,0x08,
                                  0x74,0x72,0x61,0x6e,0x73,0x66,0x65,
                                  0x72},
            .data_len = 15
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(TransferTx) / sizeof(TransferTx[0]);
    parser_status_e status_tx = parse_tx(buf,TransferTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    if (memcmp(TransferTx[7].data, ONG_ADDR, ADDRESS_LEN) != 0 &&
        memcmp(TransferTx[7].data, ONT_ADDR, ADDRESS_LEN) != 0) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           TransferTx[1].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           TransferTx[3].data);
    //contract addr
    memcpy(G_context.display_data.content_two,TransferTx[7].data,ADDRESS_LEN);

    if(!get_native_token_amount(TransferTx[7].data,
                                TransferTx[5].data_len,
                                TransferTx[5].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e transaction_native_transfer_v2_deserialize(buffer_t *buf) {
    cfg_t TransferV2Tx[] = {
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x51,0xc1,0x0a,
                                  0x74,0x72,0x61,0x6e,0x73,0x66,0x65,
                                  0x72,0x56,0x32},
            .data_len = 17
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(TransferV2Tx) / sizeof(TransferV2Tx[0]);
    parser_status_e status_tx = parse_tx(buf,TransferV2Tx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
     if (status_tx != PARSING_OK) {
        return status_tx;
    }
    if (memcmp(TransferV2Tx[7].data, ONG_ADDR, ADDRESS_LEN) != 0 &&
        memcmp(TransferV2Tx[7].data, ONT_ADDR, ADDRESS_LEN) != 0) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
    script_hash_to_address(G_context.display_data.from,
                               sizeof(G_context.display_data.from),
                               TransferV2Tx[1].data);

    script_hash_to_address(G_context.display_data.to,
                               sizeof(G_context.display_data.to),
                               TransferV2Tx[3].data);
    //contract addr
    memcpy(G_context.display_data.content_two,TransferV2Tx[7].data,ADDRESS_LEN);

    if(!get_native_token_amount(TransferV2Tx[7].data,
                                TransferV2Tx[5].data_len,
                                TransferV2Tx[5].values,
                                 G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e transaction_native_transfer_from_deserialize(buffer_t *buf) {
    cfg_t TransferFromTx[] = {
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x00,0xc6,0x6b},
            .data_len = 6
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x6a,0x7c,0xc8,0x6c,
                                 0x0c,0x74,0x72,0x61,0x6e,0x73,0x66,0x65,0x72,
                                 0x46,0x72,0x6f,0x6d},
            .data_len = 21
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(TransferFromTx) / sizeof(TransferFromTx[0]);
    parser_status_e status_tx = parse_tx(buf,TransferFromTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    if (memcmp(TransferFromTx[9].data, ONG_ADDR, ADDRESS_LEN) != 0 &&
        memcmp(TransferFromTx[9].data, ONT_ADDR, ADDRESS_LEN) != 0) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }

    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           TransferFromTx[1].data);

    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           TransferFromTx[3].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           TransferFromTx[5].data);
    //contract addr
    memcpy(G_context.display_data.content_two,TransferFromTx[9].data,ADDRESS_LEN);

    if(!get_native_token_amount(TransferFromTx[9].data,
                                TransferFromTx[7].data_len,
                                TransferFromTx[7].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }

    return PARSING_OK;
}

parser_status_e transaction_native_transfer_from_v2_deserialize(buffer_t *buf) {
    cfg_t TransferFromV2Tx[] = {
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x00,0xc6,0x6b},
            .data_len = 6
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x6a,0x7c,0xc8,0x6c,0x0e,
                                  0x74,0x72,0x61,0x6e,0x73,0x66,0x65,
                                  0x72, 0x46, 0x72,0x6f,0x6d,0x56,0x32},
            .data_len = 23
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(TransferFromV2Tx) / sizeof(TransferFromV2Tx[0]);
    parser_status_e status_tx = parse_tx(buf,TransferFromV2Tx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    if (memcmp(TransferFromV2Tx[9].data, ONG_ADDR, ADDRESS_LEN) != 0 &&
        memcmp(TransferFromV2Tx[9].data, ONT_ADDR, ADDRESS_LEN) != 0) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }

    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           TransferFromV2Tx[1].data); //sender

    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           TransferFromV2Tx[3].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           TransferFromV2Tx[5].data);

    //contract addr
    memcpy(G_context.display_data.content_two,TransferFromV2Tx[9].data,ADDRESS_LEN);

    if(!get_native_token_amount(TransferFromV2Tx[9].data,
                                TransferFromV2Tx[7].data_len,
                                TransferFromV2Tx[7].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e transaction_approve_deserialize(buffer_t *buf) {
    cfg_t ApproveTx[] = {
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x07,
                                  0x61,0x70,0x70,0x72,0x6f,0x76,0x65},
            .data_len = 12
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(ApproveTx) / sizeof(ApproveTx[0]);
    parser_status_e status_tx = parse_tx(buf,ApproveTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    if (memcmp(ApproveTx[7].data, ONG_ADDR, ADDRESS_LEN) != 0 &&
        memcmp(ApproveTx[7].data, ONT_ADDR, ADDRESS_LEN) != 0) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }

    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           ApproveTx[1].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           ApproveTx[3].data);

    //contract addr
    memcpy(G_context.display_data.content_two,ApproveTx[7].data,ADDRESS_LEN);

    if(!get_native_token_amount(ApproveTx[7].data,
                                ApproveTx[5].data_len,
                                ApproveTx[5].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e transaction_approve_v2_deserialize(buffer_t *buf) {
    cfg_t ApproveV2Tx[] = {
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
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x09,
                                  0x61,0x70,0x70,0x72,0x6f,
                                  0x76,0x65,0x56,0x32},
            .data_len = 14
        },
        {
            .data_type = CONTRACT_ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(ApproveV2Tx) / sizeof(ApproveV2Tx[0]);
    parser_status_e status_tx = parse_tx(buf,ApproveV2Tx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    if (memcmp(ApproveV2Tx[7].data, ONG_ADDR, ADDRESS_LEN) != 0 &&
        memcmp(ApproveV2Tx[7].data, ONT_ADDR, ADDRESS_LEN) != 0) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }

    script_hash_to_address(G_context.display_data.from,
                           sizeof(G_context.display_data.from),
                           ApproveV2Tx[1].data);

    script_hash_to_address(G_context.display_data.to,
                           sizeof(G_context.display_data.to),
                           ApproveV2Tx[3].data);

    //contract addr
    memcpy(G_context.display_data.content_two,ApproveV2Tx[7].data,ADDRESS_LEN);

    if(!get_native_token_amount(ApproveV2Tx[7].data,
                                ApproveV2Tx[5].data_len,
                                ApproveV2Tx[5].values,
                                G_context.display_data.amount,
                                sizeof (G_context.display_data.amount))) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}