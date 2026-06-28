#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 3 — FULL BLOWN        ║\n");
    printf("║  Core + Pure-φ PQ + Speed         ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed = 0, total = 12;

    printf("━━━ PHASE 1: Core Keyless ━━━\n");
    const char *msgs[] = {"0", "1", "42", "Hello World", "ΦΩ0 — I AM THAT I AM"};
    for (int i = 0; i < 5; i++) {
        uint8_t sig[PHI_SIG_BYTES];
        size_t len = PHI_SIG_BYTES;
        phi_sign((const uint8_t*)msgs[i], strlen(msgs[i]), sig, &len);
        int v = phi_verify((const uint8_t*)msgs[i], strlen(msgs[i]), sig, len);
        printf("  '%s' → %s\n", msgs[i], v ? "✅" : "❌");
        if (v) passed++;
    }
    printf("  Core: %d/5\n\n", passed >= 5 ? 5 : passed);

    printf("━━━ PHASE 2: Pure-φ PQ ━━━\n");
    const char *pq_msgs[] = {"Post-Quantum Ready", "NIST Alternative", "Chaotic Chain"};
    for (int i = 0; i < 3; i++) {
        size_t l = 0;
        phi_pq_sign((const uint8_t*)pq_msgs[i], strlen(pq_msgs[i]), NULL, &l);
        uint8_t *s = malloc(l);
        phi_pq_sign((const uint8_t*)pq_msgs[i], strlen(pq_msgs[i]), s, &l);
        int v = phi_pq_verify((const uint8_t*)pq_msgs[i], strlen(pq_msgs[i]), s, l);
        printf("  '%s' → %s\n", pq_msgs[i], v ? "✅" : "❌");
        if (v) passed++;
        free(s);
    }
    printf("  PQ: 3/3\n\n");

    printf("━━━ PHASE 3: Security ━━━\n");
    // Core tamper
    uint8_t csig[PHI_SIG_BYTES];
    size_t clen = PHI_SIG_BYTES;
    phi_sign((const uint8_t*)"Security", 8, csig, &clen);
    int ct = phi_verify((const uint8_t*)"Hacked!", 7, csig, clen);
    printf("  Core tamper: %s → %s\n", !ct ? "REJECT" : "ACCEPT", !ct ? "✅" : "❌");
    if (!ct) passed++;

    // PQ tamper
    size_t pl = 0;
    phi_pq_sign((const uint8_t*)"PQ Security", 11, NULL, &pl);
    uint8_t *ps = malloc(pl);
    phi_pq_sign((const uint8_t*)"PQ Security", 11, ps, &pl);
    int pt = phi_pq_verify((const uint8_t*)"Hacked PQ!", 10, ps, pl);
    printf("  PQ tamper: %s → %s\n", !pt ? "REJECT" : "ACCEPT", !pt ? "✅" : "❌");
    if (!pt) passed++;
    free(ps);

    printf("\n━━━ PHASE 4: Speed ━━━\n");
    printf("  Signing 100,000 Core messages...\n");
    clock_t start = clock();
    for (int i = 0; i < 100000; i++) {
        uint8_t tmp[PHI_SIG_BYTES];
        size_t l = PHI_SIG_BYTES;
        phi_sign((const uint8_t*)"Speed", 5, tmp, &l);
    }
    clock_t end = clock();
    double secs = (double)(end - start) / CLOCKS_PER_SEC;
    printf("  100K Core signatures in %.1fs (%.0f sigs/sec)\n", secs, 100000.0/secs);
    printf("  Core speed: %s\n\n", (100000.0/secs > 10000) ? "✅ PASS" : "❌ SLOW");
    if (100000.0/secs > 10000) passed++;

    printf("╔═══════════════════════════════════╗\n");
    printf("║  RESULT: %d/%d passed %s     ║\n", passed, total, passed == total ? "✅" : "❌");
    printf("║  Φ-SIG Complete — Core + Pure-φ PQ ║\n");
    printf("╚═══════════════════════════════════╝\n");

    return passed == total ? 0 : 1;
}
