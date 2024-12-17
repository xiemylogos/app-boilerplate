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
    PERSON_MESSAGE_PARSING_ERROR = -17,
    FROM_PARSING_ERROR = -18,
    CONTRACT_ADDR_PARSING_ERROR = -19,
    BUFFER_OFFSET_MOVE_ERROR = -20

} parser_status_e;

/**
 * Enumeration with expected INS of APDU commands.
 */
typedef enum {
    GET_VERSION = 0x03,                /// version of the application
    GET_APP_NAME = 0x04,               /// name of the application
    GET_PUBLIC_KEY = 0x05,             /// public key of corresponding BIP32 path
    SIGN_TX = 0x02,                    /// sign transaction with BIP32 path
    SIGN_PERSON_MESSAGE = 0x07,        /// sign person message
    SIGN_OEP4_TX = 0x08,                /// sign OEP4 transaction with BIP32 path
    SIGN_REGISTER_CANDIDATE = 0x09,     ///sign registerCandidate
    SIGN_WITH_DRAW  = 0x0A,             ///sign withdraw
    SIGN_QUIT_NODE  = 0x0B,             ///sign quitNode
    SIGN_ADD_INIT_POS = 0x0C,           ///sign addInitPos
    SIGN_REDUCE_INIT_POS = 0x0D,        ///sign  reduceInitPos
    SIGN_CHANGE_MAX_AUTHORIZATION = 0x0E,  ///sign changeMaxAuthorization
    SIGN_SET_FEE_PERCENT_AGE  = 0x0F,   ///sign setFeePercentage
    SIGN_AUTHORIZE_FOR_PEER = 0x10,     ///sign authorizeForPeer
    SIGN_UN_AUTHORIZE_FOR_PEER = 0x11,  ///sign unAuthorizeForPeer
    SIGN_WITH_DRAW_ONG  = 0x12,         ///sign withdrawOng
    SIGN_WITH_DRAW_FEE  = 0x13          ///sign withdrawFee
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
    CONFIRM_OEP4_TRANSACTION,            /// confirm oep4 transaction information
    CONFIRM_REGISTER_CANDIDATE,          ///confirm registerCandidate
    CONFIRM_WITHDRAW,                    ///confirm withdraw
    CONFIRM_QUIT_NODE,                   ///confirm quitNode
    CONFIRM_ADD_INIT_POS,                ///confirm addInitPos
    CONFIRM_REDUCE_INIT_POS,             ///confirm reduceInitPos
    CONFIRM_CHANGE_MAX_AUTHORIZATION,    ///confirm changeMaxAuthorization
    CONFIRM_SET_FEE_PERCENTAGE,          ///confirm setFeePercentage
    CONFIRM_AUTHORIZE_FOR_PEER,          ///confirm authorizeForPeer
    CONFIRM_UN_AUTHORIZE_FOR_PEER,       ///confirm unAuthorizeForPeer
    CONFIRM_WITHDRAW_ONG,                ///confirm withdrawOng
    CONFIRM_WITHDRAW_FEE                 ///confirm withdrawFee
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


typedef  struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    register_candidate_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}register_candidate_ctx_t;

typedef  struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    withdraw_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}withdraw_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    quit_node_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}quit_node_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    add_init_pos_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}add_init_pos_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    reduce_init_pos_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}reduce_init_pos_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    change_max_authorization_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}change_max_authorization_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    set_fee_percentage_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}set_fee_percentage_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    authorize_for_peer_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}authorize_for_peer_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    un_authorize_for_peer_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}un_authorize_for_peer_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    withdraw_ong_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}withdraw_ong_ctx_t;

typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];  /// raw transaction serialized
    size_t raw_tx_len;                    /// length of raw transaction
    withdraw_fee_t transaction;        /// structured transaction
    uint8_t m_hash[32];                   /// message hash digest
    uint8_t signature[MAX_DER_SIG_LEN];   /// transaction signature encoded in DER
    uint8_t signature_len;                /// length of transaction signature
    uint8_t v;
}withdraw_fee_ctx_t;
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
        register_candidate_ctx_t register_candidate_tx_info; ///registerCandidate transaction context
        withdraw_ctx_t  withdraw_tx_info;                    ///withdraw transaction context
        quit_node_ctx_t  quit_node_tx_info;                  ///quitNode transaction context
        add_init_pos_ctx_t  add_init_pos_tx_info;            ///addInitPos transaction context
        reduce_init_pos_ctx_t reduce_init_pos_tx_info;       ///reduceInitPos transaction context
        change_max_authorization_ctx_t change_max_authorization_tx_info; ///changeMaxAuthorization transaction context
        set_fee_percentage_ctx_t  set_fee_percentage_tx_info;  ///setFeePercentage transaction context
        authorize_for_peer_ctx_t authorize_for_peer_tx_info;   ///authorizeForPeer transaction context
        un_authorize_for_peer_ctx_t un_authorize_for_peer_tx_info; ///unAuthorizeForPeer transaction context
        withdraw_ong_ctx_t  withdraw_ong_tx_info;             ///withdrawOng transaction context
        withdraw_fee_ctx_t  withdraw_fee_tx_info;             ///withdrawFee transaction context
    };
    request_type_e req_type;                  /// user request
    uint32_t bip32_path[MAX_BIP32_PATH];      /// BIP32 path
    uint8_t bip32_path_len;                   /// length of BIP32 path
} global_ctx_t;
