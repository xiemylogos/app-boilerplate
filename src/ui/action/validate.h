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
 * Action for personal msg information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_personal_msg(bool choice);

/**
 * Action for oep4 transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_oep4_transaction(bool choice);
/**
 * Action for register candidate transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_register_candidate_transaction(bool choice);
/**
 * Action for withdraw transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_withdraw_transaction(bool choice);
/**
 * Action for quit node transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_quit_node_transaction(bool choice);
/**
 * Action for add init pos transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_add_init_pos_transaction(bool choice);
/**
 * Action for reduce init pos transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_reduce_init_pos_transaction(bool choice);
/**
 * Action for change max authorization transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_change_max_authorization_transaction(bool choice);
/**
 * Action for set fee percentage transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_set_fee_percentage_transaction(bool choice);
/**
 * Action for authorize for peer transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_authorize_for_peer_transaction(bool choice);
/**
 * Action for un_authorize for peer transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_un_authorize_for_peer_transaction(bool choice);
/**
 * Action for withdraw fee transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_withdraw_fee_transaction(bool choice);
/**
 * Action for approve transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_approve_transaction(bool choice);
