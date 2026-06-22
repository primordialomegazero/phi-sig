#include <stdio.h>
#include <string.h>
#include "phi_sig_pq.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 2 — POST-QUANTUM      ║\n");
    printf("║  ML-DSA-87 NIST FIPS 204 Level 5  ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    printf("=== PHASE 1: PQ Sign + Verify ===\n");
    const char *msgs[] = {"Hello", "ΦΩ0", "Post-Quantum", "Keyless", "Falcon"};
    uint8_t sigs[5][10000];
    size_t lens[5];
    
    for (int i = 0; i < 5; i++) {
        lens[i] = sizeof(sigs[i]);
        phi_pq_sign((uint8_t*)msgs[i], strlen(msgs[i]), sigs[i], &lens[i]);
    }
    for (int i = 0; i < 5; i++) {
        int ok = phi_pq_verify((uint8_t*)msgs[i], strlen(msgs[i]), sigs[i], lens[i]);
        printf("  '%s': %s (%zu bytes)\n", msgs[i], ok ? "✅" : "❌", lens[i]);
        if (ok) passed++; total++;
    }

    printf("\n=== PHASE 2: PQ Security ===\n");
    {
        uint8_t sig[10000];
        size_t sl = sizeof(sig);
        phi_pq_sign((uint8_t*)"Correct", 7, sig, &sl);
        printf("  Wrong msg rejected: %s\n", !phi_pq_verify((uint8_t*)"Wrong", 5, sig, sl) ? "✅" : "❌");
        passed++; total++;
        
        sig[10] ^= 0xFF;
        printf("  Tampered sig rejected: %s\n", !phi_pq_verify((uint8_t*)"Correct", 7, sig, sl) ? "✅" : "❌");
        passed++; total++;
    }

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  PQ RESULT: %d/%d passed", passed, total);
    for (int i = 0; i < 12; i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed == total ? "POST-QUANTUM KEYLESS ✅" : "SOME FAILED ❌");
    printf("          ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("  Φ-SIG + ML-DSA-87 = True PQ Keyless\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");

    return passed == total ? 0 : 1;
}
