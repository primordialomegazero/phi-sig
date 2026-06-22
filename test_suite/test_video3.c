#include <stdio.h>
#include <string.h>
#include <time.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    int passed = 0, total = 0;
    time_t start, end;
    
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 3 — FULL BLOWN        ║\n");
    printf("║  Core + PQ + 100K Speed           ║\n");
    printf("╚═══════════════════════════════════╝\n\n");
    
    // ==========================================
    // PHASE 1: Core Keyless (Quick verify)
    // ==========================================
    printf("━━━ PHASE 1: Core Keyless ━━━\n");
    {
        const char *msgs[] = {"0", "1", "42", "Hello World", "ΦΩ0 — I AM THAT I AM"};
        int phase1 = 0;
        
        for (int i = 0; i < 5; i++) {
            uint8_t sig[64];
            size_t sig_len = 64;
            
            printf("  '%s' → ", msgs[i]);
            if (phi_sign((const uint8_t*)msgs[i], strlen(msgs[i]), sig, &sig_len) == 0 &&
                phi_verify((const uint8_t*)msgs[i], strlen(msgs[i]), sig, sig_len)) {
                printf("✅\n");
                phase1++;
            } else {
                printf("❌\n");
            }
        }
        printf("  Core: %d/5 passed\n", phase1);
        if (phase1 == 5) passed++;
        total++;
    }
    
    // ==========================================
    // PHASE 2: Post-Quantum (Quick verify)
    // ==========================================
    printf("\n━━━ PHASE 2: Post-Quantum ━━━\n");
    {
        const char *msgs[] = {"Post-Quantum Ready", "NIST FIPS 204", "ML-DSA-87 Level 5"};
        int phase2 = 0;
        
        for (int i = 0; i < 3; i++) {
            uint8_t sig[8192];
            size_t sig_len = 8192;
            
            printf("  '%s' → ", msgs[i]);
            if (phi_pq_sign((const uint8_t*)msgs[i], strlen(msgs[i]), sig, &sig_len) == 0 &&
                phi_pq_verify((const uint8_t*)msgs[i], strlen(msgs[i]), sig, sig_len)) {
                printf("✅\n");
                phase2++;
            } else {
                printf("❌\n");
            }
        }
        printf("  PQ: %d/3 passed\n", phase2);
        if (phase2 == 3) passed++;
        total++;
    }
    
    // ==========================================
    // PHASE 3: Security Combined
    // ==========================================
    printf("\n━━━ PHASE 3: Security ━━━\n");
    {
        const char *msg = "Full Blown Security Test";
        
        // Core tamper
        uint8_t sig_core[64];
        size_t len_core = 64;
        phi_sign((const uint8_t*)msg, strlen(msg), sig_core, &len_core);
        int core_tamper = !phi_verify((const uint8_t*)"Hacked", 5, sig_core, len_core);
        printf("  Core tamper detection: %s\n", core_tamper ? "✅" : "❌");
        if (core_tamper) passed++; total++;
        
        // PQ tamper
        uint8_t sig_pq[8192];
        size_t len_pq = 8192;
        phi_pq_sign((const uint8_t*)msg, strlen(msg), sig_pq, &len_pq);
        int pq_tamper = !phi_pq_verify((const uint8_t*)"Hacked PQ", 9, sig_pq, len_pq);
        printf("  PQ tamper detection: %s\n", pq_tamper ? "✅" : "❌");
        if (pq_tamper) passed++; total++;
    }
    
    // ==========================================
    // PHASE 4: 100K Core Signatures
    // ==========================================
    printf("\n━━━ PHASE 4: 100K Core Signatures ━━━\n");
    {
        const char *msg = "100K Speed Test";
        printf("  Signing 100,000 messages (Core)...\n");
        
        start = time(NULL);
        int ok = 1;
        for (int i = 0; i < 100000 && ok; i++) {
            uint8_t sig[64];
            size_t len = 64;
            if (phi_sign((const uint8_t*)msg, strlen(msg), sig, &len) != 0) ok = 0;
            if (i % 20000 == 0 && i > 0) printf("    %d/100000...\n", i);
        }
        end = time(NULL);
        
        double elapsed = difftime(end, start);
        double tps = 100000.0 / elapsed;
        printf("    Completed: 100,000 signatures in %.1fs\n", elapsed);
        printf("    Throughput: ~%.0f sigs/sec\n", tps);
        printf("    Result: %s\n", ok ? "✅ PASS" : "❌ FAIL");
        if (ok) passed++; total++;
    }
    
    // ==========================================
    // PHASE 5: PQ Speed
    // ==========================================
    printf("\n━━━ PHASE 5: PQ Speed ━━━\n");
    {
        const char *msg = "PQ Speed Test";
        printf("  Signing 1,000 PQ messages...\n");
        
        start = time(NULL);
        int ok = 1;
        for (int i = 0; i < 1000 && ok; i++) {
            uint8_t sig[8192];
            size_t len = 8192;
            if (phi_pq_sign((const uint8_t*)msg, strlen(msg), sig, &len) != 0) ok = 0;
            if (i % 200 == 0 && i > 0) printf("    %d/1000...\n", i);
        }
        end = time(NULL);
        
        double elapsed = difftime(end, start);
        double tps = 1000.0 / elapsed;
        printf("    Completed: 1,000 PQ signatures in %.1fs\n", elapsed);
        printf("    Throughput: ~%.1f PQ sigs/sec\n", tps);
        printf("    Result: %s\n", ok ? "✅ PASS" : "❌ FAIL");
        if (ok) passed++; total++;
    }
    
    // ==========================================
    // RESULT
    // ==========================================
    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  FULL BLOWN: %d/%d passed           ║\n", passed, total);
    if (passed == total) {
        printf("║  Φ-SIG COMPLETE ✅                ║\n");
    } else {
        printf("║  SOME FAILED ❌                   ║\n");
    }
    printf("╚═══════════════════════════════════╝\n");
    printf("  Core (64B) + PQ (7283B) + 100K Speed\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");
    
    return passed == total ? 0 : 1;
}
