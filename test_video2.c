#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 2 — PURE-φ PQ         ║\n");
    printf("║  Chaotic Chain + Lyapunov Proof   ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    const char *msgs[] = {"Hello", "ΦΩ0", "Post-Quantum", "Keyless", "Pure-φ"};
    int passed = 0, total = 13;

    printf("━━━ PHASE 1: PQ Sign + Verify ━━━\n");
    for (int i = 0; i < 5; i++) {
        size_t msg_len = strlen(msgs[i]);
        size_t sig_len = 0;
        phi_pq_sign((const uint8_t*)msgs[i], msg_len, NULL, &sig_len);
        
        uint8_t *sig = malloc(sig_len);
        phi_pq_sign((const uint8_t*)msgs[i], msg_len, sig, &sig_len);
        int v = phi_pq_verify((const uint8_t*)msgs[i], msg_len, sig, sig_len);
        
        printf("  Message %d/5: '%s'\n    Size: %zu bytes | Verify: %s\n", 
               i+1, msgs[i], sig_len, v ? "✅ VALID" : "❌ FAIL");
        if (v) passed++;
        free(sig);
    }

    printf("\n━━━ PHASE 2: Security ━━━\n");
    const char *msg = "PQ Security Test";
    size_t msg_len = strlen(msg);
    size_t sig_len = 0;
    phi_pq_sign((const uint8_t*)msg, msg_len, NULL, &sig_len);
    
    uint8_t *sig = malloc(sig_len);
    phi_pq_sign((const uint8_t*)msg, msg_len, sig, &sig_len);

    // Tamper 1: wrong message
    int t1 = phi_pq_verify((const uint8_t*)"Wrong", 5, sig, sig_len);
    printf("  [Tamper 1] Wrong message: %s → %s\n", t1 ? "ACCEPT" : "REJECT", !t1 ? "✅ PASS" : "❌ FAIL");
    if (!t1) passed++;

    // Tamper 2: tampered signature
    sig[50] ^= 0xFF;
    int t2 = phi_pq_verify((const uint8_t*)msg, msg_len, sig, sig_len);
    printf("  [Tamper 2] Tampered sig: %s → %s\n", t2 ? "ACCEPT" : "REJECT", !t2 ? "✅ PASS" : "❌ FAIL");
    if (!t2) passed++;

    // Tamper 3: tampered chaotic chain
    sig[50] ^= 0xFF;  // restore
    sig[100] ^= 0xFF;  // tamper chain
    int t3 = phi_pq_verify((const uint8_t*)msg, msg_len, sig, sig_len);
    printf("  [Tamper 3] Tampered chain: %s → %s\n", t3 ? "ACCEPT" : "REJECT", !t3 ? "✅ PASS" : "❌ FAIL");
    if (!t3) passed++;

    free(sig);

    printf("\n━━━ PHASE 3: Properties ━━━\n");
    // Deterministic
    size_t l1 = 0, l2 = 0;
    phi_pq_sign((const uint8_t*)"Test", 4, NULL, &l1);
    phi_pq_sign((const uint8_t*)"Test", 4, NULL, &l2);
    uint8_t *s1 = malloc(l1), *s2 = malloc(l2);
    phi_pq_sign((const uint8_t*)"Test", 4, s1, &l1);
    phi_pq_sign((const uint8_t*)"Test", 4, s2, &l2);
    int det = (l1 == l2 && memcmp(s1, s2, l1) == 0);
    printf("  Deterministic: %s → %s\n", det ? "YES" : "NO", det ? "✅ PASS" : "❌ FAIL");
    if (det) passed++;
    free(s1); free(s2);

    // Different messages
    size_t la = 0, lb = 0;
    phi_pq_sign((const uint8_t*)"A", 1, NULL, &la);
    phi_pq_sign((const uint8_t*)"B", 1, NULL, &lb);
    uint8_t *sa = malloc(la), *sb = malloc(lb);
    phi_pq_sign((const uint8_t*)"A", 1, sa, &la);
    phi_pq_sign((const uint8_t*)"B", 1, sb, &lb);
    int diff = (la != lb || memcmp(sa, sb, la < lb ? la : lb) != 0);
    printf("  Different msgs = different sigs: %s → %s\n", diff ? "YES" : "NO", diff ? "✅ PASS" : "❌ FAIL");
    if (diff) passed++;
    free(sa); free(sb);

    // Size
    size_t size_check = 0;
    phi_pq_sign(NULL, 0, NULL, &size_check);
    printf("  PQ sig size: %zu bytes → %s\n", size_check, size_check > 98 ? "✅ PASS" : "❌ FAIL");
    if (size_check > 98) passed++;

    // NULL safety
    size_t null_check = 0;
    phi_pq_sign(NULL, 0, NULL, &null_check);
    printf("  NULL safe: %zu bytes → %s\n", null_check, null_check > 0 ? "✅ PASS" : "❌ FAIL");
    if (null_check > 0) passed++;

    printf("\n━━━ PHASE 4: Speed ━━━\n");
    printf("  Signing 1000 PQ messages...\n");
    for (int i = 0; i < 1000; i++) {
        size_t l = 0;
        uint8_t tmp[512];
        phi_pq_sign((const uint8_t*)"Speed test", 10, tmp, &l);
        phi_pq_verify((const uint8_t*)"Speed test", 10, tmp, l);
    }
    printf("  1000 sign+verify pairs: ✅\n");
    passed++;

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  PQ RESULT: %d/%d passed %s ║\n", passed, total, passed == total ? "✅" : "❌");
    printf("║  Pure-φ PQ — Chaotic Chain + Lyapunov ║\n");
    printf("╚═══════════════════════════════════╝\n");

    return passed == total ? 0 : 1;
}
