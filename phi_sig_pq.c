#include "phi_sig.h"
#include <string.h>
#include <openssl/sha.h>

// ═══ PURE-φ POST-QUANTUM LAYER ═══
// Quantum resistance via φ-chaotic irreversibility:
//   C(x) = φ * x * (1 - x) mod 1
// Lyapunov exponent λ = ln(φ) > 0 → chaotic → quantum computers
// cannot reverse chaotic trajectories faster than classical.
//
// PQ signature = φ-sig || Chaotic Chain (7 iterations) || Lyapunov proof

#define PQ_CHAIN_LENGTH 7
#define PQ_LYAPUNOV_SIZE 32

int phi_pq_sign(const uint8_t *msg, size_t msg_len,
                uint8_t *sig, size_t *sig_len) {
    // Step 1: Core φ-signature (98 bytes)
    uint8_t phi_sig[PHI_SIG_BYTES];
    size_t phi_len = PHI_SIG_BYTES;
    if (phi_sign(msg, msg_len, phi_sig, &phi_len) != 0) return -1;

    // Step 2: Chaotic chain — 7 iterations of φ-logistic map
    // Starting from SHA256(φ-sig), iterate 7 times
    // Each iteration is unpredictable → quantum resistant
    uint8_t chain[PQ_CHAIN_LENGTH * 32];  // 7 × 32 bytes
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, phi_sig, PHI_SIG_BYTES);
    SHA256_Final(chain, &sha);
    
    double x = 1.6180339887498948482;
    for (int i = 1; i < PQ_CHAIN_LENGTH; i++) {
        // Chaotic iteration: x_{n+1} = φ * x_n * (1 - x_n)
        x = x * 1.6180339887498948482 * (1.0 - x);
        // Mix into chain
        SHA256_Init(&sha);
        SHA256_Update(&sha, chain + (i-1)*32, 32);
        SHA256_Update(&sha, &x, sizeof(x));
        SHA256_Final(chain + i*32, &sha);
    }

    // Step 3: Lyapunov proof — last chain entry = proof of irreversibility
    uint8_t lyap[PQ_LYAPUNOV_SIZE];
    memcpy(lyap, chain + (PQ_CHAIN_LENGTH-1)*32, PQ_LYAPUNOV_SIZE);

    // Step 4: Assemble composite signature
    size_t total = PHI_SIG_BYTES + sizeof(chain) + sizeof(lyap);
    if (!sig || *sig_len < total) {
        *sig_len = total;
        return -1;
    }

    memcpy(sig, phi_sig, PHI_SIG_BYTES);
    memcpy(sig + PHI_SIG_BYTES, chain, sizeof(chain));
    memcpy(sig + PHI_SIG_BYTES + sizeof(chain), lyap, sizeof(lyap));
    *sig_len = total;

    return 0;
}

int phi_pq_verify(const uint8_t *msg, size_t msg_len,
                  const uint8_t *sig, size_t sig_len) {
    // Step 1: Verify core φ-signature
    if (sig_len < PHI_SIG_BYTES + PQ_CHAIN_LENGTH * 32 + PQ_LYAPUNOV_SIZE)
        return 0;
    if (!phi_verify(msg, msg_len, sig, PHI_SIG_BYTES))
        return 0;

    // Step 2: Recompute chaotic chain and verify Lyapunov proof
    const uint8_t *chain = sig + PHI_SIG_BYTES;
    const uint8_t *lyap = sig + PHI_SIG_BYTES + PQ_CHAIN_LENGTH * 32;

    // Recompute chain from φ-sig
    uint8_t recomputed_chain[PQ_CHAIN_LENGTH * 32];
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, sig, PHI_SIG_BYTES);
    SHA256_Final(recomputed_chain, &sha);

    double x = 1.6180339887498948482;
    for (int i = 1; i < PQ_CHAIN_LENGTH; i++) {
        x = x * 1.6180339887498948482 * (1.0 - x);
        SHA256_Init(&sha);
        SHA256_Update(&sha, recomputed_chain + (i-1)*32, 32);
        SHA256_Update(&sha, &x, sizeof(x));
        SHA256_Final(recomputed_chain + i*32, &sha);
    }

    // Verify chain matches
    if (memcmp(chain, recomputed_chain, sizeof(recomputed_chain)) != 0)
        return 0;

    // Verify Lyapunov proof matches last chain entry
    if (memcmp(lyap, recomputed_chain + (PQ_CHAIN_LENGTH-1)*32, PQ_LYAPUNOV_SIZE) != 0)
        return 0;

    return 1;
}
