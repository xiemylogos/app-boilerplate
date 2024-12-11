#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"

/**
 * Deserialize raw oep4 transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized authorizeForPeer transaction.
 * @param[out]    authorizeForPeer  tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */

parser_status_e authorize_for_peer_deserialize(buffer_t *buf,size_t length, authorize_for_peer_t *tx);


