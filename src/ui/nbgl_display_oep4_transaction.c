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
#include "../uint128.h"
#include "token_info.h"


// Buffer where the oep4 transaction amount string is written
static char g_amount[30];
static char g_fee[40];
static char g_gasPrice[40];
static char g_gasLimit[40];
// Buffer where the oep4 transaction address string is written
static char g_fromAddr[40];
static char g_toAddr[40];
static char g_decimals[20];

#define OEP4_MAX_PAIRS        7

static nbgl_layoutTagValue_t pairs[OEP4_MAX_PAIRS];
static nbgl_layoutTagValueList_t pairsList;

// called when long press button on 3rd page is long-touched or when reject footer is touched
static void oep4_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_oep4_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

static uint8_t setTagValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));

    uint8_t decimals = 1;
   if (memcmp(G_context.tx_info.oep4_tx_info.payload.contract_addr,WTK_ADDR,20) == 0) {
       decimals = 9;
    } else if (memcmp(G_context.tx_info.oep4_tx_info.payload.contract_addr,MYT_ADDR,20) == 0 ) {
        decimals = 18;
    }

     //decimals
    memset(g_decimals, 0, sizeof(g_decimals));
    if (!format_u64(g_decimals,sizeof(g_decimals),G_context.tx_info.oep4_tx_info.payload.value_len)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "value_len";
    pairs[nbPairs].value = g_decimals;
    nbPairs++;

    memset(g_amount, 0, sizeof(g_amount));
    if (G_context.tx_info.oep4_tx_info.payload.value_len >= 81) {
           format_fpu64_trimmed(g_amount,
                                     sizeof(g_amount),
                                     G_context.tx_info.oep4_tx_info.payload.value[0],
                                     decimals);
        } else {
            if (G_context.tx_info.oep4_tx_info.payload.value_len < 8) {
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

    pairs[nbPairs].item = "amount";
    pairs[nbPairs].value = g_amount;
    nbPairs++;


    //fromAddr
    memset(g_fromAddr, 0, sizeof(g_fromAddr));
    if (script_hash_to_address(g_fromAddr,sizeof(g_fromAddr),G_context.tx_info.oep4_tx_info.payload.from) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "From";
    pairs[nbPairs].value = g_fromAddr;
    nbPairs++;
     //toAddr
    memset(g_toAddr, 0, sizeof(g_toAddr));
    if (script_hash_to_address(g_toAddr,sizeof(g_toAddr),G_context.tx_info.oep4_tx_info.payload.to) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "to";
    pairs[nbPairs].value = g_toAddr;
    nbPairs++;
    //fee
    memset(g_fee, 0, sizeof(g_fee));
    format_fpu64_trimmed(g_fee,sizeof(g_fee),G_context.tx_info.oep4_tx_info.gas_price*G_context.tx_info.oep4_tx_info.gas_limit,9);
    pairs[nbPairs].item = "Fee:Ong";
    pairs[nbPairs].value = g_fee;
    nbPairs++;
    //gasPrice
    memset(g_gasPrice, 0, sizeof(g_gasPrice));
    if (!format_u64(g_gasPrice,sizeof(g_gasPrice),G_context.tx_info.oep4_tx_info.gas_price)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasPrice";
    pairs[nbPairs].value = g_gasPrice;
    nbPairs++;
    //gasLimit
    memset(g_gasLimit, 0, sizeof(g_gasLimit));
    if (!format_u64(g_gasLimit,sizeof(g_gasLimit),G_context.tx_info.oep4_tx_info.gas_limit)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasLimit";
    pairs[nbPairs].value = g_gasLimit;
    nbPairs++;
    return nbPairs;
}

// Public function to start the oep4 transaction review
// - Check if the app is in the right state for oep4 transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the oep4 transaction review
int ui_display_oep4_transaction_bs_choice() {
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
                           oep4_tx_review_choice);
    return 0;
}

// Flow used to display a clear-signed transaction
int ui_display_oep4_transaction() {
    return ui_display_oep4_transaction_bs_choice();
}

#endif
