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
            if (G_context.display_data.pubkey_number == 1) {
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
                strcat(g_title,ONE);
                pairs[nbPairs].item = g_title;
                pairs[nbPairs].value = G_context.display_data.peer_pubkey;
                nbPairs++;
            }
            if (i==1) {
                memset(g_title_two,0,sizeof(g_title_two));
                memcpy(g_title_two,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strcat(g_title_two,TWO);
                pairs[nbPairs].item = g_title_two;
                pairs[nbPairs].value = G_context.display_data.content_three;
                nbPairs++;
            }
            if (i==2) {
                memset(g_title_three,0,sizeof(g_title_three));
                memcpy(g_title_three,NBGL_PEER_PUBKEY,sizeof(NBGL_PEER_PUBKEY));
                strcat(g_title_three,THREE);
                pairs[nbPairs].item = g_title_three;
                pairs[nbPairs].value = G_context.display_data.content_four;
                nbPairs++;
            }
        }
        if (G_context.display_data.pubkey_number >1) {
            memset(g_pubkey_number,0,sizeof(g_pubkey_number));
            if (!format_u64(g_pubkey_number, sizeof(g_pubkey_number), G_context.display_data.pubkey_number)) {
                 return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
            }
            pairs[nbPairs].item = NODE_AMOUNT;
            pairs[nbPairs].value = g_pubkey_number;
            nbPairs++;
        }
    }
    if(G_context.tx_type == OEP4_TRANSACTION ||
        G_context.tx_type == NEO_VM_OEP4_APPROVE ||
        G_context.tx_type == WASM_VM_OEP4_APPROVE ||
        G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
        G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
        uint8_t decimals = 0;
        decimals = get_oep4_token_decimals((uint8_t *)G_context.display_data.content);
        if (decimals == 0) {
            pairs[nbPairs].item = DECIMALS;
            pairs[nbPairs].value = DECIMALS_UNKNOWN;
            nbPairs++;
        }
    }

     if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
        G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
        G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
        G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
         pairs[nbPairs].item = SENDER;
         pairs[nbPairs].value = G_context.display_data.content;
         nbPairs++;
    }

    if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
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
            if (memcmp(G_context.display_data.content_two,ONT_ADDR,20) == 0) {
                pairs[nbPairs].item = ONT_AMOUNT;
            } else if (memcmp(G_context.display_data.content_two,ONG_ADDR,20) == 0) {
                pairs[nbPairs].item = ONG_AMOUNT;
            }
            if(G_context.tx_type == OEP4_TRANSACTION ||
                G_context.tx_type == NEO_VM_OEP4_APPROVE ||
                G_context.tx_type == WASM_VM_OEP4_APPROVE ||
                G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
                G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
                pairs[nbPairs].item = AMOUNT;
            }
             pairs[nbPairs].value = G_context.display_data.amount;
             nbPairs++;

             //fromAddr
             pairs[nbPairs].item = FROM;
             pairs[nbPairs].value = G_context.display_data.from;
             nbPairs++;
             //toAddr
             pairs[nbPairs].item = TO;
             pairs[nbPairs].value = G_context.display_data.to;
             nbPairs++;
    }

   //fee
    pairs[nbPairs].item = FEE_ONG;
    pairs[nbPairs].value = G_context.display_data.fee;
    nbPairs++;
    //
    pairs[nbPairs].item = SIGNER;
    pairs[nbPairs].value = G_context.display_data.signer;
    nbPairs++;

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

static void set_display_title_content(void) {
    if (G_context.tx_type == ADD_INIT_POS) {
        review_title = ADD_INIT_POS_TITLE;
        review_content = ADD_INIT_POS_CONTENT;
    } else if (G_context.tx_type == APPROVE ||
               G_context.tx_type == APPROVE_V2 ||
               G_context.tx_type == NEO_VM_OEP4_APPROVE ||
               G_context.tx_type == WASM_VM_OEP4_APPROVE) {
        review_title = SIGN_APPROVE_TX_TITLE;
        review_content = SIGN_APPROVE_TX_CONTENT;
    } else if (G_context.tx_type == AUTHORIZE_FOR_PEER) {
        review_title = AUTHORIZE_FOR_PEER_TITLE;
        review_content = AUTHORIZE_FOR_PEER_CONTENT;
    } else if (G_context.tx_type == CHANGE_MAX_AUTHORIZATION) {
        review_title = CHANGE_MAX_AUTHORIZATION_TITLE;
        review_content = CHANGE_MAX_AUTHORIZATION_CONTENT;
    } else if (G_context.tx_type == OEP4_TRANSACTION) {
        review_title = OEP4_TX_TITLE;
        review_content = OEP4_TX_CONTENT;
    } else if (G_context.tx_type == QUIT_NODE) {
        review_title = QUIT_NODE_TITLE;
        review_content = QUIT_NODE_CONTENT;
    } else if (G_context.tx_type == REDUCE_INIT_POS) {
        review_title = REDUCE_INIT_POS_TITLE;
        review_content = REDUCE_INIT_POS_CONTENT;
    } else if (G_context.tx_type == REGISTER_CANDIDATE) {
        review_title = REGISTER_CANDIDATE_TITLE;
        review_content = REGISTER_CANDIDATE_CONTENT;
    } else if (G_context.tx_type == SET_FEE_PERCENTAGE) {
        review_title = SET_FEE_PERCENTAGE_TITLE;
        review_content = SET_FEE_PERCENTAGE_CONTENT;
    } else if (G_context.tx_type == TRANSFER_TRANSACTION ||
               G_context.tx_type == TRANSFER_V2_TRANSACTION) {
        if (memcmp(G_context.display_data.content_two, ONT_ADDR, 20) == 0) {
            review_title = NATIVE_ONT_TRANSFER_TITLE;
            review_content = NATIVE_ONT_TRANSFER_CONTENT;
        } else if (memcmp(G_context.display_data.content_two, ONG_ADDR, 20) == 0) {
            review_title = NATIVE_ONG_TRANSFER_TITLE;
            review_content = NATIVE_ONG_TRANSFER_CONTENT;
        }
    } else if (G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
               G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
               G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
               G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
        review_title = TRANSFER_FROM_TITLE;
        review_content = TRANSFER_FROM_CONTENT;
    } else if (G_context.tx_type == UN_AUTHORIZE_FOR_PEER) {
        review_title = UN_AUTHORIZE_FOR_PEER_TITLE;
        review_content = UN_AUTHORIZE_FOR_PEER_CONTENT;
    } else if (G_context.tx_type == WITHDRAW_FEE) {
        review_title = WITHDRAW_FEE_TITLE;
        review_content = WITHDRAW_FEE_CONTENT;
    } else if (G_context.tx_type == WITHDRAW) {
        review_title = WITHDRAW_TITLE;
        review_content = WITHDRAW_CONTENT;
    } else {
        review_title = TRANSFER_FROM_TITLE;
        review_content = TRANSFER_FROM_CONTENT;
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
