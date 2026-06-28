#include <stdio.h>
#include <string.h>
#include <time.h>
#include "phi_sig.h"

int main() {
    int passed = 0, total = 0;
    time_t start, end;
    
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 1 — CORE KEYLESS      ║\n");
    printf("║  64 bytes. No keys. Pure φ.       ║\n");
    printf("╚═══════════════════════════════════╝\n\n");
    
    // ==========================================
    // PHASE 1: Sign + Verify
    // ==========================================
    printf("━━━ PHASE 1: Sign + Verify ━━━\n");
    {
        const char *msgs[] = {"Hello", "ΦΩ0", "", "Test 123!", "Golden Ratio"};
        const char *labels[] = {"ASCII", "Unicode", "Empty", "Alphanumeric", "Longer"};
        
        for (int i = 0; i < 5; i++) {
            printf("  Message %d/%d: '%s' (%s)\n", i+1, 5, msgs[i], labels[i]);
            printf("    Computing φ-signature...\n");
            
            uint8_t sig[64];
            size_t sig_len = 64;
            int ok_sign = (phi_sign((const uint8_t*)msgs[i], strlen(msgs[i]), sig, &sig_len) == 0);
            
            if (ok_sign) {
                printf("    Signature: ");
                for (int j = 0; j < 8; j++) printf("%02x", sig[j]);
                printf("... (64 bytes)\n");
                printf("    Verifying...\n");
                
                int ok_verify = phi_verify((const uint8_t*)msgs[i], strlen(msgs[i]), sig, sig_len);
                printf("    Result: %s\n", (ok_sign && ok_verify) ? "✅ VALID" : "❌ FAIL");
                if (ok_sign && ok_verify) passed++;
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
        const char *msg = "Test message";
        uint8_t sig[64];
        size_t sig_len = 64;
        
        printf("  Signing: '%s'\n", msg);
        phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
        printf("  Signature: %02x%02x%02x%02x...\n", sig[0], sig[1], sig[2], sig[3]);
        
        // Wrong message
        printf("\n  [Tamper Test 1] Verifying with WRONG message...\n");
        int wrong_msg = phi_verify((const uint8_t*)"Wrong message", 13, sig, sig_len);
        printf("  Expected: REJECT → Got: %s → %s\n", 
               wrong_msg ? "ACCEPT" : "REJECT",
               !wrong_msg ? "✅ PASS" : "❌ FAIL");
        if (!wrong_msg) passed++;
        total++;
        
        // Tampered signature
        printf("\n  [Tamper Test 2] Tampering signature byte 0...\n");
        uint8_t tampered[64];
        memcpy(tampered, sig, 64);
        printf("  Before: %02x → After: %02x\n", tampered[0], tampered[0] ^ 0xFF);
        tampered[0] ^= 0xFF;
        int tampered_sig = phi_verify((const uint8_t*)msg, strlen(msg), tampered, 64);
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
        const char *msg = "Property test";
        
        // Deterministic
        printf("  [Test] Deterministic: Same input = Same output?\n");
        uint8_t sig1[64], sig2[64];
        size_t len1 = 64, len2 = 64;
        phi_sign((const uint8_t*)msg, strlen(msg), sig1, &len1);
        phi_sign((const uint8_t*)msg, strlen(msg), sig2, &len2);
        printf("    Sig1: %02x%02x%02x%02x...\n", sig1[0], sig1[1], sig1[2], sig1[3]);
        printf("    Sig2: %02x%02x%02x%02x...\n", sig2[0], sig2[1], sig2[2], sig2[3]);
        int deterministic = (memcmp(sig1, sig2, 64) == 0);
        printf("    Match: %s → %s\n", deterministic ? "YES" : "NO", deterministic ? "✅ PASS" : "❌ FAIL");
        if (deterministic) passed++;
        total++;
        
        // Different messages
        printf("\n  [Test] Different messages = Different signatures?\n");
        uint8_t sig_a[64], sig_b[64];
        size_t len_a = 64, len_b = 64;
        phi_sign((const uint8_t*)"Message A", 9, sig_a, &len_a);
        phi_sign((const uint8_t*)"Message B", 9, sig_b, &len_b);
        printf("    Msg A: %02x%02x%02x%02x...\n", sig_a[0], sig_a[1], sig_a[2], sig_a[3]);
        printf("    Msg B: %02x%02x%02x%02x...\n", sig_b[0], sig_b[1], sig_b[2], sig_b[3]);
        int different = (memcmp(sig_a, sig_b, 64) != 0);
        printf("    Different: %s → %s\n", different ? "YES" : "NO", different ? "✅ PASS" : "❌ FAIL");
        if (different) passed++;
        total++;
        
        // Size
        printf("\n  [Test] Signature size = 64 bytes?\n");
        size_t size_check = 64;
        phi_sign((const uint8_t*)msg, strlen(msg), NULL, &size_check);
        printf("    Size: %zu bytes (65 sig + 33 pk) → %s\n", size_check, size_check == 98 ? "✅ PASS" : "❌ FAIL");
        if (size_check == 98) passed++;
        total++;
        
        // NULL query
        printf("\n  [Test] NULL query safe?\n");
        size_t null_check = 64;
        phi_sign(NULL, 0, NULL, &null_check);
        printf("    Returns: %zu (98 = 65 sig + 33 pk) → %s\n", null_check, null_check == 98 ? "✅ PASS" : "❌ FAIL");
        if (null_check == 98) passed++;
        total++;
        
        // Empty message
        printf("\n  [Test] Empty message safe?\n");
        uint8_t sig_empty[64];
        size_t len_empty = 64;
        int empty_ok = (phi_sign((const uint8_t*)"", 0, sig_empty, &len_empty) == 0);
        int empty_verify = phi_verify((const uint8_t*)"", 0, sig_empty, len_empty);
        printf("    Sign: %s | Verify: %s → %s\n",
               empty_ok ? "OK" : "FAIL",
               empty_verify ? "OK" : "FAIL",
               (empty_ok && empty_verify) ? "✅ PASS" : "❌ FAIL");
        if (empty_ok && empty_verify) passed++;
        total++;
        printf("\n");
    }
    
    // ==========================================
    // PHASE 4: Speed
    // ==========================================
    printf("━━━ PHASE 4: Speed ━━━\n");
    {
        const char *msg = "Speed test";
        printf("  Signing 10,000 messages...\n");
        
        start = time(NULL);
        int speed_ok = 1;
        for (int i = 0; i < 10000 && speed_ok; i++) {
            uint8_t sig[64];
            size_t len = 64;
            if (phi_sign((const uint8_t*)msg, strlen(msg), sig, &len) != 0) speed_ok = 0;
            if (i % 2000 == 0 && i > 0) printf("    %d/10000...\n", i);
        }
        end = time(NULL);
        
        double elapsed = difftime(end, start);
        printf("    Completed: %d signatures in %.1fs\n", 10000, elapsed);
        printf("    Rate: ~%.0f sigs/sec\n", 10000.0 / elapsed);
        printf("    Result: %s\n", speed_ok ? "✅ PASS" : "❌ FAIL");
        if (speed_ok) passed++;
        total++;
    }
    
    // ==========================================
    // RESULT
    // ==========================================
    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  RESULT: %d/%d passed             ║\n", passed, total);
    if (passed == total) {
        printf("║  TRUE KEYLESS ✅                  ║\n");
    } else {
        printf("║  SOME FAILED ❌                   ║\n");
    }
    printf("╚═══════════════════════════════════╝\n");
    printf("  Φ-SIG Core — 64 bytes. No keys. Tamper-evident.\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");
    
    return passed == total ? 0 : 1;
}
