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
    
    printf("=== PHI-SIG FULL DEBUG ===\n\n");
    phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
    
    // Check ALL layers
    for (int depth = 1; depth < FRACTAL_DEPTH; depth++) {
        const uint8_t *prev = sig + (depth - 1) * 32;
        const uint8_t *layer = sig + depth * 32;
        
        uint8_t expected[32];
        phi_fractal(prev, 32 * depth, expected, 32);
        
        int match = memcmp(layer, expected, 32) == 0;
        printf("Layer %d: prev_len=%d ", depth, 32*depth);
        printf("match=%s\n", match ? "YES" : "NO");
        
        if (!match) {
            printf("  layer[0:8]    = ");
            for(int i=0;i<8;i++) printf("%02x", layer[i]);
            printf("\n  expected[0:8] = ");
            for(int i=0;i<8;i++) printf("%02x", expected[i]);
            printf("\n");
        }
    }
    
    // Holographic check
    printf("\n--- Holographic ---\n");
    uint8_t hologram_expected[32];
    SHA256(sig, 32 * FRACTAL_DEPTH, hologram_expected);
    phi_fractal(hologram_expected, 32, hologram_expected, 32);
    
    const uint8_t *hologram = sig + 32 * FRACTAL_DEPTH;
    int holo_match = memcmp(hologram, hologram_expected, 32) == 0;
    printf("Hologram match: %s\n", holo_match ? "YES" : "NO");
    if (!holo_match) {
        printf("  holo[0:8]    = ");
        for(int i=0;i<8;i++) printf("%02x", hologram[i]);
        printf("\n  expected[0:8]= ");
        for(int i=0;i<8;i++) printf("%02x", hologram_expected[i]);
        printf("\n");
    }
    
    // Also check the verify function's core check
    printf("\n--- Verify core logic ---\n");
    uint8_t expected_core[32];
    SHA256((const uint8_t*)msg, strlen(msg), expected_core);
    phi_fractal(expected_core, 32, expected_core, 32);
    printf("Core match: %s\n", memcmp(sig, expected_core, 32)==0 ? "YES" : "NO");
    
    return 0;
}
