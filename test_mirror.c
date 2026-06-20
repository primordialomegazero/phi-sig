#include "phi_sig_mirror.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-NOTARY — Hybrid Keyless + Mirror Signatures           ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    const char* doc = "I, Dan Fernandez, hereby declare this document authentic.";
    const char* owner = "ΦΩ0-PRIMORDIAL-OMEGA-ZERO";
    
    uint8_t entry[640];
    size_t entry_len = sizeof(entry);
    
    printf("Document: %s\n", doc);
    printf("Owner: %s\n\n", owner);
    
    phi_notarize((const uint8_t*)doc, strlen(doc),
                 (const uint8_t*)owner, strlen(owner), entry, &entry_len);
    printf("Notary Entry: %zu bytes\n", entry_len);
    
    uint64_t ts;
    memcpy(&ts, entry, 8);
    printf("Timestamp: %lu\n\n", ts);
    
    printf("Test 1 (correct owner): %s\n",
           phi_notarize_verify(entry, entry_len, (const uint8_t*)owner, strlen(owner)) ? "VALID ✅" : "INVALID ❌");
    
    const char* wrong = "NOT-THE-OWNER";
    printf("Test 2 (wrong owner): %s\n",
           phi_notarize_verify(entry, entry_len, (const uint8_t*)wrong, strlen(wrong)) ? "VALID ❌" : "INVALID ✅");
    
    uint8_t tampered[640];
    memcpy(tampered, entry, entry_len);
    tampered[80] ^= 0xFF;
    printf("Test 3 (tampered): %s\n",
           phi_notarize_verify(tampered, entry_len, (const uint8_t*)owner, strlen(owner)) ? "VALID ❌" : "INVALID ✅");
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-NOTARY — Hybrid Keyless + Mirror                      ║\n");
    printf("║  Layer 1: Keyless Φ-SIG (anyone can verify)              ║\n");
    printf("║  Layer 2: Mirror Φ-SIG (only owner can verify)           ║\n");
    printf("║  Tamper-Evident | Post-Quantum | Keyless                 ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
