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
#include "utils.h"

static char g_msg[1045];

static nbgl_layoutTagValue_t pairs[1];
static nbgl_layoutTagValueList_t pairList;

static void personal_msg_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_personal_msg(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_MESSAGE_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_MESSAGE_REJECTED, ui_menu_main);
    }
}


// Public function to start the personal msg review
// - Check if the app is in the right state for personal msg review
// - Display the first screen of the personal msg review
int ui_display_personal_msg_bs_choice() {
    if (G_context.req_type != CONFIRM_MESSAGE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    explicit_bzero(pairs, sizeof(pairs));

    memset(g_msg, 0, sizeof(g_msg));
    memcpy(g_msg, SIGN_MAGIC, sizeof(SIGN_MAGIC) - 2);
    int msglen = utf8_strlen(G_context.personal_msg_info.raw_msg);

    char lengthStr[10];
    snprintf(lengthStr, sizeof(lengthStr), "%d", msglen);
    int totalLength = sizeof(SIGN_MAGIC) - 2 + strlen(lengthStr) + G_context.personal_msg_info.raw_msg_len + 1;
    memcpy(g_msg + sizeof(SIGN_MAGIC) - 2, lengthStr, strlen(lengthStr));

    for (size_t i=0;i< G_context.personal_msg_info.raw_msg_len;i++) {
       g_msg[sizeof(SIGN_MAGIC) - 2+strlen(lengthStr) + i] = (char)(G_context.personal_msg_info.msg_info.personal_msg[i]);
    }
    g_msg[totalLength-1] = '\0';

    pairs[0].item = "msg";
    pairs[0].value = g_msg;
    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 1;
    pairList.pairs = pairs;

    // Start review flow
    nbgl_useCaseReview(TYPE_MESSAGE,
                           &pairList,
                           &C_icon_ont_64px,
                           "verify the message",
                           NULL,
                           "Sign the message",
                           personal_msg_review_choice);
    return 0;
}


// Flow used to display a clear-signed personal msg
int ui_display_personal_msg() {
    return ui_display_personal_msg_bs_choice();
}

#endif
