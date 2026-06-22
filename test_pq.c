#include <stdio.h>
#include <string.h>
#include "phi_sig_pq.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG PQ — POST-QUANTUM TEST     ║\n");
    printf("║  Φ-SIG + ML-DSA-87 (NIST L5)      ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    printf("=== TEST 1: PQ Sign + Verify ===\n");
    const char *msgs[] = {"Hello", "ΦΩ0", "Post-Quantum", "Keyless", "Falcon"};
    uint8_t sigs[5][10000];
    size_t sig_lens[5];
    
    // Sign all first
    for (int i = 0; i < 5; i++) {
        sig_lens[i] = sizeof(sigs[i]);
        phi_pq_sign((uint8_t*)msgs[i], strlen(msgs[i]), sigs[i], &sig_lens[i]);
    }
    
    // Then verify all
    for (int i = 0; i < 5; i++) {
        int ok = phi_pq_verify((uint8_t*)msgs[i], strlen(msgs[i]), sigs[i], sig_lens[i]);
        printf("  '%s': %s (%zu bytes)\n", msgs[i], ok ? "✅" : "❌", sig_lens[i]);
        if (ok) passed++; total++;
    }

    printf("\n=== TEST 2: Wrong Message ===\n");
    {
        uint8_t sig[10000];
        size_t sig_len = sizeof(sig);
        phi_pq_sign((uint8_t*)"Correct", 7, sig, &sig_len);
        int ok = !phi_pq_verify((uint8_t*)"Wrong", 5, sig, sig_len);
        printf("  Wrong msg rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    printf("\n=== TEST 3: Tampered Signature ===\n");
    {
        uint8_t sig[10000];
        size_t sig_len = sizeof(sig);
        phi_pq_sign((uint8_t*)"Test", 4, sig, &sig_len);
        sig[10] ^= 0xFF;
        int ok = !phi_pq_verify((uint8_t*)"Test", 4, sig, sig_len);
        printf("  Tampered sig rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  PQ RESULT: %d/%d passed", passed, total);
    for (int i = 0; i < 12; i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed == total ? "POST-QUANTUM KEYLESS ✅" : "SOME FAILED ❌");
    printf("          ║\n");
    printf("╚═══════════════════════════════════╝\n");

    return passed == total ? 0 : 1;
}
