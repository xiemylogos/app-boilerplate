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

#include "sign_tx.h"
#include "../sw.h"
#include "../globals.h"
#include "../ui/display.h"
#include "../transaction/types.h"
#include "../transaction/govern_deserialize.h"


int handler_sign_register_candidate_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_REGISTER_CANDIDATE;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_REGISTER_CANDIDATE) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.register_candidate_tx_info.raw_tx_len + cdata->size > sizeof(G_context.register_candidate_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.register_candidate_tx_info.raw_tx + G_context.register_candidate_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.register_candidate_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.register_candidate_tx_info.raw_tx,
                .size = G_context.register_candidate_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = register_candidate_tx_deserialize(&buf, &G_context.register_candidate_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.register_candidate_tx_info.raw_tx,
                               G_context.register_candidate_tx_info.raw_tx_len,
                               G_context.register_candidate_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.register_candidate_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.register_candidate_tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.register_candidate_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }
            memcpy(G_context.register_candidate_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.register_candidate_tx_info.m_hash), G_context.register_candidate_tx_info.m_hash);

            return ui_display_register_candidate_tx();
        }
    }
    return 0;
}

int handler_sign_withdraw_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_WITHDRAW;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_WITHDRAW) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.withdraw_tx_info.raw_tx_len + cdata->size > sizeof(G_context.withdraw_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.withdraw_tx_info.raw_tx + G_context.withdraw_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.withdraw_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.withdraw_tx_info.raw_tx,
                .size = G_context.withdraw_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = withdraw_tx_deserialize(&buf, &G_context.withdraw_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.withdraw_tx_info.raw_tx,
                               G_context.withdraw_tx_info.raw_tx_len,
                               G_context.withdraw_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.withdraw_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.withdraw_tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.withdraw_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.withdraw_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.withdraw_tx_info.m_hash), G_context.withdraw_tx_info.m_hash);

            return ui_display_withdraw_tx();
        }
    }
    return 0;
}

int handler_sign_quit_node_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_QUIT_NODE;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_QUIT_NODE) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.quit_node_tx_info.raw_tx_len + cdata->size > sizeof(G_context.quit_node_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.quit_node_tx_info.raw_tx + G_context.quit_node_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.quit_node_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.quit_node_tx_info.raw_tx,
                .size = G_context.quit_node_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = quit_node_tx_deserialize(&buf, &G_context.quit_node_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.quit_node_tx_info.raw_tx,
                               G_context.quit_node_tx_info.raw_tx_len,
                               G_context.quit_node_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.quit_node_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.quit_node_tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.quit_node_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.quit_node_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.quit_node_tx_info.m_hash), G_context.quit_node_tx_info.m_hash);

            return ui_display_quit_node_tx();
        }
    }
    return 0;
}

int handler_sign_add_init_pos_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_ADD_INIT_POS;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_ADD_INIT_POS) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.add_init_pos_tx_info.raw_tx_len + cdata->size > sizeof(G_context.add_init_pos_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.add_init_pos_tx_info.raw_tx + G_context.add_init_pos_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.add_init_pos_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.add_init_pos_tx_info.raw_tx,
                .size = G_context.add_init_pos_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = add_init_pos_tx_deserialize(&buf, &G_context.add_init_pos_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.add_init_pos_tx_info.raw_tx,
                               G_context.add_init_pos_tx_info.raw_tx_len,
                               G_context.add_init_pos_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.add_init_pos_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.add_init_pos_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.add_init_pos_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.add_init_pos_tx_info.m_hash), G_context.add_init_pos_tx_info.m_hash);

            return ui_display_add_init_pos_tx();
        }
    }
    return 0;
}

int handler_sign_reduce_init_pos_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_REDUCE_INIT_POS;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else {  // parse transaction
        if (G_context.req_type != CONFIRM_REDUCE_INIT_POS) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.reduce_init_pos_tx_info.raw_tx_len + cdata->size > sizeof(G_context.reduce_init_pos_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.reduce_init_pos_tx_info.raw_tx + G_context.reduce_init_pos_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.reduce_init_pos_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.reduce_init_pos_tx_info.raw_tx,
                .size = G_context.reduce_init_pos_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = reduce_init_pos_tx_deserialize(&buf,&G_context.reduce_init_pos_tx_info.transaction);

            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }

            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.reduce_init_pos_tx_info.raw_tx,
                               G_context.reduce_init_pos_tx_info.raw_tx_len,
                               G_context.reduce_init_pos_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.reduce_init_pos_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.reduce_init_pos_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.reduce_init_pos_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n",sizeof(G_context.reduce_init_pos_tx_info.m_hash),G_context.reduce_init_pos_tx_info.m_hash);

            return ui_display_reduce_init_pos_tx();
        }
    }
    return 0;
}

int handler_sign_change_max_authorization_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_CHANGE_MAX_AUTHORIZATION;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_CHANGE_MAX_AUTHORIZATION) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.change_max_authorization_tx_info.raw_tx_len + cdata->size > sizeof(G_context.change_max_authorization_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.change_max_authorization_tx_info.raw_tx + G_context.change_max_authorization_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.change_max_authorization_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.change_max_authorization_tx_info.raw_tx,
                .size = G_context.change_max_authorization_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = change_max_authorization_tx_deserialize(&buf, &G_context.change_max_authorization_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.change_max_authorization_tx_info.raw_tx,
                               G_context.change_max_authorization_tx_info.raw_tx_len,
                               G_context.change_max_authorization_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.change_max_authorization_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.change_max_authorization_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.change_max_authorization_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.change_max_authorization_tx_info.m_hash), G_context.change_max_authorization_tx_info.m_hash);

            return ui_display_change_max_authorization_tx();
        }
    }
    return 0;
}

int handler_sign_set_fee_percentage_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_SET_FEE_PERCENTAGE;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_SET_FEE_PERCENTAGE) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.set_fee_percentage_tx_info.raw_tx_len + cdata->size > sizeof(G_context.set_fee_percentage_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.set_fee_percentage_tx_info.raw_tx + G_context.set_fee_percentage_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.set_fee_percentage_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.set_fee_percentage_tx_info.raw_tx,
                .size = G_context.set_fee_percentage_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = set_fee_percentage_tx_deserialize(&buf, &G_context.set_fee_percentage_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.set_fee_percentage_tx_info.raw_tx,
                               G_context.set_fee_percentage_tx_info.raw_tx_len,
                               G_context.set_fee_percentage_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.set_fee_percentage_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.set_fee_percentage_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.set_fee_percentage_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.set_fee_percentage_tx_info.m_hash), G_context.set_fee_percentage_tx_info.m_hash);

            return ui_display_set_fee_percentage_tx();
        }
    }
    return 0;
}

int handler_sign_authorize_for_peer_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_AUTHORIZE_FOR_PEER;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_AUTHORIZE_FOR_PEER) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.authorize_for_peer_tx_info.raw_tx_len + cdata->size > sizeof(G_context.authorize_for_peer_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.authorize_for_peer_tx_info.raw_tx + G_context.authorize_for_peer_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.authorize_for_peer_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.authorize_for_peer_tx_info.raw_tx,
                .size = G_context.authorize_for_peer_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = authorize_for_peer_tx_deserialize(&buf, &G_context.authorize_for_peer_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.authorize_for_peer_tx_info.raw_tx,
                               G_context.authorize_for_peer_tx_info.raw_tx_len,
                               G_context.authorize_for_peer_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.authorize_for_peer_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.authorize_for_peer_tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.authorize_for_peer_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.authorize_for_peer_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.authorize_for_peer_tx_info.m_hash), G_context.authorize_for_peer_tx_info.m_hash);

            return ui_display_authorize_for_peer_tx();
        }
    }
    return 0;
}

int handler_sign_un_authorize_for_peer_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_UN_AUTHORIZE_FOR_PEER;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_UN_AUTHORIZE_FOR_PEER) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.un_authorize_for_peer_tx_info.raw_tx_len + cdata->size > sizeof(G_context.un_authorize_for_peer_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.un_authorize_for_peer_tx_info.raw_tx + G_context.un_authorize_for_peer_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.un_authorize_for_peer_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.un_authorize_for_peer_tx_info.raw_tx,
                .size = G_context.un_authorize_for_peer_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = un_authorize_for_peer_tx_deserialize(&buf, &G_context.un_authorize_for_peer_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.un_authorize_for_peer_tx_info.raw_tx,
                               G_context.un_authorize_for_peer_tx_info.raw_tx_len,
                               G_context.un_authorize_for_peer_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];
            if (cx_sha256_hash(G_context.un_authorize_for_peer_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.un_authorize_for_peer_tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.un_authorize_for_peer_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.un_authorize_for_peer_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.un_authorize_for_peer_tx_info.m_hash), G_context.un_authorize_for_peer_tx_info.m_hash);

            return ui_display_un_authorize_for_peer_tx();
        }
    }
    return 0;
}

int handler_sign_withdraw_ong_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_WITHDRAW_ONG;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_WITHDRAW_ONG) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.withdraw_ong_tx_info.raw_tx_len + cdata->size > sizeof(G_context.withdraw_ong_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.withdraw_ong_tx_info.raw_tx + G_context.withdraw_ong_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.withdraw_ong_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.withdraw_ong_tx_info.raw_tx,
                .size = G_context.withdraw_ong_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = withdraw_ong_tx_deserialize(&buf, &G_context.withdraw_ong_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.withdraw_ong_tx_info.raw_tx,
                               G_context.withdraw_ong_tx_info.raw_tx_len,
                               G_context.withdraw_ong_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }
            uint8_t second_hash[32];

            if (cx_sha256_hash(G_context.withdraw_ong_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.withdraw_ong_tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.withdraw_ong_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.withdraw_ong_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.withdraw_ong_tx_info.m_hash), G_context.withdraw_ong_tx_info.m_hash);

            return ui_display_withdraw_ong_tx();
        }
    }
    return 0;
}

int handler_sign_withdraw_fee_tx(buffer_t *cdata, uint8_t chunk, bool more) {
    if (chunk == 0) {  // first APDU, parse BIP32 path
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.req_type = CONFIRM_WITHDRAW_FEE;
        G_context.state = STATE_NONE;

        if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
            !buffer_read_bip32_path(cdata,
                                    G_context.bip32_path,
                                    (size_t) G_context.bip32_path_len)) {
            return io_send_sw(SW_WRONG_DATA_LENGTH);
        }

        return io_send_sw(SW_OK);

    } else { // parse transaction
        if (G_context.req_type != CONFIRM_WITHDRAW_FEE) {
            return io_send_sw(SW_BAD_STATE);
        }
        if (G_context.withdraw_fee_tx_info.raw_tx_len + cdata->size > sizeof(G_context.withdraw_fee_tx_info.raw_tx)) {
            return io_send_sw(SW_WRONG_TX_LENGTH);
        }
        if (!buffer_move(cdata,
                         G_context.withdraw_fee_tx_info.raw_tx + G_context.withdraw_fee_tx_info.raw_tx_len,
                         cdata->size)) {
            return io_send_sw(SW_TX_PARSING_FAIL);
        }
        G_context.withdraw_fee_tx_info.raw_tx_len += cdata->size;

        if (more) {
            // more APDUs with transaction part are expected.
            // Send a SW_OK to signal that we have received the chunk
            return io_send_sw(SW_OK);

        } else {
            // last APDU for this transaction, let's parse, display and request a sign confirmation

            buffer_t buf = {.ptr = G_context.withdraw_fee_tx_info.raw_tx,
                .size = G_context.withdraw_fee_tx_info.raw_tx_len,
                .offset = 0};

            parser_status_e status = withdraw_fee_tx_deserialize(&buf, &G_context.withdraw_fee_tx_info.transaction);
            PRINTF("Parsing status: %d.\n", status);
            if (status != PARSING_OK) {
                return io_send_sw(SW_TX_PARSING_FAIL);
            }
            G_context.state = STATE_PARSED;

            if (cx_sha256_hash(G_context.withdraw_fee_tx_info.raw_tx,
                               G_context.withdraw_fee_tx_info.raw_tx_len,
                               G_context.withdraw_fee_tx_info.m_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            uint8_t second_hash[32];

            if (cx_sha256_hash(G_context.withdraw_fee_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.tx_info.m_hash, second_hash, 32);

            if (cx_sha256_hash(G_context.withdraw_fee_tx_info.m_hash, 32, second_hash) != CX_OK) {
                return io_send_sw(SW_TX_HASH_FAIL);
            }

            memcpy(G_context.withdraw_fee_tx_info.m_hash, second_hash, 32);

            PRINTF("Hash: %.*H\n", sizeof(G_context.withdraw_fee_tx_info.m_hash), G_context.withdraw_fee_tx_info.m_hash);

            return ui_display_withdraw_fee_tx();
        }
    }
    return 0;
}
