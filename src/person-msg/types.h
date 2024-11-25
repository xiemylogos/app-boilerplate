#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

typedef struct {
    uint8_t  *person_msg;
    uint64_t  person_msg_len;
} person_msg_info;