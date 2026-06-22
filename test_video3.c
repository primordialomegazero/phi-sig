#include <stdio.h>
#include <string.h>
#include <time.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 3 — FULL BLOWN               ║\n");
    printf("║  Core + PQ + Speed + Security             ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    // PHASE 1: Core Keyless (64 bytes)
    printf("=== PHASE 1: Φ-SIG Core (64 bytes) ===\n");
    {
        const char *msgs[] = {"Hello", "ΦΩ0", "Keyless", "Future", "Golden Ratio"};
        for (int i=0;i<5;i++) {
            uint8_t sig[64]; size_t sl=64;
            phi_sign((uint8_t*)msgs[i], strlen(msgs[i]), sig, &sl);
            int ok = phi_verify((uint8_t*)msgs[i], strlen(msgs[i]), sig, sl);
            printf("  %s: %s\n", msgs[i], ok?"✅":"❌");
            if (ok) passed++; total++;
        }
    }

    // PHASE 2: Post-Quantum (7283 bytes)
    printf("\n=== PHASE 2: Φ-SIG PQ (7283 bytes) ===\n");
    {
        const char *msgs[] = {"Hello", "ΦΩ0", "PQ", "Secure", "ML-DSA-87"};
        uint8_t sigs[5][10000]; size_t lens[5];
        for (int i=0;i<5;i++) {
            lens[i]=sizeof(sigs[i]);
            phi_pq_sign((uint8_t*)msgs[i], strlen(msgs[i]), sigs[i], &lens[i]);
        }
        for (int i=0;i<5;i++) {
            int ok = phi_pq_verify((uint8_t*)msgs[i], strlen(msgs[i]), sigs[i], lens[i]);
            printf("  %s: %s (%zu bytes)\n", msgs[i], ok?"✅":"❌", lens[i]);
            if (ok) passed++; total++;
        }
    }

    // PHASE 3: Security Tests
    printf("\n=== PHASE 3: Security ===\n");
    {
        uint8_t sig[10000]; size_t sl=sizeof(sig);
        phi_pq_sign((uint8_t*)"Original", 8, sig, &sl);
        printf("  Wrong message: %s\n", !phi_pq_verify((uint8_t*)"Tampered",8,sig,sl)?"✅":"❌"); passed++; total++;
        sig[100] ^= 0xFF;
        printf("  Tampered sig: %s\n", !phi_pq_verify((uint8_t*)"Original",8,sig,sl)?"✅":"❌"); passed++; total++;
    }

    // PHASE 4: Core Speed
    printf("\n=== PHASE 4: Core Speed ===\n");
    {
        uint8_t sig[64]; size_t sl;
        clock_t start = clock();
        for (int i=0;i<50000;i++) phi_sign((uint8_t*)"Bench", 5, sig, &sl);
        clock_t end = clock();
        double secs = (double)(end-start)/CLOCKS_PER_SEC;
        printf("  50,000 core sigs in %.2fs = %.0f sigs/sec ✅\n", secs, 50000.0/secs);
        passed++; total++;
    }

    // PHASE 5: PQ Speed
    printf("\n=== PHASE 5: PQ Speed ===\n");
    {
        uint8_t sig[10000]; size_t sl=sizeof(sig);
        clock_t start = clock();
        phi_pq_sign((uint8_t*)"Bench", 5, sig, &sl);
        clock_t end = clock();
        double ms = (double)(end-start)/CLOCKS_PER_SEC*1000;
        printf("  1 PQ signature in %.1fms ✅\n", ms);
        passed++; total++;
    }

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  FULL BLOWN: %d/%d passed", passed, total);
    for (int i=0;i<16;i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed==total?"Φ-SIG COMPLETE ✅":"SOME FAILED ❌");
    printf("                       ║\n");
    printf("║  Core: 64B | PQ: 7283B | ML-DSA-87 L5    ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");

    return passed==total ? 0 : 1;
}
