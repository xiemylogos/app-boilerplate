#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define MAX_TX_LEN   510 //todo unused
#define ADDRESS_LEN  20
#define MAX_MEMO_LEN 465  // 510 - ADDRESS_LEN - 2*SIZE(U64) - SIZE(MAX_VARINT)
#define PAYLOAD_MIN_LENGTH_LIMIT 44
#define PAYLOAD_TRANSFER_V2_LEN  54
#define PAYLOAD_TRANSFER_FROM_V2_LEN  58

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
#define  WithdrawOng "withdrawOng"
#define  WithdrawFee "withdrawFee"
#define  Approve   "approve"


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
    uint8_t value_len;
    uint64_t value[2];
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
    uint8_t *payer;        /// pointer to address (20 bytes)
    uint8_t *peer_pubkey;
    uint8_t *account;
    uint64_t init_pos;
    uint8_t ont_id_len;
    uint8_t *ont_id;
    uint8_t  key_no_len;
    uint64_t key_no;
}register_candidate_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *account;
    uint64_t peer_pubkey_length;
    uint8_t *peer_pubkey;
    uint64_t withdraw_list_number;
    uint8_t  withdraw_list_len;
    uint8_t *withdraw_list;
}withdraw_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *peer_pubkey;
    uint8_t *account;
}quit_node_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *peer_pubkey;
    uint8_t *account;
    uint64_t pos;
}add_init_pos_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *peer_pubkey;
    uint8_t *account;
    uint64_t pos;
}reduce_init_pos_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *peer_pubkey;
    uint8_t *account;
    uint64_t max_authorize;
}change_max_authorization_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *peer_pubkey;
    uint8_t *account;
    uint64_t peer_cost;
    uint64_t stake_cost;
}set_fee_percentage_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *account;
    uint64_t peer_pubkey_length;
    uint8_t *peer_pubkey;
    uint8_t pos_list_number;
    uint8_t  pos_list_len;
    uint8_t *pos_list;
}authorize_for_peer_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *account;
    uint64_t peer_pubkey_length;
    uint8_t *peer_pubkey;
    uint8_t pos_list_number;
    uint8_t  pos_list_len;
    uint8_t *pos_list;
}un_authorize_for_peer_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *account;
}withdraw_ong_t;

typedef struct {
    uint8_t version;
    uint8_t tx_type;
    uint32_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint8_t *payer;
    uint8_t *account;
}withdraw_fee_t;