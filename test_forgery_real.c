#include <stdio.h>
#include <string.h>
#include "phi_sig.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG REAL FORGERY TEST           ║\n");
    printf("╚═══════════════════════════════════╝\n\n");
    
    uint8_t pk[33], sk[32], sig[98];
    size_t siglen = 98;
    int passed = 0;
    
    // Generate a legitimate signature
    phi_keygen(pk, sk);
    phi_sign((uint8_t*)"Alice", 5, sig, &siglen);
    
    // TEST 1: Legitimate signature must verify
    printf("TEST 1: Legitimate signature\n");
    int v1 = phi_verify((uint8_t*)"Alice", 5, sig, siglen);
    printf("  sig(Alice) vs 'Alice': %s\n", v1 ? "✅ ACCEPT" : "❌ REJECT");
    if (v1) passed++;
    
    // TEST 2: Wrong message must REJECT
    printf("\nTEST 2: Wrong message\n");
    int v2 = phi_verify((uint8_t*)"Bob", 3, sig, siglen);
    printf("  sig(Alice) vs 'Bob': %s\n", v2 ? "❌ ACCEPT (FORGED!)" : "✅ REJECT");
    if (!v2) passed++;
    
    // TEST 3: Random signature must REJECT
    printf("\nTEST 3: Random signature\n");
    uint8_t random_sig[98];
    for (int i = 0; i < 98; i++) random_sig[i] = rand() & 0xFF;
    int v3 = phi_verify((uint8_t*)"Test", 4, random_sig, 98);
    printf("  Random sig vs 'Test': %s\n", v3 ? "❌ ACCEPT (FORGED!)" : "✅ REJECT");
    if (!v3) passed++;
    
    // TEST 4: All-zeros must REJECT
    printf("\nTEST 4: All-zeros signature\n");
    uint8_t zero_sig[98];
    memset(zero_sig, 0, 98);
    int v4 = phi_verify((uint8_t*)"Test", 4, zero_sig, 98);
    printf("  Zero sig vs 'Test': %s\n", v4 ? "❌ ACCEPT (FORGED!)" : "✅ REJECT");
    if (!v4) passed++;
    
    // TEST 5: Tampered signature must REJECT
    printf("\nTEST 5: Tampered signature\n");
    uint8_t tampered[98];
    memcpy(tampered, sig, 98);
    tampered[10] ^= 0xFF;
    int v5 = phi_verify((uint8_t*)"Alice", 5, tampered, 98);
    printf("  Tampered sig vs 'Alice': %s\n", v5 ? "❌ ACCEPT (FORGED!)" : "✅ REJECT");
    if (!v5) passed++;
    
    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  RESULT: %d/5 passed               ║\n", passed);
    printf("║  %s              ║\n", passed == 5 ? "TRUE SIGNATURE ✅" : "BROKEN ❌");
    printf("╚═══════════════════════════════════╝\n");
    
    return passed == 5 ? 0 : 1;
}
