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
#include "action/validate.h"
#include "../transaction/types.h"
#include "../menu.h"
#include "../utils.h"
#include "../transaction/utils.h"
#include "types.h"

// Buffer where the transaction address string is written
static char g_content[66];
static char g_content_two[66];
static char g_content_three[66];
static char g_content_five[20];
static char g_title[60];
static char g_title_two[40];
static char g_title_three[40];

#define MAX_PAIRS        11

static nbgl_contentTagValue_t pairs[MAX_PAIRS];
static nbgl_contentTagValueList_t pairsList;

//registerCandidate
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void govern_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_govern_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

static uint8_t governTxTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
    if (G_context.tx_type == REGISTER_CANDIDATE ||
        G_context.tx_type == QUIT_NODE ||
        G_context.tx_type == ADD_INIT_POS ||
        G_context.tx_type == REDUCE_INIT_POS ||
        G_context.tx_type == CHANGE_MAX_AUTHORIZATION ||
        G_context.tx_type == SET_FEE_PERCENTAGE) {
        pairs[nbPairs].item = NBGL_PEER_PUBKEY;
        pairs[nbPairs].value = G_context.display_data.peer_pubkey;
        nbPairs++;
    }
    if (G_context.tx_type == REGISTER_CANDIDATE ||
         G_context.tx_type == QUIT_NODE ||
        G_context.tx_type == ADD_INIT_POS ||
        G_context.tx_type == REDUCE_INIT_POS ||
        G_context.tx_type == CHANGE_MAX_AUTHORIZATION ||
        G_context.tx_type == SET_FEE_PERCENTAGE ||
        G_context.tx_type == WITHDRAW_FEE ||
        G_context.tx_type == WITHDRAW ||
        G_context.tx_type == AUTHORIZE_FOR_PEER ||
        G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
        pairs[nbPairs].item = ACCOUNT;
        pairs[nbPairs].value = G_context.display_data.content;
        nbPairs++;
    }
    if (G_context.tx_type == REGISTER_CANDIDATE ||
        G_context.tx_type == ADD_INIT_POS ||
        G_context.tx_type == REDUCE_INIT_POS ||
        G_context.tx_type == CHANGE_MAX_AUTHORIZATION ||
        G_context.tx_type == SET_FEE_PERCENTAGE ||
        G_context.tx_type == WITHDRAW ||
        G_context.tx_type == AUTHORIZE_FOR_PEER ||
        G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
        if (G_context.tx_type == CHANGE_MAX_AUTHORIZATION) {
            pairs[nbPairs].item = MAX_AUTHORIZE;
        } else if( G_context.tx_type == SET_FEE_PERCENTAGE){
            pairs[nbPairs].item = PEER_COST;
        } else if (G_context.tx_type == WITHDRAW) {
            if (G_context.tx_info.withdraw_tx_info.withdraw_number ==1) {
                pairs[nbPairs].item = AMOUNT;
            } else {
                pairs[nbPairs].item = TOTAL_WITHDRAW;
            }
        } else {
           pairs[nbPairs].item = POS;
        }
        pairs[nbPairs].value = G_context.display_data.amount;
        nbPairs++;
    }
    if( G_context.tx_type == SET_FEE_PERCENTAGE) {
        pairs[nbPairs].item = STAKE_COST;
        pairs[nbPairs].value = G_context.display_data.content_two;
        nbPairs++;
    }
    if (G_context.tx_type == REGISTER_CANDIDATE) {
        pairs[nbPairs].item = STAKE_FEE;
        pairs[nbPairs].value = STAKE_FEE_ONG;
        nbPairs++;
    }

    if (G_context.tx_type == WITHDRAW) {
         //peer pubkey
         for(uint8_t i=0;i<G_context.tx_info.withdraw_tx_info.peer_pubkey_number;i++) {
            if(i>2) {
                break;
            }
            if (i==0) {
                memset(g_content, 0, sizeof(g_content));
                memcpy(g_content, G_context.tx_info.withdraw_tx_info.peer_pubkey[i], 66);
                pairs[nbPairs].item = PEER_PUBKEY;
                pairs[nbPairs].value = g_content;
                nbPairs++;
            }
            if (i==1) {
                memset(g_content_two, 0, sizeof(g_content_two));
                memcpy(g_content_two, G_context.tx_info.withdraw_tx_info.peer_pubkey[i], 66);
                pairs[nbPairs].item = PEER_PUBKEY;
                pairs[nbPairs].value = g_content_two;
                nbPairs++;
            }
            if (i==2) {
                 memset(g_content_three, 0, sizeof(g_content_three));
                memcpy(g_content_three, G_context.tx_info.withdraw_tx_info.peer_pubkey[i], 66);
                pairs[nbPairs].item = PEER_PUBKEY;
                pairs[nbPairs].value = g_content_three;
                nbPairs++;
            }
        }
        if (G_context.tx_info.withdraw_tx_info.peer_pubkey_number >1) {
            memset(g_content_five,0,sizeof(g_content_five));
            if (!format_u64(g_content_five, sizeof(g_content_five), G_context.tx_info.withdraw_tx_info.peer_pubkey_number)) {
                 return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
            pairs[nbPairs].item = NODE_AMOUNT;
            pairs[nbPairs].value = g_content_five;
            nbPairs++;
        }
    }
    if (G_context.tx_type == AUTHORIZE_FOR_PEER) {
        for(uint8_t i=0;i<G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey_number;i++) {
            if(i>2) {
                    break;
                }
            if (i==0) {
                memset(g_content, 0, sizeof(g_content));
                memcpy(g_content, G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey[i], 66);
                memset(g_title,0,sizeof(g_title));
                memcpy(g_title,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strcat(g_title,ONE);
                pairs[nbPairs].item = g_title;
                pairs[nbPairs].value = g_content;
                nbPairs++;
                }
            if (i==1) {
                memset(g_content_two, 0, sizeof(g_content_two));
                memcpy(g_content_two, G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey[i], 66);
                memset(g_title_two,0,sizeof(g_title_two));
                memcpy(g_title_two,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strcat(g_title,TWO);
                pairs[nbPairs].item = g_title_two;
                pairs[nbPairs].value = g_content_two;
                nbPairs++;
            }
            if (i==2) {
                memset(g_content_three, 0, sizeof(g_content_three));
                memcpy(g_content_three, G_context.tx_info.authorize_for_peer_tx_info.peer_pubkey[i], 66);
                memset(g_title_three,0,sizeof(g_title_three));
                memcpy(g_title_three,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strcat(g_title,THREE);
                pairs[nbPairs].item = g_title_three;
                pairs[nbPairs].value = g_content_three;
                nbPairs++;
            }
        }
    }
    if(G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
        for(uint8_t i=0;i<G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey_number;i++) {
            if(i>2) {
                break;
            }
            if (i==0) {
                memset(g_content, 0, sizeof(g_content));
                memcpy(g_content, G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey[i], 66);
                pairs[nbPairs].item = PEER_PUBKEY;
                pairs[nbPairs].value = g_content;
                nbPairs++;
            }
            if (i==1) {
                memset(g_content_two, 0, sizeof(g_content_two));
                memcpy(g_content_two, G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey[i], 66);
                pairs[nbPairs].item = PEER_PUBKEY;
                pairs[nbPairs].value = g_content_two;
                nbPairs++;
            }
            if (i==2) {
                memset(g_content_three, 0, sizeof(g_content_three));
                memcpy(g_content_three, G_context.tx_info.un_authorize_for_peer_tx_info.peer_pubkey[i], 66);
                pairs[nbPairs].item = PEER_PUBKEY;
                pairs[nbPairs].value = g_content_three;
                nbPairs++;
            }
        }
    }
    //fee
    pairs[nbPairs].item = FEE_ONG;
    pairs[nbPairs].value = G_context.display_data.fee;
    nbPairs++;

    pairs[nbPairs].item = SIGNER;
    pairs[nbPairs].value = G_context.display_data.signer;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_register_candidate_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REGISTER_CANDIDATE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review registerCandidate transaction",
                           NULL,
                           "Sign registerCandidate transaction",
                           govern_tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_register_candidate_tx() {
    return ui_display_register_candidate_tx_choice();
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review withdraw transaction",
                           NULL,
                           "Sign withdraw transaction",
                           govern_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_tx() {
    return ui_display_withdraw_tx_choice();
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_quit_node_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != QUIT_NODE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review quitNode transaction",
                           NULL,
                           "Sign quitNode transaction",
                           govern_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_quit_node_tx() {
    return ui_display_quit_node_tx_choice();
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_add_init_pos_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != ADD_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review addInitPos transaction",
                           NULL,
                           "Sign addInitPos transaction",
                           govern_tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_add_init_pos_tx() {
    return ui_display_add_init_pos_tx_choice();
}
// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_reduce_init_pos_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != REDUCE_INIT_POS) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review reduceInitPos transaction",
                           NULL,
                           "Sign reduceInitPos transaction",
                           govern_tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_reduce_init_pos_tx() {
    return ui_display_reduce_init_pos_tx_choice();
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_change_max_authorization_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != CHANGE_MAX_AUTHORIZATION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review changeMaxAuthorization transaction",
                           NULL,
                           "Sign changeMaxAuthorization transaction",
                           govern_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_change_max_authorization_tx() {
    return ui_display_change_max_authorization_tx_choice();
}
// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_set_fee_percentage_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != SET_FEE_PERCENTAGE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review setFeePercentage transaction",
                           NULL,
                           "Sign setFeePercentage transaction",
                           govern_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_set_fee_percentage_tx() {
    return ui_display_set_fee_percentage_tx_choice();
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_authorize_for_peer_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review AuthorizeForPeer transaction",
                           NULL,
                           "Sign AuthorizeForPeer transaction",
                           govern_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_authorize_for_peer_tx() {
    return ui_display_authorize_for_peer_tx_choice();
}
// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_un_authorize_for_peer_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != UN_AUTHORIZE_FOR_PEER) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review unAuthorizeForPeer transaction",
                           NULL,
                           "Sign unAuthorizeForPeer transaction",
                           govern_tx_review_choice);

    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_un_authorize_for_peer_tx() {
    return ui_display_un_authorize_for_peer_tx_choice();
}
// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_withdraw_fee_tx_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != WITHDRAW_FEE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = governTxTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review withdrawFee transaction",
                           NULL,
                           "Sign withdrawFee transaction",
                           govern_tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_withdraw_fee_tx() {
    return ui_display_withdraw_fee_tx_choice();
}

#endif