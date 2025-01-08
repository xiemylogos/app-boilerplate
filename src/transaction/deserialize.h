#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"

parser_status_e check_native_end_data(buffer_t *buf);
/**
 * Deserialize native transfer raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_native_transfer_deserialize(buffer_t *buf, ont_transaction_t *tx);
/**
 * Deserialize native transferV2 raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_native_transfer_v2_deserialize(buffer_t *buf, ont_transaction_t *tx);
/**
 * Deserialize native transfer from raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_native_transfer_from_deserialize(buffer_t *buf, ont_transaction_from_t *tx);
/**
 * Deserialize native transfer from V2 raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_native_transfer_from_v2_deserialize(buffer_t *buf, ont_transaction_from_t *tx);
/**
 * Deserialize native approve raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_approve_deserialize(buffer_t *buf, ont_transaction_t *tx);
/**
 * Deserialize native approve V2 raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_approve_v2_deserialize(buffer_t *buf, ont_transaction_t *tx);

