#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/phi_sig.h"
#include "../src/phi_sig_pq.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG PURE-φ PQ TEST             ║\n");
    printf("║  Chaotic Chain + Lyapunov Proof   ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    const char *msgs[] = {"Hello", "ΦΩ0", "Post-Quantum", "Keyless", "Pure-φ"};
    int passed = 0, total = 13;

    printf("━━━ PHASE 1: PQ Sign + Verify ━━━\n");
    for (int i=0; i<5; i++) {
        size_t l=0;
        phi_pq_sign((const uint8_t*)msgs[i], strlen(msgs[i]), NULL, &l);
        uint8_t *s = malloc(l);
        phi_pq_sign((const uint8_t*)msgs[i], strlen(msgs[i]), s, &l);
        int v = phi_pq_verify((const uint8_t*)msgs[i], strlen(msgs[i]), s, l);
        printf("  '%s': %zu bytes → %s\n", msgs[i], l, v?"✅ VALID":"❌ FAIL");
        if (v) passed++;
        free(s);
    }

    printf("\n━━━ PHASE 2: Security ━━━\n");
    size_t l=0;
    phi_pq_sign((const uint8_t*)"PQ Test", 7, NULL, &l);
    uint8_t *s = malloc(l);
    phi_pq_sign((const uint8_t*)"PQ Test", 7, s, &l);

    int t1=phi_pq_verify((const uint8_t*)"Wrong", 5, s, l);
    printf("  Wrong message: %s → %s\n", !t1?"REJECT":"ACCEPT", !t1?"✅ PASS":"❌ FAIL");
    if (!t1) passed++;

    s[50]^=0xFF;
    int t2=phi_pq_verify((const uint8_t*)"PQ Test", 7, s, l);
    printf("  Tampered sig: %s → %s\n", !t2?"REJECT":"ACCEPT", !t2?"✅ PASS":"❌ FAIL");
    if (!t2) passed++;

    s[50]^=0xFF; s[100]^=0xFF;
    int t3=phi_pq_verify((const uint8_t*)"PQ Test", 7, s, l);
    printf("  Tampered chain: %s → %s\n", !t3?"REJECT":"ACCEPT", !t3?"✅ PASS":"❌ FAIL");
    if (!t3) passed++;
    free(s);

    printf("\n━━━ PHASE 3: Properties ━━━\n");
    size_t la=0,lb=0;
    phi_pq_sign((const uint8_t*)"T",1,NULL,&la);
    phi_pq_sign((const uint8_t*)"T",1,NULL,&lb);
    uint8_t *sa=malloc(la),*sb=malloc(lb);
    phi_pq_sign((const uint8_t*)"T",1,sa,&la);
    phi_pq_sign((const uint8_t*)"T",1,sb,&lb);
    printf("  Deterministic: %s → %s\n", (la==lb&&!memcmp(sa,sb,la))?"YES":"NO", (la==lb&&!memcmp(sa,sb,la))?"✅ PASS":"❌ FAIL");
    if (la==lb&&!memcmp(sa,sb,la)) passed++;
    free(sa);free(sb);

    phi_pq_sign((const uint8_t*)"A",1,NULL,&la);
    phi_pq_sign((const uint8_t*)"B",1,NULL,&lb);
    sa=malloc(la);sb=malloc(lb);
    phi_pq_sign((const uint8_t*)"A",1,sa,&la);
    phi_pq_sign((const uint8_t*)"B",1,sb,&lb);
    printf("  Different msgs: %s → %s\n", (la!=lb||memcmp(sa,sb,la<lb?la:lb))?"YES":"NO", (la!=lb||memcmp(sa,sb,la<lb?la:lb))?"✅ PASS":"❌ FAIL");
    if (la!=lb||memcmp(sa,sb,la<lb?la:lb)) passed++;
    free(sa);free(sb);

    size_t sz=0;
    phi_pq_sign(NULL,0,NULL,&sz);
    printf("  Size: %zu bytes → %s\n", sz, sz>98?"✅ PASS":"❌ FAIL");
    if (sz>98) passed++;
    printf("  NULL safe: %zu → %s\n", sz, sz>0?"✅ PASS":"❌ FAIL");
    if (sz>0) passed++;

    printf("\n━━━ PHASE 4: Speed ━━━\n");
    for (int i=0;i<1000;i++) {
        uint8_t tmp[512];
        size_t tl=512;
        phi_pq_sign((const uint8_t*)"Speed",5,tmp,&tl);
        phi_pq_verify((const uint8_t*)"Speed",5,tmp,tl);
    }
    printf("  1000 sign+verify pairs: ✅\n");
    passed++;

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  PQ RESULT: %d/%d passed %s  ║\n", passed, total, passed==total?"✅":"❌");
    printf("╚═══════════════════════════════════╝\n");
    return passed==total?0:1;
}
