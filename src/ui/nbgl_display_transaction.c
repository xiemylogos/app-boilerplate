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

// Buffer where the transaction amount string is written
static char g_amount[40];
static char g_fee[40];
static char g_gasPrice[40];
static char g_gasLimit[40];
// Buffer where the transaction address string is written
static char g_fromAddr[40];
static char g_toAddr[40];
static char g_signer[40];
static char g_sender[40];

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
     // Format amount and address to g_amount and g_address buffers
    memset(g_amount, 0, sizeof(g_amount));
    // Setup data to display
    if (memcmp(G_context.tx_info.tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
         pairs[nbPairs].item = "ONT Amount";
         uint decimals = 0;
        if(G_context.tx_type == TRANSFER_V2_TRANSACTION) {
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
        pairs[nbPairs].item = "ONG Amount";
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

    pairs[nbPairs].value = g_amount;
    nbPairs++;
    //fromAddr
    memset(g_fromAddr, 0, sizeof(g_fromAddr));
    if (script_hash_to_address(g_fromAddr,sizeof(g_fromAddr),G_context.tx_info.tx_info.payload.from) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "From";
    pairs[nbPairs].value = g_fromAddr;
    nbPairs++;
     //toAddr
    memset(g_toAddr, 0, sizeof(g_toAddr));
    if (script_hash_to_address(g_toAddr,sizeof(g_toAddr),G_context.tx_info.tx_info.payload.to) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "to";
    pairs[nbPairs].value = g_toAddr;
    nbPairs++;
    //fee
    memset(g_fee, 0, sizeof(g_fee));
    format_fpu64_trimmed(g_fee,sizeof(g_fee),G_context.tx_info.tx_info.gas_price*G_context.tx_info.tx_info.gas_limit,9);
    pairs[nbPairs].item = "Fee:ONG";
    pairs[nbPairs].value = g_fee;
    nbPairs++;
    //gasPrice
    memset(g_gasPrice, 0, sizeof(g_gasPrice));
    if (!format_u64(g_gasPrice,sizeof(g_gasPrice),G_context.tx_info.tx_info.gas_price)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasPrice";
    pairs[nbPairs].value = g_gasPrice;
    nbPairs++;
    //gasLimit
    memset(g_gasLimit, 0, sizeof(g_gasLimit));
    if (!format_u64(g_gasLimit,sizeof(g_gasLimit),G_context.tx_info.tx_info.gas_limit)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasLimit";
    pairs[nbPairs].value = g_gasLimit;
    nbPairs++;

    memset(g_signer, 0, sizeof(g_signer));
    if (!ont_address_from_pubkey(g_signer,sizeof(g_signer))) {
        return io_send_sw(SW_DISPLAY_SIGNER_FAIL);
    }
    pairs[nbPairs].item = "signer";
    pairs[nbPairs].value = g_signer;
    nbPairs++;

    return nbPairs;
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_transaction_bs_choice() {
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
    return ui_display_transaction_bs_choice();
}

int ui_display_blind_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairs[0].item = "transaction";
    pairs[0].value = "transaction blind signing";

    memset(g_signer, 0, sizeof(g_signer));
    if (!ont_address_from_pubkey(g_signer,sizeof(g_signer))) {
        return io_send_sw(SW_DISPLAY_SIGNER_FAIL);
    }
    pairs[1].item = "signer";
    pairs[1].value = g_signer;

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

int ui_display_blind_signed_transaction() {
    return ui_display_blind_transaction_bs_choice();
}

static void approve_tx_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_approve_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

static uint8_t setTagApproveValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
     // Format amount and address to g_amount and g_address buffers
    memset(g_amount, 0, sizeof(g_amount));

    if (memcmp(G_context.tx_info.tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
         pairs[nbPairs].item = "ONT value";
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
        pairs[nbPairs].item = "ONG value";
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

    pairs[nbPairs].value = g_amount;
    nbPairs++;
    //fromAddr
    memset(g_fromAddr, 0, sizeof(g_fromAddr));
    if (script_hash_to_address(g_fromAddr,sizeof(g_fromAddr),G_context.tx_info.tx_info.payload.from) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "From";
    pairs[nbPairs].value = g_fromAddr;
    nbPairs++;
     //toAddr
    memset(g_toAddr, 0, sizeof(g_toAddr));
    if (script_hash_to_address(g_toAddr,sizeof(g_toAddr),G_context.tx_info.tx_info.payload.to) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "to";
    pairs[nbPairs].value = g_toAddr;
    nbPairs++;
    //fee
    memset(g_fee, 0, sizeof(g_fee));
    format_fpu64_trimmed(g_fee,sizeof(g_fee),G_context.tx_info.tx_info.gas_price*G_context.tx_info.tx_info.gas_limit,9);
    pairs[nbPairs].item = "Fee:ONG";
    pairs[nbPairs].value = g_fee;
    nbPairs++;
    //gasPrice
    memset(g_gasPrice, 0, sizeof(g_gasPrice));
    if (!format_u64(g_gasPrice,sizeof(g_gasPrice),G_context.tx_info.tx_info.gas_price)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasPrice";
    pairs[nbPairs].value = g_gasPrice;
    nbPairs++;
    //gasLimit
    memset(g_gasLimit, 0, sizeof(g_gasLimit));
    if (!format_u64(g_gasLimit,sizeof(g_gasLimit),G_context.tx_info.tx_info.gas_limit)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasLimit";
    pairs[nbPairs].value = g_gasLimit;
    nbPairs++;

    memset(g_signer, 0, sizeof(g_signer));
    if (!ont_address_from_pubkey(g_signer,sizeof(g_signer))) {
        return io_send_sw(SW_DISPLAY_SIGNER_FAIL);
    }
    pairs[nbPairs].item = "signer";
    pairs[nbPairs].value = g_signer;
    nbPairs++;

    return nbPairs;
}

int ui_display_approve_transaction_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != APPROVE &&
            G_context.tx_type != APPROVE_V2)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagApproveValuePairs();
    pairsList.pairs = pairs;

    if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction Approve ONT",
                              NULL,
                              "Sign transaction Aprove ONT",
                              approve_tx_review_choice);
   } else if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
          nbgl_useCaseReview(TYPE_TRANSACTION,
                              &pairsList,
                              &C_icon_ont_64px,
                              "Review transaction Approve ONG",
                              NULL,
                               "Sign transaction Aprove ONG",
                              approve_tx_review_choice);
   }
    return 0;
}

int ui_display_approve_tx() {
    return ui_display_approve_transaction_bs_choice();
}


static uint8_t setTagFromValuePairs(void) {
    uint8_t nbPairs = 0;
    explicit_bzero(pairs, sizeof(pairs));
     // Format amount and address to g_amount and g_address buffers
    memset(g_amount, 0, sizeof(g_amount));
    // Setup data to display
    if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONT_ADDR,20) == 0) {
        pairs[nbPairs].item = "ONT Amount";
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

    } else if (memcmp(G_context.tx_info.from_tx_info.payload.contract_addr,ONG_ADDR,20) == 0) {
        pairs[nbPairs].item = "ONG Amount";
        uint decimals = 9;
        if(G_context.tx_type == TRANSFER_FROM_V2_TRANSACTION) {
              decimals = 18;
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
    }



    pairs[nbPairs].value = g_amount;
    nbPairs++;
    //sender
    memset(g_sender, 0, sizeof(g_sender));
    if (script_hash_to_address(g_sender,sizeof(g_sender),G_context.tx_info.from_tx_info.payload.sender) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "Sender";
    pairs[nbPairs].value = g_fromAddr;
    nbPairs++;
    //fromAddr
    memset(g_fromAddr, 0, sizeof(g_fromAddr));
    if (script_hash_to_address(g_fromAddr,sizeof(g_fromAddr),G_context.tx_info.from_tx_info.payload.from) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "From";
    pairs[nbPairs].value = g_fromAddr;
    nbPairs++;
     //toAddr
    memset(g_toAddr, 0, sizeof(g_toAddr));
    if (script_hash_to_address(g_toAddr,sizeof(g_toAddr),G_context.tx_info.from_tx_info.payload.to) ==
        -1) {
           return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
    pairs[nbPairs].item = "to";
    pairs[nbPairs].value = g_toAddr;
    nbPairs++;
    //fee
    memset(g_fee, 0, sizeof(g_fee));
    format_fpu64_trimmed(g_fee,sizeof(g_fee),G_context.tx_info.from_tx_info.gas_price*G_context.tx_info.from_tx_info.gas_limit,9);
    pairs[nbPairs].item = "Fee:ONG";
    pairs[nbPairs].value = g_fee;
    nbPairs++;
    //gasPrice
    memset(g_gasPrice, 0, sizeof(g_gasPrice));
    if (!format_u64(g_gasPrice,sizeof(g_gasPrice),G_context.tx_info.from_tx_info.gas_price)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasPrice";
    pairs[nbPairs].value = g_gasPrice;
    nbPairs++;
    //gasLimit
    memset(g_gasLimit, 0, sizeof(g_gasLimit));
    if (!format_u64(g_gasLimit,sizeof(g_gasLimit),G_context.tx_info.from_tx_info.gas_limit)) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    pairs[nbPairs].item = "gasLimit";
    pairs[nbPairs].value = g_gasLimit;
    nbPairs++;

    memset(g_signer, 0, sizeof(g_signer));
    if (!ont_address_from_pubkey(g_signer,sizeof(g_signer))) {
        return io_send_sw(SW_DISPLAY_SIGNER_FAIL);
    }
    pairs[nbPairs].item = "signer";
    pairs[nbPairs].value = g_signer;
    nbPairs++;

    return nbPairs;
}

int ui_display_transaction_from_bs_choice() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED
        || (G_context.tx_type != TRANSFER_FROM_V2_TRANSACTION &&
            G_context.tx_type != TRANSFER_FROM_TRANSACTION)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    explicit_bzero(&pairsList, sizeof(pairsList));
    pairsList.nbPairs = setTagFromValuePairs();
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
    return ui_display_transaction_from_bs_choice();
}

#endif
