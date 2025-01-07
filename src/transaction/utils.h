#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include "buffer.h"
#include "types.h"
#include "../types.h"


/**
 * Check if memo is encoded using ASCII characters.
 *
 * @param[in] memo
 *   Pointer to input byte buffer.
 * @param[in] memo_len
 *   Length of input byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool transaction_utils_check_encoding(const uint8_t *memo, uint64_t memo_len);

/**
 * Format memo as string.
 *
 * @param[in]  memo
 *   Pointer to input byte buffer.
 * @param[in]  memo_len
 *   Length of input byte buffer.
 * @param[out] dst
 *   Pointer to output string.
 * @param[in]  dst_len
 *   Length of output string.
 *
 * @return true if success, false otherwise.
 *
 */
bool transaction_utils_format_memo(const uint8_t *memo,
                                   uint64_t memo_len,
                                   char *dst,
                                   uint64_t dst_len);


uint64_t getBytesValueByLen(buffer_t *buf,uint8_t len);

uint64_t getValueByLen(uint8_t *value,uint8_t len);

parser_status_e transaction_deserialize_header(buffer_t *buf,transaction_header_t *tx);