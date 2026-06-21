#include "phi_sig_halimaw.h"
#include <stdio.h>
#include <string.h>

#define ITER 1000
int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Φ-SIG SSS HALIMAW v4.1 — FIXED OQS + SCHNORR             ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    const char *m="ΦΩ0 SSS TEST",*sec="observer_secret_phi",*wrong="attacker";
    PhiAlgorithm A[]={PHI_ALG_SCHNORR_SECP256K1,PHI_ALG_FALCON_1024,PHI_ALG_ML_DSA_87};
    const char *N[]={"Schnorr-secp256k1","Falcon-1024 (NIST L5)","ML-DSA-87 (NIST L5)"};
    
    for(int a=0;a<3;a++){printf("=== %s ===\n",N[a]);PhiSignature s;clock_t t=clock();
        int ok=phi_sign_halimaw((const uint8_t*)m,strlen(m),(const uint8_t*)sec,strlen(sec),&s,A[a]);
        double ms=(double)(clock()-t)*1000/CLOCKS_PER_SEC;
        int vc=phi_verify_halimaw((const uint8_t*)m,strlen(m),(const uint8_t*)sec,strlen(sec),&s);
        int vw=phi_verify_halimaw((const uint8_t*)m,strlen(m),(const uint8_t*)wrong,strlen(wrong),&s);
        printf("  Sign: %s (%.2f ms, %zu bytes)\n",ok?"✅":"❌",ms,phi_serialize(&s,NULL,0));
        printf("  Verify(correct): %s\n",vc?"VALID ✅":"INVALID ❌");
        printf("  Verify(wrong):   %s\n",vw?"VALID ❌":"INVALID ✅");
        int pass=0,tamp=0;clock_t st=clock();
        for(int i=0;i<ITER;i++){char b[128];snprintf(b,128,"t%d_φ",i);PhiSignature ts;
            phi_sign_halimaw((const uint8_t*)b,strlen(b),(const uint8_t*)sec,strlen(sec),&ts,A[a]);
            if(phi_verify_halimaw((const uint8_t*)b,strlen(b),(const uint8_t*)sec,strlen(sec),&ts))pass++;
            b[0]^=0xFF;if(!phi_verify_halimaw((const uint8_t*)b,strlen(b),(const uint8_t*)sec,strlen(sec),&ts))tamp++;}
        double tps=ITER/((double)(clock()-st)/CLOCKS_PER_SEC);
        printf("  Stress: %d/%d (%.0f TPS), tamper:%d/%d\n\n",pass,ITER,tps,tamp,ITER);}
    
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  SSS HALIMAW — OQS ACTIVE | SCHNORR FIXED                 ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}
