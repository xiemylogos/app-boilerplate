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

// Buffer where the transaction amount string is written
static char g_amount[30];
// Buffer where the transaction address string is written
static char g_fromAddr[40];
static char g_toAddr[40];

#define MAX_PAIRS        3

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
     // Format amount and address to g_amount and g_address buffers
    memset(g_amount, 0, sizeof(g_amount));
    // Setup data to display
    if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONT_ADDR,20) == 0) {
        pairs[nbPairs].item = "ONT Amount";
        format_fpu64_trimmed(g_amount,sizeof(g_amount),G_context.tx_info.transaction.payload.value,9);
    } else if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONG_ADDR,20) == 0) {
        pairs[nbPairs].item = "ONG Amount";
        format_fpu64_trimmed(g_amount,sizeof(g_amount),G_context.tx_info.transaction.payload.value,9);
    }
    pairs[nbPairs].value = g_amount;
    nbPairs++;
    //fromAddr
    memset(g_fromAddr, 0, sizeof(g_fromAddr));
    if (script_hash_to_address(g_fromAddr,sizeof(g_fromAddr),G_context.tx_info.transaction.payload.from) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "From";
    pairs[nbPairs].value = g_fromAddr;
    nbPairs++;
     //toAddr
    memset(g_toAddr, 0, sizeof(g_toAddr));
    if (script_hash_to_address(g_toAddr,sizeof(g_toAddr),G_context.tx_info.transaction.payload.to) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "to";
    pairs[nbPairs].value = g_toAddr;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));

    pairsList.nbPairs = setTagValuePairs();
    pairsList.pairs = pairs;

   if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONT_ADDR,20) == 0) {
        nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairsList,
                           &C_icon_ont_64px,
                           "Review transaction\nto send ONT",
                           NULL,
                           "Sign transaction\nto send ONT",
                           tx_review_choice);
   } else if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONG_ADDR,20) == 0) {
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
    return ui_display_transaction_bs_choice();
}

#endif
