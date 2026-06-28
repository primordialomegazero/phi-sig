#include <stdio.h>
#include <string.h>
#include "phi_sig.h"

int main() {
    const char *msg = "Hello";
    uint8_t sig[PHI_SIG_BYTES];
    size_t sig_len = PHI_SIG_BYTES;
    
    printf("Signing...\n");
    int ret = phi_sign((const uint8_t*)msg, strlen(msg), sig, &sig_len);
    printf("Sign result: %d, sig_len: %zu\n", ret, sig_len);
    
    printf("Verifying...\n");
    int v = phi_verify((const uint8_t*)msg, strlen(msg), sig, sig_len);
    printf("Verify result: %d\n", v);
    
    return 0;
}
