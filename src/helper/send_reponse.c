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

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memmove

#include "buffer.h"

#include "send_response.h"
#include "../constants.h"
#include "../globals.h"
#include "../sw.h"

int helper_send_response_pubkey() {
    uint8_t resp[1 + PUBKEY_LEN + 1 + CHAINCODE_LEN] = {0};
    size_t offset = 0;

    resp[offset++] = PUBKEY_LEN;
    memmove(resp + offset, G_context.pk_info.raw_public_key, PUBKEY_LEN);
    offset += PUBKEY_LEN;
    resp[offset++] = CHAINCODE_LEN;
    memmove(resp + offset, G_context.pk_info.chain_code, CHAINCODE_LEN);
    offset += CHAINCODE_LEN;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.tx_info.signature_len;
    memmove(resp + offset, G_context.tx_info.signature, G_context.tx_info.signature_len);
    offset += G_context.tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}


int helper_person_msg_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.person_msg_info.signature_len;
    memmove(resp + offset, G_context.person_msg_info.signature, G_context.person_msg_info.signature_len);
    offset += G_context.person_msg_info.signature_len;
    resp[offset++] = (uint8_t) G_context.person_msg_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_oep4_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.oep4_tx_info.signature_len;
    memmove(resp + offset, G_context.oep4_tx_info.signature, G_context.oep4_tx_info.signature_len);
    offset += G_context.oep4_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.oep4_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_register_candidate_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.register_candidate_tx_info.signature_len;
    memmove(resp + offset, G_context.register_candidate_tx_info.signature, G_context.register_candidate_tx_info.signature_len);
    offset += G_context.register_candidate_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.register_candidate_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_withdraw_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.withdraw_tx_info.signature_len;
    memmove(resp + offset, G_context.withdraw_tx_info.signature, G_context.withdraw_tx_info.signature_len);
    offset += G_context.withdraw_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.withdraw_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_quit_node_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.quit_node_tx_info.signature_len;
    memmove(resp + offset, G_context.quit_node_tx_info.signature, G_context.quit_node_tx_info.signature_len);
    offset += G_context.quit_node_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.quit_node_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_add_init_pos_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.add_init_pos_tx_info.signature_len;
    memmove(resp + offset, G_context.add_init_pos_tx_info.signature, G_context.add_init_pos_tx_info.signature_len);
    offset += G_context.add_init_pos_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.add_init_pos_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_reduce_init_pos_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.reduce_init_pos_tx_info.signature_len;
    memmove(resp + offset, G_context.reduce_init_pos_tx_info.signature, G_context.reduce_init_pos_tx_info.signature_len);
    offset += G_context.reduce_init_pos_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.reduce_init_pos_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_change_max_authorization_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.change_max_authorization_tx_info.signature_len;
    memmove(resp + offset, G_context.change_max_authorization_tx_info.signature, G_context.change_max_authorization_tx_info.signature_len);
    offset += G_context.change_max_authorization_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.change_max_authorization_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_set_fee_percentage_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.set_fee_percentage_tx_info.signature_len;
    memmove(resp + offset, G_context.set_fee_percentage_tx_info.signature, G_context.set_fee_percentage_tx_info.signature_len);
    offset += G_context.set_fee_percentage_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.set_fee_percentage_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}


int helper_authorize_for_peer_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.authorize_for_peer_tx_info.signature_len;
    memmove(resp + offset, G_context.authorize_for_peer_tx_info.signature, G_context.authorize_for_peer_tx_info.signature_len);
    offset += G_context.authorize_for_peer_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.authorize_for_peer_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}


int helper_un_authorize_for_peer_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.un_authorize_for_peer_tx_info.signature_len;
    memmove(resp + offset, G_context.un_authorize_for_peer_tx_info.signature, G_context.un_authorize_for_peer_tx_info.signature_len);
    offset += G_context.un_authorize_for_peer_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.un_authorize_for_peer_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}


int helper_withdraw_ong_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.withdraw_ong_tx_info.signature_len;
    memmove(resp + offset, G_context.withdraw_ong_tx_info.signature, G_context.withdraw_ong_tx_info.signature_len);
    offset += G_context.withdraw_ong_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.withdraw_ong_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}


int helper_withdraw_fee_tx_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.withdraw_fee_tx_info.signature_len;
    memmove(resp + offset, G_context.withdraw_fee_tx_info.signature, G_context.withdraw_fee_tx_info.signature_len);
    offset += G_context.withdraw_fee_tx_info.signature_len;
    resp[offset++] = (uint8_t) G_context.withdraw_fee_tx_info.v;

    return io_send_response_pointer(resp, offset, SW_OK);
}
