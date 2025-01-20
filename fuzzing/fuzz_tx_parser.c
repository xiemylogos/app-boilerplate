#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "transaction/utils.h"
#include "transaction/types.h"
#include "format.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = data, .size = size, .offset = 0};
    transaction_header_t tx_header;
    transaction_deserialize_header(&buf,&tx_header);
    return 0;
}
