#pragma once

#include "buffer.h"

#include "../types.h"

/**
 * Deserialize raw neo vm oep4 transfer transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_neo_vm_transaction_deserialize(buffer_t *buf);
/**
 * Deserialize raw wasm vm oep4 transfer transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_wasm_vm_transaction_deserialize(buffer_t *buf);
/**
 * Deserialize raw neo vm oep4 approve transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_neo_vm_approve_transaction_deserialize(buffer_t *buf);
/**
 * Deserialize raw wasm vm oep4 approve transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_wasm_vm_approve_transaction_deserialize(buffer_t *buf);
/**
 * Deserialize raw neo vm oep4 transfer from  transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_neo_vm_transfer_from_transaction_deserialize(buffer_t *buf);
/**
 * Deserialize raw wasm vm oep4 transfer from transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e oep4_wasm_vm_transfer_from_transaction_deserialize(buffer_t *buf);
