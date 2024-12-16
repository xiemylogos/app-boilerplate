#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#include "buffer.h"

int handler_sign_register_candidate_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_withdraw_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_quit_node_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_add_init_pos_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_reduce_init_pos_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_change_max_authorization_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_set_fee_percentage_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_authorize_for_peer_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_un_authorize_for_peer_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_withdraw_ong_tx(buffer_t *cdata, uint8_t chunk, bool more);

int handler_sign_withdraw_fee_tx(buffer_t *cdata, uint8_t chunk, bool more);
