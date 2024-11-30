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
#include "nbgl_use_case.h"
#include "io.h"
#include "bip32.h"
#include "format.h"

#include "display.h"
#include "../globals.h"
#include "../sw.h"
#include "action/validate.h"
#include "../menu.h"

static char g_msg[30];
static char g_msg_len[43];
static nbgl_layoutTagValue_t pairs[2];
static nbgl_layoutTagValueList_t pairList;

static void person_msg_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_person_msg(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_MESSAGE_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_MESSAGE_REJECTED, ui_menu_main);
    }
}


// Public function to start the person msg review
// - Check if the app is in the right state for person msg review
// - Display the first screen of the person msg review
int ui_display_person_msg_bs_choice() {
    if (G_context.req_type != CONFIRM_MESSAGE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    // Setup data to display
    pairs[0].item = "person msg";
    pairs[0].value = g_msg;
    pairs[1].item = "person msg len";
     pairs[1].value = g_msg_len;

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 2;
    pairList.pairs = pairs;

    // Start review flow
    nbgl_useCaseReview(TYPE_MESSAGE,
                       &pairList,
                       &C_app_boilerplate_64px,
                       "verify person msg",
                       NULL,
                       "sign person msg",
                       person_msg_review_choice);
    return 0;
}


// Flow used to display a clear-signed person msg
int ui_display_person_msg() {
    return ui_display_person_msg_bs_choice();
}

#endif
