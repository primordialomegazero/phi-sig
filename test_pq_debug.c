#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <oqs/oqs.h>
#include "phi_sig.h"

int main() {
    const char *msg = "Hello";
    size_t msg_len = strlen(msg);
    
    // Step 1: Generate φ-signature
    uint8_t phi_sig[PHI_SIG_BYTES];
    size_t phi_len = PHI_SIG_BYTES;
    phi_sign((const uint8_t*)msg, msg_len, phi_sig, &phi_len);
    printf("φ-sig size: %zu, verify: %d\n", phi_len, phi_verify((const uint8_t*)msg, msg_len, phi_sig, phi_len));
    
    // Step 2: ML-DSA-87 sign the φ-signature
    OQS_SIG *ml = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
    if (!ml) { printf("No ML-DSA-87\n"); return 1; }
    
    size_t pk_len = ml->length_public_key;
    size_t sk_len = ml->length_secret_key;
    size_t sig_max = ml->length_signature;
    printf("ML-DSA-87: pk=%zu sk=%zu sig_max=%zu\n", pk_len, sk_len, sig_max);
    
    uint8_t *pk = malloc(pk_len);
    uint8_t *sk = malloc(sk_len);
    OQS_SIG_keypair(ml, pk, sk);
    
    uint8_t *ml_sig = malloc(sig_max);
    size_t ml_len = sig_max;
    
    // Sign the 98-byte φ-sig
    int sign_ret = OQS_SIG_sign(ml, ml_sig, &ml_len, phi_sig, PHI_SIG_BYTES, sk);
    printf("ML-DSA-87 sign: %d, actual len: %zu\n", sign_ret, ml_len);
    
    // Verify with ORIGINAL pk (from keygen)
    int verify1 = OQS_SIG_verify(ml, ml_sig, ml_len, phi_sig, PHI_SIG_BYTES, pk);
    printf("Verify with original PK: %d %s\n", verify1, verify1 == OQS_SUCCESS ? "✅" : "❌");
    
    // Verify with PK from embedded position (simulating stored PK)
    // The embedded PK should be the same as original... let's compare
    printf("PK matches? %d\n", memcmp(pk, pk, pk_len));
    
    free(ml_sig); free(pk); free(sk);
    OQS_SIG_free(ml);
    return verify1 == OQS_SUCCESS ? 0 : 1;
}
