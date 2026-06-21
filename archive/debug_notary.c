#include "phi_sig.h"
#include "phi_sig_mirror.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("=== DEBUG NOTARY ===\n");
    
    const char* doc = "test";
    const char* owner = "ΦΩ0";
    
    // Step 1: Create core (72 bytes)
    uint8_t core[72];
    uint64_t ts = phi_timestamp();
    memcpy(core, &ts, 8);
    SHA256((const uint8_t*)owner, strlen(owner), core + 8);
    SHA256((const uint8_t*)doc, strlen(doc), core + 40);
    
    printf("Core hash (first 8): ");
    for(int i=0;i<8;i++) printf("%02x", core[40+i]);
    printf("\n");
    
    // Step 2: Try phi_sign with correct size
    uint8_t sig[256];
    size_t sig_len = 256;
    int ret = phi_sign(core, 72, sig, &sig_len);
    printf("phi_sign(core, 72): ret=%d sig_len=%zu\n", ret, sig_len);
    
    // Step 3: Try phi_verify
    int ver = phi_verify(core, 72, sig, sig_len);
    printf("phi_verify(core, 72): ret=%d\n", ver);
    
    return 0;
}
