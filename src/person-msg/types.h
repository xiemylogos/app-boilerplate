#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

static const char SIGN_MAGIC[] =
    "\x19"
    "Ontology Signed Message:\n";

typedef struct {
    uint8_t  *person_msg;       /// person_msg (variable length)
} person_msg_info;