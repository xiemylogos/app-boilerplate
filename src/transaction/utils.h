#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include "buffer.h"
#include "types.h"
#include "../types.h"


uint64_t getBytesValueByLen(buffer_t *buf,uint8_t len);

uint64_t getValueByLen(uint8_t *value,uint8_t len);

uint64_t getValue(uint8_t *value,uint8_t begin_len,uint8_t end_len);

parser_status_e transaction_deserialize_header(buffer_t *buf,transaction_header_t *tx);