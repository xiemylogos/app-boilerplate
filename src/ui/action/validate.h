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
void validate_transaction_new(bool choice);
/**
 * Action for personal msg information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */

void validate_personal_msg(bool choice);
/**
 * Action for register candidate transaction information validation.
 *
 * @param[in] choice
 *   User choice (either approved or rejectd).
 *
 */
void validate_govern_transaction(bool choice);
