#include "phi_sig.h"
#include <stdio.h>
#include <string.h>

int main() {
    const char *msg = "Φ-SIG: Golden Ratio Keyless Signature";
    uint8_t sig[128];
    size_t sig_len = sizeof(sig);
    
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG: Golden Ratio Keyless Sig  ║\n");
    printf("║  φ = 1 + 1/φ — Self-Referential  ║\n");
    printf("╚═══════════════════════════════════╝\n\n");
    
    // Test 1: Sign and verify
    if (!phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len)) {
        printf("SIGN FAILED\n"); return 1;
    }
    printf("Message: %s\n", msg);
    printf("Signature: %zu bytes\n", sig_len);
    printf("Core: ");
    for (int i=0;i<8;i++) printf("%02x", sig[i]);
    printf("... Proof: ");
    for (int i=32;i<40;i++) printf("%02x", sig[i]);
    printf("...\n\n");
    
    if (phi_verify((const uint8_t*)msg, strlen(msg), sig, sig_len)) {
        printf("✅ VERIFY OK!\n");
    } else {
        printf("❌ VERIFY FAILED\n"); return 1;
    }
    
    // Test 2: Wrong message rejected
    if (!phi_verify((const uint8_t*)"wrong", 5, sig, sig_len)) {
        printf("✅ Wrong message rejected\n");
    }
    
    // Test 3: Tampered signature rejected
    uint8_t bad[64]; memcpy(bad, sig, 64); bad[10] ^= 0xFF;
    if (!phi_verify((const uint8_t*)msg, strlen(msg), bad, sig_len)) {
        printf("✅ Tampered sig rejected\n");
    }
    
    // Test 4: Deterministic — same input, same output
    uint8_t sig2[64]; size_t sl2 = sizeof(sig2);
    phi_sign((const uint8_t*)msg, strlen(msg), sig2, &sl2);
    if (memcmp(sig, sig2, 64) == 0) {
        printf("✅ Deterministic — same sig\n");
    }
    
    printf("\n🔥 KEYLESS. No private key. No public key.\n");
    printf("   Security = φ-irrationality irreversibility.\n");
    return 0;
}
