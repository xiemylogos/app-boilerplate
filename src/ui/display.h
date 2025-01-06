#pragma once

#include <stdbool.h>  // bool

/**
 * Callback to reuse action with approve/reject in step FLOW.
 */
typedef void (*action_validate_cb)(bool);

/**
 * Display address on the device and ask confirmation to export.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_address(void);

/**
 * Display transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_transaction(void);

/**
 * Display person msg information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_personal_msg(void);


/**
 * Display oep4 transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_oep4_transaction(void);

int ui_display_register_candidate_tx(void);

int ui_display_withdraw_tx(void);

int ui_display_quit_node_tx(void);

int ui_display_add_init_pos_tx(void);

int ui_display_reduce_init_pos_tx(void);

int ui_display_change_max_authorization_tx(void);

int ui_display_set_fee_percentage_tx(void);

int ui_display_authorize_for_peer_tx(void);

int ui_display_un_authorize_for_peer_tx(void);

int ui_display_withdraw_ong_tx(void);

int ui_display_withdraw_fee_tx(void);


int ui_display_blind_signed_transaction(void);

int ui_display_approve_tx(void);

int ui_display_transaction_from(void);

int ui_display_oep4_approve_tx(void);