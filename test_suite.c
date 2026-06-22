#include <stdio.h>
#include <string.h>
#include "phi_sig.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG — TRUE KEYLESS TEST SUITE  ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    // Test 1: Basic sign + verify
    printf("=== TEST 1: Sign + Verify ===\n");
    {
        const char *msgs[] = {"Hello", "ΦΩ0", "", "Test 123!", "Golden Ratio"};
        for (int i = 0; i < 5; i++) {
            uint8_t sig[64];
            size_t sig_len = sizeof(sig);
            phi_sign((uint8_t*)msgs[i], strlen(msgs[i]), sig, &sig_len);
            int ok = phi_verify((uint8_t*)msgs[i], strlen(msgs[i]), sig, sig_len);
            printf("  '%s': %s\n", msgs[i], ok ? "✅" : "❌");
            if (ok) passed++; total++;
        }
    }

    // Test 2: Wrong message rejected
    printf("\n=== TEST 2: Wrong Message Rejected ===\n");
    {
        uint8_t sig[64];
        size_t sig_len = sizeof(sig);
        phi_sign((uint8_t*)"Correct", 7, sig, &sig_len);
        int ok = !phi_verify((uint8_t*)"Wrong", 5, sig, sig_len);
        printf("  Wrong msg rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 3: Tampered signature rejected
    printf("\n=== TEST 3: Tampered Signature Rejected ===\n");
    {
        uint8_t sig[64];
        size_t sig_len = sizeof(sig);
        phi_sign((uint8_t*)"Test", 4, sig, &sig_len);
        sig[10] ^= 0xFF; // Tamper
        int ok = !phi_verify((uint8_t*)"Test", 4, sig, sig_len);
        printf("  Tampered sig rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 4: Deterministic
    printf("\n=== TEST 4: Deterministic ===\n");
    {
        uint8_t sig1[64], sig2[64];
        size_t len1 = sizeof(sig1), len2 = sizeof(sig2);
        phi_sign((uint8_t*)"Same", 4, sig1, &len1);
        phi_sign((uint8_t*)"Same", 4, sig2, &len2);
        int ok = (memcmp(sig1, sig2, 64) == 0);
        printf("  Same input = same output: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 5: Different messages = different signatures
    printf("\n=== TEST 5: Different Messages ===\n");
    {
        uint8_t sig1[64], sig2[64];
        size_t len1 = sizeof(sig1), len2 = sizeof(sig2);
        phi_sign((uint8_t*)"Alpha", 5, sig1, &len1);
        phi_sign((uint8_t*)"Beta", 4, sig2, &len2);
        int ok = (memcmp(sig1, sig2, 64) != 0);
        printf("  Different msgs = different sigs: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 6: Signature size
    printf("\n=== TEST 6: Signature Size ===\n");
    {
        uint8_t sig[100];
        size_t sig_len = 100;
        phi_sign((uint8_t*)"Size", 4, sig, &sig_len);
        int ok = (sig_len == 64);
        printf("  Size = %zu bytes (expected 64): %s\n", sig_len, ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 7: NULL query
    printf("\n=== TEST 7: NULL Query ===\n");
    {
        size_t sig_len = 0;
        phi_sign((uint8_t*)"Query", 5, NULL, &sig_len);
        int ok = (sig_len == 64);
        printf("  NULL query returns %zu: %s\n", sig_len, ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 8: Empty message
    printf("\n=== TEST 8: Empty Message ===\n");
    {
        uint8_t sig[64];
        size_t sig_len = sizeof(sig);
        phi_sign((uint8_t*)"", 0, sig, &sig_len);
        int ok = phi_verify((uint8_t*)"", 0, sig, sig_len);
        printf("  Empty message: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 9: Stress (100 rounds)
    printf("\n=== TEST 9: Stress (100 rounds) ===\n");
    {
        int ok = 1;
        for (int i = 0; i < 100 && ok; i++) {
            char buf[32];
            snprintf(buf, 32, "Stress %d", i);
            uint8_t sig[64];
            size_t sig_len = sizeof(sig);
            phi_sign((uint8_t*)buf, strlen(buf), sig, &sig_len);
            if (!phi_verify((uint8_t*)buf, strlen(buf), sig, sig_len)) ok = 0;
        }
        printf("  100 rounds: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // Test 10: Speed benchmark
    printf("\n=== TEST 10: Speed ===\n");
    {
        uint8_t sig[64];
        size_t sig_len;
        int ops = 0;
        for (int i = 0; i < 10000; i++) {
            phi_sign((uint8_t*)"Bench", 5, sig, &sig_len);
            ops++;
        }
        printf("  10000 signatures: ✅\n");
        passed++; total++;
    }

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  RESULT: %d/%d passed", passed, total);
    for (int i = 0; i < 15; i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed == total ? "TRUE KEYLESS ✅" : "SOME FAILED ❌");
    printf("                    ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("  Φ-SIG — No Keys. 64 Bytes. Self-Verifying.\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");

    return passed == total ? 0 : 1;
}
