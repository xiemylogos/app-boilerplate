#pragma once

#include <stddef.h>

int script_hash_to_address(char* out, size_t out_len, const unsigned char* script_hash);

size_t utf8_strlen(const char* str);
