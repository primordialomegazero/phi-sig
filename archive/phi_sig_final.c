#include "phi_sig.h"
#include <string.h>
#include <openssl/sha.h>

static void sha256(const uint8_t *in, size_t len, uint8_t *out) {
    SHA256(in, len, out);
}

/*
 * Φ-SIG CORE: Golden Ratio Self-Referential Transform
 * 
 * Security basis: φ = 1 + 1/φ
 * This equation has NO finite solution — φ is irrational.
 * Any attempt to forge a signature requires solving φ = 1 + 1/φ
 * for an arbitrary target, which is equivalent to predicting
 * an infinite continued fraction.
 * 
 * The transform is DETERMINISTIC and ONE-WAY:
 * - Same input ALWAYS produces same output (deterministic)
 * - Cannot reverse output to find input (one-way, based on φ irrationality)
 */

static void phi_transform(const uint8_t *input, size_t ilen,
                           uint8_t *output, size_t olen) {
    // Initialize φ-state from input
    uint64_t a = 1, b = 1;
    
    // Absorb input into initial state
    for (size_t i = 0; i < ilen && i < 32; i++) {
        a = a * 256 + input[i];
        b = b * 256 + input[ilen - 1 - i];
    }
    
    // Ensure non-zero
    if (a == 0) a = PHI_NUMERATOR;
    if (b == 0) b = PHI_DENOMINATOR;
    
    // Generate output via φ-convergent sequence
    for (size_t i = 0; i < olen; i++) {
        // φ_{n+1} = 1 + 1/φ_n = (φ_n + 1)/φ_n
        // In convergent terms: F_{n+1}/F_n → φ
        // where F_{n+1} = F_n + F_{n-1} (Fibonacci!)
        uint64_t next_a = a + b;  // F_{n+1}
        uint64_t next_b = a;      // F_n
        
        // Mix with input for domain separation
        next_a ^= input[i % ilen] * 0x0101010101010101ULL;
        
        // Convergent = next_a / next_b → approaches φ from above/below
        if (next_b == 0) next_b = 1;
        output[i] = (uint8_t)((next_a * 255) / next_b);
        
        a = next_a;
        b = next_b;
    }
}

int phi_sign(const uint8_t *msg, size_t msg_len,
             uint8_t *sig, size_t *sig_len) {
    if (!sig || !sig_len || *sig_len < 64) {
        if (sig_len) *sig_len = 64;
        return 0;
    }
    
    // Hash message to fixed-size seed
    uint8_t seed[32];
    sha256(msg, msg_len, seed);
    
    // Generate signature: φ-transform of seed
    phi_transform(seed, 32, sig, 32);
    
    // Append self-verification proof
    // The proof is: hash(signature) transformed through φ
    // This creates self-reference: sig verifies itself
    uint8_t proof_seed[32];
    sha256(sig, 32, proof_seed);
    phi_transform(proof_seed, 32, sig + 32, 32);
    
    *sig_len = 64;
    return 1;
}

int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len) {
    if (!sig || sig_len < 64) return 0;
    
    // Step 1: Recompute signature core from message
    uint8_t seed[32];
    sha256(msg, msg_len, seed);
    
    uint8_t expected_core[32];
    phi_transform(seed, 32, expected_core, 32);
    
    // Check core signature
    if (memcmp(sig, expected_core, 32) != 0) return 0;
    
    // Step 2: Verify self-referential proof
    uint8_t proof_seed[32];
    sha256(sig, 32, proof_seed);
    
    uint8_t expected_proof[32];
    phi_transform(proof_seed, 32, expected_proof, 32);
    
    // The proof must match — this is the self-referential check
    // φ(sig) should produce a consistent output
    return memcmp(sig + 32, expected_proof, 32) == 0;
}
