#include "phi_sig.h"
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

extern void phi_fractal(const uint8_t *input, size_t input_len,
                         uint8_t *output, size_t output_len);

int main() {
    const char* msg = "test";
    uint8_t sig[256];
    size_t sig_len = sizeof(sig);
    
    printf("=== PHI-SIG DEBUG ===\n\n");
    
    // Sign
    phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
    printf("Sign OK, sig_len=%zu\n", sig_len);
    
    // Manual verify step by step
    printf("\n--- Layer 0 (core) ---\n");
    uint8_t expected_core[32];
    SHA256((const uint8_t*)msg, strlen(msg), expected_core);
    
    uint8_t core_output[32];
    phi_fractal(expected_core, 32, core_output, 32);
    
    printf("sig[0:8]   = ");
    for(int i=0;i<8;i++) printf("%02x", sig[i]);
    printf("\ncore_out   = ");
    for(int i=0;i<8;i++) printf("%02x", core_output[i]);
    printf("\nMatch: %s\n", memcmp(sig, core_output, 32)==0 ? "YES" : "NO");
    
    // Layer 1
    printf("\n--- Layer 1 ---\n");
    uint8_t layer1_out[32];
    phi_fractal(sig, 32, layer1_out, 32);
    printf("sig[32:40] = ");
    for(int i=0;i<8;i++) printf("%02x", sig[32+i]);
    printf("\nlayer1_out = ");
    for(int i=0;i<8;i++) printf("%02x", layer1_out[i]);
    printf("\nMatch: %s\n", memcmp(sig+32, layer1_out, 32)==0 ? "YES" : "NO");
    
    // Check if phi_fractal is deterministic
    printf("\n--- Determinism check ---\n");
    uint8_t test[32] = {0};
    test[0] = 42;
    uint8_t out1[32], out2[32];
    phi_fractal(test, 32, out1, 32);
    phi_fractal(test, 32, out2, 32);
    printf("Same input -> same output: %s\n", memcmp(out1, out2, 32)==0 ? "YES" : "NO");
    
    return 0;
}
