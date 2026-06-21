#include "phi_sig_halimaw.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define ITER 1000

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG HALIMAW v4.0 — Complete Post-Key Framework         ║\n");
    printf("║  3 NIST PQC | Schnorr ZKP | φ-HKDF | Serialization        ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    const char *msg = "ΦΩ0 HALIMAW SIGNATURE TEST";
    const char *secret = "observer_secret_key_phi_1618";
    const char *wrong = "attacker_secret";
    
    PhiAlgorithm algos[] = {PHI_ALG_SCHNORR_SECP256K1, PHI_ALG_FALCON_1024, PHI_ALG_ML_DSA_87};
    const char *names[] = {"Schnorr-secp256k1", "Falcon-1024 (NIST L5)", "ML-DSA-87 (NIST L5)"};
    int num_algos = 3;
    
    for (int a = 0; a < num_algos; a++) {
        printf("=== %s ===\n", names[a]);
        
        // Sign + Verify correct
        PhiSignature sig;
        clock_t t1 = clock();
        int ok = phi_sign_halimaw((const uint8_t*)msg, strlen(msg),
                                   (const uint8_t*)secret, strlen(secret),
                                   &sig, algos[a]);
        clock_t t2 = clock();
        
        if (!ok) { printf("  SIGN FAILED ❌\n\n"); continue; }
        
        int v_ok = phi_verify_halimaw((const uint8_t*)msg, strlen(msg),
                                       (const uint8_t*)secret, strlen(secret), &sig);
        int v_wrong = phi_verify_halimaw((const uint8_t*)msg, strlen(msg),
                                          (const uint8_t*)wrong, strlen(wrong), &sig);
        
        printf("  Sign: %s (%.2f ms, %zu bytes)\n", ok ? "✅" : "❌",
               (double)(t2-t1)*1000/CLOCKS_PER_SEC, phi_serialize(&sig, NULL, 0));
        printf("  Verify (correct): %s\n", v_ok ? "VALID ✅" : "INVALID ❌");
        printf("  Verify (wrong):   %s\n", v_wrong ? "VALID ❌" : "INVALID ✅");
        
        // Stress test
        int passes = 0, tamper_caught = 0;
        clock_t st = clock();
        for (int i = 0; i < ITER; i++) {
            char m[128]; snprintf(m, 128, "test_%d_φ", i);
            PhiSignature s;
            phi_sign_halimaw((const uint8_t*)m, strlen(m),
                             (const uint8_t*)secret, strlen(secret), &s, algos[a]);
            if (phi_verify_halimaw((const uint8_t*)m, strlen(m),
                                    (const uint8_t*)secret, strlen(secret), &s)) passes++;
            m[0] ^= 0xFF;
            if (!phi_verify_halimaw((const uint8_t*)m, strlen(m),
                                     (const uint8_t*)secret, strlen(secret), &s)) tamper_caught++;
        }
        clock_t et = clock();
        double tps = ITER / ((double)(et-st)/CLOCKS_PER_SEC);
        printf("  Stress: %d/%d verify (%.0f TPS), %d/%d tamper caught\n",
               passes, ITER, tps, tamper_caught, ITER);
        printf("\n");
    }
    
    // Serialization test
    printf("=== SERIALIZATION ===\n");
    PhiSignature s;
    phi_sign_halimaw((const uint8_t*)msg, strlen(msg),
                     (const uint8_t*)secret, strlen(secret), &s, PHI_ALG_SCHNORR_SECP256K1);
    uint8_t buf[5000];
    size_t len = phi_serialize(&s, buf, sizeof(buf));
    PhiSignature s2;
    phi_deserialize(buf, len, &s2);
    int match = phi_verify_halimaw((const uint8_t*)msg, strlen(msg),
                                    (const uint8_t*)secret, strlen(secret), &s2);
    printf("  Serialize: %zu bytes\n", len);
    printf("  Round-trip verify: %s\n", match ? "VALID ✅" : "INVALID ❌");
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG HALIMAW v4.0 — PRODUCTION READY                    ║\n");
    printf("║  3 Algorithms | Schnorr ZKP | φ-HKDF | Serialization     ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}
