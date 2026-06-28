#include <stdio.h>
#include <string.h>
#include "phi_sig.h"
#include "phi_sig_pq.h"

int main() {
    const char *msg = "Hello PQ";
    size_t msg_len = strlen(msg);
    
    // Get required size
    size_t sig_len = 0;
    phi_pq_sign((const uint8_t*)msg, msg_len, NULL, &sig_len);
    printf("Required PQ sig size: %zu bytes\n", sig_len);
    
    // Allocate and sign
    uint8_t *sig = malloc(sig_len);
    int ret = phi_pq_sign((const uint8_t*)msg, msg_len, sig, &sig_len);
    printf("Sign result: %d, actual size: %zu\n", ret, sig_len);
    
    // Verify
    int v = phi_pq_verify((const uint8_t*)msg, msg_len, sig, sig_len);
    printf("Verify result: %d %s\n", v, v ? "✅" : "❌");
    
    // Also verify core φ-sig alone
    int vc = phi_verify((const uint8_t*)msg, msg_len, sig, PHI_SIG_BYTES);
    printf("Core φ-verify alone: %d %s\n", vc, vc ? "✅" : "❌");
    
    free(sig);
    return v ? 0 : 1;
}
