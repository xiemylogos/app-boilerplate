#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define MAX_TX_LEN   510 //todo unused
#define ADDRESS_LEN  20
#define MAX_MEMO_LEN 465  // 510 - ADDRESS_LEN - 2*SIZE(U64) - SIZE(MAX_VARINT)
#define PAYLOAD_MIN_LENGTH_LIMIT 44
#define PAYLOAD_TRANSFER_V2_LEN  54
#define PAYLOAD_TRANSFER_FROM_V2_LEN  58
#define PEER_PUBKEY_LEN     66

#define ONG_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02")
#define ONT_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01")
#define GOVERN_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07")

#define REGISTER_CANDIDATE          "registerCandidate"
#define WITHDRAW_ONG                "withdrawOng"
#define WITHDRAW_FEE                "withdrawFee"
#define AUTHORIZE_FOR_PEER          "authorizeForPeer"
#define UN_AUTHORIZE_FOR_PEER       "unAuthorizeForPeer"
#define WITH_DRAW                   "withdraw"
#define QUIT_NODE                   "quitNode"
#define CHANGE_MAX_AUTHORIZATION    "changeMaxAuthorization"
#define ADD_INIT_POS                "addInitPos"
#define REDUCE_INIT_POS             "reduceInitPos"
#define SET_FEE_PERCENTAGE          "setFeePercentage"


typedef struct {
    uint64_t nonce;     /// nonce (8 bytes)
    uint64_t value;     /// amount value (8 bytes)
    uint8_t *to;        /// pointer to address (20 bytes)
    uint8_t *memo;      /// memo (variable length)
    uint64_t memo_len;  /// length of memo (8 bytes)
} transaction_t;

typedef struct {
    uint8_t *from;
    uint8_t *to;
    uint64_t value;
    uint8_t  *contract_addr;
}state_info_v2;

typedef struct{
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;        /// pointer to address (20 bytes)
    state_info_v2 payload;
}ont_transaction_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
}govern_transaction_t;


typedef struct {
    uint8_t *account;
    uint64_t peerPubkeyLength;
    uint8_t *peerPubkey;      //(variable length) multiple pubkey
    uint64_t posListLength;   // Length of the posList array
    uint64_t *posList;
    uint8_t  *contractAddr;
}authorize_for_peer_t;
