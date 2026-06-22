#include <stdio.h>
#include <string.h>
#include <oqs/oqs.h>

int main() {
    printf("=== PQ DIRECT TEST (no wrapper) ===\n");
    
    // Test Falcon-1024 directly
    const char *msg = "Hello PQ";
    
    // Sign
    uint8_t pk[OQS_SIG_falcon_1024_length_public_key];
    uint8_t sk[OQS_SIG_falcon_1024_length_secret_key];
    uint8_t sig[OQS_SIG_falcon_1024_length_signature];
    size_t sig_len = sizeof(sig);
    
    if (OQS_SIG_falcon_1024_keypair(pk, sk) != OQS_SUCCESS) {
        printf("KEYGEN FAILED\n"); return 1;
    }
    printf("Keygen: OK\n");
    
    if (OQS_SIG_falcon_1024_sign(sig, &sig_len, (uint8_t*)msg, 8, sk) != OQS_SUCCESS) {
        printf("SIGN FAILED\n"); return 1;
    }
    printf("Sign: OK (actual len=%zu)\n", sig_len);
    
    // Verify
    if (OQS_SIG_falcon_1024_verify((uint8_t*)msg, 8, sig, sig_len, pk) == OQS_SUCCESS) {
        printf("Verify: OK ✅\n");
    } else {
        printf("Verify: FAIL ❌\n");
    }
    
    return 0;
}
