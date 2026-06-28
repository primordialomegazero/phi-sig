#include "phi_sig.h"
#include <oqs/oqs.h>
#include <string.h>
#include <stdlib.h>

static OQS_SIG *global_ml = NULL;
static uint8_t *global_pk = NULL;
static uint8_t *global_sk = NULL;
static size_t pk_len = 0, sk_len = 0, sig_len_max = 0;
static int keys_initialized = 0;

static void init_keys(void) {
    if (!keys_initialized) {
        global_ml = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
        if (global_ml) {
            pk_len = global_ml->length_public_key;
            sk_len = global_ml->length_secret_key;
            sig_len_max = global_ml->length_signature;
            global_pk = malloc(pk_len);
            global_sk = malloc(sk_len);
            if (OQS_SIG_keypair(global_ml, global_pk, global_sk) == OQS_SUCCESS)
                keys_initialized = 1;
        }
    }
}

int phi_pq_sign(const uint8_t *msg, size_t msg_len,
                uint8_t *sig, size_t *sig_len) {
    init_keys();
    if (!keys_initialized) return -1;

    uint8_t phi_sig[PHI_SIG_BYTES];
    size_t phi_len = PHI_SIG_BYTES;
    if (phi_sign(msg, msg_len, phi_sig, &phi_len) != 0) return -1;

    uint8_t *ml_sig = malloc(sig_len_max);
    size_t ml_len = sig_len_max;

    if (OQS_SIG_sign(global_ml, ml_sig, &ml_len, phi_sig, PHI_SIG_BYTES, global_sk) != OQS_SUCCESS) {
        free(ml_sig); return -1;
    }

    // Total = φ-sig (98) + ML-DSA-87 sig (actual ml_len) + PK
    size_t total = PHI_SIG_BYTES + ml_len + pk_len;
    if (!sig || *sig_len < total) { *sig_len = total; free(ml_sig); return -1; }

    memcpy(sig, phi_sig, PHI_SIG_BYTES);
    memcpy(sig + PHI_SIG_BYTES, ml_sig, ml_len);
    memcpy(sig + PHI_SIG_BYTES + ml_len, global_pk, pk_len);
    *sig_len = total;

    free(ml_sig);
    return 0;
}

int phi_pq_verify(const uint8_t *msg, size_t msg_len,
                  const uint8_t *sig, size_t sig_len) {
    init_keys();
    if (!keys_initialized) return 0;
    if (sig_len < PHI_SIG_BYTES + pk_len) return 0;

    // Verify φ-signature first (first 98 bytes)
    if (!phi_verify(msg, msg_len, sig, PHI_SIG_BYTES)) return 0;

    // ML-DSA-87 sig is between φ-sig and PK
    size_t ml_sig_len = sig_len - PHI_SIG_BYTES - pk_len;
    
    // Verify ML-DSA-87: message = φ-sig (98 bytes), signed with ML-DSA-87
    if (OQS_SIG_verify(global_ml, sig + PHI_SIG_BYTES, ml_sig_len,
                       sig, PHI_SIG_BYTES, sig + PHI_SIG_BYTES + ml_sig_len) != OQS_SUCCESS)
        return 0;

    return 1;
}
