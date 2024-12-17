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

#include "govern_deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e register_candidate_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}

parser_status_e withdraw_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e quit_node_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}


parser_status_e withdraw_ong_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}

parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf, ont_transaction_t *tx) {
    return PARSING_OK;
}
