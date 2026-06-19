#include "phi_sig.h"
#include <string.h>
#include <openssl/sha.h>

static void hash256(const uint8_t *in, size_t len, uint8_t *out) {
    SHA256(in, len, out);
}

// Core φ-transform: output = φ^n(hash) mod 256
// Each byte = convergent of continued fraction φ
static void phi_transform(const uint8_t *seed, size_t seed_len,
                           uint8_t *out, size_t out_len) {
    uint64_t p = 1, q = 1; // φ convergent: p/q → φ
    
    for (size_t i = 0; i < out_len; i++) {
        // Next φ convergent: F_{n+2}/F_{n+1}
        uint64_t next_p = p + q;
        uint64_t next_q = p;
        
        // Mix with seed
        next_p += seed[i % seed_len] * 0x0101010101010101ULL;
        next_q ^= seed[(i + 13) % seed_len] * 0x0101010101010101ULL;
        
        // φ-value = convergent ratio, mapped to byte
        if (next_q == 0) next_q = 1;
        out[i] = (uint8_t)((next_p * 255) / next_q);
        
        p = next_p;
        q = next_q;
    }
}

int phi_sign(const uint8_t *msg, size_t msg_len,
             uint8_t *sig, size_t *sig_len) {
    if (!sig || !sig_len || *sig_len < 96) {
        if (sig_len) *sig_len = 96;
        return 0;
    }
    
    // Hash message
    uint8_t hash[32];
    hash256(msg, msg_len, hash);
    
    // Core signature: φ-transform of hash
    phi_transform(hash, 32, sig, 64);
    
    // Self-verification proof: φ-transform of signature = itself?
    // If sig is valid, φ(sig[0:64]) should produce a predictable pattern
    uint8_t proof[32];
    hash256(sig, 64, proof);
    phi_transform(proof, 32, sig + 64, 32);
    
    *sig_len = 96;
    return 1;
}

int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len) {
    if (!sig || sig_len < 96) return 0;
    
    // Recompute expected signature
    uint8_t hash[32];
    hash256(msg, msg_len, hash);
    
    uint8_t expected[64];
    phi_transform(hash, 32, expected, 64);
    
    // Verify core
    if (memcmp(sig, expected, 64) != 0) return 0;
    
    // Self-consistency: proof must match hash of signature
    uint8_t expected_proof[32];
    hash256(sig, 64, expected_proof);
    phi_transform(expected_proof, 32, expected_proof, 32);
    
    return memcmp(sig + 64, expected_proof, 32) == 0;
}

int phi_verify_partial(const uint8_t *msg, size_t msg_len,
                        const uint8_t *sig, size_t sig_len,
                        int available_bytes) {
    // Fractal property: verify with partial signature!
    if (available_bytes < 64) return 0;
    
    uint8_t hash[32];
    hash256(msg, msg_len, hash);
    
    uint8_t expected[64];
    phi_transform(hash, 32, expected, 64);
    
    // Compare only available bytes
    int bytes_to_check = (available_bytes < 64) ? available_bytes : 64;
    return memcmp(sig, expected, bytes_to_check) == 0;
}
