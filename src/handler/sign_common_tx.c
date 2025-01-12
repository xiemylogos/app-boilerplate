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

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"
#include "buffer.h"

#include "sign_common_tx.h"
#include "../sw.h"
#include "../globals.h"
#include "../ui/display.h"
#include "../transaction/types.h"
#include "../transaction/deserialize.h"
#include "../transaction/govern_deserialize.h"
#include "../transaction/oep4_deserialize.h"

int handler_sign_common_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_TRANSACTION;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else {  // parse transaction

        if (G_context.req_type != CONFIRM_TRANSACTION) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.tx_info.raw_tx_len + cdata->size > sizeof(G_context.tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.tx_info.raw_tx + G_context.tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation
            buffer_t buf = {.ptr = G_context.tx_info.raw_tx,
                .size = G_context.tx_info.raw_tx_len,
                .offset = 0};
             //version
            uint8_t version;
            uint8_t tx_type;
            if(!buffer_read_u8(&buf,&version)) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            if(version != 0x00) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            //txType
            if(!buffer_read_u8(&buf,&tx_type)) {
                   return io_send_sw(SW_TX_PARSING_FAIL);
            }
            parser_status_e status = PARSING_OK;
            //parse transaction
            if (tx_type == 0xd1) { //InvokeNeo
                if (memcmp(buf.ptr + buf.size - 22 - 1, OntologyNativeInvoke, 22) == 0) {
                    if(memcmp(buf.ptr + buf.size - 46 - 10 - 1, TransferV2, 10) == 0) {
                        status =  transaction_native_transfer_v2_deserialize(&buf, &G_context.tx_info.tx_info);
                        G_context.tx_type = TRANSFER_V2_TRANSACTION;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-17-1,RegisterCandidate,17) == 0) {
                        status =  register_candidate_tx_deserialize(&buf, &G_context.tx_info.register_candidate_tx_info);
                        G_context.tx_type = REGISTER_CANDIDATE;
                        G_context.state = STATE_PARSED;
                    }  else if (memcmp(buf.ptr + buf.size-46-8-1,Withdraw,8) == 0) {
                        status =  withdraw_tx_deserialize(&buf, &G_context.tx_info.withdraw_tx_info);
                        G_context.tx_type = WITHDRAW;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-8-1, QuitNode,8) == 0) {
                        status =  quit_node_tx_deserialize(&buf, &G_context.tx_info.quit_node_tx_info);
                        G_context.tx_type = QUIT_NODE;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-10-1,AddInitPos,10) ==0) {
                        status =  add_init_pos_tx_deserialize(&buf, &G_context.tx_info.add_init_pos_tx_info);
                        G_context.tx_type = ADD_INIT_POS;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-13-1,ReduceInitPos,13) ==0) {
                        status =  reduce_init_pos_tx_deserialize(&buf, &G_context.tx_info.reduce_init_pos_tx_info);
                        G_context.tx_type = REDUCE_INIT_POS;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-22-1,ChangeMaxAuthorization,22) ==0) {
                        status =  change_max_authorization_tx_deserialize(&buf, &G_context.tx_info.change_max_authorization_tx_info);
                        G_context.tx_type = CHANGE_MAX_AUTHORIZATION;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-16-1,SetFeePercentage,16) ==0) {
                        status =  set_fee_percentage_tx_deserialize(&buf, &G_context.tx_info.set_fee_percentage_tx_info);
                        G_context.tx_type = SET_FEE_PERCENTAGE;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-16-1,AuthorizeForPeer,16) == 0) {
                        status =  authorize_for_peer_tx_deserialize(&buf, &G_context.tx_info.authorize_for_peer_tx_info);
                        G_context.tx_type = AUTHORIZE_FOR_PEER;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-18-1,UnAuthorizeForPeer,18) ==0) {
                        status =  un_authorize_for_peer_tx_deserialize(&buf, &G_context.tx_info.un_authorize_for_peer_tx_info);
                        G_context.tx_type = UN_AUTHORIZE_FOR_PEER;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-11-1,WithdrawFee,11) == 0) {
                        status = withdraw_fee_tx_deserialize(&buf, &G_context.tx_info.withdraw_fee_tx_info);
                        G_context.tx_type = WITHDRAW_FEE;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-7-1,Approve,7) == 0) {
                        status = transaction_approve_deserialize(&buf, &G_context.tx_info.tx_info);
                        G_context.tx_type = APPROVE;
                        G_context.state = STATE_PARSED;
                    }else if (memcmp(buf.ptr + buf.size-46-9-1,ApproveV2,9) == 0) {
                        status = transaction_approve_v2_deserialize(&buf, &G_context.tx_info.tx_info);
                        G_context.tx_type = APPROVE_V2;
                        G_context.state = STATE_PARSED;
                    }else if (memcmp(buf.ptr + buf.size - 46 - 8 - 1, Transfer, 8) == 0){
                        status =  transaction_native_transfer_deserialize(&buf, &G_context.tx_info.tx_info);
                        G_context.tx_type = TRANSFER_TRANSACTION;
                        G_context.state = STATE_PARSED;
                    }else if (memcmp(buf.ptr + buf.size - 46 - 12 - 1, TransferFrom, 12) == 0) {
                        status =  transaction_native_transfer_from_deserialize(&buf, &G_context.tx_info.from_tx_info);
                        G_context.tx_type = TRANSFER_FROM_TRANSACTION;
                        G_context.state = STATE_PARSED;
                    }else if (memcmp(buf.ptr + buf.size - 46 - 14 - 1, TransferFromV2, 14) == 0){
                        status =  transaction_native_transfer_from_v2_deserialize(&buf, &G_context.tx_info.from_tx_info);
                        G_context.tx_type = TRANSFER_FROM_V2_TRANSACTION;
                        G_context.state = STATE_PARSED;
                    }else {
                        status = TX_PARSING_ERROR;
                    }
                } else if(memcmp(buf.ptr+buf.size - 21-8-1, Transfer, 8) == 0) { //neovm oep4
                    status = oep4_neo_vm_transaction_deserialize(&buf, &G_context.tx_info.oep4_tx_info);
                    G_context.tx_type = OEP4_TRANSACTION;
                    G_context.state = STATE_PARSED;
                } else if(memcmp(buf.ptr+buf.size - 21-7-1, Approve, 7) == 0){ //neovm oep4
                    status = oep4_neo_vm_approve_transaction_deserialize(&buf, &G_context.tx_info.oep4_tx_info);
                    G_context.tx_type = NEO_VM_OEP4_APPROVE;
                    G_context.state = STATE_PARSED;
                }else if (memcmp(buf.ptr+buf.size - 21-12-1, TransferFrom, 12) == 0){ //neovm oep4
                    status = oep4_neo_vm_transfer_from_transaction_deserialize(&buf, &G_context.tx_info.oep4_from_tx_info);
                    G_context.tx_type = NEO_VM_OEP4_TRANSFER_FROM;
                    G_context.state = STATE_PARSED;
                }else {
                    status = TX_PARSING_ERROR;
                }
            } else if (tx_type == 0xd2) { //InvokeWasm
                if(memcmp(buf.ptr+buf.size-56-8-1,Transfer,8) == 0) {   //wasm oep4
                    status = oep4_wasm_vm_transaction_deserialize(&buf, &G_context.tx_info.oep4_tx_info);
                    G_context.tx_type = OEP4_TRANSACTION;
                    G_context.state = STATE_PARSED;
                } else if (memcmp(buf.ptr+buf.size-56-7-1,Approve,7) == 0) { //wasm oep4
                    status = oep4_wasm_vm_approve_transaction_deserialize(&buf, &G_context.tx_info.oep4_tx_info);
                    G_context.tx_type = WASM_VM_OEP4_APPROVE;
                    G_context.state = STATE_PARSED;
                } else if(memcmp(buf.ptr+buf.size-76-12-1,TransferFrom,12) == 0){ //wasm oep4
                    status = oep4_wasm_vm_transfer_from_transaction_deserialize(&buf, &G_context.tx_info.oep4_from_tx_info);
                    G_context.tx_type = WASM_VM_OEP4_TRANSFER_FROM;
                    G_context.state = STATE_PARSED;
                }else {
                    status = TX_PARSING_ERROR;
                }
            } else {
                status = TX_PARSING_ERROR;
            }
            return handler_hash_tx_and_display_tx(status);
        }
    }
    return 0;
}

int handler_hash_tx_and_display_tx(int status) {
    if (status != PARSING_OK) {
        if (!N_storage.blind_signed_allowed) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
    }

    if (cx_sha256_hash(G_context.tx_info.raw_tx,
                       G_context.tx_info.raw_tx_len,
                       G_context.tx_info.m_hash) != CX_OK) {
        return io_send_sw(SW_TX_HASH_FAIL);
    }

    uint8_t second_hash[32];
    if (cx_sha256_hash(G_context.tx_info.m_hash, 32, second_hash) != CX_OK) {
        return io_send_sw(SW_TX_HASH_FAIL);
    }

    memcpy(G_context.tx_info.m_hash, second_hash, 32);

    if (cx_sha256_hash(G_context.tx_info.m_hash, 32, second_hash) != CX_OK) {
        return io_send_sw(SW_TX_HASH_FAIL);
    }

    memcpy(G_context.tx_info.m_hash, second_hash, 32);
    explicit_bzero(&second_hash, sizeof(second_hash));
    PRINTF("Hash: %.*H\n", sizeof(G_context.tx_info.m_hash), G_context.tx_info.m_hash);
    if (status != PARSING_OK) {
        if (N_storage.blind_signed_allowed) {
            return ui_display_blind_signing_transaction();
        }
    } else {
        if (G_context.tx_type == TRANSFER_V2_TRANSACTION ||
            G_context.tx_type == TRANSFER_TRANSACTION) {
            return ui_display_transaction();
        } else if (G_context.tx_type == OEP4_TRANSACTION) {
            return ui_display_oep4_transaction();
        } else if (G_context.tx_type == REGISTER_CANDIDATE) {
            return ui_display_register_candidate_tx();
        } else if (G_context.tx_type == WITHDRAW) {
            return ui_display_withdraw_tx();
        } else if (G_context.tx_type == QUIT_NODE) {
            return ui_display_quit_node_tx();
        } else if (G_context.tx_type == ADD_INIT_POS) {
            return ui_display_add_init_pos_tx();
        } else if (G_context.tx_type == REDUCE_INIT_POS) {
            return ui_display_reduce_init_pos_tx();
        } else if (G_context.tx_type == CHANGE_MAX_AUTHORIZATION) {
            return ui_display_change_max_authorization_tx();
        } else if (G_context.tx_type == SET_FEE_PERCENTAGE) {
            return ui_display_set_fee_percentage_tx();
        } else if (G_context.tx_type == AUTHORIZE_FOR_PEER) {
            return ui_display_authorize_for_peer_tx();
        } else if (G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
            return ui_display_un_authorize_for_peer_tx();
        } else if (G_context.tx_type == WITHDRAW_FEE) {
            return ui_display_withdraw_fee_tx();
        } else if (G_context.tx_type == APPROVE ||
                   G_context.tx_type == APPROVE_V2) {
            return ui_display_approve_tx();
        } else if (G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
                   G_context.tx_type == TRANSFER_FROM_TRANSACTION) {
            return ui_display_transaction_from();
        } else if (G_context.tx_type == NEO_VM_OEP4_APPROVE ||
                   G_context.tx_type == WASM_VM_OEP4_APPROVE) {
            return ui_display_oep4_approve_tx();
        } else if(G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
                   G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
            return ui_display_oep4_transfer_from_tx();
        }
    }
    return 0;
}