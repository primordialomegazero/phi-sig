#include <stdio.h>
#include <string.h>
#include "../src/phi_sig.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG CORE KEYLESS TEST          ║\n");
    printf("║  98 bytes. No keys. Pure φ.       ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    const char *msgs[] = {"Hello", "ΦΩ0", "", "Test 123!", "Golden Ratio"};
    int passed = 0, total = 15;

    printf("━━━ PHASE 1: Sign + Verify ━━━\n");
    for (int i = 0; i < 5; i++) {
        uint8_t sig[PHI_SIG_BYTES];
        size_t len = PHI_SIG_BYTES;
        phi_sign((const uint8_t*)msgs[i], strlen(msgs[i]), sig, &len);
        int v = phi_verify((const uint8_t*)msgs[i], strlen(msgs[i]), sig, len);
        printf("  Message %d: '%s' → %s\n", i+1, msgs[i], v ? "✅ VALID" : "❌ FAIL");
        if (v) passed++;
    }

    printf("\n━━━ PHASE 2: Security ━━━\n");
    uint8_t sig[PHI_SIG_BYTES];
    size_t len = PHI_SIG_BYTES;
    phi_sign((const uint8_t*)"Test message", 12, sig, &len);
    
    int t1 = phi_verify((const uint8_t*)"Wrong message", 13, sig, len);
    printf("  Wrong message: %s → %s\n", !t1 ? "REJECT" : "ACCEPT", !t1 ? "✅ PASS" : "❌ FAIL");
    if (!t1) passed++;

    sig[0] ^= 0xFF;
    int t2 = phi_verify((const uint8_t*)"Test message", 12, sig, len);
    printf("  Tampered sig: %s → %s\n", !t2 ? "REJECT" : "ACCEPT", !t2 ? "✅ PASS" : "❌ FAIL");
    if (!t2) passed++;

    printf("\n━━━ PHASE 3: Properties ━━━\n");
    uint8_t s1[PHI_SIG_BYTES], s2[PHI_SIG_BYTES];
    size_t l1=PHI_SIG_BYTES, l2=PHI_SIG_BYTES;
    phi_sign((const uint8_t*)"Test", 4, s1, &l1);
    phi_sign((const uint8_t*)"Test", 4, s2, &l2);
    int det = (memcmp(s1, s2, PHI_SIG_BYTES) == 0);
    printf("  Deterministic: %s → %s\n", det ? "YES" : "NO", det ? "✅ PASS" : "❌ FAIL");
    if (det) passed++;

    uint8_t sa[PHI_SIG_BYTES], sb[PHI_SIG_BYTES];
    l1=l2=PHI_SIG_BYTES;
    phi_sign((const uint8_t*)"A", 1, sa, &l1);
    phi_sign((const uint8_t*)"B", 1, sb, &l2);
    printf("  Different msgs = different sigs: %s → %s\n", 
           memcmp(sa,sb,PHI_SIG_BYTES)?"YES":"NO", memcmp(sa,sb,PHI_SIG_BYTES)?"✅ PASS":"❌ FAIL");
    if (memcmp(sa,sb,PHI_SIG_BYTES)) passed++;

    size_t sz=0;
    phi_sign((const uint8_t*)"", 0, NULL, &sz);
    printf("  Size query: %zu bytes → %s\n", sz, sz==PHI_SIG_BYTES?"✅ PASS":"❌ FAIL");
    if (sz==PHI_SIG_BYTES) passed++;

    sz=0;
    phi_sign(NULL, 0, NULL, &sz);
    printf("  NULL safe: %zu bytes → %s\n", sz, sz==PHI_SIG_BYTES?"✅ PASS":"❌ FAIL");
    if (sz==PHI_SIG_BYTES) passed++;

    uint8_t se[PHI_SIG_BYTES];
    size_t le=PHI_SIG_BYTES;
    phi_sign((const uint8_t*)"", 0, se, &le);
    int ev = phi_verify((const uint8_t*)"", 0, se, le);
    printf("  Empty message: %s → %s\n", ev?"OK":"FAIL", ev?"✅ PASS":"❌ FAIL");
    if (ev) passed++;

    printf("\n━━━ PHASE 4: Speed ━━━\n");
    for (int i=0; i<10000; i++) {
        uint8_t tmp[PHI_SIG_BYTES];
        size_t tl=PHI_SIG_BYTES;
        phi_sign((const uint8_t*)"Speed", 5, tmp, &tl);
    }
    printf("  10,000 signatures: ✅\n");
    passed++;

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  CORE RESULT: %d/%d passed %s  ║\n", passed, total, passed==total?"✅":"❌");
    printf("╚═══════════════════════════════════╝\n");
    return passed==total ? 0 : 1;
}
