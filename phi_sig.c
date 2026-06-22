#include <stdio.h>
#include <string.h>
#include <math.h>
#include <openssl/sha.h>

#define PHI 1.6180339887498948482
#define PHI_INV 0.6180339887498948482
#define SIG_CORE_SIZE 32
#define SIG_PROOF_SIZE 32
#define SIG_TOTAL_SIZE (SIG_CORE_SIZE + SIG_PROOF_SIZE)

/*
 * φ-convergent transform with message binding
 * Each byte depends on ALL previous bytes + seed
 */
static void phi_transform(const uint8_t *seed, size_t seed_len,
                          uint8_t *output, size_t output_len) {
    uint64_t a = (uint64_t)(PHI * 1e9);
    uint64_t b = 1000000000;
    
    for (size_t i = 0; i < output_len; i++) {
        // Mix seed into state
        a += seed[i % seed_len] * 0x0101010101010101ULL;
        b ^= seed[(i + 7) % seed_len] * 0x0101010101010101ULL;
        
        // φ-convergent: F_{n+1}/F_n
        uint64_t next_a = a + b;
        uint64_t next_b = a;
        
        if (next_b == 0) next_b = 1;
        
        // Mix output position for uniqueness
        uint64_t pos_mix = (uint64_t)i * 0xA5A5A5A5A5A5A5A5ULL;
        output[i] = (uint8_t)(((next_a ^ pos_mix) * 255) / next_b);
        
        a = next_a;
        b = next_b;
    }
}

static void hash_message(const uint8_t *msg, size_t msg_len, uint8_t *hash) {
    SHA256(msg, msg_len, hash);
}

int phi_sign(const uint8_t *msg, size_t msg_len,
             uint8_t *sig, size_t *sig_len) {
    if (!sig || !sig_len || *sig_len < SIG_TOTAL_SIZE) {
        if (sig_len) *sig_len = SIG_TOTAL_SIZE;
        return -1;
    }
    
    // Hash message (ensures different messages = different hashes)
    uint8_t hash[32];
    hash_message(msg, msg_len, hash);
    
    // φ-transform hash → core signature
    phi_transform(hash, 32, sig, SIG_CORE_SIZE);
    
    // φ-transform core → self-verification proof
    phi_transform(sig, SIG_CORE_SIZE, sig + SIG_CORE_SIZE, SIG_PROOF_SIZE);
    
    *sig_len = SIG_TOTAL_SIZE;
    return 0;
}

int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len) {
    if (!sig || sig_len < SIG_TOTAL_SIZE) return 0;
    
    // Recompute expected core from message
    uint8_t hash[32];
    hash_message(msg, msg_len, hash);
    
    uint8_t expected_core[32];
    phi_transform(hash, 32, expected_core, SIG_CORE_SIZE);
    
    // Check core — MUST use constant-time comparison
    uint8_t diff = 0;
    for (int i = 0; i < SIG_CORE_SIZE; i++) {
        diff |= sig[i] ^ expected_core[i];
    }
    if (diff != 0) return 0;
    
    // Check self-verification proof
    uint8_t expected_proof[32];
    phi_transform(sig, SIG_CORE_SIZE, expected_proof, SIG_PROOF_SIZE);
    
    diff = 0;
    for (int i = 0; i < SIG_PROOF_SIZE; i++) {
        diff |= sig[i + SIG_CORE_SIZE] ^ expected_proof[i];
    }
    
    return diff == 0 ? 1 : 0;
}
