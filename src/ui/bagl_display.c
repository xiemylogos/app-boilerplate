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

#ifdef HAVE_BAGL

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "ux.h"
#include "glyphs.h"
#include "io.h"
#include "bip32.h"
#include "format.h"

#include "display.h"
#include "constants.h"
#include "../globals.h"
#include "../sw.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../menu.h"
#include "../utils.h"
#include "../transaction/utils.h"
#include "types.h"

static action_validate_cb g_validate_callback;
static char g_address[40];
static char g_content_two[30];

static char g_personal_msg[66];

#define MAX_NUM_STEPS  10

const ux_flow_step_t *ux_display_tx_flow[MAX_NUM_STEPS];

// Validate/Invalidate public key and go back to home
static void ui_action_validate_pubkey(bool choice) {
    validate_pubkey(choice);
    ui_menu_main();
}

// Validate/Invalidate transaction and go back to home
static void ui_action_validate_transaction(bool choice) {
    validate_transaction(choice);
    ui_menu_main();
}

// Validate/Invalidate personal msg and go back to home
static void ui_action_validate_personal_msg(bool choice) {
    validate_personal_msg(choice);
    ui_menu_main();
}

static void ui_action_validate_govern_transaction(bool choice) {
    validate_govern_transaction(choice);
    ui_menu_main();
}

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_addr_step, pn, {&C_icon_eye, "Confirm Address"});
// Step with title/text for address
UX_STEP_NOCB(ux_display_from_address_step,
             bnnn_paging,
             {
                 .title = FROM,
                 .text = G_context.display_data.from,
             });

UX_STEP_NOCB(ux_display_to_address_step,
             bnnn_paging,
             {
                 .title = TO,
                 .text = G_context.display_data.to,
             });

UX_STEP_NOCB(ux_display_sender_address_step,
             bnnn_paging,
             {
                 .title = SENDER,
                 .text = G_context.display_data.content,
             });

UX_STEP_NOCB(ux_display_signer_address_step,
             bnnn_paging,
             {
                 .title = SIGNER,
                 .text = G_context.display_data.signer,
             });

UX_STEP_NOCB(ux_display_address_step,
             bnnn_paging,
             {
                 .title = ADDRESS,
                 .text =  G_context.display_data.content,
             });
// Step with approve button
UX_STEP_CB(ux_display_approve_step,
           pb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve",
           });
// Step with reject button
UX_STEP_CB(ux_display_reject_step,
           pb,
           (*g_validate_callback)(false),
           {
               &C_icon_crossmark,
               "Reject",
           });

UX_STEP_CB(ux_display_new_approve_step,
           pb,
           validate_transaction_new(true),
           {
               &C_icon_validate_14,
               "Approve",
           });
// Step with reject button
UX_STEP_CB(ux_display_new_reject_step,
           pb,
           validate_transaction_new(false),
           {
               &C_icon_crossmark,
               "Reject",
           });

UX_STEP_NOCB(ux_display_fee_step,
             bnnn_paging,
             {
                 .title = FEE_ONG,
                 .text = G_context.display_data.fee,
             });

UX_STEP_NOCB(ux_display_account_step,
             bnnn_paging,
             {
                 .title = ACCOUNT,
                 .text = G_context.display_data.content,
             });
UX_STEP_NOCB(ux_display_peer_pubkey_step,
             bnnn_paging,
             {
                 .title = PEER_PUBKEY,
                 .text = G_context.display_data.peer_pubkey,
             });

UX_STEP_NOCB(ux_display_pos_list_step,
             bnnn_paging,
             {
                 .title = POS,
                 .text = G_context.display_data.amount,
             });

UX_STEP_NOCB(ux_display_peer_cost_step,
             bnnn_paging,
             {
                 .title = PEER_COST,
                 .text = G_context.display_data.amount,
             });

UX_STEP_NOCB(ux_display_stake_cost_step,
             bnnn_paging,
             {
                 .title = STAKE_COST,
                 .text = G_context.display_data.content_two,
             });

UX_STEP_NOCB(ux_display_max_authorize_step,
             bnnn_paging,
             {
                 .title = MAX_AUTHORIZE,
                 .text = G_context.display_data.amount,
             });

UX_STEP_NOCB(ux_display_pos_step,
             bnnn_paging,
             {
                 .title = POS,
                 .text = G_context.display_data.amount,
             });

UX_STEP_NOCB(ux_display_with_draw_step,
             bnnn_paging,
             {
                 .title = TOTAL_WITHDRAW,
                 .text = G_context.display_data.amount,
             });

UX_STEP_NOCB(ux_display_init_pos_step,
             bnnn_paging,
             {
                 .title = POS,
                 .text = G_context.display_data.amount,
             });


UX_STEP_NOCB(ux_display_personal_msg_step,
             bnnn_paging,
             {
                 .title = MSG,
                 .text = g_personal_msg,
             });

UX_STEP_NOCB(ux_display_decimals_step,
             bnnn_paging,
             {
                 .title = DECIMALS,
                 .text = g_content_two,
             });

// FLOW to display address:
// #1 screen: eye icon + "Confirm Address"
// #2 screen: display address
// #3 screen: approve button
// #4 screen: reject button
UX_FLOW(ux_display_pubkey_flow,
        &ux_display_confirm_addr_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_address() {
    if (G_context.req_type != CONFIRM_ADDRESS || G_context.state != STATE_NONE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_address, 0, sizeof(g_address));
    if (!ont_address_by_pubkey(G_context.pk_info.raw_public_key, g_address, sizeof(g_address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_pubkey;

    ux_flow_init(0, ux_display_pubkey_flow, NULL);

    return 0;
}

// Step with icon and text
UX_STEP_NOCB(ux_display_review_step,
             pnn,
             {
                 &C_icon_eye,
                 "Review",
                 "Transaction",
             });

UX_STEP_NOCB(ux_display_approve_review_step,
             pnn,
             {
                 &C_icon_eye,
                 "Review",
                 "Approve Tx",
             });

// Step with title/text for amount
UX_STEP_NOCB(ux_display_amount_step,
             bnnn_paging,
             {
                 .title = AMOUNT,
                 .text = G_context.display_data.amount,
             });

UX_STEP_NOCB(ux_display_review_blind_signed_step,
              bnnn_paging,
             {
                 .title = BLIND_SIGNING,
                 .text = g_personal_msg,
             });

UX_STEP_NOCB(ux_display_review_blind_msg_signed_step,
             bnnn_paging,
             {
                 .title = BLIND_SIGNING,
                 .text = g_personal_msg,
             });

// FLOW to display transaction information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display amount
// #3 screen : display destination address
// #4 screen : approve button
// #5 screen : reject button
UX_FLOW(ux_display_transaction_flow,
        &ux_display_review_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_amount_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_bagl_display_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED ||
        (G_context.tx_type != TRANSFER_V2_TRANSACTION &&
         G_context.tx_type != TRANSFER_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_transaction_flow, NULL);
    return 0;
}

int ui_display_transaction() {
    return ui_bagl_display_transaction_choice();
}

UX_FLOW(ux_display_approve_transaction_flow,
        &ux_display_approve_review_step,
        &ux_display_amount_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_bagl_display_approve_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != APPROVE &&
            G_context.tx_type != APPROVE_V2)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_approve_transaction_flow, NULL);
    return 0;
}

int ui_display_approve_tx() {
    return ui_bagl_display_approve_transaction_choice();
}

UX_FLOW(ux_display_transaction_from_flow,
        &ux_display_review_step,
        &ux_display_sender_address_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_amount_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_transaction_from_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        ||(G_context.tx_type != TRANSFER_FROM_V2_TRANSACTION &&
            G_context.tx_type != TRANSFER_FROM_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_transaction_from_flow, NULL);
    return 0;
}

int ui_display_transaction_from() {
    return ui_display_transaction_from_choice();
}


UX_FLOW(ux_display_blind_signed_transaction_flow,
        &ux_display_review_step,
        &ux_display_review_blind_signed_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_blind_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_personal_msg, 0, sizeof(g_personal_msg));
    memcpy(g_personal_msg, BLIND_SIGN_TX_MSG, sizeof(BLIND_SIGN_TX_MSG));

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_blind_signed_transaction_flow, NULL);

    return 0;
}

int ui_display_blind_signing_transaction() {
    return ui_bagl_display_blind_transaction_bs_choice();
}

// FLOW to display personal msg information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display amount
// #3 screen : display destination address
// #4 screen : approve button
// #5 screen : reject button
UX_FLOW(ux_display_personal_msg_flow,
        &ux_display_review_step,
        &ux_display_personal_msg_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_personal_msg_choice() {
    if (G_context.req_type != CONFIRM_MESSAGE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_personal_msg, 0, sizeof(g_personal_msg));
    memcpy(g_personal_msg, SIGN_MAGIC, sizeof(SIGN_MAGIC) - 2);
    int msglen = utf8_strlen(G_context.personal_msg_info.raw_msg);

    char lengthStr[10];
    snprintf(lengthStr, sizeof(lengthStr), "%d", msglen);
    int totalLength = sizeof(SIGN_MAGIC) - 2 + strlen(lengthStr) + G_context.personal_msg_info.raw_msg_len + 1;
    memcpy(g_personal_msg + sizeof(SIGN_MAGIC) - 2, lengthStr, strlen(lengthStr));

    for (size_t i=0;i< G_context.personal_msg_info.raw_msg_len;i++) {
        if((sizeof(SIGN_MAGIC) - 2+strlen(lengthStr) + i) > 64) {
                break;
            }
       g_personal_msg[sizeof(SIGN_MAGIC) - 2+strlen(lengthStr) + i] = (char)(G_context.personal_msg_info.msg_info.personal_msg[i]);
    }
    g_personal_msg[totalLength-1] = '\0';

    g_validate_callback = &ui_action_validate_personal_msg;
    ux_flow_init(0, ux_display_personal_msg_flow, NULL);
    return 0;
}

int ui_display_personal_msg() {
    return ui_display_bagl_personal_msg_choice();
}


// FLOW to display oep4 transaction information:
// #1 screen : eye icon + "Review oep4 Transaction"
// #2 screen : display amount
// #3 screen : display destination address
// #4 screen : approve button
// #5 screen : reject button
UX_FLOW(ux_display_oep4_transaction_flow,
        &ux_display_review_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_decimals_step,
        &ux_display_amount_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_oep4_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != OEP4_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    uint8_t decimals = 0;

    decimals = get_oep4_token_decimals((uint8_t *)G_context.display_data.content);
    //decimals = get_oep4_token_decimals(G_context.tx_info.oep4_tx_info.payload.contract_addr);

    memset(g_content_two, 0, sizeof(g_content_two));
    if (decimals ==0) {
        memcpy(g_content_two,DECIMALS_UNKNOWN,sizeof(DECIMALS_UNKNOWN));
    }
    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_oep4_transaction_flow, NULL);
    return 0;
}

int ui_display_oep4_transaction() {
    return ui_bagl_display_oep4_transaction_choice();
}

UX_FLOW(ux_display_oep4_approve_flow,
        &ux_display_review_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_decimals_step,
        &ux_display_amount_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_oep4_approve_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != NEO_VM_OEP4_APPROVE &&
            G_context.tx_type != WASM_VM_OEP4_APPROVE)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    uint8_t decimals = 0;
    decimals = get_oep4_token_decimals((uint8_t *)G_context.display_data.content);

    //decimals = get_oep4_token_decimals(G_context.tx_info.oep4_tx_info.payload.contract_addr);

    memset(g_content_two, 0, sizeof(g_content_two));
    if (decimals == 0) {
        memcpy(g_content_two,DECIMALS_UNKNOWN,sizeof(DECIMALS_UNKNOWN));
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_oep4_approve_flow, NULL);
    return 0;
}

int ui_display_oep4_approve_tx() {
    return ui_bagl_display_oep4_approve_choice();
}

UX_FLOW(ux_display_oep4_transfer_from_flow,
        &ux_display_review_step,
        &ux_display_sender_address_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_decimals_step,
        &ux_display_amount_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_oep4_transfer_from_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != NEO_VM_OEP4_TRANSFER_FROM &&
            G_context.tx_type != WASM_VM_OEP4_TRANSFER_FROM)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    uint8_t decimals = 0;
    decimals = get_oep4_token_decimals((uint8_t *)G_context.display_data.content);

   // decimals = get_oep4_token_decimals(G_context.tx_info.oep4_from_tx_info.payload.contract_addr);

    memset(g_content_two, 0, sizeof(g_content_two));
    if (decimals !=0) {
       if (!format_u64(g_content_two, sizeof(g_content_two), decimals)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
        }
    } else {
        memcpy(g_content_two,DECIMALS_UNKNOWN,sizeof(DECIMALS_UNKNOWN));
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_oep4_transfer_from_flow, NULL);
    return 0;
}

int ui_display_oep4_transfer_from_tx() {
    return ui_display_oep4_transfer_from_transaction_choice();
}

//registerCandidate
UX_FLOW(ux_display_register_candidate_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_init_pos_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_register_candidate_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REGISTER_CANDIDATE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_register_candidate_transaction_flow, NULL);
    return 0;
}

int ui_display_register_candidate_tx() {
    return ui_bagl_display_register_candidate_tx_choice();
}


//withdraw
UX_FLOW(ux_display_withdraw_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_with_draw_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_withdraw_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;

    ux_flow_init(0, ux_display_withdraw_transaction_flow, NULL);
    return 0;
}

int ui_display_withdraw_tx() {
    return ui_bagl_display_withdraw_tx_choice();
}


//quitNode
UX_FLOW(ux_display_quit_node_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_quit_node_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != QUIT_NODE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_quit_node_transaction_flow, NULL);
    return 0;
}

int ui_display_quit_node_tx() {
    return ui_bagl_display_quit_node_tx_choice();
}


//addInitPos
UX_FLOW(ux_display_add_init_pos_transaction_flow,
        &ux_display_review_step,
        &ux_display_account_step,
        &ux_display_peer_pubkey_step,
        &ux_display_pos_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_add_init_pos_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != ADD_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_add_init_pos_transaction_flow, NULL);
    return 0;
}

int ui_display_add_init_pos_tx() {
    return ui_bagl_display_add_init_pos_tx_choice();
}


//reduceInitPos
UX_FLOW(ux_display_reduce_init_pos_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_pos_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_reduce_init_pos_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REDUCE_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_reduce_init_pos_transaction_flow, NULL);
    return 0;
}

int ui_display_reduce_init_pos_tx() {
    return ui_bagl_display_reduce_init_pos_tx_choice();
}


//changeMaxAuthorization
UX_FLOW(ux_display_change_max_authorization_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_max_authorize_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_change_max_authorization_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != CHANGE_MAX_AUTHORIZATION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_change_max_authorization_transaction_flow, NULL);

    return 0;
}

int ui_display_change_max_authorization_tx() {
    return ui_display_bagl_change_max_authorization_tx_choice();
}


//setFeePercentage
UX_FLOW(ux_display_set_fee_percentage_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_peer_cost_step,
        &ux_display_stake_cost_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_bagl_set_fee_percentage_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != SET_FEE_PERCENTAGE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_set_fee_percentage_transaction_flow, NULL);
    return 0;
}

int ui_display_set_fee_percentage_tx() {
    return ui_display_bagl_set_fee_percentage_tx_choice();
}


//authorizeForPeer
UX_FLOW(ux_display_authorize_for_peer_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_pos_list_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_bagl_authorize_for_peer_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_authorize_for_peer_transaction_flow, NULL);
    return 0;
}

int ui_display_authorize_for_peer_tx() {
    return ui_display_bagl_authorize_for_peer_tx_choice();
}


//unAuthorizeForPeer
UX_FLOW(ux_display_un_authorize_for_peer_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_pos_list_step,
        &ux_display_fee_step,
        &ux_display_signer_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_un_authorize_for_peer_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != UN_AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_un_authorize_for_peer_transaction_flow, NULL);
    return 0;
}

int ui_display_un_authorize_for_peer_tx() {
    return ui_display_bagl_un_authorize_for_peer_tx_choice();
}

//withdrawFee
UX_FLOW(ux_display_withdraw_fee_transaction_flow,
        &ux_display_review_step,
        &ux_display_account_step,
        &ux_display_signer_address_step,
        &ux_display_fee_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_withdraw_fee_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW_FEE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_govern_transaction;
    ux_flow_init(0, ux_display_withdraw_fee_transaction_flow, NULL);
    return 0;
}

int ui_display_withdraw_fee_tx() {
    return ui_display_bagl_withdraw_fee_tx_choice();
}


//optimize
static void create_transaction_flow(void) {
     uint8_t index = 0;
     if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
         return;
     }
     ux_display_tx_flow[index++] = &ux_display_review_step;

    if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
         G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
         G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
         G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
        ux_display_tx_flow[index++] = &ux_display_sender_address_step;
    }
     if(G_context.tx_type == TRANSFER_V2_TRANSACTION ||
         G_context.tx_type == TRANSFER_TRANSACTION ||
         G_context.tx_type == APPROVE ||
         G_context.tx_type == APPROVE_V2 ||
         G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
         G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
         G_context.tx_type == NEO_VM_OEP4_APPROVE ||
         G_context.tx_type == WASM_VM_OEP4_APPROVE ||
         G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
         G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
         ux_display_tx_flow[index++] = &ux_display_from_address_step;
         ux_display_tx_flow[index++] = &ux_display_to_address_step;
         ux_display_tx_flow[index++] = &ux_display_amount_step;
     }
     if(G_context.tx_type == OEP4_TRANSACTION ||
         G_context.tx_type == NEO_VM_OEP4_APPROVE ||
         G_context.tx_type == WASM_VM_OEP4_APPROVE ||
         G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
         G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
             uint8_t decimals = 0;
             decimals = get_oep4_token_decimals((uint8_t *)G_context.display_data.content);
            memset(g_content_two, 0, sizeof(g_content_two));
          if (decimals ==0) {
             memcpy(g_content_two,DECIMALS_UNKNOWN,sizeof(DECIMALS_UNKNOWN));
             ux_display_tx_flow[index++] = &ux_display_decimals_step;
          }
     }
     if (G_context.tx_type == REGISTER_CANDIDATE ||
         G_context.tx_type == WITHDRAW ||
         G_context.tx_type == QUIT_NODE ||
         G_context.tx_type == ADD_INIT_POS ||
         G_context.tx_type == REDUCE_INIT_POS ||
         G_context.tx_type == CHANGE_MAX_AUTHORIZATION ||
         G_context.tx_type == SET_FEE_PERCENTAGE ||
         G_context.tx_type == AUTHORIZE_FOR_PEER ||
         G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
        ux_display_tx_flow[index++] = &ux_display_peer_pubkey_step;
        ux_display_tx_flow[index++] = &ux_display_address_step;
     }
     if(G_context.tx_type == REGISTER_CANDIDATE) {
         ux_display_tx_flow[index++] = &ux_display_init_pos_step;
     }
     if(G_context.tx_type == WITHDRAW) {
         ux_display_tx_flow[index++] = &ux_display_with_draw_step;
     }
     if(G_context.tx_type == ADD_INIT_POS ||
         G_context.tx_type ==  REDUCE_INIT_POS) {
         ux_display_tx_flow[index++] = &ux_display_pos_step;
     }
     if(G_context.tx_type == CHANGE_MAX_AUTHORIZATION) {
        ux_display_tx_flow[index++] = &ux_display_max_authorize_step;
     }
     if(G_context.tx_type == SET_FEE_PERCENTAGE) {
         ux_display_tx_flow[index++] = &ux_display_peer_cost_step;
         ux_display_tx_flow[index++] = &ux_display_stake_cost_step;
     }
     if(G_context.tx_type == AUTHORIZE_FOR_PEER ||
        G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
         ux_display_tx_flow[index++] = &ux_display_pos_list_step;
     }
     if(G_context.tx_type == WITHDRAW_FEE) {
         ux_display_tx_flow[index++] = &ux_display_account_step;
     }

     ux_display_tx_flow[index++] = &ux_display_fee_step;
     ux_display_tx_flow[index++] = &ux_display_signer_address_step;
     ux_display_tx_flow[index++] = &ux_display_new_approve_step;
     ux_display_tx_flow[index++] = &ux_display_new_reject_step;
     ux_display_tx_flow[index++] = FLOW_END_STEP;
}

int ui_display_tx() {
     create_transaction_flow();
     ux_flow_init(0, ux_display_tx_flow, NULL);
     return 0;
}

#endif
