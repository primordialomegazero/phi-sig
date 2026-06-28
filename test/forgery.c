#include <stdio.h>
#include <string.h>
#include "../src/phi_sig.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG FORGERY TEST               ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed=0,total=5;
    uint8_t sa[PHI_SIG_BYTES],sb[PHI_SIG_BYTES];
    size_t la=PHI_SIG_BYTES,lb=PHI_SIG_BYTES;

    phi_sign((const uint8_t*)"Alice",5,sa,&la);
    phi_sign((const uint8_t*)"Bob",3,sb,&lb);

    printf("TEST 1: Legitimate signature\n");
    int v1=phi_verify((const uint8_t*)"Alice",5,sa,la);
    printf("  sig(Alice) vs 'Alice': %s → %s\n\n", v1?"ACCEPT":"REJECT", v1?"✅ PASS":"❌ FAIL");
    if (v1) passed++;

    printf("TEST 2: Wrong message\n");
    int v2=phi_verify((const uint8_t*)"Bob",3,sa,la);
    printf("  sig(Alice) vs 'Bob': %s → %s\n\n", !v2?"REJECT":"ACCEPT", !v2?"✅ PASS":"❌ FAIL");
    if (!v2) passed++;

    printf("TEST 3: Random signature\n");
    uint8_t rnd[PHI_SIG_BYTES];
    for(int i=0;i<PHI_SIG_BYTES;i++) rnd[i]=i*7+13;
    int v3=phi_verify((const uint8_t*)"Test",4,rnd,PHI_SIG_BYTES);
    printf("  Random sig vs 'Test': %s → %s\n\n", !v3?"REJECT":"ACCEPT", !v3?"✅ PASS":"❌ FAIL");
    if (!v3) passed++;

    printf("TEST 4: All-zeros signature\n");
    uint8_t zro[PHI_SIG_BYTES]={0};
    int v4=phi_verify((const uint8_t*)"Test",4,zro,PHI_SIG_BYTES);
    printf("  Zero sig vs 'Test': %s → %s\n\n", !v4?"REJECT":"ACCEPT", !v4?"✅ PASS":"❌ FAIL");
    if (!v4) passed++;

    printf("TEST 5: Tampered signature\n");
    sa[10]^=0xFF;
    int v5=phi_verify((const uint8_t*)"Alice",5,sa,la);
    printf("  Tampered sig vs 'Alice': %s → %s\n\n", !v5?"REJECT":"ACCEPT", !v5?"✅ PASS":"❌ FAIL");
    if (!v5) passed++;

    printf("╔═══════════════════════════════════╗\n");
    printf("║  RESULT: %d/%d passed %s     ║\n", passed,total,passed==total?"✅":"❌");
    printf("╚═══════════════════════════════════╝\n");
    return passed==total?0:1;
}
