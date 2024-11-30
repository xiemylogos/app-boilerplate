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
