#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"

/**
 * Deserialize raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */

parser_status_e check_native_end_data(buffer_t *buf);

parser_status_e transaction_native_transfer_deserialize(buffer_t *buf, ont_transaction_t *tx);

parser_status_e transaction_native_transfer_v2_deserialize(buffer_t *buf, ont_transaction_t *tx);

parser_status_e transaction_native_transfer_from_deserialize(buffer_t *buf, ont_transaction_t *tx);

parser_status_e transaction_native_transfer_from_v2_deserialize(buffer_t *buf, ont_transaction_t *tx);


parser_status_e transaction_approve_deserialize(buffer_t *buf, ont_transaction_t *tx);
