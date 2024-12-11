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
#include "buffer.h"

#include "govern_deserialize.h"
#include "utils.h"
#include "types.h"
#include "constants.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif


parser_status_e authorize_for_peer_deserialize(buffer_t *buf,size_t length, authorize_for_peer_t *tx) {
    if (!buffer_can_read(buf, length)) {
        return WRONG_LENGTH_ERROR;
    }
   /*
    if(memcmp(buf->ptr+buf->offset + length - 46 - 10 - 1, "transferV2", 10) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    */
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->account = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return FROM_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    if (!buffer_read_u64(buf, &tx->peerPubkeyLength, LE)) {
        return VALUE_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    if (!buffer_seek_cur(buf,1)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->peerPubkey = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, PEER_PUBKEY_LEN*tx->peerPubkeyLength)) {
        return TO_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    if (!buffer_read_u64(buf, &tx->posListLength, LE)) {
        return VALUE_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->posList = (uint8_t*)(buf->ptr+buf->offset);
    if (!buffer_seek_cur(buf, 8*tx->posListLength)) {
        return TO_PARSING_ERROR;
    }
    if (!buffer_seek_cur(buf,4)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    if(memcmp(buf->ptr+buf->offset,AUTHORIZE_FOR_PEER, 16) != 0) {
        return PARSE_STRING_MATCH_ERROR;
    }
    if (!buffer_seek_cur(buf,17)) {
        return BUFFER_OFFSET_MOVE_ERROR;
    }
    tx->contract_addr = (uint8_t *) (buf->ptr + buf->offset);
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return CONTRACT_ADDR_PARSING_ERROR;
    }
}
