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

#include "govern_deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"
#include "format.h"
#include "../globals.h"
#include "parse.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif


parser_status_e register_candidate_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(RegisterCandidateTx) / sizeof(RegisterCandidateTx[0]);
    parser_status_e status_tx = parse_tx(buf,RegisterCandidateTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, RegisterCandidateTx[1].data, PEER_PUBKEY_LEN);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           RegisterCandidateTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               RegisterCandidateTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e withdraw_tx_deserialize(buffer_t *buf) {
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
            .data_type = MULTIPLE_PUBKEY_DATA_TYPE,
            .data_info = {}

        },
        {
            .data_type = MULTIPLE_AMOUNT_DATA_TYPE,
            .data_info = {}
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
    size_t numElements = sizeof(WithDrawTx) / sizeof(WithDrawTx[0]);
    parser_status_e status_tx = parse_tx(buf,WithDrawTx,numElements,NATIVE_VM_OPERATOR,resultArray,storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           WithDrawTx[1].data);
    
    for(size_t i=0; i< WithDrawTx[3].data_info.data_number;i++) {
        uint64_t pos = WithDrawTx[3].data_info.pos_start+i;
        if(i==0) {
            memcpy(G_context.display_data.peer_pubkey, resultArray[pos], PEER_PUBKEY_LEN);
        }
#if !defined(TARGET_NANOS) //For other devices, only three pubkey are displayed at most
        G_context.display_data.pubkey_number = WithDrawTx[3].data_info.data_number;
        if(i==1) {
            memcpy(G_context.display_data.content_three, resultArray[pos], PEER_PUBKEY_LEN);
        }
        if (i==2) {
            memcpy(G_context.display_data.content_four, resultArray[pos], PEER_PUBKEY_LEN);
        }
#endif
    }

    uint64_t  total_value = 0;
    for(size_t j =0;j<WithDrawTx[4].data_info.data_number;j++) {
        uint64_t  pos = WithDrawTx[4].data_info.pos_start+j;
        total_value += getValueByLen(resultArray[pos], 8);
    }

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               total_value)) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e quit_node_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(QuitNodeTx) / sizeof(QuitNodeTx[0]);
    parser_status_e status_tx = parse_tx(buf,QuitNodeTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, QuitNodeTx[1].data, PEER_PUBKEY_LEN);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           QuitNodeTx[3].data);

    return PARSING_OK;
}


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(AddInitPosTx) / sizeof(AddInitPosTx[0]);
    parser_status_e status_tx = parse_tx(buf,AddInitPosTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, AddInitPosTx[1].data, PEER_PUBKEY_LEN);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           AddInitPosTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               AddInitPosTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}

parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(ReduceInitPosTx) / sizeof(ReduceInitPosTx[0]);
    parser_status_e status_tx = parse_tx(buf,ReduceInitPosTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, ReduceInitPosTx[1].data, PEER_PUBKEY_LEN);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           ReduceInitPosTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               ReduceInitPosTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(ChangeMaxAuthorizationTx) / sizeof(ChangeMaxAuthorizationTx[0]);
    parser_status_e status_tx = parse_tx(buf,ChangeMaxAuthorizationTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, ChangeMaxAuthorizationTx[1].data, PEER_PUBKEY_LEN);
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           ChangeMaxAuthorizationTx[3].data);

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               ChangeMaxAuthorizationTx[5].values[0])) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(SetFeePercentageTx) / sizeof(SetFeePercentageTx[0]);
    parser_status_e status_tx = parse_tx(buf,SetFeePercentageTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    memcpy(G_context.display_data.peer_pubkey, SetFeePercentageTx[1].data, PEER_PUBKEY_LEN);
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
}


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf) {
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
            .data_type = MULTIPLE_PUBKEY_DATA_TYPE,
            .data_info = {}
        },
        {
            .data_type = MULTIPLE_AMOUNT_DATA_TYPE,
            .data_info = {}
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
    size_t numElements = sizeof(AuthorizeForPeerTx) / sizeof(AuthorizeForPeerTx[0]);
    parser_status_e status_tx = parse_tx(buf,AuthorizeForPeerTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           AuthorizeForPeerTx[1].data);

    for(size_t i=0; i< AuthorizeForPeerTx[3].data_info.data_number;i++) {
        uint64_t pos = AuthorizeForPeerTx[3].data_info.pos_start+i;
        if(i==0) {
            memcpy(G_context.display_data.peer_pubkey, resultArray[pos], PEER_PUBKEY_LEN);
        }
#if !defined(TARGET_NANOS) //For other devices, only three pubkey are displayed at most
        G_context.display_data.pubkey_number = AuthorizeForPeerTx[3].data_info.data_number;
        if(i==1) {
            memcpy(G_context.display_data.content_three, resultArray[pos], PEER_PUBKEY_LEN);
        }
        if (i==2) {
            memcpy(G_context.display_data.content_four, resultArray[pos], PEER_PUBKEY_LEN);
        }
#endif
    }

    uint64_t  total_value = 0;
    for(size_t j =0;j<AuthorizeForPeerTx[4].data_info.data_number;j++) {
        uint64_t  pos = AuthorizeForPeerTx[4].data_info.pos_start+j;
        total_value += getValueByLen(resultArray[pos], 8);
    }

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               total_value)) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf) {
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
            .data_type = MULTIPLE_PUBKEY_DATA_TYPE,
            .data_info = {}
        },
        {
            .data_type = MULTIPLE_AMOUNT_DATA_TYPE,
            .data_info = {}
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
    size_t numElements = sizeof(UnAuthorizeForPeerTx) / sizeof(UnAuthorizeForPeerTx[0]);
    parser_status_e status_tx = parse_tx(buf,UnAuthorizeForPeerTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }
    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           UnAuthorizeForPeerTx[1].data);

    for(size_t i=0; i< UnAuthorizeForPeerTx[3].data_info.data_number;i++) {
        uint64_t pos = UnAuthorizeForPeerTx[3].data_info.pos_start+i;
        if(i==0) {
            memcpy(G_context.display_data.peer_pubkey, resultArray[pos], PEER_PUBKEY_LEN);
        }
#if !defined(TARGET_NANOS) //For other devices, only three pubkey are displayed at most
        G_context.display_data.pubkey_number = UnAuthorizeForPeerTx[3].data_info.data_number;
        if(i==1) {
            memcpy(G_context.display_data.content_three, resultArray[pos], PEER_PUBKEY_LEN);
        }
        if (i==2) {
            memcpy(G_context.display_data.content_four, resultArray[pos], PEER_PUBKEY_LEN);
        }
#endif
    }

    uint64_t  total_value = 0;
    for(size_t j =0;j<UnAuthorizeForPeerTx[4].data_info.data_number;j++) {
        uint64_t  pos = UnAuthorizeForPeerTx[4].data_info.pos_start+j;
        total_value += getValueByLen(resultArray[pos], 8);
    }

    if(!convert_uint64_to_char(G_context.display_data.amount,
                               sizeof (G_context.display_data.amount),
                               total_value)) {
        return DATA_PARSING_ERROR;
    }
    return PARSING_OK;
}


parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf) {
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
    size_t numElements = sizeof(WithdrawFeeTx) / sizeof(WithdrawFeeTx[0]);
    parser_status_e status_tx = parse_tx(buf,WithdrawFeeTx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    if (status_tx != PARSING_OK) {
        return status_tx;
    }

    script_hash_to_address(G_context.display_data.content,
                           sizeof(G_context.display_data.content),
                           WithdrawFeeTx[1].data);

    return PARSING_OK;
}
