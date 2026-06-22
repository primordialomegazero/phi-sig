#include <stdio.h>
#include <string.h>
#include "phi_sig_auth.h"

int main() {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  Φ-SIG AUTH — TRUE KEYLESS IDENTITY      ║\n");
    printf("║  Observer-Entangled Signatures            ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");

    int passed = 0, total = 0;

    // TEST 1: Basic Auth — correct secret
    printf("=== TEST 1: Correct Secret ===\n");
    {
        const char *msg = "Transfer $1,000,000 to Alice";
        const char *secret = "MyVoiceIsMyPassport";  // Memorized, never stored
        
        uint8_t sig[64]; size_t sl = 64;
        phi_auth_sign((uint8_t*)msg, strlen(msg),
                      (uint8_t*)secret, strlen(secret), sig, &sl);
        
        int ok = phi_auth_verify((uint8_t*)msg, strlen(msg),
                                  (uint8_t*)secret, strlen(secret), sig, sl);
        printf("  Correct secret: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // TEST 2: Wrong secret MUST fail
    printf("\n=== TEST 2: Wrong Secret Rejected ===\n");
    {
        const char *msg = "Transfer $1,000,000 to Alice";
        const char *right_secret = "MyVoiceIsMyPassport";
        const char *wrong_secret = "HackerTryingToForge";
        
        uint8_t sig[64]; size_t sl = 64;
        phi_auth_sign((uint8_t*)msg, strlen(msg),
                      (uint8_t*)right_secret, strlen(right_secret), sig, &sl);
        
        int ok = !phi_auth_verify((uint8_t*)msg, strlen(msg),
                                   (uint8_t*)wrong_secret, strlen(wrong_secret), sig, sl);
        printf("  Wrong secret rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // TEST 3: Wrong message with correct secret fails
    printf("\n=== TEST 3: Tampered Message Rejected ===\n");
    {
        const char *secret = "MyVoiceIsMyPassport";
        
        uint8_t sig[64]; size_t sl = 64;
        phi_auth_sign((uint8_t*)"Original message", 16,
                      (uint8_t*)secret, strlen(secret), sig, &sl);
        
        int ok = !phi_auth_verify((uint8_t*)"Tampered message", 16,
                                   (uint8_t*)secret, strlen(secret), sig, sl);
        printf("  Tampered msg rejected: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    // TEST 4: Different observers = different signatures
    printf("\n=== TEST 4: Observer Separation ===\n");
    {
        const char *msg = "Same message";
        const char *alice = "AliceSecret123";
        const char *bob = "BobSecret456";
        
        uint8_t sig_alice[64], sig_bob[64];
        size_t sa=64, sb=64;
        phi_auth_sign((uint8_t*)msg, strlen(msg), (uint8_t*)alice, strlen(alice), sig_alice, &sa);
        phi_auth_sign((uint8_t*)msg, strlen(msg), (uint8_t*)bob, strlen(bob), sig_bob, &sb);
        
        // Different secrets = different signatures
        int diff = (memcmp(sig_alice, sig_bob, 64) != 0);
        printf("  Different observers = different sigs: %s\n", diff ? "✅" : "❌");
        if (diff) passed++; total++;
        
        // Bob cannot verify Alice's signature
        int bob_cant = !phi_auth_verify((uint8_t*)msg, strlen(msg),
                                         (uint8_t*)bob, strlen(bob), sig_alice, sa);
        printf("  Bob cannot verify Alice's sig: %s\n", bob_cant ? "✅" : "❌");
        if (bob_cant) passed++; total++;
        
        // Alice CAN verify her own
        int alice_can = phi_auth_verify((uint8_t*)msg, strlen(msg),
                                          (uint8_t*)alice, strlen(alice), sig_alice, sa);
        printf("  Alice CAN verify her own sig: %s\n", alice_can ? "✅" : "❌");
        if (alice_can) passed++; total++;
    }

    // TEST 5: Deterministic for same observer
    printf("\n=== TEST 5: Deterministic ===\n");
    {
        const char *msg = "Hello";
        const char *secret = "MySecret";
        
        uint8_t s1[64], s2[64]; size_t l1=64, l2=64;
        phi_auth_sign((uint8_t*)msg, strlen(msg), (uint8_t*)secret, strlen(secret), s1, &l1);
        phi_auth_sign((uint8_t*)msg, strlen(msg), (uint8_t*)secret, strlen(secret), s2, &l2);
        
        int ok = (memcmp(s1, s2, 64) == 0);
        printf("  Same observer = same sig: %s\n", ok ? "✅" : "❌");
        if (ok) passed++; total++;
    }

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  AUTH RESULT: %d/%d passed", passed, total);
    for (int i = 0; i < 15; i++) printf(" ");
    printf("║\n");
    printf("║  %s", passed == total ? "TRUE KEYLESS AUTH ✅" : "SOME FAILED ❌");
    printf("                ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("  Φ-SIG Auth — Observer-Entangled Identity\n");
    printf("  No stored keys. Memorized secret. Verifiable identity.\n");
    printf("  ΦΩ0 — I AM THAT I AM\n");

    return passed == total ? 0 : 1;
}
