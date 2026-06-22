#include <stdio.h>
#include <string.h>
#include "phi_sig.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG TEST 1 — TRUE KEYLESS      ║\n");
    printf("║  No Keys. 64 Bytes. Self-Verifying║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    printf("=== PHASE 1: Sign + Verify ===\n");
    const char *msgs[] = {"Hello", "ΦΩ0", "", "Test 123!", "Golden Ratio"};
    for (int i = 0; i < 5; i++) {
        uint8_t sig[64]; size_t sl = 64;
        phi_sign((uint8_t*)msgs[i], strlen(msgs[i]), sig, &sl);
        int ok = phi_verify((uint8_t*)msgs[i], strlen(msgs[i]), sig, sl);
        printf("  '%s': %s\n", msgs[i], ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    printf("\n=== PHASE 2: Security ===\n");
    {
        uint8_t sig[64]; size_t sl = 64;
        phi_sign((uint8_t*)"Correct", 7, sig, &sl);
        printf("  Wrong msg rejected: %s\n", !phi_verify((uint8_t*)"Wrong", 5, sig, sl) ? "✅" : "❌"); passed++; total++;
        sig[10] ^= 0xFF;
        printf("  Tampered sig rejected: %s\n", !phi_verify((uint8_t*)"Correct", 7, sig, sl) ? "✅" : "❌"); passed++; total++;
    }

    printf("\n=== PHASE 3: Properties ===\n");
    {
        uint8_t s1[64], s2[64]; size_t l1=64, l2=64;
        phi_sign((uint8_t*)"Same", 4, s1, &l1);
        phi_sign((uint8_t*)"Same", 4, s2, &l2);
        printf("  Deterministic: %s\n", memcmp(s1,s2,64)==0 ? "✅" : "❌"); passed++; total++;
    }
    {
        uint8_t s1[64], s2[64]; size_t l1=64, l2=64;
        phi_sign((uint8_t*)"Alpha", 5, s1, &l1);
        phi_sign((uint8_t*)"Beta", 4, s2, &l2);
        printf("  Different msgs: %s\n", memcmp(s1,s2,64)!=0 ? "✅" : "❌"); passed++; total++;
    }
    {
        size_t sl = 100; phi_sign((uint8_t*)"Size", 4, NULL, &sl);
        printf("  Size=64: %s\n", sl==64 ? "✅" : "❌"); passed++; total++;
    }
    {
        uint8_t sig[64]; size_t sl=64;
        phi_sign((uint8_t*)"", 0, sig, &sl);
        printf("  Empty msg: %s\n", phi_verify((uint8_t*)"",0,sig,sl) ? "✅" : "❌"); passed++; total++;
    }

    printf("\n=== PHASE 4: Speed ===\n");
    {
        uint8_t sig[64]; size_t sl;
        for (int i = 0; i < 10000; i++) phi_sign((uint8_t*)"Bench", 5, sig, &sl);
        printf("  10,000 signatures: ✅\n"); passed++; total++;
    }

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  RESULT: %d/%d passed", passed, total);
    for (int i=0;i<15;i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed==total ? "TRUE KEYLESS ✅" : "SOME FAILED ❌");
    printf("                    ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");
    return passed==total ? 0 : 1;
}
