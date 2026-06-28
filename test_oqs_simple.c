#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <oqs/oqs.h>

int main() {
    const char *msg = "Hello World";
    size_t msg_len = strlen(msg);
    
    OQS_SIG *ml = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
    if (!ml) { printf("ML-DSA-87 not available\n"); return 1; }
    
    size_t pk_len = ml->length_public_key;
    size_t sk_len = ml->length_secret_key;
    size_t sig_max = ml->length_signature;
    printf("pk=%zu sk=%zu sig_max=%zu\n", pk_len, sk_len, sig_max);
    
    uint8_t *pk = malloc(pk_len);
    uint8_t *sk = malloc(sk_len);
    if (OQS_SIG_keypair(ml, pk, sk) != OQS_SUCCESS) { printf("keygen failed\n"); return 1; }
    
    uint8_t *sig = malloc(sig_max);
    size_t sig_len = sig_max;
    
    if (OQS_SIG_sign(ml, sig, &sig_len, (const uint8_t*)msg, msg_len, sk) != OQS_SUCCESS) {
        printf("sign failed\n"); return 1;
    }
    printf("Sign OK, sig_len=%zu\n", sig_len);
    
    int v = OQS_SIG_verify(ml, sig, sig_len, (const uint8_t*)msg, msg_len, pk);
    printf("Verify: %d %s\n", v, v == OQS_SUCCESS ? "✅" : "❌");
    
    free(sig); free(pk); free(sk);
    OQS_SIG_free(ml);
    return v == OQS_SUCCESS ? 0 : 1;
}
