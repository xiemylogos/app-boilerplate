#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

typedef struct {
    uint8_t  *person_msg;       /// person_msg (variable length)
    uint64_t  person_msg_len;   /// length of person_msg (8 bytes)
} person_msg_info;