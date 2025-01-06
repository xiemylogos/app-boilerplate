#pragma once

#include "buffer.h"

#include "types.h"
#include "../types.h"


/**
 * Deserialize raw person msg in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized person msg.
 * @param[out]     person msg info
 *   Pointer to person structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e personal_msg_deserialize(buffer_t *buf, personal_msg_info *info);



