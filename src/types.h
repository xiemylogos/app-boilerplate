#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include "bip32.h"

#include "constants.h"
#include "transaction/types.h"
#include "person-msg/types.h"

typedef enum {
    PARSING_OK = 1,
    NONCE_PARSING_ERROR = -1,
    TO_PARSING_ERROR = -2,
    VALUE_PARSING_ERROR = -3,
    MEMO_LENGTH_ERROR = -4,
    MEMO_PARSING_ERROR = -5,
    MEMO_ENCODING_ERROR = -6,
    WRONG_LENGTH_ERROR = -7,
    VERSION_PARSING_ERROR = -8,
    TXTYPE_PARSING_ERROR = -9,
    GASPRICE_PARSING_ERROR = -10,
    GASLIMIT_PARSING_ERROR = -11,
    PAYER_PARSING_ERROR = -12,
    PAYLOAD_LEN_PARSING_ERROR = -13,
    PAYLOAD_PARSING_ERROR = -14,
    PARSE_STRING_MATCH_ERROR = -15,
    PERSON_MESSAGE_LENGTH_ERROR = -16,
    PERSON_MESSAGE_ARSING_ERROR = -17
} parser_status_e;

/**
 * Enumeration with expected INS of APDU commands.
 */
typedef enum {
    GET_VERSION = 0x03,              /// version of the application
    GET_APP_NAME = 0x04,             /// name of the application
    GET_PUBLIC_KEY = 0x05,           /// public key of corresponding BIP32 path
    SIGN_TX = 0x06,                  /// sign transaction with BIP32 path
    SIGN_PERSONAL_MESSAGE = 0x07,    /// sign person message
    SIGN_OEP4_TX = 0x08              /// sign OEP4 transaction with BIP32 path
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
    CONFIRM_ADDRESS,          /// confirm address derived from public key
    CONFIRM_TRANSACTION,      /// confirm transaction information
    CONFIRM_MESSAGE,          /// confirm message information
    CONFIRM_OEP4_TRANSACTION  /// confirm oep4 transaction information
} request_type_e;

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
    ont_transaction_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;                            /// parity of y-coordinate of R in ECDSA signature
} transaction_ctx_t;

typedef struct {
    uint8_t raw_msg[MAX_PERSON_MSG_LEN];  /// raw transaction serialized
    size_t raw_msg_len;
    person_msg_info  msg_info;
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
} person_msg_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    ont_transaction_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;                            /// parity of y-coordinate of R in ECDSA signature
} oep4_transaction_ctx_t;

/**
 * Structure for global context.
 */
typedef struct {
    state_e state;  /// state of the context
    union {
        pubkey_ctx_t pk_info;                 /// public key context
        transaction_ctx_t tx_info;            /// transaction context
        person_msg_ctx_t person_msg_info;     /// person msg context
        oep4_transaction_ctx_t oep4_tx_info;  ///oep4 transaction context
    };
    request_type_e req_type;                  /// user request
    uint32_t bip32_path[MAX_BIP32_PATH];      /// BIP32 path
    uint8_t bip32_path_len;                   /// length of BIP32 path
} global_ctx_t;
