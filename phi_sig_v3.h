#pragma once
#include <oqs/oqs.h>
#include <openssl/evp.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define FALCON_PK 1793
#define PROOF 128
#define V3_SIZE 3500

static void H(const uint8_t *in, size_t len, uint8_t *out) {
    unsigned int o; EVP_MD_CTX *m=EVP_MD_CTX_new();
    EVP_DigestInit_ex(m, EVP_sha256(), NULL); EVP_DigestUpdate(m, in, len);
    EVP_DigestFinal_ex(m, out, &o); EVP_MD_CTX_free(m);
}

// ═══════════════════════════════════════
// Φ-SIGN v3: Falcon-1024 + Observer-Bound φ-Proof
// Observer secret is embedded in φ-proof, not in keygen
// ═══════════════════════════════════════
static int phi_sign_v3(const uint8_t *msg, size_t mlen, const uint8_t *sec, size_t slen,
                        uint8_t *out, size_t *olen) {
    if (*olen < V3_SIZE) { *olen=V3_SIZE; return 0; }
    OQS_SIG *f = OQS_SIG_new(OQS_SIG_alg_falcon_1024);
    if(!f) return 0;
    
    // Generate REAL random keypair (one-time)
    uint8_t pk[FALCON_PK], sk[2305], sig[2000];
    OQS_SIG_keypair(f, pk, sk);
    size_t sl = f->length_signature;
    OQS_SIG_sign(f, sig, &sl, msg, mlen, sk);
    
    // Store sig_len + signature + public key
    memcpy(out, &sl, 4);
    memcpy(out+4, sig, sl);
    memcpy(out+4+sl, pk, FALCON_PK);
    
    // φ-OBSERVER-BOUND PROOF: H(secret || msg || pk || φ)
    uint8_t proof[PROOF], pi[64];
    H(sec, slen, pi);  // Observer secret IN the proof
    for(int i=0;i<4;i++){
        EVP_MD_CTX*m=EVP_MD_CTX_new();EVP_DigestInit_ex(m,EVP_sha256(),NULL);
        EVP_DigestUpdate(m, pi, 32); EVP_DigestUpdate(m, msg, mlen);
        EVP_DigestUpdate(m, pk, FALCON_PK); EVP_DigestUpdate(m, &i, 1);
        unsigned int p;EVP_DigestFinal_ex(m, proof+i*32, &p);EVP_MD_CTX_free(m);
    }
    memcpy(out+4+sl+FALCON_PK, proof, PROOF);
    
    *olen = 4 + sl + FALCON_PK + PROOF;
    memset(sk,0,sizeof(sk)); OQS_SIG_free(f);
    return 1;
}

// VERIFY: Falcon verify + observer-bound φ-proof
static int phi_verify_v3(const uint8_t *msg, size_t mlen, const uint8_t *sec, size_t slen,
                          const uint8_t *in, size_t ilen) {
    if(ilen < 4+FALCON_PK+PROOF) return 0;
    size_t sl; memcpy(&sl, in, 4);
    if(sl>2000 || ilen<4+sl+FALCON_PK+PROOF) return 0;
    
    const uint8_t *sig=in+4, *pk=in+4+sl, *proof=in+4+sl+FALCON_PK;
    
    // 1. FALCON VERIFY
    OQS_SIG *f = OQS_SIG_new(OQS_SIG_alg_falcon_1024);
    if(!f) return 0;
    int ok = OQS_SIG_verify(f, sig, sl, msg, mlen, pk);
    OQS_SIG_free(f);
    if(ok != OQS_SUCCESS) return 0;
    
    // 2. φ-OBSERVER-BOUND PROOF VERIFY
    uint8_t expected[PROOF], pi[64];
    H(sec, slen, pi);
    for(int i=0;i<4;i++){
        EVP_MD_CTX*m=EVP_MD_CTX_new();EVP_DigestInit_ex(m,EVP_sha256(),NULL);
        EVP_DigestUpdate(m, pi, 32); EVP_DigestUpdate(m, msg, mlen);
        EVP_DigestUpdate(m, pk, FALCON_PK); EVP_DigestUpdate(m, &i, 1);
        unsigned int p;EVP_DigestFinal_ex(m, expected+i*32, &p);EVP_MD_CTX_free(m);
    }
    
    return memcmp(proof, expected, PROOF) == 0;
}
