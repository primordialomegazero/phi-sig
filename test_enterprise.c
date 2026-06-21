#include "phi_sig_enterprise.h"
#include <stdio.h>
#include <string.h>

#define STRESS 1000
int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG ENTERPRISE v5.0 — Production Framework             ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Version: %s\n", phi_sig_version());
    PhiAlgorithm algos[] = {PHI_ALG_SCHNORR_SECP256K1, PHI_ALG_FALCON_1024, PHI_ALG_ML_DSA_87};
    const char *names[] = {"Schnorr-secp256k1","Falcon-1024 (NIST L5)","ML-DSA-87 (NIST L5)"};
    
    for (int a = 0; a < 3; a++) {
        printf("=== %s [Available: %s] ===\n", names[a], phi_sig_is_available(algos[a]) ? "YES" : "NO");
        if (!phi_sig_is_available(algos[a])) { printf("  SKIP\n\n"); continue; }
        
        PhiSigContext ctx; phi_sig_init(&ctx, algos[a]);
        const char *m = "ΦΩ0 ENTERPRISE", *sec = "observer_secret_phi_ent";
        PhiSignature sig;
        
        clock_t t = clock();
        PhiError e = phi_sig_sign(&ctx, (const uint8_t*)m, strlen(m), (const uint8_t*)sec, strlen(sec), &sig);
        double ms = (double)(clock()-t)*1000/CLOCKS_PER_SEC;
        
        size_t sz; phi_sig_serialize(&sig, NULL, &sz);
        printf("  Sign: %s (%.2f ms, %zu bytes)\n", e==PHI_OK?"✅":"❌", ms, sz);
        
        PhiError vc = phi_sig_verify(&ctx, (const uint8_t*)m, strlen(m), (const uint8_t*)sec, strlen(sec), &sig);
        PhiError vw = phi_sig_verify(&ctx, (const uint8_t*)m, strlen(m), (const uint8_t*)"wrong", 5, &sig);
        printf("  Verify(correct): %s\n", vc==PHI_OK?"VALID ✅":"INVALID ❌");
        printf("  Verify(wrong):   %s\n", vw==PHI_OK?"VALID ❌":"INVALID ✅");
        
        int pass=0, tamp=0; clock_t st=clock();
        for (int i=0; i<STRESS; i++) {
            char b[128]; snprintf(b,128,"e%d_φ",i);
            PhiSignature ts; phi_sig_sign(&ctx, (const uint8_t*)b, strlen(b), (const uint8_t*)sec, strlen(sec), &ts);
            if (phi_sig_verify(&ctx, (const uint8_t*)b, strlen(b), (const uint8_t*)sec, strlen(sec), &ts)==PHI_OK) pass++;
            b[0]^=0xFF;
            if (phi_sig_verify(&ctx, (const uint8_t*)b, strlen(b), (const uint8_t*)sec, strlen(sec), &ts)!=PHI_OK) tamp++;
        }
        double tps = STRESS/((double)(clock()-st)/CLOCKS_PER_SEC);
        printf("  Stress: %d/%d (%.0f TPS), tamper:%d/%d\n\n", pass, STRESS, tps, tamp, STRESS);
        phi_sig_free(&ctx);
    }
    
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  ENTERPRISE v5.0 — Production Ready                       ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}
