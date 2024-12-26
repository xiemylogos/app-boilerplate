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
                return VERSION_PARSING_ERROR;
            }
            if(version != 0x00) {
                return VERSION_PARSING_ERROR;
            }
            //txType
            if(!buffer_read_u8(&buf,&tx_type)) {
                return TXTYPE_PARSING_ERROR;
            }
            //parse transaction
            if (tx_type == 0xd1) { //InvokeNeo
                if (memcmp(buf.ptr + buf.size - 22 - 1, "Ontology.Native.Invoke", 22) == 0) {
                    if(memcmp(buf.ptr + buf.size - 46 - 10 - 1, "transferV2", 10) == 0) {
                        parser_status_e status =  transaction_deserialize(&buf, &G_context.tx_info.tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = TRANSFER_TRANSACTION;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-17-1,"registerCandidate",17) == 0) {
                        parser_status_e status =  register_candidate_tx_deserialize(&buf, &G_context.tx_info.register_candidate_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = REGISTER_CANDIDATE;
                        G_context.state = STATE_PARSED;
                    }  else if (memcmp(buf.ptr + buf.size-46-8-1,"withdraw",8) == 0) {
                        parser_status_e status =  withdraw_tx_deserialize(&buf, &G_context.tx_info.withdraw_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = WITHDRAW;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-8-1, "quitNode",8) == 0) {
                        parser_status_e status =  quit_node_tx_deserialize(&buf, &G_context.tx_info.quit_node_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = QUIT_NODE;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-10-1,"addInitPos",10) ==0) {
                        parser_status_e status =  add_init_pos_tx_deserialize(&buf, &G_context.tx_info.add_init_pos_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = ADD_INIT_POS;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-13-1,"reduceInitPos",13) ==0) {
                        parser_status_e status =  reduce_init_pos_tx_deserialize(&buf, &G_context.tx_info.reduce_init_pos_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = REDUCE_INIT_POS;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-22-1,"changeMaxAuthorization",22) ==0) {
                        parser_status_e status =  change_max_authorization_tx_deserialize(&buf, &G_context.tx_info.change_max_authorization_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = CHANGE_MAX_AUTHORIZATION;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-16-1,"setFeePercentage",16) ==0) {
                        parser_status_e status =  set_fee_percentage_tx_deserialize(&buf, &G_context.tx_info.set_fee_percentage_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = SET_FEE_PERCENTAGE;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-16-1,"authorizeForPeer",16) == 0) {
                        parser_status_e status =  authorize_for_peer_tx_deserialize(&buf, &G_context.tx_info.authorize_for_peer_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = AUTHORIZE_FOR_PEER;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-18-1,"unAuthorizeForPeer",18) ==0) {
                        parser_status_e status =  un_authorize_for_peer_tx_deserialize(&buf, &G_context.tx_info.un_authorize_for_peer_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = UN_AUTHORIZE_FOR_PEER;
                        G_context.state = STATE_PARSED;
                    } else if (memcmp(buf.ptr + buf.size-46-11-1,"withdrawOng",11) ==0) {
                        parser_status_e status =  withdraw_ong_tx_deserialize(&buf, &G_context.tx_info.withdraw_ong_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = WITHDRAW_ONG;
                        G_context.state = STATE_PARSED;
                    } else if(memcmp(buf.ptr + buf.size-46-11-1,"withdrawFee",11) == 0) {
                        parser_status_e status = withdraw_fee_tx_deserialize(&buf, &G_context.tx_info.withdraw_fee_tx_info);
                        if (status != PARSING_OK) {
                            return io_send_sw(SW_TX_PARSING_FAIL);
                        }
                        G_context.tx_type = WITHDRAW_FEE;
                        G_context.state = STATE_PARSED;
                    }
                } else if(memcmp(buf.ptr+buf.size - 21-8-1, "transfer", 8) == 0) {
                    parser_status_e status = oep4_neo_vm_transaction_deserialize(&buf, &G_context.tx_info.oep4_tx_info);
                    if (status != PARSING_OK) {
                        return io_send_sw(status);
                    }
                    G_context.tx_type = OEP4_TRANSACTION;
                    G_context.state = STATE_PARSED;
                }
            } else if (tx_type == 0xd2) { //InvokeWasm
                parser_status_e status = oep4_wasm_vm_transaction_deserialize(&buf, &G_context.tx_info.oep4_tx_info);
                if (status != PARSING_OK) {
                    return io_send_sw(status);
                }
                G_context.tx_type = OEP4_TRANSACTION;
                G_context.state = STATE_PARSED;
            } else {
                return TXTYPE_PARSING_ERROR;
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

            if ( G_context.tx_type == TRANSFER_TRANSACTION) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_transaction();
#else
                    return ui_display_transaction();
#endif
                } else {
                    return ui_display_transaction();
                }
            } else if(G_context.tx_type == OEP4_TRANSACTION) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_oep4_transaction();
#else
                    return ui_display_oep4_transaction();
#endif
                } else {
                    return ui_display_oep4_transaction();
                }
            } else if (G_context.tx_type == REGISTER_CANDIDATE) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_register_candidate_tx();
#else
                    return ui_display_register_candidate_tx();
#endif
                } else {
                    return ui_display_register_candidate_tx();
                }
            } else if (G_context.tx_type == WITHDRAW) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_withdraw_tx();
#else
                    return ui_display_withdraw_tx();
#endif
                } else {
                    return ui_display_withdraw_tx();
                }
            } else if (G_context.tx_type == QUIT_NODE) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_quit_node_tx();
#else
                    return ui_display_quit_node_tx();
#endif
                } else {
                    return ui_display_quit_node_tx();
                }
            } else if (G_context.tx_type == ADD_INIT_POS) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_add_init_pos_tx();
#else
                    return ui_display_add_init_pos_tx();
#endif
                } else {
                    return ui_display_add_init_pos_tx();
                }
            } else if (G_context.tx_type == REDUCE_INIT_POS) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_reduce_init_pos_tx();
#else
                    return ui_display_reduce_init_pos_tx();
#endif
                } else {
                    return ui_display_reduce_init_pos_tx();
                }
            } else if (G_context.tx_type == CHANGE_MAX_AUTHORIZATION) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_change_max_authorization_tx();
#else
                    return ui_display_change_max_authorization_tx();
#endif
                } else {
                    return ui_display_change_max_authorization_tx();
                }
            } else if (G_context.tx_type == SET_FEE_PERCENTAGE) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_set_fee_percentage_tx();
#else
                    return ui_display_set_fee_percentage_tx();
#endif
                } else {
                    return ui_display_set_fee_percentage_tx();
                }
            } else if (G_context.tx_type == AUTHORIZE_FOR_PEER) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_authorize_for_peer_tx();
#else
                    return ui_display_authorize_for_peer_tx();
#endif
                } else {
                    return ui_display_authorize_for_peer_tx();
                }
            } else if (G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_un_authorize_for_peer_tx();
#else
                    return ui_display_un_authorize_for_peer_tx();
#endif
                } else {
                    return ui_display_un_authorize_for_peer_tx();
                }
            } else if (G_context.tx_type == WITHDRAW_ONG) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_withdraw_ong_tx();
#else
                    return ui_display_withdraw_ong_tx();
#endif
                } else {
                    return ui_display_withdraw_ong_tx();
                }
            } else if (G_context.tx_type == WITHDRAW_FEE) {
                if (N_storage.blind_signed_allowed) {
#ifdef HAVE_NBGL
                    return ui_display_blind_signed_withdraw_fee_tx();
#else
                    return ui_display_withdraw_fee_tx();
#endif
                } else {
                    return ui_display_withdraw_fee_tx();
                }
            }
        }
    }

    return 0;
}
