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
#include "../address.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../menu.h"
#include "../utils.h"
#include "../uint128.h"
#include "../transaction/utils.h"

#define BLIND_SIGN_TX "Blind signing Transaction"
#define BLIND_SIGN_TX_MSG "Accept risk and send"

static action_validate_cb g_validate_callback;
static char g_amount[41];
static char g_address[40];
static char g_fromAddr[40];
static char g_sender[40];

static char g_peerPubkey[66];
static char g_posList[60];
static char g_peerCost[30];
static char g_stakeCost[30];
static char g_maxAuthorize[30];
static char g_pos[20];
static char g_withdrawList[40];
static char g_initPost[30];
static char g_ontId[30];
static char g_keyNo[20];
static char g_decimals[30];
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

static void ui_action_validate_approve_transaction(bool choice) {
    validate_approve_transaction(choice);
    ui_menu_main();
}

// Validate/Invalidate person msg and go back to home
static void ui_action_validate_person_msg(bool choice) {
    validate_person_msg(choice);
    ui_menu_main();
}

// Validate/Invalidate oep4 transaction and go back to home
static void ui_action_validate_oep4_transaction(bool choice) {
    validate_oep4_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_register_candidate_transaction(bool choice) {
    validate_register_candidate_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_withdraw_transaction(bool choice) {
    validate_withdraw_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_quit_node_transaction(bool choice) {
    validate_quit_node_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_add_init_pos_transaction(bool choice) {
    validate_add_init_pos_transaction(choice);
    ui_menu_main();
}


static void ui_action_validate_reduce_init_pos_transaction(bool choice) {
    validate_reduce_init_pos_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_change_max_authorization_transaction(bool choice) {
    validate_change_max_authorization_transaction(choice);
    ui_menu_main();
}


static void ui_action_validate_set_fee_percentage_transaction(bool choice) {
    validate_set_fee_percentage_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_authorize_for_peer_transaction(bool choice) {
    validate_authorize_for_peer_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_un_authorize_for_peer_transaction(bool choice) {
    validate_un_authorize_for_peer_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_withdraw_ong_transaction(bool choice) {
    validate_withdraw_ong_transaction(choice);
    ui_menu_main();
}

static void ui_action_validate_withdraw_fee_transaction(bool choice) {
    validate_withdraw_fee_transaction(choice);
    ui_menu_main();
}

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_addr_step, pn, {&C_icon_eye, "Confirm Address"});
// Step with title/text for address
UX_STEP_NOCB(ux_display_from_address_step,
             bnnn_paging,
             {
                 .title = "fromAddr",
                 .text = g_fromAddr,
             });

UX_STEP_NOCB(ux_display_to_address_step,
             bnnn_paging,
             {
                 .title = "toAddr",
                 .text = g_address,
             });

UX_STEP_NOCB(ux_display_sender_address_step,
             bnnn_paging,
             {
                 .title = "sender",
                 .text = g_sender,
             });

UX_STEP_NOCB(ux_display_address_step,
             bnnn_paging,
             {
                 .title = "Address",
                 .text = g_address,
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

UX_STEP_NOCB(ux_display_account_step,
             bnnn_paging,
             {
                 .title = "account",
                 .text = g_address,
             });
UX_STEP_NOCB(ux_display_peer_pubkey_step,
             bnnn_paging,
             {
                 .title = "peerPubkey",
                 .text = g_peerPubkey,
             });

UX_STEP_NOCB(ux_display_pos_list_step,
             bnnn_paging,
             {
                 .title = "posList",
                 .text = g_posList,
             });

UX_STEP_NOCB(ux_display_peer_cost_step,
             bnnn_paging,
             {
                 .title = "peerCost",
                 .text = g_peerCost,
             });

UX_STEP_NOCB(ux_display_stake_cost_step,
             bnnn_paging,
             {
                 .title = "stakeCost",
                 .text = g_stakeCost,
             });

UX_STEP_NOCB(ux_display_max_authorize_step,
             bnnn_paging,
             {
                 .title = "maxAuthorize",
                 .text = g_maxAuthorize,
             });

UX_STEP_NOCB(ux_display_pos_step,
             bnnn_paging,
             {
                 .title = "pos",
                 .text = g_pos,
             });

UX_STEP_NOCB(ux_display_with_draw_step,
             bnnn_paging,
             {
                 .title = "withdrawList",
                 .text = g_withdrawList,
             });

UX_STEP_NOCB(ux_display_init_pos_step,
             bnnn_paging,
             {
                 .title = "initPos",
                 .text = g_initPost,
             });

UX_STEP_NOCB(ux_display_ont_id_step,
             bnnn_paging,
             {
                 .title = "ontId",
                 .text = g_ontId,
             });

UX_STEP_NOCB(ux_display_key_no_step,
             bnnn_paging,
             {
                 .title = "keyNo",
                 .text = g_keyNo,
             });

UX_STEP_NOCB(ux_display_person_msg_step,
             bnnn_paging,
             {
                 .title = "person msg",
                 .text = g_address,
             });

UX_STEP_NOCB(ux_display_decimals_step,
             bnnn_paging,
             {
                 .title = "decimals",
                 .text = g_decimals,
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
    uint8_t address[ADDRESS_LEN] = {0};
    if (!address_from_pubkey(G_context.pk_info.raw_public_key, address, sizeof(address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    if (script_hash_to_address(g_address,sizeof(g_address),address) == -1) {
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
                 .title = "Amount",
                 .text = g_amount,
             });

UX_STEP_NOCB(ux_display_review_blind_signed_step,
              bnnn_paging,
             {
                 .title = "blind siging",
                 .text = g_peerPubkey,
             });

UX_STEP_NOCB(ux_display_review_blind_msg_signed_step,
             bnnn_paging,
             {
                 .title = "blind siging",
                 .text = g_peerPubkey,
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
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_bagl_display_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED ||
        (G_context.tx_type != TRANSFER_V2_TRANSACTION &&
         G_context.tx_type != TRANSFER_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_amount, 0, sizeof(g_amount));

    if (memcmp(G_context.tx_info.tx_info.payload.contract_addr, ONT_ADDR, 20) == 0) {
        uint decimals = 0;
        if(G_context.tx_type == TRANSFER_V2_TRANSACTION) {
              decimals = 9;
        }
        if (G_context.tx_info.tx_info.payload.value_len >= 81) {
                format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.tx_info.payload.value[0],
                                     decimals);
        } else {
            if (G_context.tx_info.tx_info.payload.value_len <= 8) {
                    format_fpu64_trimmed(g_amount,
                                         sizeof(g_amount),
                                         G_context.tx_info.tx_info.payload.value[0],
                                         decimals);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.tx_info.payload.value[0];
                tostring128(&values, 10, amount, sizeof(amount));
                process_precision(amount, decimals, g_amount, sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
        }
    } else if (memcmp(G_context.tx_info.tx_info.payload.contract_addr, ONG_ADDR, 20) == 0) {
        uint decimals = 9;
        if(G_context.tx_type == TRANSFER_V2_TRANSACTION) {
              decimals = 18;
        }
        if (G_context.tx_info.tx_info.payload.value_len >= 81) {
                format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.tx_info.payload.value[0],
                                     decimals);
        } else {
            if (G_context.tx_info.tx_info.payload.value_len <= 8) {
                    format_fpu64_trimmed(g_amount,
                                         sizeof(g_amount),
                                         G_context.tx_info.tx_info.payload.value[0],
                                         decimals);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.tx_info.payload.value[0];
                tostring128(&values, 10, amount, sizeof(amount));
                process_precision(amount, decimals, g_amount, sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
        }
    }

    PRINTF("Amount: %s\n", g_amount);

    memset(g_address, 0, sizeof(g_address));

    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.tx_info.payload.to) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_fromAddr, 0, sizeof(g_fromAddr));

    if (script_hash_to_address(g_fromAddr,
                                   sizeof(g_fromAddr),
                                   G_context.tx_info.tx_info.payload.from) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_transaction_flow, NULL);
    return 0;
}

int ui_display_transaction() {
    return ui_bagl_display_transaction_bs_choice();
}



UX_FLOW(ux_display_approve_transaction_flow,
        &ux_display_approve_review_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_amount_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_bagl_display_approve_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != APPROVE &&
            G_context.tx_type != APPROVE_V2)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_amount, 0, sizeof(g_amount));
    if (memcmp(G_context.tx_info.tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
         uint decimals = 0;
        if(G_context.tx_type == APPROVE_V2) {
              decimals = 9;
        }
         if (G_context.tx_info.tx_info.payload.value_len >= 81) {
             format_fpu64_trimmed(g_amount,sizeof(g_amount),G_context.tx_info.tx_info.payload.value[0],decimals);
         } else {
            if (G_context.tx_info.tx_info.payload.value_len <= 8) {
                format_fpu64_trimmed(g_amount,sizeof(g_amount),G_context.tx_info.tx_info.payload.value[0],decimals);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.tx_info.payload.value[0];
                tostring128(&values,10,amount,sizeof(amount));
                process_precision(amount,decimals,g_amount,sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
         }
    } else if (memcmp(G_context.tx_info.tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
         uint decimals = 9;
        if(G_context.tx_type == APPROVE_V2) {
              decimals = 18;
        }
        if (G_context.tx_info.tx_info.payload.value_len >= 81) {
           format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.tx_info.payload.value[0],
                                     decimals);
        } else {
            if (G_context.tx_info.tx_info.payload.value_len <= 8) {
                format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.tx_info.payload.value[0],
                                     decimals);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.tx_info.payload.value[0];
                tostring128(&values, 10, amount, sizeof(amount));
                process_precision(amount,decimals, g_amount, sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
        }
    }

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.tx_info.payload.to) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_fromAddr, 0, sizeof(g_fromAddr));

    if (script_hash_to_address(g_fromAddr,
                                   sizeof(g_fromAddr),
                                   G_context.tx_info.tx_info.payload.from) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_approve_transaction;
    ux_flow_init(0, ux_display_approve_transaction_flow, NULL);
    return 0;
}

int ui_display_approve_tx() {
    return ui_bagl_display_approve_transaction_bs_choice();
}

UX_FLOW(ux_display_transaction_from_flow,
        &ux_display_review_step,
        &ux_display_sender_address_step,
        &ux_display_from_address_step,
        &ux_display_to_address_step,
        &ux_display_amount_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_transaction_from_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        ||(G_context.tx_type != TRANSFER_FROM_V2_TRANSACTION &&
            G_context.tx_type != TRANSFER_FROM_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_amount, 0, sizeof(g_amount));

    if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr, ONT_ADDR, 20) == 0) {
        uint decimals = 0;
        if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION) {
              decimals = 9;
        }
        if (G_context.tx_info.from_tx_info.payload.value_len >= 81) {
                format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.from_tx_info.payload.value[0],
                                     decimals);
        } else {
            if (G_context.tx_info.from_tx_info.payload.value_len <= 8) {
                    format_fpu64_trimmed(g_amount,
                                         sizeof(g_amount),
                                         G_context.tx_info.from_tx_info.payload.value[0],
                                         decimals);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.from_tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.from_tx_info.payload.value[0];
                tostring128(&values, 10, amount, sizeof(amount));
                process_precision(amount, decimals, g_amount, sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
        }
    } else if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr, ONG_ADDR, 20) == 0) {
         uint decimals = 9;
        if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION) {
              decimals = 18;
        }
        if (G_context.tx_info.from_tx_info.payload.value_len >= 81) {
                format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.from_tx_info.payload.value[0],
                                     18);
        } else {
            if (G_context.tx_info.from_tx_info.payload.value_len <= 8) {
                    format_fpu64_trimmed(g_amount,
                                         sizeof(g_amount),
                                         G_context.tx_info.from_tx_info.payload.value[0],
                                         18);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.from_tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.from_tx_info.payload.value[0];
                tostring128(&values, 10, amount, sizeof(amount));
                process_precision(amount, 18, g_amount, sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
        }
    }

    memset(g_sender, 0, sizeof(g_sender));
    if (script_hash_to_address(g_sender,sizeof(g_sender),G_context.tx_info.from_tx_info.payload.sender) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.from_tx_info.payload.to) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_fromAddr, 0, sizeof(g_fromAddr));

    if (script_hash_to_address(g_fromAddr,
                                   sizeof(g_fromAddr),
                                   G_context.tx_info.from_tx_info.payload.from) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_transaction;
    ux_flow_init(0, ux_display_transaction_from_flow, NULL);
    return 0;
}

int ui_display_transaction_from() {
    return ui_display_transaction_from_bs_choice();
}


UX_FLOW(ux_display_blind_signed_transaction_flow,
        &ux_display_review_step,
        &ux_display_review_blind_signed_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_blind_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    g_validate_callback = &ui_action_validate_transaction;
        memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
        memcpy(g_peerPubkey, BLIND_SIGN_TX_MSG, sizeof(BLIND_SIGN_TX_MSG));
        ux_flow_init(0, ux_display_blind_signed_transaction_flow, NULL);

    return 0;
}

int ui_display_blind_signed_transaction() {
    return ui_bagl_display_blind_transaction_bs_choice();
}

// FLOW to display person msg information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display amount
// #3 screen : display destination address
// #4 screen : approve button
// #5 screen : reject button
UX_FLOW(ux_display_person_msg_flow,
        &ux_display_review_step,
        &ux_display_person_msg_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_person_msg_bs_choice() {
    if (G_context.req_type != CONFIRM_MESSAGE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_address, 0, sizeof(g_address));
    memcpy(g_address,
           G_context.person_msg_info.msg_info.person_msg,
           sizeof(g_address)-1);
    g_address[G_context.person_msg_info.raw_msg_len + 1] = '\0';
    g_validate_callback = &ui_action_validate_person_msg;
    ux_flow_init(0, ux_display_person_msg_flow, NULL);
    return 0;
}

int ui_display_person_msg() {
    return ui_display_bagl_person_msg_bs_choice();
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
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_oep4_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != OEP4_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_amount, 0, sizeof(g_amount));
    uint8_t decimals = 1;
    bool know_decimals = false;
   if (memcmp(G_context.tx_info.oep4_tx_info.payload.contract_addr,WTK_ADDR,20) == 0) {
       decimals = 9;
       know_decimals = true;
    } else if (memcmp(G_context.tx_info.oep4_tx_info.payload.contract_addr,MYT_ADDR,20) == 0 ) {
        decimals = 18;
        know_decimals = true;
    } else if (memcmp(G_context.tx_info.oep4_tx_info.payload.contract_addr,WING_ADDR,20) == 0 ) {
        decimals = 9;
        know_decimals = true;
    }
    memset(g_decimals, 0, sizeof(g_decimals));
    if (know_decimals) {
       if (!format_u64(g_decimals, sizeof(g_decimals), decimals)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
        }
    } else {
        memcpy(g_decimals,"decimals unknown",sizeof("decimals unknown"));
    }
    memset(g_amount, 0, sizeof(g_amount));
    if (G_context.tx_info.oep4_tx_info.payload.value_len >= 81) {
           format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.oep4_tx_info.payload.value[0],
                                     decimals);
        } else {
            if (G_context.tx_info.oep4_tx_info.payload.value_len <= 8) {
                format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.oep4_tx_info.payload.value[0],
                                     decimals);
            } else {
                char amount[41];
                uint128_t values;
                values.elements[0] = G_context.tx_info.oep4_tx_info.payload.value[1];
                values.elements[1] = G_context.tx_info.oep4_tx_info.payload.value[0];
                tostring128(&values, 10, amount, sizeof(amount));
                process_precision(amount, decimals, g_amount, sizeof(g_amount));
                explicit_bzero(&amount, sizeof(amount));
                clear128(&values);
            }
        }
    PRINTF("Amount: %s\n", g_amount);

    memset(g_fromAddr, 0, sizeof(g_fromAddr));

    if (script_hash_to_address(g_fromAddr,
                                   sizeof(g_fromAddr),
                                   G_context.tx_info.oep4_tx_info.payload.from) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_address, 0, sizeof(g_address));

    if (script_hash_to_address(g_address,
                               sizeof(g_address),
                               G_context.tx_info.oep4_tx_info.payload.to) == -1){
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_oep4_transaction;
    ux_flow_init(0, ux_display_oep4_transaction_flow, NULL);
    return 0;
}

int ui_display_oep4_transaction() {
    return ui_bagl_display_oep4_transaction_bs_choice();
}


//registerCandidate
UX_FLOW(ux_display_register_candidate_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_init_pos_step,
        &ux_display_ont_id_step,
        &ux_display_key_no_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_register_candidate_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REGISTER_CANDIDATE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_address, 0, sizeof(g_address));

    if (script_hash_to_address(g_address,
                               sizeof(g_address),
                               G_context.tx_info.register_candidate_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.register_candidate_tx_info.peer_pubkey, 66);

    memset(g_initPost, 0, sizeof(g_initPost));
    if (!format_u64(g_initPost,
                        sizeof(g_initPost),
                        G_context.tx_info.register_candidate_tx_info.init_pos)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }

    memset(g_ontId, 0, sizeof(g_ontId));
    memcpy(g_ontId,
           G_context.tx_info.register_candidate_tx_info.ont_id,
           G_context.tx_info.register_candidate_tx_info.ont_id_len);

    memset(g_keyNo, 0, sizeof(g_keyNo));
    if (!format_u64(g_keyNo,
                        sizeof(g_keyNo),
                        G_context.tx_info.register_candidate_tx_info.key_no)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }

    g_validate_callback = &ui_action_validate_register_candidate_transaction;
    ux_flow_init(0, ux_display_register_candidate_transaction_flow, NULL);
    return 0;
}

int ui_display_register_candidate_tx() {
    return ui_bagl_display_register_candidate_tx_bs_choice();
}


//withdraw
UX_FLOW(ux_display_withdraw_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_with_draw_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_withdraw_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.withdraw_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.withdraw_tx_info.peer_pubkey, 66);

    memset(g_withdrawList, 0, sizeof(g_withdrawList));
    if (G_context.tx_info.withdraw_tx_info.withdraw_list_len < 81) {
            uint64_t value = getValueByLen(G_context.tx_info.withdraw_tx_info.withdraw_list,
                                           G_context.tx_info.withdraw_tx_info.withdraw_list_len);
            if (!format_u64(g_withdrawList, sizeof(g_withdrawList), value)) {
                return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
    } else {
            if (!format_u64(g_withdrawList,
                            sizeof(g_withdrawList),
                            G_context.tx_info.withdraw_tx_info.withdraw_list_len - 80)) {
                return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
    }

    g_validate_callback = &ui_action_validate_withdraw_transaction;

    ux_flow_init(0, ux_display_withdraw_transaction_flow, NULL);
    return 0;
}

int ui_display_withdraw_tx() {
    return ui_bagl_display_withdraw_tx_bs_choice();
}


//quitNode
UX_FLOW(ux_display_quit_node_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_quit_node_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != QUIT_NODE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.quit_node_tx_info.peer_pubkey, 66);

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.quit_node_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }

    g_validate_callback = &ui_action_validate_quit_node_transaction;
    ux_flow_init(0, ux_display_quit_node_transaction_flow, NULL);
    return 0;
}

int ui_display_quit_node_tx() {
    return ui_bagl_display_quit_node_tx_bs_choice();
}


//addInitPos
UX_FLOW(ux_display_add_init_pos_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_pos_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_add_init_pos_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != ADD_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.add_init_pos_tx_info.peer_pubkey, 66);

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.add_init_pos_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_pos, 0, sizeof(g_pos));
    if (!format_u64(g_pos, sizeof(g_pos), G_context.tx_info.add_init_pos_tx_info.pos)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }


    g_validate_callback = &ui_action_validate_add_init_pos_transaction;
    ux_flow_init(0, ux_display_add_init_pos_transaction_flow, NULL);
    return 0;
}

int ui_display_add_init_pos_tx() {
    return ui_bagl_display_add_init_pos_tx_bs_choice();
}


//reduceInitPos
UX_FLOW(ux_display_reduce_init_pos_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_pos_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_bagl_display_reduce_init_pos_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REDUCE_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.reduce_init_pos_tx_info.peer_pubkey, 66);

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.reduce_init_pos_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_pos, 0, sizeof(g_pos));
    if (!format_u64(g_pos, sizeof(g_pos), G_context.tx_info.reduce_init_pos_tx_info.pos)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    g_validate_callback = &ui_action_validate_reduce_init_pos_transaction;
    ux_flow_init(0, ux_display_reduce_init_pos_transaction_flow, NULL);
    return 0;
}

int ui_display_reduce_init_pos_tx() {
    return ui_bagl_display_reduce_init_pos_tx_bs_choice();
}


//changeMaxAuthorization
UX_FLOW(ux_display_change_max_authorization_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_max_authorize_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_change_max_authorization_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != CHANGE_MAX_AUTHORIZATION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.change_max_authorization_tx_info.peer_pubkey, 66);

    memset(g_address, 0, sizeof(g_address));

    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.change_max_authorization_tx_info.account) ==
            -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_maxAuthorize, 0, sizeof(g_maxAuthorize));
    if (!format_u64(g_maxAuthorize,
                        sizeof(g_maxAuthorize),
                        G_context.tx_info.change_max_authorization_tx_info.max_authorize)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    g_validate_callback = &ui_action_validate_change_max_authorization_transaction;

    ux_flow_init(0, ux_display_change_max_authorization_transaction_flow, NULL);

    return 0;
}

int ui_display_change_max_authorization_tx() {
    return ui_display_bagl_change_max_authorization_tx_bs_choice();
}


//setFeePercentage
UX_FLOW(ux_display_set_fee_percentage_transaction_flow,
        &ux_display_review_step,
        &ux_display_peer_pubkey_step,
        &ux_display_address_step,
        &ux_display_peer_cost_step,
        &ux_display_stake_cost_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_bagl_set_fee_percentage_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != SET_FEE_PERCENTAGE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.set_fee_percentage_tx_info.peer_pubkey, 66);

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.set_fee_percentage_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_peerCost, 0, sizeof(g_peerCost));
    if (!format_u64(g_peerCost,
                        sizeof(g_peerCost),
                        G_context.tx_info.set_fee_percentage_tx_info.peer_cost)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }

    memset(g_stakeCost, 0, sizeof(g_stakeCost));
    if (!format_u64(g_stakeCost,
                    sizeof(g_stakeCost),
                    G_context.tx_info.set_fee_percentage_tx_info.stake_cost)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    g_validate_callback = &ui_action_validate_set_fee_percentage_transaction;
    ux_flow_init(0, ux_display_set_fee_percentage_transaction_flow, NULL);
    return 0;
}

int ui_display_set_fee_percentage_tx() {
    return ui_display_bagl_set_fee_percentage_tx_bs_choice();
}


//authorizeForPeer
UX_FLOW(ux_display_authorize_for_peer_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_pos_list_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_bagl_authorize_for_peer_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.authorize_for_peer_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey, 66);

    memset(g_posList, 0, sizeof(g_posList));
    uint64_t value = getValueByLen(G_context.tx_info.authorize_for_peer_tx_info.pos_list,
                                       G_context.tx_info.authorize_for_peer_tx_info.pos_list_len);
    if (!format_u64(g_posList, sizeof(g_posList), value)) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    g_validate_callback = &ui_action_validate_authorize_for_peer_transaction;
    ux_flow_init(0, ux_display_authorize_for_peer_transaction_flow, NULL);
    return 0;
}

int ui_display_authorize_for_peer_tx() {
    return ui_display_bagl_authorize_for_peer_tx_bs_choice();
}


//unAuthorizeForPeer
UX_FLOW(ux_display_un_authorize_for_peer_transaction_flow,
        &ux_display_review_step,
        &ux_display_address_step,
        &ux_display_peer_pubkey_step,
        &ux_display_pos_list_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_un_authorize_for_peer_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != UN_AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.un_authorize_for_peer_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey, 66);

    memset(g_posList, 0, sizeof(g_posList));
    if (G_context.tx_info.un_authorize_for_peer_tx_info.pos_list_len < 81) {
            uint64_t value =
                getValueByLen(G_context.tx_info.un_authorize_for_peer_tx_info.pos_list,
                              G_context.tx_info.un_authorize_for_peer_tx_info.pos_list_len);
            if (!format_u64(g_posList, sizeof(g_posList), value)) {
                return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
    } else {
            if (!format_u64(g_posList,
                            sizeof(g_posList),
                            G_context.tx_info.un_authorize_for_peer_tx_info.pos_list_len - 80)) {
                return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
    }
    g_validate_callback = &ui_action_validate_un_authorize_for_peer_transaction;
    ux_flow_init(0, ux_display_un_authorize_for_peer_transaction_flow, NULL);
    return 0;
}

int ui_display_un_authorize_for_peer_tx() {
    return ui_display_bagl_un_authorize_for_peer_tx_bs_choice();
}


//withdrawOng
UX_FLOW(ux_display_withdraw_ong_transaction_flow,
        &ux_display_review_step,
        &ux_display_account_step,
        &ux_display_approve_step,
        &ux_display_reject_step);


int ui_display_bagl_withdraw_ong_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW_ONG) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                               sizeof(g_address),
                               G_context.tx_info.withdraw_fee_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    g_validate_callback = &ui_action_validate_withdraw_ong_transaction;
    ux_flow_init(0, ux_display_withdraw_ong_transaction_flow, NULL);
    return 0;
}

int ui_display_withdraw_ong_tx() {
    return ui_display_bagl_withdraw_ong_tx_bs_choice();
}


//withdrawFee
UX_FLOW(ux_display_withdraw_fee_transaction_flow,
        &ux_display_review_step,
        &ux_display_account_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_bagl_withdraw_fee_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW_FEE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_address, 0, sizeof(g_address));
    if (script_hash_to_address(g_address,
                                   sizeof(g_address),
                                   G_context.tx_info.withdraw_fee_tx_info.account) == -1) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    g_validate_callback = &ui_action_validate_withdraw_fee_transaction;

    ux_flow_init(0, ux_display_withdraw_fee_transaction_flow, NULL);
    return 0;
}

int ui_display_withdraw_fee_tx() {
    return ui_display_bagl_withdraw_fee_tx_bs_choice();
}


#endif
