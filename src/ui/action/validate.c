/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include <stdbool.h>  // bool

#include "crypto_helpers.h"

#include "validate.h"
#include "../menu.h"
#include "../../sw.h"
#include "../../globals.h"
#include "../../helper/send_response.h"

void validate_pubkey(bool choice) {
    if (choice) {
        helper_send_response_pubkey();
    } else {
        io_send_sw(SW_DENY);
    }
}

static int crypto_sign_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.tx_info.m_hash,
                                                      sizeof(G_context.tx_info.m_hash),
                                                      G_context.tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.tx_info.signature);

    G_context.tx_info.signature_len = sig_len;
    G_context.tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_person_message(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.person_msg_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.person_msg_info.m_hash,
                                                      sizeof(G_context.person_msg_info.m_hash),
                                                      G_context.person_msg_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.person_msg_info.signature);

    G_context.person_msg_info.signature_len = sig_len;
    G_context.person_msg_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_person_msg(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_person_message() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_person_msg_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_oep4_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.oep4_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.oep4_tx_info.m_hash,
                                                      sizeof(G_context.oep4_tx_info.m_hash),
                                                      G_context.oep4_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.oep4_tx_info.signature);

    G_context.oep4_tx_info.signature_len = sig_len;
    G_context.oep4_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_oep4_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_oep4_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_oep4_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_register_candidate_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.register_candidate_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.register_candidate_tx_info.m_hash,
                                                      sizeof(G_context.register_candidate_tx_info.m_hash),
                                                      G_context.register_candidate_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.register_candidate_tx_info.signature);

    G_context.register_candidate_tx_info.signature_len = sig_len;
    G_context.register_candidate_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_register_candidate_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_register_candidate_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_register_candidate_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_withdraw_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.withdraw_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.withdraw_tx_info.m_hash,
                                                      sizeof(G_context.withdraw_tx_info.m_hash),
                                                      G_context.withdraw_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.withdraw_tx_info.signature);

    G_context.withdraw_tx_info.signature_len = sig_len;
    G_context.withdraw_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_withdraw_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_withdraw_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_withdraw_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_quit_node_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.quit_node_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.quit_node_tx_info.m_hash,
                                                      sizeof(G_context.quit_node_tx_info.m_hash),
                                                      G_context.quit_node_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.quit_node_tx_info.signature);

    G_context.quit_node_tx_info.signature_len = sig_len;
    G_context.quit_node_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_quit_node_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_quit_node_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_quit_node_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_add_init_pos_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.add_init_pos_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.add_init_pos_tx_info.m_hash,
                                                      sizeof(G_context.add_init_pos_tx_info.m_hash),
                                                      G_context.add_init_pos_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.add_init_pos_tx_info.signature);

    G_context.add_init_pos_tx_info.signature_len = sig_len;
    G_context.add_init_pos_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_add_init_pos_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_add_init_pos_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_add_init_pos_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_reduce_init_pos_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.reduce_init_pos_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.reduce_init_pos_tx_info.m_hash,
                                                      sizeof(G_context.reduce_init_pos_tx_info.m_hash),
                                                      G_context.reduce_init_pos_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.reduce_init_pos_tx_info.signature);

    G_context.reduce_init_pos_tx_info.signature_len = sig_len;
    G_context.reduce_init_pos_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_reduce_init_pos_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_reduce_init_pos_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_reduce_init_pos_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}


static int crypto_sign_change_max_authorization_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.change_max_authorization_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.change_max_authorization_tx_info.m_hash,
                                                      sizeof(G_context.change_max_authorization_tx_info.m_hash),
                                                      G_context.change_max_authorization_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.change_max_authorization_tx_info.signature);

    G_context.change_max_authorization_tx_info.signature_len = sig_len;
    G_context.change_max_authorization_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_change_max_authorization_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_change_max_authorization_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_change_max_authorization_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}

static int crypto_sign_set_fee_percentage_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.set_fee_percentage_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.set_fee_percentage_tx_info.m_hash,
                                                      sizeof(G_context.set_fee_percentage_tx_info.m_hash),
                                                      G_context.set_fee_percentage_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.set_fee_percentage_tx_info.signature);

    G_context.set_fee_percentage_tx_info.signature_len = sig_len;
    G_context.set_fee_percentage_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_set_fee_percentage_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_set_fee_percentage_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_set_fee_percentage_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}

static int crypto_sign_authorize_for_peer_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.authorize_for_peer_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.authorize_for_peer_tx_info.m_hash,
                                                      sizeof(G_context.authorize_for_peer_tx_info.m_hash),
                                                      G_context.authorize_for_peer_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.authorize_for_peer_tx_info.signature);

    G_context.authorize_for_peer_tx_info.signature_len = sig_len;
    G_context.authorize_for_peer_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_authorize_for_peer_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_authorize_for_peer_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_authorize_for_peer_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}

static int crypto_sign_un_authorize_for_peer_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.un_authorize_for_peer_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.un_authorize_for_peer_tx_info.m_hash,
                                                      sizeof(G_context.un_authorize_for_peer_tx_info.m_hash),
                                                      G_context.un_authorize_for_peer_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.un_authorize_for_peer_tx_info.signature);

    G_context.un_authorize_for_peer_tx_info.signature_len = sig_len;
    G_context.un_authorize_for_peer_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_un_authorize_for_peer_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_un_authorize_for_peer_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_un_authorize_for_peer_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}

static int crypto_sign_withdraw_ong_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.withdraw_ong_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.withdraw_ong_tx_info.m_hash,
                                                      sizeof(G_context.withdraw_ong_tx_info.m_hash),
                                                      G_context.withdraw_ong_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.withdraw_ong_tx_info.signature);

    G_context.withdraw_ong_tx_info.signature_len = sig_len;
    G_context.withdraw_ong_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_withdraw_ong_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_withdraw_ong_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_withdraw_ong_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}

static int crypto_sign_withdraw_fee_tx(void) {
    uint32_t info = 0;
    size_t sig_len = sizeof(G_context.withdraw_fee_tx_info.signature);

    cx_err_t error = bip32_derive_ecdsa_sign_hash_256(CX_CURVE_256R1,
                                                      G_context.bip32_path,
                                                      G_context.bip32_path_len,
                                                      CX_RND_RFC6979 | CX_LAST,
                                                      CX_SHA256,
                                                      G_context.withdraw_fee_tx_info.m_hash,
                                                      sizeof(G_context.withdraw_fee_tx_info.m_hash),
                                                      G_context.withdraw_fee_tx_info.signature,
                                                      &sig_len,
                                                      &info);
    if (error != CX_OK) {
        return -1;
    }

    PRINTF("Signature: %.*H\n", sig_len, G_context.withdraw_fee_tx_info.signature);

    G_context.withdraw_fee_tx_info.signature_len = sig_len;
    G_context.withdraw_fee_tx_info.v = (uint8_t)(info & CX_ECCINFO_PARITY_ODD);

    return 0;
}

void validate_withdraw_fee_transaction(bool choice) {
    if (choice) {
        G_context.state = STATE_APPROVED;

        if (crypto_sign_withdraw_fee_tx() != 0) {
            G_context.state = STATE_NONE;
            io_send_sw(SW_SIGNATURE_FAIL);
        } else {
            helper_withdraw_fee_tx_send_response_sig();
        }
    } else {
        G_context.state = STATE_NONE;
        io_send_sw(SW_DENY);
    }
}
