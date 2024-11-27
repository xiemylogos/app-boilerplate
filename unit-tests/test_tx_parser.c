#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/serialize.h"
#include "transaction/deserialize.h"
#include "types.h"
#include "format.h"

static void test_tx_serialization(void **state) {
    (void) state;

    transaction_t tx;
    // clang-format off
    uint8_t raw_tx[] = {
        // nonce (8)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        // to (20)
        0x7a, 0xc3, 0x39, 0x97, 0x54, 0x4e, 0x31, 0x75,
        0xd2, 0x66, 0xbd, 0x02, 0x24, 0x39, 0xb2, 0x2c,
        0xdb, 0x16, 0x50, 0x8c,
        // value (8)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x08, 0x07,
        // memo length (varint: 1-9)
        0xf1,
        // memo (var: 241)
        0x54, 0x68, 0x65, 0x20, 0x54, 0x68, 0x65, 0x6f,
        0x72, 0x79, 0x20, 0x6f, 0x66, 0x20, 0x47, 0x72,
        0x6f, 0x75, 0x70, 0x73, 0x20, 0x69, 0x73, 0x20,
        0x61, 0x20, 0x62, 0x72, 0x61, 0x6e, 0x63, 0x68,
        0x20, 0x6f, 0x66, 0x20, 0x6d, 0x61, 0x74, 0x68,
        0x65, 0x6d, 0x61, 0x74, 0x69, 0x63, 0x73, 0x20,
        0x69, 0x6e, 0x20, 0x77, 0x68, 0x69, 0x63, 0x68,
        0x20, 0x6f, 0x6e, 0x65, 0x20, 0x64, 0x6f, 0x65,
        0x73, 0x20, 0x73, 0x6f, 0x6d, 0x65, 0x74, 0x68,
        0x69, 0x6e, 0x67, 0x20, 0x74, 0x6f, 0x20, 0x73,
        0x6f, 0x6d, 0x65, 0x74, 0x68, 0x69, 0x6e, 0x67,
        0x20, 0x61, 0x6e, 0x64, 0x20, 0x74, 0x68, 0x65,
        0x6e, 0x20, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x72,
        0x65, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x72,
        0x65, 0x73, 0x75, 0x6c, 0x74, 0x20, 0x77, 0x69,
        0x74, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x72,
        0x65, 0x73, 0x75, 0x6c, 0x74, 0x20, 0x6f, 0x62,
        0x74, 0x61, 0x69, 0x6e, 0x65, 0x64, 0x20, 0x66,
        0x72, 0x6f, 0x6d, 0x20, 0x64, 0x6f, 0x69, 0x6e,
        0x67, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x61,
        0x6d, 0x65, 0x20, 0x74, 0x68, 0x69, 0x6e, 0x67,
        0x20, 0x74, 0x6f, 0x20, 0x73, 0x6f, 0x6d, 0x65,
        0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x65, 0x6c,
        0x73, 0x65, 0x2c, 0x20, 0x6f, 0x72, 0x20, 0x73,
        0x6f, 0x6d, 0x65, 0x74, 0x68, 0x69, 0x6e, 0x67,
        0x20, 0x65, 0x6c, 0x73, 0x65, 0x20, 0x74, 0x6f,
        0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x61, 0x6d,
        0x65, 0x20, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x2e,
        0x20, 0x4e, 0x65, 0x77, 0x6d, 0x61, 0x6e, 0x2c,
        0x20, 0x4a, 0x61, 0x6d, 0x65, 0x73, 0x20, 0x52,
        0x2e
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);

    assert_int_equal(status, PARSING_OK);

    uint8_t output[300];
    int length = transaction_serialize(&tx, output, sizeof(output));
    assert_int_equal(length, sizeof(raw_tx));
    assert_memory_equal(raw_tx, output, sizeof(raw_tx));
}

static void test_ont_tx_serialization(void **state) {
    (void) state;

    ont_transaction_t tx;
    // clang-format off
    uint8_t payload_tx[] = {
        0, 198, 107, 20, 5, 129, 93, 52, 224, 233, 171, 115, 161, 117, 236, 134,
        255, 178, 74, 173, 91, 238, 32, 241, 106, 124, 200, 20, 20, 81, 16, 132,
        137, 51, 124, 128, 85, 169, 193, 237, 145, 88, 201, 71, 210, 32, 112, 215,
        106, 124, 200, 8, 0, 0, 100, 167, 179, 182, 224, 13, 106, 124, 200, 108,
        81, 193, 10, 116, 114, 97, 110, 115, 102, 101, 114, 86, 50, 20, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 104, 22, 79, 110,
        116, 111, 108, 111, 103, 121, 46, 78, 97, 116, 105, 118, 101, 46, 73,
        110, 118, 111, 107, 101
    };

    uint8_t raw_tx[] = {
        0, 209, 21, 174, 2, 171, 196, 9, 0, 0, 0, 0, 0, 0, 32, 78,
        0, 0, 0, 0, 0, 0, 5, 129, 93, 52, 224, 233, 171, 115, 161, 117,
        236, 134, 255, 178, 74, 173, 91, 238, 32, 241, 123, 0, 198, 107,
        20, 5, 129, 93, 52, 224, 233, 171, 115, 161, 117, 236, 134, 255,
        178, 74, 173, 91, 238, 32, 241, 106, 124, 200, 20, 20, 81, 16,
        132, 137, 51, 124, 128, 85, 169, 193, 237, 145, 88, 201, 71, 210,
        32, 112, 215, 106, 124, 200, 8, 0, 0, 100, 167, 179, 182, 224,
        13, 106, 124, 200, 108, 81, 193, 10, 116, 114, 97, 110, 115, 102,
        101, 114, 86, 50, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 2, 0, 104, 22, 79, 110, 116, 111, 108, 111, 103,
        121, 46, 78, 97, 116, 105, 118, 101, 46, 73, 110, 118, 111, 107,
        101
    };

   uint8_t payer[] = {
        5, 129, 93, 52, 224, 233, 171, 115,
        161, 117, 236, 134, 255, 178, 74, 173,
        91, 238, 32, 241
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);
	
 //   char address[21] = {0};

  //   format_hex(tx.payer,20,address,sizeof(address));
   //  printf("address :%s\n",address);

    assert_int_equal(status, PARSING_OK);
    assert_int_equal(tx.version,0);
    assert_int_equal(tx.tx_type,209);
    assert_int_equal(tx.nonce,2869079573);
    assert_int_equal(tx.gas_price,2500);
    assert_int_equal(tx.gas_limit,20000);
    if(memcmp(tx.payer,payer,20) == 0 ) {
        assert_int_equal(20,20);
    } else {
        assert_string_equal(tx.payer,"abc");
    }
}



static void test_state_info_serialization(void **state) {
    (void) state;

    state_info_v2 info;
    // clang-format off
    uint8_t payload_tx[] = {
        0, 198, 107, 20, 5, 129, 93, 52, 224, 233, 171, 115, 161, 117, 236, 134,
        255, 178, 74, 173, 91, 238, 32, 241, 106, 124, 200, 20, 20, 81, 16, 132,
        137, 51, 124, 128, 85, 169, 193, 237, 145, 88, 201, 71, 210, 32, 112, 215,
        106, 124, 200, 8, 0, 0, 100, 167, 179, 182, 224, 13, 106, 124, 200, 108,
        81, 193, 10, 116, 114, 97, 110, 115, 102, 101, 114, 86, 50, 20, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 104, 22, 79, 110,
        116, 111, 108, 111, 103, 121, 46, 78, 97, 116, 105, 118, 101, 46, 73,
        110, 118, 111, 107, 101
    };

    buffer_t buf = {.ptr = payload_tx, .size = sizeof(payload_tx), .offset = 0};
    if (memcmp(buf->ptr+sizeof(payload_tx)-46-10,"transferV2",22) != 0) {
        assert_int_equal(sizeof(payload_tx)-46-10,67)
    }

    parser_status_e status = state_info_deserialize(&buf,sizeof(payload_tx), &info);
    assert_int_equal(info.value,1000000000000000000);
}

static void test_payer_address(void **state) {
    (void) state;

    uint8_t payer[] = {
        5, 129, 93, 52, 224, 233, 171, 115,
        161, 117, 236, 134, 255, 178, 74, 173,
        91, 238, 32, 241
    };

    char address[20] = {0};
    //base58_encode(payer, 20, address, sizeof(address));
    //base58_encode(payer, 20, address, sizeof(address));
    //assert_int_equal(base58_encode(payer, 20, address, sizeof(address)),1);
    assert_string_equal(payer,"abc");
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_ont_tx_serialization)};

    //const struct CMUnitTest tests[] = {cmocka_unit_test(test_state_info_serialization)};
    //const struct CMUnitTest tests[] = {cmocka_unit_test(test_payer_address)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
