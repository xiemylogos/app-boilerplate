#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

typedef struct {
    uint8_t  *person_msg;       /// person_msg (variable length)
} person_msg_info;