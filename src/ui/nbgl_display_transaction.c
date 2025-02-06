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
#include "../transaction/utils.h"
#include "types.h"

static char g_pubkey_number[20];
static char g_title[60];
static char g_title_two[60];
static char g_title_three[60];

#define MAX_PAIRS        11

static nbgl_contentTagValue_t pairs[MAX_PAIRS];
static nbgl_contentTagValueList_t pairsList;

static const char *review_title;
static const char *review_content;
// called when long press button on 3rd page is long-touched or when reject footer is touched
static void tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

static uint8_t setTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    #define ADD_PAIR(tagName, valueName) \
        do { \
            pairs[nbPairs].item = tagName; \
            pairs[nbPairs].value = valueName; \
            nbPairs++; \
        } while (0)

    bool isCommonTx = (G_context.tx_type == REGISTER_CANDIDATE ||
                       G_context.tx_type == QUIT_NODE ||
                       G_context.tx_type == ADD_INIT_POS ||
                       G_context.tx_type == REDUCE_INIT_POS ||
                       G_context.tx_type == CHANGE_MAX_AUTHORIZATION ||
                       G_context.tx_type == SET_FEE_PERCENTAGE ||
                       G_context.tx_type == WITHDRAW_FEE ||
                       G_context.tx_type == WITHDRAW ||
                       G_context.tx_type == AUTHORIZE_FOR_PEER ||
                       G_context.tx_type == UN_AUTHORIZE_FOR_PEER);

    if (isCommonTx) {
        ADD_PAIR(ACCOUNT, G_context.display_data.content);
    }

    if (G_context.tx_type == REGISTER_CANDIDATE ||
        G_context.tx_type == QUIT_NODE ||
        G_context.tx_type == ADD_INIT_POS ||
        G_context.tx_type == REDUCE_INIT_POS ||
        G_context.tx_type == CHANGE_MAX_AUTHORIZATION ||
        G_context.tx_type == SET_FEE_PERCENTAGE) {
        ADD_PAIR(NBGL_PEER_PUBKEY, G_context.display_data.peer_pubkey);
    }

    if (G_context.tx_type == WITHDRAW  ||
        G_context.tx_type == AUTHORIZE_FOR_PEER ||
        G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
         for(uint8_t i=0;i<G_context.display_data.pubkey_number;i++) {
            if(i>2) {
                break;
            }
            if (i==0) {
                memset(g_title,0,sizeof(g_title));
                memcpy(g_title,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                if (G_context.display_data.pubkey_number >1) {
                    strlcat(g_title, ONE,sizeof(g_title));
                }
                ADD_PAIR(g_title,G_context.display_data.peer_pubkey);
            }
            if (i==1) {
                memset(g_title_two,0,sizeof(g_title_two));
                memcpy(g_title_two,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strlcat(g_title_two,TWO,sizeof(g_title_two));
                ADD_PAIR(g_title_two,G_context.display_data.content_three);
            }
            if (i==2) {
                memset(g_title_three,0,sizeof(g_title_three));
                memcpy(g_title_three,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strlcat(g_title_three,THREE,sizeof(g_title_three));
                ADD_PAIR(g_title_three,G_context.display_data.content_four);
            }
        }
        if (G_context.display_data.pubkey_number >1) {
            memset(g_pubkey_number,0,sizeof(g_pubkey_number));
            if (!format_u64(g_pubkey_number, sizeof(g_pubkey_number), G_context.display_data.pubkey_number)) {
                 return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
            ADD_PAIR(NODE_AMOUNT,g_pubkey_number);
        }
    }

    if (isCommonTx && G_context.tx_type != QUIT_NODE && G_context.tx_type != WITHDRAW_FEE) {
        const char* item = NULL;
        switch (G_context.tx_type) {
            case CHANGE_MAX_AUTHORIZATION: item = MAX_AUTHORIZE; break;
            case SET_FEE_PERCENTAGE: item = PEER_COST; break;
            case WITHDRAW: item = (G_context.display_data.pubkey_number == 1) ? AMOUNT : TOTAL_WITHDRAW; break;
            case UN_AUTHORIZE_FOR_PEER: item = AMOUNT; break;
            default: item = POS; break;
        }
        ADD_PAIR(item, G_context.display_data.amount);
    }
    if (G_context.tx_type == SET_FEE_PERCENTAGE) {
        ADD_PAIR(STAKE_COST, G_context.display_data.content_two);
    }

    if (G_context.tx_type == REGISTER_CANDIDATE) {
        ADD_PAIR(STAKE_FEE, STAKE_FEE_ONG);
    }
    // OEP4 and related transactions
    if (G_context.tx_type == OEP4_TRANSACTION ||
        G_context.tx_type == NEO_VM_OEP4_APPROVE ||
        G_context.tx_type == WASM_VM_OEP4_APPROVE ||
        G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
        G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
        if (G_context.display_data.decimals == 0) {
            ADD_PAIR(DECIMALS, DECIMALS_UNKNOWN);
        }
    }
    // Transfer and Approve transactions
    if (G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
        G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
        G_context.tx_type == TRANSFER_TRANSACTION ||
        G_context.tx_type == TRANSFER_V2_TRANSACTION ||
        G_context.tx_type == APPROVE ||
        G_context.tx_type == APPROVE_V2 ||
        G_context.tx_type == OEP4_TRANSACTION ||
        G_context.tx_type == NEO_VM_OEP4_APPROVE ||
        G_context.tx_type == WASM_VM_OEP4_APPROVE ||
        G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
        G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
        ADD_PAIR(AMOUNT, G_context.display_data.amount);

        if (G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
            G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
            G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
            G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
            ADD_PAIR(SENDER, G_context.display_data.content);
        }

        ADD_PAIR(FROM, G_context.display_data.from);
        ADD_PAIR(TO, G_context.display_data.to);
    }
    // Fee
    ADD_PAIR((G_context.tx_type == REGISTER_CANDIDATE) ? GAS_FEE : FEE_ONG, G_context.display_data.fee);

    // Signer
    ADD_PAIR(SIGNER, G_context.display_data.signer);
    return nbPairs;
}

int ui_display_blind_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairs[0].item = BLIND_SIGN_TX;
    pairs[0].value = BLIND_SIGNING;

    pairs[1].item = SIGNER;
    pairs[1].value = G_context.display_data.signer;

    pairsList.pairs = pairs;
    pairsList.nbPairs = 2;
    review_title = BLIND_SIGNING_TITLE;
    review_content = BLIND_SIGNING_CONTENT;
    nbgl_useCaseReviewBlindSigning(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              BLIND_SIGNING_TITLE,
                              NULL,
                              BLIND_SIGNING_CONTENT,
                               NULL,
                              tx_review_choice);

    return 0;
}

int ui_display_blind_signing_transaction() {
    return ui_display_blind_transaction_bs_choice();
}

typedef struct {
    tx_type_e tx_type;
    const char *title;
    const char *content;
} tx_display_t;

static const tx_display_t tx_display_map[] = {
    {ADD_INIT_POS, ADD_INIT_POS_TITLE, ADD_INIT_POS_CONTENT},
    {APPROVE, SIGN_APPROVE_TX_TITLE, SIGN_APPROVE_TX_CONTENT},
    {APPROVE_V2, SIGN_APPROVE_TX_TITLE, SIGN_APPROVE_TX_CONTENT},
    {NEO_VM_OEP4_APPROVE, SIGN_APPROVE_TX_TITLE, SIGN_APPROVE_TX_CONTENT},
    {WASM_VM_OEP4_APPROVE, SIGN_APPROVE_TX_TITLE, SIGN_APPROVE_TX_CONTENT},
    {AUTHORIZE_FOR_PEER, AUTHORIZE_FOR_PEER_TITLE, AUTHORIZE_FOR_PEER_CONTENT},
    {CHANGE_MAX_AUTHORIZATION, CHANGE_MAX_AUTHORIZATION_TITLE, CHANGE_MAX_AUTHORIZATION_CONTENT},
    {OEP4_TRANSACTION, OEP4_TX_TITLE, OEP4_TX_CONTENT},
    {QUIT_NODE, QUIT_NODE_TITLE, QUIT_NODE_CONTENT},
    {REDUCE_INIT_POS, REDUCE_INIT_POS_TITLE, REDUCE_INIT_POS_CONTENT},
    {REGISTER_CANDIDATE, REGISTER_CANDIDATE_TITLE, REGISTER_CANDIDATE_CONTENT},
    {SET_FEE_PERCENTAGE, SET_FEE_PERCENTAGE_TITLE, SET_FEE_PERCENTAGE_CONTENT},
    {TRANSFER_TRANSACTION, NATIVE_ONG_OR_ONT_TRANSFER_TITLE, NATIVE_ONG_OR_ONT_TRANSFER_CONTENT},
    {TRANSFER_V2_TRANSACTION, NATIVE_ONG_OR_ONT_TRANSFER_TITLE, NATIVE_ONG_OR_ONT_TRANSFER_CONTENT},
    {TRANSFER_FROM_TRANSACTION, TRANSFER_FROM_TITLE, TRANSFER_FROM_CONTENT},
    {TRANSFER_FROM_V2_TRANSACTION, TRANSFER_FROM_TITLE, TRANSFER_FROM_CONTENT},
    {NEO_VM_OEP4_TRANSFER_FROM, TRANSFER_FROM_TITLE, TRANSFER_FROM_CONTENT},
    {WASM_VM_OEP4_TRANSFER_FROM, TRANSFER_FROM_TITLE, TRANSFER_FROM_CONTENT},
    {UN_AUTHORIZE_FOR_PEER, UN_AUTHORIZE_FOR_PEER_TITLE, UN_AUTHORIZE_FOR_PEER_CONTENT},
    {WITHDRAW_FEE, WITHDRAW_FEE_TITLE, WITHDRAW_FEE_CONTENT},
    {WITHDRAW, WITHDRAW_TITLE, WITHDRAW_CONTENT},
};


static void set_display_title_content(void) {
    review_title = TRANSFER_FROM_TITLE;
    review_content = TRANSFER_FROM_CONTENT;

    size_t num_tx_types = sizeof(tx_display_map) / sizeof(tx_display_map[0]);
    for (size_t i = 0; i < num_tx_types; i++) {
        if (G_context.tx_type == tx_display_map[i].tx_type) {
            review_title = tx_display_map[i].title;
            review_content = tx_display_map[i].content;
            break;
        }
    }
}

int ui_display_tx_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    set_display_title_content();
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           review_title,
                           NULL,
                           review_content,
                           tx_review_choice);
    return 0;
}

int  ui_display_transaction() {
    return ui_display_tx_transaction_choice();
}
#endif
