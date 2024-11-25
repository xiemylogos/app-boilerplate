#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"

/**
 * Deserialize raw oep4 transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_transaction_deserialize(buffer_t *buf, ont_transaction_t *tx);

parser_status_e oep4_state_info_deserialize(buffer_t *buf,size_t length, state_info_v2 *tx);


