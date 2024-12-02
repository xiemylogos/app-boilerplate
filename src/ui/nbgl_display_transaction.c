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

// Buffer where the transaction amount string is written
static char g_amount[30];
// Buffer where the transaction address string is written
static char g_address[43];

static nbgl_layoutTagValue_t pairs[2];
static nbgl_layoutTagValueList_t pairList;

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

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    // Format amount and address to g_amount and g_address buffers
    memset(g_amount, 0, sizeof(g_amount));
    char amount[30] = {0};
    if (!format_fpu64(amount,
                      sizeof(amount),
                      G_context.tx_info.transaction.payload.value,
                      EXPONENT_SMALLEST_UNIT)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    uint8_t ONG_ADDR[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
    };

    uint8_t ONT_ADDR[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
    };

    if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONT_ADDR,20) == 0) {
        snprintf(g_amount, sizeof(g_amount), "ont %.*s", sizeof(amount), amount);
    } else if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONG_ADDR,20) == 0) {
        snprintf(g_amount, sizeof(g_amount), "ong %.*s", sizeof(amount), amount);
    }

   // snprintf(g_amount, sizeof(g_amount), "bal %.*s", sizeof(amount), amount);
    memset(g_address, 0, sizeof(g_address));

    if (format_hex(G_context.tx_info.transaction.payload.to, ADDRESS_LEN, g_address, sizeof(g_address)) ==
        -1) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    // Setup data to display
    pairs[0].item = "Amount";
    pairs[0].value = g_amount;
    pairs[1].item = "Address";
    pairs[1].value = g_address;

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 2;
    pairList.pairs = pairs;

   if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONT_ADDR,20) == 0) {
        nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairList,
                           &C_icon_ont_64px,
                           "Review transaction\nto send ONT",
                           NULL,
                           "Sign transaction\nto send ONT",
                           tx_review_choice);
   } else if (memcmp(G_context.tx_info.transaction.payload.contract_addr,ONG_ADDR,20) == 0) {
         nbgl_useCaseReview(TYPE_TRANSACTION,
                           &pairList,
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
