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
parser_status_e person_msg_deserialize(buffer_t *buf, person_msg_info *info);



