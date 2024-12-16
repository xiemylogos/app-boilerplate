#pragma once

#include <stdbool.h>  // bool

/**
 * Action for public key validation and export.
 *
 * @param[in] choice
 *   User choice (either approved or rejected).
 *
 */
void validate_pubkey(bool choice);

/**
 * Action for transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_transaction(bool choice);

/**
 * Action for person msg information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_person_msg(bool choice);

/**
 * Action for oep4 transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_oep4_transaction(bool choice);


void validate_register_candidate_transaction(bool choice);


void validate_withdraw_transaction(bool choice);


void validate_quit_node_transaction(bool choice);


void validate_add_init_pos_transaction(bool choice);


void validate_reduce_init_pos_transaction(bool choice);


void validate_change_max_authorization_transaction(bool choice);


void validate_set_fee_percentage_transaction(bool choice);


void validate_authorize_for_peer_transaction(bool choice);


void validate_un_authorize_for_peer_transaction(bool choice);


void validate_withdraw_ong_transaction(bool choice);

void validate_withdraw_fee_transaction(bool choice);