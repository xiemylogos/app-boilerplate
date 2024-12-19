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

// Buffer where the transaction address string is written
static char g_addr[40];
static char g_peerPubkey[200];
static char g_posList[200];
static char g_peerCost[30];
static char g_stakeCost[30];
static char g_maxAuthorize[30];
static char g_pos[30];
static char g_withdrawList[200];
static char g_initPost[30];
static char g_ontId[100];
static char g_keyNo[30];

#define MAX_PAIRS        20

static nbgl_contentTagValue_t pairs[MAX_PAIRS];
static nbgl_contentTagValueList_t pairsList;

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

static uint8_t registerCandidateTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.register_candidate_tx_info.peer_pubkey,66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.register_candidate_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_initPost,0,sizeof(g_initPost));
    if (!format_u64(g_initPost,sizeof(g_initPost),G_context.tx_info.register_candidate_tx_info.init_pos)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "init_pos";
    pairs[nbPairs].value = g_initPost;
    nbPairs++;

    memset(g_ontId,0,sizeof(g_ontId));
    memcpy(g_ontId, G_context.tx_info.register_candidate_tx_info.ont_id, G_context.tx_info.register_candidate_tx_info.ont_id_len);
    pairs[nbPairs].item = "ontId";
    pairs[nbPairs].value = g_ontId;
    nbPairs++;

    memset(g_keyNo,0,sizeof(g_keyNo));
    if (!format_u64(g_keyNo,sizeof(g_keyNo),G_context.tx_info.register_candidate_tx_info.key_no)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "key_no";
    pairs[nbPairs].value = g_keyNo;
    nbPairs++;
    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_register_candidate_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REGISTER_CANDIDATE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = registerCandidateTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review registerCandidate transaction",
                           NULL,
                           "Sign registerCandidate transaction",
                           register_candidate_tx_review_choice);
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

static uint8_t withdrawTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    //account
    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.withdraw_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.withdraw_tx_info.peer_pubkey, G_context.tx_info.withdraw_tx_info.peer_pubkey_length*66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_withdrawList,0,sizeof(g_withdrawList));
    memcpy(g_withdrawList, G_context.tx_info.withdraw_tx_info.withdraw_list, G_context.tx_info.withdraw_tx_info.withdraw_list_length*4);
    pairs[nbPairs].item = "withdrawList";
    pairs[nbPairs].value = g_withdrawList;
    nbPairs++;
    return nbPairs;
}


// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = withdrawTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review withdraw transaction",
                           NULL,
                           "Sign withdraw transaction",
                           withdraw_tx_review_choice);

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

static uint8_t quitNodeTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.quit_node_tx_info.peer_pubkey,66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.quit_node_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_quit_node_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != QUIT_NODE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = quitNodeTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review quitNode transaction",
                           NULL,
                           "Sign quitNode transaction",
                           quit_node_tx_review_choice);

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

static uint8_t addInitPosTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.add_init_pos_tx_info.peer_pubkey,66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.add_init_pos_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_pos,0,sizeof(g_pos));
    if (!format_u64(g_pos,sizeof(g_pos),G_context.tx_info.add_init_pos_tx_info.pos)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "pos";
    pairs[nbPairs].value = g_pos;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_add_init_pos_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        | G_context.tx_type != ADD_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = addInitPosTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review addInitPos transaction",
                           NULL,
                           "Sign addInitPos transaction",
                           add_init_pos_tx_review_choice);
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

static uint8_t reduceInitPosTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.reduce_init_pos_tx_info.peer_pubkey,66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.reduce_init_pos_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_pos,0,sizeof(g_pos));
    if (!format_u64(g_pos,sizeof(g_pos),G_context.tx_info.reduce_init_pos_tx_info.pos)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "pos";
    pairs[nbPairs].value = g_pos;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_reduce_init_pos_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REDUCE_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = reduceInitPosTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review reduceInitPos transaction",
                           NULL,
                           "Sign reduceInitPos transaction",
                           reduce_init_pos_tx_review_choice);

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

static uint8_t changeMaxAuthorizationTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.change_max_authorization_tx_info.peer_pubkey,66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.change_max_authorization_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_maxAuthorize,0,sizeof(g_maxAuthorize));
    if (!format_u64(g_maxAuthorize,sizeof(g_maxAuthorize),G_context.tx_info.change_max_authorization_tx_info.max_authorize)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "maxAuthorize";
    pairs[nbPairs].value = g_maxAuthorize;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_change_max_authorization_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != CHANGE_MAX_AUTHORIZATION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = changeMaxAuthorizationTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review changeMaxAuthorization transaction",
                           NULL,
                           "Sign changeMaxAuthorization transaction",
                           change_max_authorization_tx_review_choice);



    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_change_max_authorization_tx() {
    return ui_display_change_max_authorization_tx_bs_choice();
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

static uint8_t setFeePercentageTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    //account
    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.set_fee_percentage_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.set_fee_percentage_tx_info.peer_pubkey,66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_peerCost,0,sizeof(g_peerCost));
    if (!format_u64(g_peerCost,sizeof(g_peerCost),G_context.tx_info.set_fee_percentage_tx_info.peer_cost)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "peerCost";
    pairs[nbPairs].value = g_peerCost;
    nbPairs++;

    memset(g_stakeCost,0,sizeof(g_stakeCost));
    if (!format_u64(g_stakeCost,sizeof(g_stakeCost),G_context.tx_info.set_fee_percentage_tx_info.stake_cost)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "stakeCost";
    pairs[nbPairs].value = g_stakeCost;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_set_fee_percentage_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != SET_FEE_PERCENTAGE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = setFeePercentageTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review setFeePercentage transaction",
                           NULL,
                           "Sign setFeePercentage transaction",
                           set_fee_percentage_tx_review_choice);


    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_set_fee_percentage_tx() {
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

static uint8_t setAuthorizeForPeerTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    //account
    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.un_authorize_for_peer_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;
    /*
    memset(g_maxAuthorize,0,sizeof(g_maxAuthorize));
    if (!format_u64(g_maxAuthorize,sizeof(g_maxAuthorize),G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey_length)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "peer_pubkey_length";
    pairs[nbPairs].value = g_maxAuthorize;
    nbPairs++;

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey, G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey_length*66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_stakeCost,0,sizeof(g_stakeCost));
    if (!format_u64(g_stakeCost,sizeof(g_stakeCost),G_context.tx_info.authorize_for_peer_tx_info.pos_list_length)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "posListLength";
    pairs[nbPairs].value = g_stakeCost;
    nbPairs++;

    memset(g_posList,0,sizeof(g_posList));
    memcpy(g_posList, G_context.tx_info.authorize_for_peer_tx_info.pos_list, G_context.tx_info.authorize_for_peer_tx_info.pos_list_length*1);
    pairs[nbPairs].item = "posList";
    pairs[nbPairs].value = g_posList;
    nbPairs++;
*/
    return nbPairs;
}


// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_authorize_for_peer_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = setAuthorizeForPeerTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review AuthorizeForPeer transaction",
                           NULL,
                           "Sign AuthorizeForPeer transaction",
                           authorize_for_peer_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_authorize_for_peer_tx() {
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

static uint8_t setunAuthorizeForPeerTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    //account
    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.un_authorize_for_peer_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;

    memset(g_maxAuthorize,0,sizeof(g_maxAuthorize));
    if (!format_u64(g_maxAuthorize,sizeof(g_maxAuthorize),G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey_length)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "peer_pubkey_length";
    pairs[nbPairs].value = g_maxAuthorize;
    nbPairs++;

    memset(g_peerPubkey, 0, sizeof(g_peerPubkey));
    memcpy(g_peerPubkey, G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey, G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey_length*66);
    pairs[nbPairs].item = "peerPubkey";
    pairs[nbPairs].value = g_peerPubkey;
    nbPairs++;

    memset(g_stakeCost,0,sizeof(g_stakeCost));
    if (!format_u64(g_stakeCost,sizeof(g_stakeCost),G_context.tx_info.un_authorize_for_peer_tx_info.pos_list_length)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "posListLength";
    pairs[nbPairs].value = g_stakeCost;
    nbPairs++;

    memset(g_posList,0,sizeof(g_posList));
    memcpy(g_posList, G_context.tx_info.un_authorize_for_peer_tx_info.pos_list, G_context.tx_info.un_authorize_for_peer_tx_info.pos_list_length*1);
    pairs[nbPairs].item = "posList";
    pairs[nbPairs].value = g_posList;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_un_authorize_for_peer_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        | G_context.tx_type != UN_AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = setunAuthorizeForPeerTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review unAuthorizeForPeer transaction",
                           NULL,
                           "Sign unAuthorizeForPeer transaction",
                           un_authorize_for_peer_tx_review_choice);

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

static uint8_t setwithdrawOngTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    //account
    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.withdraw_fee_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;
    return nbPairs;
}
// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_ong_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW_ONG) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = setwithdrawOngTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review withdrawOng transaction",
                           NULL,
                           "Sign withdrawOng transaction",
                           withdraw_ong_tx_review_choice);

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

static uint8_t setwithdrawFeeTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    //account
    memset(g_addr, 0, sizeof(g_addr));
    if (script_hash_to_address(g_addr,sizeof(g_addr),G_context.tx_info.withdraw_fee_tx_info.account) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "account";
    pairs[nbPairs].value = g_addr;
    nbPairs++;
    return nbPairs;
}
// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_fee_tx_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW_FEE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = setwithdrawFeeTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review withdrawFee transaction",
                           NULL,
                           "Sign withdrawFee transaction",
                           withdraw_fee_tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_fee_tx() {
    return ui_display_withdraw_fee_tx_bs_choice();
}

#endif