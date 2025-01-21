#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <cmocka.h>
//#include "transaction/deserialize.h"
//#include "transaction/oep4_deserialize.h"
#include "types.h"
#include "transaction/parse.h"

/*
static void test_ont_tx_serialization(void **state) {
    (void) state;

    ont_transaction_t tx;
    // clang-format off
    const char *hex_str = "00d115ae02abc409000000000000204e00000000000005815d34e0e9ab73a175ec86ffb24aad5bee20f17b00c66b1405815d34e0e9ab73a175ec86ffb24aad5bee20f16a7cc8141451108489337c8055a9c1ed9158c947d22070d76a7cc808000064a7b3b6e00d6a7cc86c51c10a7472616e7366657256321400000000000000000000000000000000000000020068164f6e746f6c6f67792e4e61746976652e496e766f6b6500";
    size_t len = strlen(hex_str) / 2;
    uint8_t raw_tx[len];
    for (size_t i = 0; i < len; i++) {
        sscanf(hex_str + 2 * i, "%2hhx", &raw_tx[i]);
    }

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_native_transfer_deserialize(&buf, &tx);
    assert_int_equal(status, PARSING_OK);
}


static void test_oep4_transaction(void **state) {
    (void) state;

    ont_transaction_t tx;
    const char *hex_str = "00d1aecb0000c409000000000000204e00000000000085fd562eb92bc63ef6bd7b13dd09629e0c7c36445005806409760a140b61799f2c6bc9cd78513b784dd7b859386d8db914d0228f7ca935e47bfddc36f24d1f8cf5a5746aa653c1087472616e736665726780ef586ef5fff2b1ea837839d662a527cd9fc50000";
    size_t len = strlen(hex_str) / 2;
    uint8_t oep4_tx[len];
    for (size_t i = 0; i < len; i++) {
        sscanf(hex_str + 2 * i, "%2hhx", &oep4_tx[i]);
    }

    buffer_t buf = {.ptr = oep4_tx, .size = sizeof(oep4_tx), .offset = 0};
    parser_status_e status = oep4_neo_vm_transaction_deserialize(&buf, &tx);
    assert_int_equal(status, PARSING_OK);
}
*/
static void test_new_parse_tx(void **state) {
    (void) state;

    ont_transaction_t tx;
    const char *hex_str = "00c66b1405815d34e0e9ab73a175ec86ffb24aad5bee20f16a7cc8141451108489337c8055a9c1ed9158c947d22070d76a7cc808000064a7b3b6e00d6a7cc86c51c10a7472616e7366657256321400000000000000000000000000000000000000020068164f6e746f6c6f67792e4e61746976652e496e766f6b6500";
    size_t len = strlen(hex_str) / 2;
    uint8_t raw_tx[len];
    for (size_t i = 0; i < len; i++) {
        sscanf(hex_str + 2 * i, "%2hhx", &raw_tx[i]);
    }
    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    cfg_t TransferV2Tx[] = {
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00,0xc6,0x6b},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL,
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8},
            .data_len = 3
        },
        {
            .data_type = AMOUNT_DATA_TYPE,
            .data = NULL,
        },
        {//6a 7c c8 6c 51 c1 0a 74 72 61 6e 73 66 65 72 56 32 14
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x6a,0x7c,0xc8,0x6c,0x51,0xc1,0x0a,
                                  0x74,0x72,0x61,0x6e,0x73,0x66,0x65,
                                  0x72,0x56,0x32},
            .data_len = 17
        },
        {
            .data_type = ADDRESS_DATA_TYPE,
            .data = NULL
        },
        {
            .data_type = OP_CODE_DATA_TYPE,
            .data = (uint8_t []) {0x00, 0x68, 0x16, 0x4F, 0x6E, 0x74, 0x6F, 0x6C,
                                  0x6F, 0x67, 0x79, 0x2E, 0x4E, 0x61, 0x74, 0x69,
                                  0x76, 0x65, 0x2E, 0x49, 0x6E, 0x76, 0x6F, 0x6B,
                                  0x65, 0x00},
            .data_len = 26
        }
    };
    uint8_t *resultArray[MAX_RESULT_SIZE] = {0};
    uint8_t storage[MAX_RESULT_SIZE][VALUE_SIZE] = {0};
    size_t numElements = sizeof(TransferV2Tx) / sizeof(TransferV2Tx[0]);
    parser_status_e status_tx = parse_tx(buf,TransferV2Tx,numElements,NATIVE_VM_OPERATOR,resultArray, storage);
    assert_int_equal(status_tx, PARSING_OK);
}

int main() {
	const struct CMUnitTest tests[] = {/*cmocka_unit_test(test_ont_tx_serialization),
                cmocka_unit_test(test_oep4_transaction),*/
                                           cmocka_unit_test(test_new_parse_tx)
	};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
