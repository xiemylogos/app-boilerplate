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

#ifdef HAVE_NBGL

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "glyphs.h"
#include "os_io_seproxyhal.h"
#include "nbgl_use_case.h"
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

//registerCandidate
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void register_candidate_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_register_candidate_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_register_candidate_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_REGISTER_CANDIDATE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_register_candidate_tx() {
    return ui_display_register_candidate_tx_bs_choice();
}

//withdraw
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void withdraw_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_withdraw_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_WITHDRAW || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_tx() {
    return ui_display_withdraw_tx_bs_choice();
}


//quitNode
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void quit_node_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_quit_node_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_quit_node_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_QUIT_NODE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_quit_node_tx() {
    return ui_display_quit_node_tx_bs_choice();
}

//addInitPos
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void add_init_pos_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_add_init_pos_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_add_init_pos_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_ADD_INIT_POS || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_add_init_pos_tx() {
    return ui_display_add_init_pos_tx_bs_choice();
}


//reduceInitPos
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void reduce_init_pos_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_reduce_init_pos_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_reduce_init_pos_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_REDUCE_INIT_POS || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_reduce_init_pos_tx() {
    return ui_display_reduce_init_pos_tx_bs_choice();
}

//changeMaxAuthorization
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void change_max_authorization_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_change_max_authorization_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_change_max_authorization_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_CHANGE_MAX_AUTHORIZATION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_change_max_authorization_tx() {
    return change_max_authorization_tx_review_choice();
}

//setFeePercentage
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void set_fee_percentage_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_set_fee_percentage_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_set_fee_percentage_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_SET_FEE_PERCENTAGE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_change_set_fee_percentage_tx() {
    return ui_display_set_fee_percentage_tx_bs_choice();
}

//authorizeForPeer
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void authorize_for_peer_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_authorize_for_peer_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_authorize_for_peer_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_AUTHORIZE_FOR_PEER || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_change_authorize_for_peer_tx() {
    return ui_display_authorize_for_peer_tx_bs_choice();
}


//unAuthorizeForPeer
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void un_authorize_for_peer_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_un_authorize_for_peer_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_un_authorize_for_peer_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_UN_AUTHORIZE_FOR_PEER || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_un_authorize_for_peer_tx() {
    return ui_display_un_authorize_for_peer_tx_bs_choice();
}


//withdrawOng
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void withdraw_ong_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_withdraw_ong_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_ong_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_WITHDRAW_ONG || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_ong_peer_tx() {
    return ui_display_withdraw_ong_tx_bs_choice();
}

//withdrawOng
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void withdraw_ong_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_withdraw_ong_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_ong_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_WITHDRAW_ONG || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_ong_tx() {
    return ui_display_withdraw_ong_tx_bs_choice();
}

//withdrawFee
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void withdraw_fee_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_withdraw_fee_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_fee_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_WITHDRAW_FEE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_fee_tx() {
    return ui_display_withdraw_fee_tx_bs_choice();
}

#endif