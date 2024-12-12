#include "crypto.h"
#include "cx_errors.h"

int crypto_derive_private_key(cx_ecfp_private_key_t *private_key, const uint32_t *bip32_path, uint8_t bip32_path_len) {
    cx_err_t error = CX_OK;
    uint8_t raw_private_key[64] = {0};

    CX_CHECK(os_derive_bip32_with_seed_no_throw(0,
                                                CX_CURVE_256R1,
                                                bip32_path,
                                                bip32_path_len,
                                                raw_private_key,
                                                NULL,
                                                NULL,
                                                0));

    CX_CHECK(cx_ecfp_init_private_key_no_throw(CX_CURVE_256R1, raw_private_key, 32, private_key));

    end:
    explicit_bzero(&raw_private_key, sizeof(raw_private_key));
    if (error != CX_OK) {
        // Make sure the caller doesn't use uninitialized data in case
        // the return code is not checked.
        explicit_bzero(private_key, sizeof(cx_ecfp_256_private_key_t));
        return -1;
    }
    return 0;
}


int crypto_init_public_key(cx_ecfp_private_key_t *private_key,
                           cx_ecfp_public_key_t *public_key,
                           uint8_t raw_public_key[static 64]) {
    cx_err_t error = CX_OK;

    // generate corresponding public key
    CX_CHECK(cx_ecfp_generate_pair_no_throw(CX_CURVE_256R1, public_key, private_key, true));

    end:
    if (error != CX_OK) {
        return -1;
    }

    memcpy(raw_public_key, public_key->W + 1, 64);

    return 0;
}