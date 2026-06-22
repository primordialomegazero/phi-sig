#include <stdio.h>
#include <string.h>
#include <time.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    int passed = 0, total = 0;
    time_t start, end;
    
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 2 — POST-QUANTUM      ║\n");
    printf("║  ML-DSA-87 (NIST FIPS 204 L5)    ║\n");
    printf("╚═══════════════════════════════════╝\n\n");
    
    // ==========================================
    // PHASE 1: PQ Sign + Verify
    // ==========================================
    printf("━━━ PHASE 1: PQ Sign + Verify ━━━\n");
    {
        const char *msgs[] = {"Hello", "ΦΩ0", "Post-Quantum", "Keyless", "ML-DSA-87"};
        const char *labels[] = {"ASCII", "Unicode", "Longer", "Concept", "Algorithm"};
        
        for (int i = 0; i < 5; i++) {
            printf("  Message %d/%d: '%s' (%s)\n", i+1, 5, msgs[i], labels[i]);
            printf("    Computing φ-signature + ML-DSA-87...\n");
            
            uint8_t sig[8192];
            size_t sig_len = 8192;
            int ok_sign = (phi_pq_sign((const uint8_t*)msgs[i], strlen(msgs[i]), sig, &sig_len) == 0);
            
            if (ok_sign) {
                printf("    Signature: %zu bytes (Core: 64 + ML-DSA-87: %zu)\n", sig_len, sig_len - 64);
                printf("    Verifying...\n");
                
                int ok_verify = phi_pq_verify((const uint8_t*)msgs[i], strlen(msgs[i]), sig, sig_len);
                printf("    Result: %s\n", (ok_verify) ? "✅ VALID" : "❌ FAIL");
                if (ok_verify) passed++;
            } else {
                printf("    Result: ❌ SIGN FAILED\n");
            }
            total++;
            printf("\n");
        }
    }
    
    // ==========================================
    // PHASE 2: Security
    // ==========================================
    printf("━━━ PHASE 2: Security ━━━\n");
    {
        const char *msg = "Post-Quantum Test";
        uint8_t sig[8192];
        size_t sig_len = 8192;
        
        printf("  Signing: '%s'\n", msg);
        phi_pq_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
        printf("  PQ Signature: %zu bytes\n", sig_len);
        
        // Wrong message
        printf("\n  [Tamper Test 1] Verifying with WRONG message...\n");
        int wrong_msg = phi_pq_verify((const uint8_t*)"Hacked message", 14, sig, sig_len);
        printf("  Expected: REJECT → Got: %s → %s\n",
               wrong_msg ? "ACCEPT" : "REJECT",
               !wrong_msg ? "✅ PASS" : "❌ FAIL");
        if (!wrong_msg) passed++;
        total++;
        
        // Tampered signature
        printf("\n  [Tamper Test 2] Tampering PQ signature...\n");
        uint8_t tampered[8192];
        memcpy(tampered, sig, sig_len);
        tampered[100] ^= 0xFF;  // Tamper deep in the signature
        printf("  Tampered byte 100: %02x → %02x\n", sig[100], tampered[100]);
        int tampered_sig = phi_pq_verify((const uint8_t*)msg, strlen(msg), tampered, sig_len);
        printf("  Expected: REJECT → Got: %s → %s\n",
               tampered_sig ? "ACCEPT" : "REJECT",
               !tampered_sig ? "✅ PASS" : "❌ FAIL");
        if (!tampered_sig) passed++;
        total++;
        printf("\n");
    }
    
    // ==========================================
    // PHASE 3: Properties
    // ==========================================
    printf("━━━ PHASE 3: Properties ━━━\n");
    {
        const char *msg = "PQ Property Test";
        
        // Size
        printf("  [Test] PQ Signature size...\n");
        size_t size_check = 8192;
        phi_pq_sign((const uint8_t*)msg, strlen(msg), NULL, &size_check);
        printf("    Total: %zu bytes (Core: 64 + ML-DSA-87: ~4627 + Overhead)\n", size_check);
        printf("    %s\n", size_check > 64 ? "✅ PASS" : "❌ FAIL");
        if (size_check > 64) passed++;
        total++;
        
        // Combined security
        printf("\n  [Test] Combined Security (Core + PQ)...\n");
        uint8_t sig[8192];
        size_t sig_len = 8192;
        phi_pq_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
        
        // Verify core part works
        int core_ok = phi_verify((const uint8_t*)msg, strlen(msg), sig, 64);
        printf("    Core φ-SIG valid: %s\n", core_ok ? "✅" : "❌");
        
        // Verify PQ part works
        int pq_ok = phi_pq_verify((const uint8_t*)msg, strlen(msg), sig, sig_len);
        printf("    PQ ML-DSA-87 valid: %s\n", pq_ok ? "✅" : "❌");
        printf("    Combined: %s\n", (core_ok && pq_ok) ? "✅ PASS" : "❌ FAIL");
        if (core_ok && pq_ok) passed++;
        total++;
    }
    
    // ==========================================
    // PHASE 4: Speed
    // ==========================================
    printf("\n━━━ PHASE 4: Speed ━━━\n");
    {
        const char *msg = "PQ Speed Test";
        printf("  Signing 100 PQ messages...\n");
        
        start = time(NULL);
        int speed_ok = 1;
        for (int i = 0; i < 100 && speed_ok; i++) {
            uint8_t sig[8192];
            size_t len = 8192;
            if (phi_pq_sign((const uint8_t*)msg, strlen(msg), sig, &len) != 0) speed_ok = 0;
            if (i % 20 == 0 && i > 0) printf("    %d/100...\n", i);
        }
        end = time(NULL);
        
        double elapsed = difftime(end, start);
        printf("    Completed: 100 PQ signatures in %.1fs\n", elapsed);
        printf("    Rate: ~%.1f PQ sigs/sec\n", 100.0 / elapsed);
        printf("    Result: %s\n", speed_ok ? "✅ PASS" : "❌ FAIL");
        if (speed_ok) passed++;
        total++;
    }
    
    // ==========================================
    // RESULT
    // ==========================================
    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  PQ RESULT: %d/%d passed             ║\n", passed, total);
    if (passed == total) {
        printf("║  POST-QUANTUM KEYLESS ✅          ║\n");
    } else {
        printf("║  SOME FAILED ❌                   ║\n");
    }
    printf("╚═══════════════════════════════════╝\n");
    printf("  Φ-SIG PQ — ML-DSA-87 (NIST FIPS 204 Level 5)\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");
    
    return passed == total ? 0 : 1;
}
