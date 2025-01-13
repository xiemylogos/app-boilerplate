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

// Buffer where the transaction amount string is written
#define MAX_PAIRS        8

static nbgl_contentTagValue_t pairs[MAX_PAIRS];
static nbgl_contentTagValueList_t pairsList;

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
    if(G_context.tx_type == OEP4_TRANSACTION ||
        G_context.tx_type == NEO_VM_OEP4_APPROVE ||
        G_context.tx_type == WASM_VM_OEP4_APPROVE ||
        G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
        G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
        uint8_t decimals = 0;
        decimals = get_oep4_token_decimals(G_context.tx_info.oep4_tx_info.payload.contract_addr);
        if (decimals == 0) {
            pairs[nbPairs].item = DECIMALS;
            pairs[nbPairs].value = DECIMALS_UNKNOWN;
            nbPairs++;
        }
        pairs[nbPairs].item = AMOUNT;
        pairs[nbPairs].value = G_context.display_data.amount;
        nbPairs++;
    } else {
       if (G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
        G_context.tx_type == TRANSFER_FROM_TRANSACTION) {
            if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
                pairs[nbPairs].item = ONT_AMOUNT;
            } else if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
                pairs[nbPairs].item = ONG_AMOUNT;
            }
        } else {
            if (memcmp(G_context.tx_info.tx_info.payload.contract_addr, ONT_ADDR, 20) == 0) {
                pairs[nbPairs].item = ONT_AMOUNT;
            } else if (memcmp(G_context.tx_info.tx_info.payload.contract_addr, ONG_ADDR, 20) == 0) {
                pairs[nbPairs].item = ONG_AMOUNT;
            }
        }
        pairs[nbPairs].value = G_context.display_data.amount;
        nbPairs++;
    }

    if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION ||
        G_context.tx_type == TRANSFER_FROM_TRANSACTION ||
        G_context.tx_type == NEO_VM_OEP4_TRANSFER_FROM ||
        G_context.tx_type == WASM_VM_OEP4_TRANSFER_FROM) {
      pairs[nbPairs].item = SENDER;
      pairs[nbPairs].value = G_context.display_data.content;
      nbPairs++;
    }
    //fromAddr
    pairs[nbPairs].item = FROM;
    pairs[nbPairs].value = G_context.display_data.from;
    nbPairs++;
     //toAddr
    pairs[nbPairs].item = TO;
    pairs[nbPairs].value = G_context.display_data.to;
    nbPairs++;
   //fee
    pairs[nbPairs].item = FEE_ONG;
    pairs[nbPairs].value = G_context.display_data.fee;
    nbPairs++;
    //sender
    pairs[nbPairs].item = SIGNER;
    pairs[nbPairs].value = G_context.display_data.signer;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != TRANSFER_V2_TRANSACTION &&
            G_context.tx_type != TRANSFER_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;

   if (memcmp(G_context.tx_info.tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction\nto send ONT",
                              NULL,
                              "Sign transaction\nto send ONT",
                              tx_review_choice);
   } else if (memcmp(G_context.tx_info.tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction\nto send ONG",
                              NULL,
                              "Sign transaction\nto send ONG",
                              tx_review_choice);
   }
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_transaction() {
    return ui_display_transaction_choice();
}

int ui_display_blind_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairs[0].item = "transaction";
    pairs[0].value = "transaction blind signing";

    pairs[1].item = SIGNER;
    pairs[1].value = G_context.display_data.signer;

    pairsList.pairs = pairs;
    pairsList.nbPairs = 2;
    nbgl_useCaseReviewBlindSigning(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction",
                              NULL,
                              "Accept risk and send transaction?",
                               NULL,
                              tx_review_choice);

    return 0;
}

int ui_display_blind_signing_transaction() {
    return ui_display_blind_transaction_bs_choice();
}

int ui_display_approve_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != APPROVE &&
            G_context.tx_type != APPROVE_V2)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;

    if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction Approve ONT",
                              NULL,
                              "Sign transaction Aprove ONT",
                              tx_review_choice);
   } else if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction Approve ONG",
                              NULL,
                               "Sign transaction Aprove ONG",
                              tx_review_choice);
   }
    return 0;
}

int ui_display_approve_tx() {
    return ui_display_approve_transaction_choice();
}

int ui_display_transaction_from_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != TRANSFER_FROM_V2_TRANSACTION &&
            G_context.tx_type != TRANSFER_FROM_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;

   if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction from\nto send ONT",
                              NULL,
                              "Sign transaction from\nto send ONT",
                              tx_review_choice);
   } else if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction from\nto send ONG",
                              NULL,
                              "Sign transaction from\nto send ONG",
                              tx_review_choice);
   }
    return 0;
}

int ui_display_transaction_from() {
    return ui_display_transaction_from_choice();
}

int ui_display_oep4_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || G_context.tx_type != OEP4_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review transaction\nto send token",
                           NULL,
                           "Sign transaction\nto send token",
                           tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_oep4_transaction() {
    return ui_display_oep4_transaction_choice();
}

int ui_display_oep4_approve_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != NEO_VM_OEP4_APPROVE &&
            G_context.tx_type != WASM_VM_OEP4_APPROVE)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;

    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review approve transaction",
                           NULL,
                           "Sign approve transaction",
                           tx_review_choice);
    return 0;
}

int ui_display_oep4_approve_tx() {
    return ui_display_oep4_approve_transaction_choice();
}

int ui_display_oep4_transfer_from_transaction_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != NEO_VM_OEP4_TRANSFER_FROM &&
            G_context.tx_type != WASM_VM_OEP4_TRANSFER_FROM)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;
    nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review transaction from\nto send token",
                           NULL,
                           "Sign transaction from \nto send token",
                           tx_review_choice);
    return 0;
}

int ui_display_oep4_transfer_from_tx() {
    return ui_display_oep4_transfer_from_transaction_choice();
}

#endif
