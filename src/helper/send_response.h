#pragma once

#include "os.h"
#include "macros.h"

/**
 * Length of public key.
 */
#define PUBKEY_LEN (MEMBER_SIZE(pubkey_ctx_t, raw_public_key))
/**
 * Length of chain code.
 */
#define CHAINCODE_LEN (MEMBER_SIZE(pubkey_ctx_t, chain_code))

/**
 * Helper to send APDU response with public key and chain code.
 *
 * response = PUBKEY_LEN (1) ||
 *            G_context.pk_info.public_key (PUBKEY_LEN) ||
 *            CHAINCODE_LEN (1) ||
 *            G_context.pk_info.chain_code (CHAINCODE_LEN)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_send_response_pubkey(void);

/**
 * Helper to send APDU response with signature and v (parity of
 * y-coordinate of R).
 *
 * response = G_context.tx_info.signature_len (1) ||
 *            G_context.tx_info.signature (G_context.tx_info.signature_len) ||
 *            G_context.tx_info.v (1)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_tx_send_response_sig(void);

/**
 * Helper to send APDU response with signature and v (parity of
 * y-coordinate of R). for person msg
 *
 * response = G_context.person_msg_info.signature_len (1) ||
 *            G_context.person_msg_info.signature (G_context.person_msg_info.signature_len) ||
 *            G_context.person_msg_info.v (1)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_person_msg_send_response_sig(void);
/**
 * Helper to send APDU response with signature and v (parity of
 * y-coordinate of R).
 *
 * response = G_context.oep4_tx_info.signature_len (1) ||
 *            G_context.oep4_tx_info.signature (G_context.oep4_tx_info.signature_len) ||
 *            G_context.oep4_tx_info.v (1)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_oep4_tx_send_response_sig(void);