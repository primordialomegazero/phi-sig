#include <stdio.h>
#include <string.h>
#include "phi_sig.h"

int main() {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Φ-SIG FORGERY TEST               ║\n");
    printf("║  Can we forge WITHOUT phi_sign()?  ║\n");
    printf("╚═══════════════════════════════════╝\n\n");

    uint8_t sig1[64], sig2[64], forged[98];
    size_t len1 = 98, len2 = 98;

    // Get signatures for two messages
    phi_sign((uint8_t*)"Alice", 5, sig1, &len1);
    phi_sign((uint8_t*)"Bob", 3, sig2, &len2);

    printf("sig(Alice) = ");
    for (int i=0; i<8; i++) printf("%02x", sig1[i]);
    printf("...\n");

    printf("sig(Bob)   = ");
    for (int i=0; i<8; i++) printf("%02x", sig2[i]);
    printf("...\n\n");

    // FORGERY ATTEMPT 1: Reuse signature for wrong message
    printf("FORGERY 1: sig(Alice) verified against 'Bob'\n");
    int v1 = phi_verify((uint8_t*)"Bob", 3, sig1, 98);
    printf("  Result: %s\n", v1 == 0 ? "ACCEPT ❌ (FORGED!)" : "REJECT ✅");

    // FORGERY ATTEMPT 2: XOR two signatures
    printf("\nFORGERY 2: sig(Alice) XOR sig(Bob) verified against 'Alice'\n");
    for (int i=0; i<98; i++) forged[i] = sig1[i] ^ sig2[i];
    int v2 = phi_verify((uint8_t*)"Alice", 5, forged, 98);
    printf("  Result: %s\n", v2 == 0 ? "ACCEPT ❌ (FORGED!)" : "REJECT ✅");

    // FORGERY ATTEMPT 3: All zeros signature
    printf("\nFORGERY 3: All-zeros 'signature' verified against 'Test'\n");
    uint8_t zeros[98] = {0};
    int v3 = phi_verify((uint8_t*)"Test", 4, zeros, 98);
    printf("  Result: %s\n", v3 == 0 ? "ACCEPT ❌ (FORGED!)" : "REJECT ✅");

    // FORGERY ATTEMPT 4: Replay attack
    printf("\nFORGERY 4: sig(Alice) verified against 'Alice' twice (replay)\n");
    int v4a = phi_verify((uint8_t*)"Alice", 5, sig1, 98);
    int v4b = phi_verify((uint8_t*)"Alice", 5, sig1, 98);
    printf("  First: %s, Second: %s\n", 
           v4a == 0 ? "ACCEPT" : "REJECT",
           v4b == 0 ? "ACCEPT" : "REJECT");
    printf("  Note: Replay is OK for signatures, but shows no nonce/timestamp\n");

    printf("\n╔═══════════════════════════════════╗\n");
    printf("║  FORGERY TEST COMPLETE            ║\n");
    printf("╚═══════════════════════════════════╝\n");

    return 0;
}
