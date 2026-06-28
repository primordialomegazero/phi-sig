#include <stdio.h>
#include <string.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    const char *msg = "Hello Pure-φ PQ!";
    size_t msg_len = strlen(msg);
    
    printf("╔═══════════════════════════════════╗\n");
    printf("║  Pure-φ PQ Signature Test         ║\n");
    printf("║  No OQS. No Keys. Pure φ.        ║\n");
    printf("╚═══════════════════════════════════╝\n\n");
    
    // Get size
    size_t sig_len = 0;
    phi_pq_sign((const uint8_t*)msg, msg_len, NULL, &sig_len);
    printf("PQ sig size: %zu bytes (Core: 98 + Chain: %d + Lyap: %d)\n", 
           sig_len, 7*32, 32);
    
    // Sign
    uint8_t *sig = malloc(sig_len);
    if (phi_pq_sign((const uint8_t*)msg, msg_len, sig, &sig_len) != 0) {
        printf("Sign FAILED\n"); return 1;
    }
    printf("Sign: ✅\n");
    
    // Verify
    int v = phi_pq_verify((const uint8_t*)msg, msg_len, sig, sig_len);
    printf("Verify: %s\n\n", v ? "✅ VALID" : "❌ FAIL");
    
    // Tamper test
    sig[100] ^= 0xFF;
    int vt = phi_pq_verify((const uint8_t*)msg, msg_len, sig, sig_len);
    printf("Tampered verify: %s (expected REJECT)\n", vt ? "❌ ACCEPTED" : "✅ REJECTED");
    
    free(sig);
    return v && !vt ? 0 : 1;
}
