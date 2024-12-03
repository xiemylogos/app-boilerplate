#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/serialize.h"
#include "transaction/deserialize.h"
#include "transaction/oep4_deserialize.h"
#include "person-msg/deserialize.h"
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
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 104, 22, 79, 110,
        116, 111, 108, 111, 103, 121, 46, 78, 97, 116, 105, 118, 101, 46, 73,
        110, 118, 111, 107, 101,0
    };
  
 uint8_t raw_tx[] = {
        0, 209, 21, 174, 2, 171, 196, 9, 0, 0, 0, 0, 0, 0, 32, 78,
        0, 0, 0, 0, 0, 0, 5, 129, 93, 52, 224, 233, 171, 115, 161, 117,
        236, 134, 255, 178, 74, 173, 91, 238, 32, 241, 123, 0, 198, 107, 
	20, 5, 129, 93, 52, 224, 233, 171, 115, 161, 117, 236, 134,
        255, 178, 74, 173, 91, 238, 32, 241, 106, 124, 200, 20, 20, 81, 16, 132,
        137, 51, 124, 128, 85, 169, 193, 237, 145, 88, 201, 71, 210, 32, 112, 215,
        106, 124, 200, 8, 0, 0, 100, 167, 179, 182, 224, 13, 106, 124, 200, 108,
        81, 193, 10, 116, 114, 97, 110, 115, 102, 101, 114, 86, 50, 20, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 104, 22, 79, 110,
        116, 111, 108, 111, 103, 121, 46, 78, 97, 116, 105, 118, 101, 46, 73,
        110, 118, 111, 107, 101,0
    };
   uint8_t payer[] = {
        5, 129, 93, 52, 224, 233, 171, 115,
        161, 117, 236, 134, 255, 178, 74, 173,
        91, 238, 32, 241
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);


    assert_int_equal(buf.size,167);
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
    assert_int_equal(tx.payload.value,1000000000000000000);
    uint8_t ONG_ADDR[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
    };
    if(memcmp(tx.payload.contract_addr,ONG_ADDR,20) == 0 ) {
        assert_int_equal(sizeof(ONG_ADDR),20);
    } else {
        assert_string_equal(tx.payload.contract_addr,"abc");
    }

    //parse state info
    /*
    parser_status_e status_info = state_info_deserialize(&buf,buf.size-buf.offset, &tx.payload);
    assert_int_equal(status_info, PARSING_OK);
    assert_int_equal(tx.payload.value,1000000000000000000);
    uint8_t ONG_ADDR[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
    };
    if(memcmp(tx.payload.contract_addr,ONG_ADDR,20) == 0 ) {
        assert_int_equal(sizeof(ONG_ADDR),20);
    } else {
        assert_string_equal(tx.payload.contract_addr,"abc");
    }
    */

/*
    assert_int_equal(buf.size-buf.offset,123);
    assert_int_equal(buf.offset,43);
    assert_int_equal(buf.size-buf.offset,123);
    assert_int_equal(sizeof(raw_tx)-buf.offset,123);
    
    assert_int_equal(buf1.size,123);
*/
	    
    /*
    state_info_v2 info;
    
    buffer_seek_cur(&buf1,4);
    info.from = (uint8_t*)(buf1.ptr+buf1.offset);
    if (!buffer_seek_cur(&buf1, 20)) {
	    assert_int_equal(1,2);
    }
    assert_int_equal(buf1.offset,24);
    buffer_seek_cur(&buf1,4); 
    info.to = (uint8_t*)(buf1.ptr+buf1.offset);
    if (!buffer_seek_cur(&buf, 20)) {
	    assert_int_equal(1,3);
    }
    assert_int_equal(buf.offset,48+43);
    buffer_seek_cur(&buf,4); 
    if (!buffer_read_u64(&buf, &info.value, LE)) {
	    return VALUE_PARSING_ERROR;
    }
    assert_int_equal(buf.offset,60+43);
    assert_int_equal(info.value,1000000000000000000);
    */
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
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 104, 22, 79, 110,
        116, 111, 108, 111, 103, 121, 46, 78, 97, 116, 105, 118, 101, 46, 73,
        110, 118, 111, 107, 101,0
    };
    uint8_t ONG_ADDR[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2
    };
    uint8_t ONT_ADDR[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
    };
    buffer_t buf = {.ptr = payload_tx, .size = sizeof(payload_tx), .offset = 0};
    assert_int_equal(sizeof(payload_tx),124);
    if (memcmp(buf.ptr+sizeof(payload_tx)-46-10-1,"transferV2",10) != 0) {
        assert_int_equal(sizeof(payload_tx)-46-10,68);
    }

    parser_status_e status = state_info_deserialize(&buf,sizeof(payload_tx), &info);
    assert_int_equal(status, PARSING_OK);
    assert_int_equal(info.value,1000000000000000000);
    if(memcmp(info.contract_addr,ONG_ADDR,20) == 0 ) {
        assert_int_equal(sizeof(ONG_ADDR),20);
    } else {
        assert_string_equal(info.contract_addr,"abc");
    }
}

static void test_person_msg(void **state) {
    (void) state;

    uint8_t raw__person_msg[] = {
        //person msg length
        0x0D,
        //person msg
        116,101,115,116,32,109,115,103,32,104,97,115,104
    };
    buffer_t buf = {.ptr = raw__person_msg, .size = sizeof(raw__person_msg), .offset = 0};
    assert_int_equal(buf.size,14);
    person_msg_info info;
    parser_status_e status = person_msg_deserialize(&buf, &info);
    assert_int_equal(status, PARSING_OK);
}

static void test_oep4_transaction(void **state) {
    (void) state;

    ont_transaction_t tx;
    uint8_t oep4_tx[] = {
        0, 210, 214, 79, 147, 85, 196, 9, 0, 0, 0, 0, 0, 0, 64, 156,
        0, 0, 0, 0, 0, 0, 20, 81, 16, 132, 137, 51, 124, 128, 85, 169,
        193, 237, 145, 88, 201, 71, 210, 32, 112, 215, 86, 28, 108, 152,
        194, 35, 244, 40, 93, 242, 72, 210, 207, 129, 251, 84, 183, 251,
        195, 20, 200, 65, 8, 116, 114, 97, 110, 115, 102, 101, 114, 20,
        81, 16, 132, 137, 51, 124, 128, 85, 169, 193, 237, 145, 88, 201,
        71, 210, 32, 112, 215, 236, 148, 65, 191, 50, 139, 108, 214, 96,
        123, 56, 71, 195, 11, 126, 174, 215, 86, 218, 190, 64, 66, 15,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    //AHdJGoAKkZDTGAAhQawKry6EE2CPdzhXk3
    uint8_t payer[] = {
        20, 81, 16,132,137,51,124,128,85,
        169,193,237,145,88,201,71,210,32,112,215
    };
    uint8_t to[] = {
        236,148,65,191,50,139,108,214, 96, 123, 56, 71, 195, 11, 126, 174, 215, 86, 218, 190
    };
    assert_int_equal(sizeof(oep4_tx),131);
    buffer_t buf = {.ptr = oep4_tx, .size = sizeof(oep4_tx), .offset = 0};
    parser_status_e status = oep4_transaction_deserialize(&buf, &tx);
    assert_int_equal(status, PARSING_OK);
    assert_int_equal(tx.version,0);
    assert_int_equal(tx.tx_type,210);
    assert_int_equal(tx.nonce,1435717590);
    assert_int_equal(tx.gas_price,2500);
    assert_int_equal(tx.gas_limit,40000);
    if(memcmp(tx.payer,payer,20) == 0 ) {
        assert_int_equal(20,20);
    } else {
        assert_string_equal(tx.payer,"oep4tx");
    }
    assert_int_equal(tx.payload.value,1000000);
    if(memcmp(tx.payload.from,payer,20) == 0 ) {
        assert_int_equal(20,20);
    } else {
        assert_string_equal(tx.payload.from,"oep4-from");
    }
    if(memcmp(tx.payload.to,to,20) == 0 ) {
        assert_int_equal(20,20);
    } else {
        assert_string_equal(tx.payload.to,"oep4-to");
    }
}

static void test_oep4_paylod(void **state) {
    (void) state;

    state_info_v2 info;
    uint8_t oep4_payload[] = {
        28, 108, 152,
        194, 35, 244, 40, 93, 242, 72, 210, 207, 129, 251, 84, 183, 251,
        195, 20, 200, 65, 8, 116, 114, 97, 110, 115, 102, 101, 114, 20,
        81, 16, 132, 137, 51, 124, 128, 85, 169, 193, 237, 145, 88, 201,
        71, 210, 32, 112, 215, 236, 148, 65, 191, 50, 139, 108, 214, 96,
        123, 56, 71, 195, 11, 126, 174, 215, 86, 218, 190, 64, 66, 15,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    //AHdJGoAKkZDTGAAhQawKry6EE2CPdzhXk3
    uint8_t from[] = {
        20, 81, 16,132,137,51,124,128,85,
        169,193,237,145,88,201,71,210,32,112,215
    };
    uint8_t to[] = {
        236,148,65,191,50,139,108,214, 96, 123, 56, 71, 195, 11, 126, 174, 215, 86, 218, 190
    };
    assert_int_equal(sizeof(oep4_payload),86);
    buffer_t buf = {.ptr = oep4_payload, .size = sizeof(oep4_payload), .offset = 0};
    parser_status_e status = oep4_state_info_deserialize(&buf,sizeof(oep4_payload), &info);
    assert_int_equal(status, PARSING_OK);
    assert_int_equal(info.value,1000000);
    if(memcmp(info.from,from,20) == 0 ) {
        assert_int_equal(20,20);
    } else {
        assert_string_equal(info.from,"oep4-from");
    }
    if(memcmp(info.to,to,20) == 0 ) {
        assert_int_equal(20,20);
    } else {
        assert_string_equal(info.to,"oep4-to");
    }
}

int main() {
	const struct CMUnitTest tests[] = {cmocka_unit_test(test_ont_tx_serialization),
		cmocka_unit_test(test_state_info_serialization),
                cmocka_unit_test(test_person_msg),
                cmocka_unit_test(test_oep4_transaction),
                cmocka_unit_test(test_oep4_paylod)
		/*cmocka_unit_test(test_payer_address)*/
	};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
