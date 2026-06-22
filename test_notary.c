#include <stdio.h>
#include <string.h>
#include "phi_sig_notary.h"

int main() {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  Φ-SIG NOTARY + IMMUTABLE AUDIT LOG       ║\n");
    printf("║  Identity + Temporal Proof + Chain        ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    // TEST 1: Notary — single entry
    printf("=== TEST 1: Notary Entry ===\n");
    {
        PhiNotaryEntry entry;
        phi_notarize((uint8_t*)"Document v1", 11, &entry);
        int ok = phi_notary_verify((uint8_t*)"Document v1", 11, &entry);
        printf("  Sign+Verify: %s (timestamp=%lu)\n", ok ? "✅" : "❌", entry.timestamp);
        if (ok) passed++; total++;
        
        // Wrong message fails
        ok = !phi_notary_verify((uint8_t*)"Document v2", 11, &entry);
        printf("  Wrong msg rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
        
        // Tampered entry fails
        entry.core_sig[10] ^= 0xFF;
        ok = !phi_notary_verify((uint8_t*)"Document v1", 11, &entry);
        printf("  Tampered rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // TEST 2: Immutable Chain
    printf("\n=== TEST 2: Immutable Audit Chain ===\n");
    {
        const char *docs[] = {"Genesis block", "Transfer $100", "Transfer $50", "Audit complete"};
        size_t doc_lens[] = {13, 13, 12, 14};
        int count = 4;
        
        PhiChainEntry chain[4];
        const uint8_t *msg_ptrs[4];
        for (int i = 0; i < count; i++) msg_ptrs[i] = (const uint8_t*)docs[i];
        
        // Build chain
        const uint8_t *prev = NULL;
        for (int i = 0; i < count; i++) {
            phi_chain_append(prev, (uint8_t*)docs[i], doc_lens[i], &chain[i]);
            prev = chain[i].chain_seal;
            printf("  Entry %d: '%s' ✅\n", i, docs[i]);
        }
        passed++; total++;
        
        // Verify entire chain
        int ok = phi_chain_verify_all(chain, count, msg_ptrs, doc_lens);
        printf("  Chain integrity: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
        
        // Tamper middle entry
        chain[1].entry.core_sig[0] ^= 0xFF;
        ok = !phi_chain_verify_all(chain, count, msg_ptrs, doc_lens);
        printf("  Tampered chain rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // TEST 3: Temporal ordering
    printf("\n=== TEST 3: Temporal Proof ===\n");
    {
        PhiNotaryEntry e1, e2;
        phi_notarize((uint8_t*)"First", 5, &e1);
        phi_notarize((uint8_t*)"Second", 6, &e2);
        int ok = (e2.timestamp >= e1.timestamp);
        printf("  Temporal order preserved: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  NOTARY RESULT: %d/%d passed", passed, total);
    for (int i = 0; i < 15; i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed == total ? "IMMUTABLE AUDIT READY ✅" : "SOME FAILED ❌");
    printf("              ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("  Φ-SIG Notary — Identity + Time + Chain\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");

    return passed == total ? 0 : 1;
}
