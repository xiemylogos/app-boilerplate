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

#include "sign_common_tx.h"
#include "../sw.h"
#include "../globals.h"
#include "../ui/display.h"
#include "../transaction/types.h"
#include "../transaction/deserialize.h"
#include "../transaction/govern_deserialize.h"
#include "../transaction/oep4_deserialize.h"
#include "../transaction/utils.h"

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
        if (!ont_address_from_pubkey(G_context.display_data.signer,sizeof(G_context.display_data.signer))) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
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
            uint8_t version;
            uint8_t tx_type;
            if(!buffer_read_u8(&buf,&version) || version != 0x00 || !buffer_read_u8(&buf,&tx_type)) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            transaction_header_t tx_header;
            parser_status_e header_status = transaction_deserialize_header(&buf,&tx_header);
            if (header_status != PARSING_OK) {
                if (!N_storage.blind_signed_allowed) {
                    return io_send_sw(SW_TX_PARSING_FAIL);
                } else {
                    return handler_hash_tx_and_display_tx(header_status);
                }
            }
            parser_status_e status = PARSING_OK;
            G_context.state = STATE_PARSED;
            //parse transaction payload
            if (tx_type == 0xd1) { //InvokeNeo
                if (memcmp(buf.ptr + buf.size - 22 - 1, OntologyNativeInvoke, 22) == 0) {
                    if(memcmp(buf.ptr + buf.size - 46 - 10 - 1, TransferV2, 10) == 0) {
                        G_context.tx_type = TRANSFER_V2_TRANSACTION;
                        status =  transaction_native_transfer_v2_deserialize(&buf);
                    } else if(memcmp(buf.ptr + buf.size-46-17-1,RegisterCandidate,17) == 0) {
                        G_context.tx_type = REGISTER_CANDIDATE;
                        status =  register_candidate_tx_deserialize(&buf);
                    }  else if (memcmp(buf.ptr + buf.size-46-8-1,Withdraw,8) == 0) {
                        G_context.tx_type = WITHDRAW;
                        status =  withdraw_tx_deserialize(&buf);
                    } else if (memcmp(buf.ptr + buf.size-46-8-1, QuitNode,8) == 0) {
                        G_context.tx_type = QUIT_NODE;
                        status =  quit_node_tx_deserialize(&buf);
                    } else if(memcmp(buf.ptr + buf.size-46-10-1,AddInitPos,10) ==0) {
                        G_context.tx_type = ADD_INIT_POS;
                        status =  add_init_pos_tx_deserialize(&buf);
                    } else if (memcmp(buf.ptr + buf.size-46-13-1,ReduceInitPos,13) ==0) {
                        G_context.tx_type = REDUCE_INIT_POS;
                        status =  reduce_init_pos_tx_deserialize(&buf);
                    } else if(memcmp(buf.ptr + buf.size-46-22-1,ChangeMaxAuthorization,22) ==0) {
                        G_context.tx_type = CHANGE_MAX_AUTHORIZATION;
                        status =  change_max_authorization_tx_deserialize(&buf);
                    } else if (memcmp(buf.ptr + buf.size-46-16-1,SetFeePercentage,16) ==0) {
                        G_context.tx_type = SET_FEE_PERCENTAGE;
                        status =  set_fee_percentage_tx_deserialize(&buf);
                    } else if(memcmp(buf.ptr + buf.size-46-16-1,AuthorizeForPeer,16) == 0) {
                        G_context.tx_type = AUTHORIZE_FOR_PEER;
                        status =  authorize_for_peer_tx_deserialize(&buf);
                    } else if (memcmp(buf.ptr + buf.size-46-18-1,UnAuthorizeForPeer,18) ==0) {
                        G_context.tx_type = UN_AUTHORIZE_FOR_PEER;
                        status =  un_authorize_for_peer_tx_deserialize(&buf);
                    } else if(memcmp(buf.ptr + buf.size-46-11-1,WithdrawFee,11) == 0) {
                        G_context.tx_type = WITHDRAW_FEE;
                        status = withdraw_fee_tx_deserialize(&buf);
                    } else if (memcmp(buf.ptr + buf.size-46-7-1,Approve,7) == 0) {
                        G_context.tx_type = APPROVE;
                        status = transaction_approve_deserialize(&buf);
                    }else if (memcmp(buf.ptr + buf.size-46-9-1,ApproveV2,9) == 0) {
                        G_context.tx_type = APPROVE_V2;
                        status = transaction_approve_v2_deserialize(&buf);
                    }else if (memcmp(buf.ptr + buf.size - 46 - 8 - 1, Transfer, 8) == 0){
                        G_context.tx_type = TRANSFER_TRANSACTION;
                        status =  transaction_native_transfer_deserialize(&buf);
                    }else if (memcmp(buf.ptr + buf.size - 46 - 12 - 1, TransferFrom, 12) == 0) {
                        G_context.tx_type = TRANSFER_FROM_TRANSACTION;
                        status =  transaction_native_transfer_from_deserialize(&buf);
                    }else if (memcmp(buf.ptr + buf.size - 46 - 14 - 1, TransferFromV2, 14) == 0){
                        G_context.tx_type = TRANSFER_FROM_V2_TRANSACTION;
                        status =  transaction_native_transfer_from_v2_deserialize(&buf);
                    }else {
                        status = TX_PARSING_ERROR;
                    }
                } else if(memcmp(buf.ptr+buf.size - 21-8-1, Transfer, 8) == 0) { //neovm oep4
                    G_context.tx_type = OEP4_TRANSACTION;
                    status = oep4_neo_vm_transaction_deserialize(&buf);
                } else if(memcmp(buf.ptr+buf.size - 21-7-1, Approve, 7) == 0){ //neovm oep4
                    G_context.tx_type = NEO_VM_OEP4_APPROVE;
                    status = oep4_neo_vm_approve_transaction_deserialize(&buf);
                }else if (memcmp(buf.ptr+buf.size - 21-12-1, TransferFrom, 12) == 0){ //neovm oep4
                    G_context.tx_type = NEO_VM_OEP4_TRANSFER_FROM;
                    status = oep4_neo_vm_transfer_from_transaction_deserialize(&buf);
                }else {
                    status = TX_PARSING_ERROR;
                }
            } else if (tx_type == 0xd2) { //InvokeWasm
                if(memcmp(buf.ptr+buf.size-56-8-1,Transfer,8) == 0) {   //wasm oep4
                    G_context.tx_type = OEP4_TRANSACTION;
                    status = oep4_wasm_vm_transaction_deserialize(&buf);
                } else if (memcmp(buf.ptr+buf.size-56-7-1,Approve,7) == 0) { //wasm oep4
                    G_context.tx_type = WASM_VM_OEP4_APPROVE;
                    status = oep4_wasm_vm_approve_transaction_deserialize(&buf);
                } else if(memcmp(buf.ptr+buf.size-76-12-1,TransferFrom,12) == 0){ //wasm oep4
                    G_context.tx_type = WASM_VM_OEP4_TRANSFER_FROM;
                    status = oep4_wasm_vm_transfer_from_transaction_deserialize(&buf);
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
    if (status != PARSING_OK && !N_storage.blind_signed_allowed) {
        return io_send_sw(SW_TX_PARSING_FAIL);
    }
    uint8_t second_hash[32];
    if (cx_sha256_hash(G_context.tx_info.raw_tx,G_context.tx_info.raw_tx_len,G_context.tx_info.m_hash) != CX_OK ||
        cx_sha256_hash(G_context.tx_info.m_hash, 32, second_hash) != CX_OK) {
        return io_send_sw(SW_TX_HASH_FAIL);
    }
    memcpy(G_context.tx_info.m_hash, second_hash, 32);
    if (cx_sha256_hash(G_context.tx_info.m_hash, 32, second_hash) != CX_OK) {
        return io_send_sw(SW_TX_HASH_FAIL);
    }

    memcpy(G_context.tx_info.m_hash, second_hash, 32);
    explicit_bzero(&second_hash, sizeof(second_hash));

    return (status != PARSING_OK && N_storage.blind_signed_allowed)
           ? ui_display_blind_signing_transaction()
           : ui_display_transaction();
}