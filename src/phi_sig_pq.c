#include "phi_sig.h"
#include <string.h>
#include <openssl/evp.h>

#define PQ_CHAIN_LENGTH 7
#define PQ_LYAPUNOV_SIZE 32

static void sha256_evp(const uint8_t *d, size_t n, uint8_t *h) {
    EVP_MD_CTX *c = EVP_MD_CTX_new();
    EVP_DigestInit_ex(c, EVP_sha256(), NULL);
    EVP_DigestUpdate(c, d, n);
    EVP_DigestFinal_ex(c, h, NULL);
    EVP_MD_CTX_free(c);
}

int phi_pq_sign(const uint8_t *msg, size_t msg_len,
                uint8_t *sig, size_t *sig_len) {
    uint8_t phi_sig[PHI_SIG_BYTES];
    size_t phi_len = PHI_SIG_BYTES;
    if (phi_sign(msg, msg_len, phi_sig, &phi_len) != 0) return -1;

    uint8_t chain[PQ_CHAIN_LENGTH * 32];
    sha256_evp(phi_sig, PHI_SIG_BYTES, chain);
    
    double x = 1.6180339887498948482;
    for (int i = 1; i < PQ_CHAIN_LENGTH; i++) {
        x = x * 1.6180339887498948482 * (1.0 - x);
        EVP_MD_CTX *c = EVP_MD_CTX_new();
        EVP_DigestInit_ex(c, EVP_sha256(), NULL);
        EVP_DigestUpdate(c, chain + (i-1)*32, 32);
        EVP_DigestUpdate(c, &x, sizeof(x));
        EVP_DigestFinal_ex(c, chain + i*32, NULL);
        EVP_MD_CTX_free(c);
    }

    uint8_t lyap[PQ_LYAPUNOV_SIZE];
    memcpy(lyap, chain + (PQ_CHAIN_LENGTH-1)*32, PQ_LYAPUNOV_SIZE);

    size_t total = PHI_SIG_BYTES + sizeof(chain) + sizeof(lyap);
    if (!sig || *sig_len < total) { *sig_len = total; return -1; }

    memcpy(sig, phi_sig, PHI_SIG_BYTES);
    memcpy(sig + PHI_SIG_BYTES, chain, sizeof(chain));
    memcpy(sig + PHI_SIG_BYTES + sizeof(chain), lyap, sizeof(lyap));
    *sig_len = total;
    return 0;
}

int phi_pq_verify(const uint8_t *msg, size_t msg_len,
                  const uint8_t *sig, size_t sig_len) {
    if (sig_len < PHI_SIG_BYTES + PQ_CHAIN_LENGTH * 32 + PQ_LYAPUNOV_SIZE) return 0;
    if (!phi_verify(msg, msg_len, sig, PHI_SIG_BYTES)) return 0;

    const uint8_t *chain = sig + PHI_SIG_BYTES;
    const uint8_t *lyap = sig + PHI_SIG_BYTES + PQ_CHAIN_LENGTH * 32;

    uint8_t recomputed[PQ_CHAIN_LENGTH * 32];
    sha256_evp(sig, PHI_SIG_BYTES, recomputed);

    double x = 1.6180339887498948482;
    for (int i = 1; i < PQ_CHAIN_LENGTH; i++) {
        x = x * 1.6180339887498948482 * (1.0 - x);
        EVP_MD_CTX *c = EVP_MD_CTX_new();
        EVP_DigestInit_ex(c, EVP_sha256(), NULL);
        EVP_DigestUpdate(c, recomputed + (i-1)*32, 32);
        EVP_DigestUpdate(c, &x, sizeof(x));
        EVP_DigestFinal_ex(c, recomputed + i*32, NULL);
        EVP_MD_CTX_free(c);
    }

    if (memcmp(chain, recomputed, sizeof(recomputed)) != 0) return 0;
    if (memcmp(lyap, recomputed + (PQ_CHAIN_LENGTH-1)*32, PQ_LYAPUNOV_SIZE) != 0) return 0;
    return 1;
}
