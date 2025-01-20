#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include "bip32.h"

#include "constants.h"
#include "transaction/types.h"
#include "personal-msg/types.h"

typedef enum {
    PARSING_OK = 1,
    NONCE_PARSING_ERROR = -1,
    TO_PARSING_ERROR = -2,
    VALUE_PARSING_ERROR = -3,
    PEER_PUBKEY_PARSING_ERROR = -4,
    ACCOUNT_PARSING_ERROR = -5,
    DATA_PARSING_ERROR = -6,
    WRONG_LENGTH_ERROR = -7,
    VERSION_PARSING_ERROR = -8,
    TXTYPE_PARSING_ERROR = -9,
    GASPRICE_PARSING_ERROR = -10,
    GASLIMIT_PARSING_ERROR = -11,
    PAYER_PARSING_ERROR = -12,
    PAYLOAD_LEN_PARSING_ERROR = -13,
    PAYLOAD_PARSING_ERROR = -14,
    PARSE_STRING_MATCH_ERROR = -15,
    PERSONAL_MESSAGE_LENGTH_ERROR = -16,
    PERSONAL_MESSAGE_PARSING_ERROR = -17,
    FROM_PARSING_ERROR = -18,
    CONTRACT_ADDR_PARSING_ERROR = -19,
    BUFFER_OFFSET_MOVE_ERROR = -20,
    OPCODE_PARSING_ERROR = -21,
    VALUE_LEN_PARSING_ERROR = -22,
    DATA_END_PARSING_ERROR = -23,
    TX_PARSING_ERROR = -24
} parser_status_e;

/**
 * Enumeration with expected INS of APDU commands.
 */
typedef enum {
    GET_VERSION = 0x03,                /// version of the application
    GET_APP_NAME = 0x04,               /// name of the application
    GET_PUBLIC_KEY = 0x05,             /// public key of corresponding BIP32 path
    SIGN_TX = 0x02,                    /// sign transaction with BIP32 path
    SIGN_PERSONAL_MESSAGE = 0x07,      /// sign personal message
} command_e;
/**
 * Enumeration with parsing state.
 */
typedef enum {
    STATE_NONE,     /// No state
    STATE_PARSED,   /// Transaction data parsed
    STATE_APPROVED  /// Transaction data approved
} state_e;

/**
 * Enumeration with user request type.
 */
typedef enum {
    CONFIRM_ADDRESS,                     /// confirm address derived from public key
    CONFIRM_TRANSACTION,                 /// confirm transaction information
    CONFIRM_MESSAGE,                     /// confirm message information
} request_type_e;

typedef enum {
    TRANSFER_V2_TRANSACTION,     /// confirm transferv2 transaction information
    OEP4_TRANSACTION,            /// confirm oep4 transaction information
    REGISTER_CANDIDATE,          ///confirm registerCandidate
    WITHDRAW,                    ///confirm withdraw
    QUIT_NODE,                   ///confirm quitNode
    ADD_INIT_POS,                ///confirm addInitPos
    REDUCE_INIT_POS,             ///confirm reduceInitPos
    CHANGE_MAX_AUTHORIZATION,    ///confirm changeMaxAuthorization
    SET_FEE_PERCENTAGE,          ///confirm setFeePercentage
    AUTHORIZE_FOR_PEER,          ///confirm authorizeForPeer
    UN_AUTHORIZE_FOR_PEER,       ///confirm unAuthorizeForPeer
    WITHDRAW_FEE,                ///confirm withdrawFee
    APPROVE,                     ///confirm approve
    TRANSFER_TRANSACTION,        ///confirm transfer
    TRANSFER_FROM_TRANSACTION,   ///confirm transfer from
    TRANSFER_FROM_V2_TRANSACTION,///confirm transfer from v2
    APPROVE_V2,                  ///confirm approve v2
    NEO_VM_OEP4_APPROVE,        ///confirm oep4 neo vm approve
    WASM_VM_OEP4_APPROVE,        ///confirm oep4 wasm vm approve
    NEO_VM_OEP4_TRANSFER_FROM,   ///confirm oep4 neo vm transfer from
    WASM_VM_OEP4_TRANSFER_FROM   ///confirm oep4 wasm vm transfer from
} tx_type_e;

typedef struct {
    char from[40];
    char to[40];
    char amount[40];
    char content[40];
    char signer[40];
    char content_two[40];
    char fee[20];
    char peer_pubkey[68];
    uint8_t decimals;
    uint8_t pubkey_number;
#if !defined(TARGET_NANOS)
    char content_three[68];
    char content_four[68];
#endif
}display_data_t;
/**
 * Structure for public key context information.
 */
typedef struct {
    uint8_t raw_public_key[65];  /// format (1), x-coordinate (32), y-coodinate (32)
    uint8_t chain_code[32];      /// for public key derivation
} pubkey_ctx_t;

/**
 * Structure for transaction information context.
 */
typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;                            /// parity of y-coordinate of R in ECDSA signature
} transaction_ctx_t;

typedef struct {
    uint8_t raw_msg[MAX_PERSONAL_MSG_LEN];  /// raw transaction serialized
    size_t raw_msg_len;
    personal_msg_info  msg_info;
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
} personal_msg_ctx_t;

/**
 * Structure for global context.
 */
typedef struct {
    state_e state;  /// state of the context
    union {
        pubkey_ctx_t pk_info;                 /// public key context
        transaction_ctx_t tx_info;            /// transaction context
        personal_msg_ctx_t personal_msg_info; /// personal msg context
    };
    tx_type_e tx_type;
    request_type_e req_type;                  /// user request
    display_data_t display_data;
    uint32_t bip32_path[MAX_BIP32_PATH];      /// BIP32 path
    uint8_t bip32_path_len;                   /// length of BIP32 path
} global_ctx_t;
