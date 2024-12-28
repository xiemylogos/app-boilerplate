#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"


parser_status_e register_candidate_tx_deserialize(buffer_t *buf, register_candidate_t *tx);

parser_status_e withdraw_tx_deserialize(buffer_t *buf, withdraw_t *tx);


parser_status_e quit_node_tx_deserialize(buffer_t *buf, quit_node_t *tx);


parser_status_e add_init_pos_tx_deserialize(buffer_t *buf, add_init_pos_t *tx);


parser_status_e reduce_init_pos_tx_deserialize(buffer_t *buf, reduce_init_pos_t *tx);


parser_status_e  change_max_authorization_tx_deserialize(buffer_t *buf, change_max_authorization_t *tx);


parser_status_e  set_fee_percentage_tx_deserialize(buffer_t *buf, set_fee_percentage_t *tx);


parser_status_e authorize_for_peer_tx_deserialize(buffer_t *buf, authorize_for_peer_t *tx);


parser_status_e un_authorize_for_peer_tx_deserialize(buffer_t *buf, un_authorize_for_peer_t *tx);


parser_status_e withdraw_ong_tx_deserialize(buffer_t *buf, withdraw_ong_t *tx);

parser_status_e withdraw_fee_tx_deserialize(buffer_t *buf, withdraw_fee_t *tx);

parser_status_e check_govern_end_data(buffer_t *buf,const char* param_name);