#include "phi_sig.h"
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

extern void phi_fractal(const uint8_t *input, size_t input_len,
                         uint8_t *output, size_t output_len);

int main() {
    // Test if phi_fractal modifies input
    uint8_t input[64];
    memset(input, 0x42, 64);
    uint8_t input_copy[64];
    memcpy(input_copy, input, 64);
    
    uint8_t output[32];
    phi_fractal(input, 64, output, 32);
    
    int modified = memcmp(input, input_copy, 64);
    printf("Input modified by phi_fractal: %s\n", modified ? "YES ⚠️" : "NO ✅");
    
    if (modified) {
        printf("Differences:\n");
        for(int i=0;i<64;i++) {
            if(input[i] != input_copy[i]) {
                printf("  byte[%d]: was %02x, now %02x\n", i, input_copy[i], input[i]);
            }
        }
    }
    
    return 0;
}
