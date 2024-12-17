#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"


parser_status_e register_candidate_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);

parser_status_e withdraw_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e quit_node_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e withdraw_ong_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);

parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf, ont_transaction_t *tx);
