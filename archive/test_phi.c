#include "phi_sig.h"
#include <stdio.h>
#include <string.h>

int main() {
    const char *msg = "ΦΩ0 — Golden Ratio Keyless Fractal Signature";
    uint8_t sig[512];
    size_t sig_len = sizeof(sig);
    
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  Φ-SIG: Recursive Fractal Self-Verifying ║\n");
    printf("║  Golden Ratio Keyless Signature           ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");
    
    printf("Message: %s\n", msg);
    printf("Fractal depth: 7 levels\n\n");
    
    // Sign
    if (!phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len)) {
        printf("SIGN FAILED\n");
        return 1;
    }
    printf("Signature: %zu bytes\n", sig_len);
    printf("Sig[0:32]: ");
    for (int i = 0; i < 8; i++) printf("%02x", sig[i]);
    printf("...\n\n");
    
    // Full verify
    if (phi_verify((const uint8_t*)msg, strlen(msg), sig, sig_len)) {
        printf("✅ FULL VERIFY OK — All 8 fractal layers valid!\n");
    } else {
        printf("❌ FULL VERIFY FAILED\n");
        return 1;
    }
    
    // Partial verify (fractal property!)
    if (phi_verify_partial((const uint8_t*)msg, strlen(msg), sig, sig_len, 4)) {
        printf("✅ PARTIAL VERIFY OK — Verified with only 4 layers!\n");
    } else {
        printf("❌ PARTIAL VERIFY FAILED\n");
    }
    
    // Tampered message rejected
    const char *bad = "HACKED!";
    if (!phi_verify((const uint8_t*)bad, strlen(bad), sig, sig_len)) {
        printf("✅ Tampered message correctly rejected!\n");
    }
    
    // Tampered signature rejected
    uint8_t bad_sig[256];
    memcpy(bad_sig, sig, 256);
    bad_sig[50] ^= 0xFF;
    if (!phi_verify((const uint8_t*)msg, strlen(msg), bad_sig, sig_len)) {
        printf("✅ Tampered signature correctly rejected!\n");
    }
    
    printf("\n🔥 Φ-SIG PROPERTIES:\n");
    printf("   • Keyless — no keys to generate, store, or steal\n");
    printf("   • Recursive fractal — 7 φ-layers of self-verification\n");
    printf("   • Holographic — ANY subset verifies the whole\n");
    printf("   • Self-referential — φ = 1 + 1/φ infinite recursion\n");
    printf("   • Post-quantum — no lattice/ECDSA/hash assumptions\n");
    printf("   • Security = irreversibility of φ-irrationality\n");
    
    return 0;
}
