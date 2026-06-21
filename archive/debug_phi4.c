#include "phi_sig.h"
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

extern void phi_fractal(const uint8_t *input, size_t input_len,
                         uint8_t *output, size_t output_len);

#define FRACTAL_DEPTH 7

int main() {
    const char* msg = "test";
    uint8_t sig[256];
    size_t sig_len = sizeof(sig);
    phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
    
    // Simulate phi_verify exactly
    printf("=== SIMULATING PHI_VERIFY ===\n");
    
    // Step 1: Core check
    uint8_t expected_core[32];
    SHA256((const uint8_t*)msg, strlen(msg), expected_core);
    phi_fractal(expected_core, 32, expected_core, 32);
    printf("Core: match=%d\n", memcmp(sig, expected_core, 32));
    
    // Step 2: Layer checks
    for (int depth = 1; depth < FRACTAL_DEPTH; depth++) {
        const uint8_t *prev = sig + (depth - 1) * 32;
        const uint8_t *layer = sig + depth * 32;
        
        uint8_t expected_layer[32];
        phi_fractal(prev, 32 * depth, expected_layer, 32);
        
        int cmp = memcmp(layer, expected_layer, 32);
        printf("Layer %d: prev_len=%d match=%d bytes_equal=%d\n", 
               depth, 32*depth, cmp,
               memcmp(layer, expected_layer, 32));
        
        // Print actual bytes
        printf("  layer:    ");
        for(int i=0;i<8;i++) printf("%02x", layer[i]);
        printf("\n  expected: ");
        for(int i=0;i<8;i++) printf("%02x", expected_layer[i]);
        printf("\n");
        
        if (cmp != 0) {
            // Find exactly which byte differs
            for(int i=0;i<32;i++) {
                if(layer[i] != expected_layer[i]) {
                    printf("  DIFF at byte %d: layer=%02x expected=%02x\n", i, layer[i], expected_layer[i]);
                    break;
                }
            }
        }
    }
    
    return 0;
}
