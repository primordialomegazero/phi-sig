#include "phi_sig_v2.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG v2.0 — TRUE POST-KEY SIGNATURE                     ║\n");
    printf("║  Schnorr-like Σ-Protocol on secp256k1                      ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    const char* msg = "ΦΩ0 — True Post-Key Signature Test";
    const char* observer = "user@phi-sig";
    
    uint8_t sig[PHI_SIG_V2_SIG_SIZE];
    size_t sig_len = sizeof(sig);
    
    // SIGN
    printf("=== SIGN ===\n");
    int ok = phi_sign_v2((const uint8_t*)msg, strlen(msg),
                          (const uint8_t*)observer, strlen(observer),
                          sig, &sig_len);
    printf("Sign: %s (sig=%zu bytes)\n", ok ? "OK ✅" : "FAIL ❌", sig_len);
    
    // VERIFY (correct observer)
    printf("\n=== VERIFY (correct observer) ===\n");
    int v = phi_verify_v2((const uint8_t*)msg, strlen(msg),
                           (const uint8_t*)observer, strlen(observer),
                           sig, sig_len);
    printf("Verify: %s\n", v ? "VALID ✅ (s*G == R + c*Y)" : "INVALID ❌");
    
    // VERIFY (wrong observer)
    printf("\n=== VERIFY (wrong observer) ===\n");
    const char* wrong = "attacker@evil";
    int v2 = phi_verify_v2((const uint8_t*)msg, strlen(msg),
                            (const uint8_t*)wrong, strlen(wrong),
                            sig, sig_len);
    printf("Verify: %s\n", v2 ? "VALID ❌ (should fail)" : "INVALID ✅ (observer-bound!)");
    
    // TAMPERED MESSAGE
    printf("\n=== TAMPERED MESSAGE ===\n");
    char tampered[256];
    strcpy(tampered, msg);
    tampered[0] ^= 0xFF;
    int v3 = phi_verify_v2((const uint8_t*)tampered, strlen(tampered),
                            (const uint8_t*)observer, strlen(observer),
                            sig, sig_len);
    printf("Verify: %s\n", v3 ? "VALID ❌ (should fail)" : "INVALID ✅ (tamper-evident!)");
    
    // TAMPERED SIGNATURE
    printf("\n=== TAMPERED SIGNATURE ===\n");
    uint8_t tampered_sig[PHI_SIG_V2_SIG_SIZE];
    memcpy(tampered_sig, sig, sig_len);
    tampered_sig[40] ^= 0xFF;
    int v4 = phi_verify_v2((const uint8_t*)msg, strlen(msg),
                            (const uint8_t*)observer, strlen(observer),
                            tampered_sig, sig_len);
    printf("Verify: %s\n", v4 ? "VALID ❌ (should fail)" : "INVALID ✅ (tamper-evident!)");
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG v2.0 — TRUE CRYPTOGRAPHIC SIGNATURE                ║\n");
    printf("║  Equation: s*G == R + c*Y                                 ║\n");
    printf("║  Curve: secp256k1 (same as Bitcoin)                       ║\n");
    printf("║  Observer-Bound | Tamper-Evident | Post-Key              ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
