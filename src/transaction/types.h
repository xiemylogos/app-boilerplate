#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define ADDRESS_LEN  20
#define PEER_PUBKEY_LEN   66
#define PAYLOAD_MIN_LENGTH_LIMIT 44
#define PAYLOAD_TRANSFER_LEN  54
#define PAYLOAD_TRANSFER_FROM_LEN  58

#define MAX_RESULT_SIZE 20
#define VALUE_SIZE 8
#define OPCODE_VALUE  81
#define OPCODE_OPERATION_CODE  13139050 //6a7cc8
#define UINT64_T_BYTE_LEN 8
#define TWO_UINT64_T_BYTE_LEN 16

/** the length of a SHA256 hash */
#define SHA256_HASH_LEN 32

/** the current version of the address field */
#define ADDRESS_VERSION 23

/** length of tx.output.script_hash */
#define SCRIPT_HASH_LEN 20

/** length of the checksum used to convert a script_hash into an Address. */
#define SCRIPT_HASH_CHECKSUM_LEN 4

/** length of a Address before encoding, which is the length of <address_version>+<script_hash>+<checksum> */
#define ADDRESS_LEN_PRE (1 + SCRIPT_HASH_LEN + SCRIPT_HASH_CHECKSUM_LEN)

#define VERIFICATION_SCRIPT_LENGTH 35

#define UINT160_LEN 20

#define MAX_LENGTH 40                //Accommodates 128-bit maximum
#define BASE 10                      //Decimal
#define P64_R 6                      //2^64 % 10
#define P64_Q 1844674407370955161ULL //2^64 / 10

#define ONG_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02")
#define ONT_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01")
#define WTK_ADDR ("\x77\xF1\xFF\xE3\xAD\xA5\xDD\x78\x62\xF9\x60\x1F\x5A\x0A\x05\x8A\x6B\xD8\x27\x43")
#define MYT_ADDR ("\xff\x92\xa1\xa3\x41\x8d\x53\x68\x40\x05\xaf\x98\xd5\xf1\xad\xd0\x5f\x15\xed\x19")
#define WING_ADDR ("\x80\xef\x58\x6e\xf5\xff\xf2\xb1\xea\x83\x78\x39\xd6\x62\xa5\x27\xcd\x9f\xc5\x00")

#define  OntologyNativeInvoke  "Ontology.Native.Invoke"
#define  TransferV2  "transferV2"
#define  Transfer "transfer"
#define  RegisterCandidate "registerCandidate"
#define  Withdraw "withdraw"
#define  QuitNode "quitNode"
#define  AddInitPos "addInitPos"
#define  ReduceInitPos "reduceInitPos"
#define  ChangeMaxAuthorization "changeMaxAuthorization"
#define  SetFeePercentage "setFeePercentage"
#define  AuthorizeForPeer "authorizeForPeer"
#define  UnAuthorizeForPeer "unAuthorizeForPeer"
#define  WithdrawFee "withdrawFee"
#define  Approve   "approve"
#define  TransferFromV2  "transferFromV2"
#define  TransferFrom    "transferFrom"
#define  ApproveV2       "approveV2"

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
}transaction_header_t;
