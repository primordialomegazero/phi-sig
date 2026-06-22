#include "phi_sig.h"
#include <oqs/oqs.h>
#include <string.h>
#include <stdlib.h>

static OQS_SIG *global_ml = NULL;
static uint8_t *global_pk = NULL;
static uint8_t *global_sk = NULL;
static size_t pk_len = 0, sk_len = 0, sig_len_fixed = 0;
static int keys_initialized = 0;

static void init_keys(void) {
    if (!keys_initialized) {
        global_ml = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
        if (global_ml) {
            pk_len = global_ml->length_public_key;
            sk_len = global_ml->length_secret_key;
            sig_len_fixed = global_ml->length_signature;
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
    
    uint8_t phi_sig[64];
    size_t phi_len = sizeof(phi_sig);
    if (phi_sign(msg, msg_len, phi_sig, &phi_len) != 0) return -1;
    
    uint8_t *ml_sig = malloc(sig_len_fixed);
    size_t ml_len = sig_len_fixed;
    
    if (OQS_SIG_sign(global_ml, ml_sig, &ml_len, phi_sig, 64, global_sk) != OQS_SUCCESS) {
        free(ml_sig); return -1;
    }
    
    size_t total = 64 + sig_len_fixed + pk_len;
    if (!sig || *sig_len < total) { *sig_len = total; free(ml_sig); return -1; }
    
    memcpy(sig, phi_sig, 64);
    memcpy(sig + 64, ml_sig, sig_len_fixed);
    memcpy(sig + 64 + sig_len_fixed, global_pk, pk_len);
    *sig_len = total;
    
    free(ml_sig);
    return 0;
}

int phi_pq_verify(const uint8_t *msg, size_t msg_len,
                  const uint8_t *sig, size_t sig_len) {
    init_keys();
    if (!keys_initialized) return 0;
    if (sig_len < 64 + sig_len_fixed + pk_len) return 0;
    
    const uint8_t *claimed_phi_sig = sig;
    const uint8_t *ml_sig = sig + 64;
    const uint8_t *pk = sig + 64 + sig_len_fixed;
    
    uint8_t recomputed[64];
    size_t rlen = 64;
    phi_sign(msg, msg_len, recomputed, &rlen);
    
    if (memcmp(claimed_phi_sig, recomputed, 64) != 0) return 0;
    
    return (OQS_SIG_verify(global_ml, recomputed, 64,
                            ml_sig, sig_len_fixed, pk) == OQS_SUCCESS);
}
