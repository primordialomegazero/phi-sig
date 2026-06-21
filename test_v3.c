#include "phi_sig_v3.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG v3.0 — POST-QUANTUM POST-KEY SIGNATURE             ║\n");
    printf("║  Core: Falcon-1024 (NIST FIPS 206 Level 5)                ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    const char* msg = "ΦΩ0 — Post-Quantum Post-Key Test";
    const char* secret = "alice_observer_secret_key";
    const char* wrong = "bob_attacker_secret";
    
    uint8_t sig[V3_SIZE];
    size_t sig_len = sizeof(sig);
    
    // SIGN
    printf("=== SIGN (Falcon-1024, NIST Level 5) ===\n");
    int ok = phi_sign_v3((const uint8_t*)msg, strlen(msg),
                          (const uint8_t*)secret, strlen(secret),
                          sig, &sig_len);
    printf("Sign: %s (sig=%zu bytes)\n\n", ok ? "OK ✅" : "FAIL ❌", sig_len);
    
    // VERIFY correct
    printf("=== VERIFY (correct secret) ===\n");
    printf("%s\n", phi_verify_v3((const uint8_t*)msg, strlen(msg),
        (const uint8_t*)secret, strlen(secret), sig, sig_len) ? 
        "VALID ✅ (Falcon-1024 + φ-proof)" : "INVALID ❌");
    
    // VERIFY wrong
    printf("=== VERIFY (wrong secret) ===\n");
    printf("%s\n", phi_verify_v3((const uint8_t*)msg, strlen(msg),
        (const uint8_t*)wrong, strlen(wrong), sig, sig_len) ? 
        "VALID ❌ (should fail!)" : "INVALID ✅ (observer-bound!)");
    
    // TAMPER
    char bad_msg[256]; strcpy(bad_msg, msg); bad_msg[0] ^= 0xFF;
    printf("=== TAMPERED MESSAGE ===\n");
    printf("%s\n", phi_verify_v3((const uint8_t*)bad_msg, strlen(bad_msg),
        (const uint8_t*)secret, strlen(secret), sig, sig_len) ? 
        "VALID ❌" : "INVALID ✅ (tamper-evident!)");
    
    uint8_t bad_sig[V3_SIZE];
    memcpy(bad_sig, sig, sig_len); bad_sig[100] ^= 0xFF;
    printf("=== TAMPERED SIGNATURE ===\n");
    printf("%s\n", phi_verify_v3((const uint8_t*)msg, strlen(msg),
        (const uint8_t*)secret, strlen(secret), bad_sig, sig_len) ? 
        "VALID ❌" : "INVALID ✅ (tamper-evident!)");
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG v3.0 — POST-QUANTUM POST-KEY                      ║\n");
    printf("║  Falcon-1024 (NIST Level 5) | Observer-Secret | φ-Proof  ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}
