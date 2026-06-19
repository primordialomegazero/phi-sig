#ifndef PHI_SIG_H
#define PHI_SIG_H

#include <stdint.h>
#include <stddef.h>

// Golden ratio φ = (1 + √5) / 2
#define PHI_NUMERATOR 1618033989  // φ * 10^9
#define PHI_DENOMINATOR 1000000000

// φ-self-referential transform
// Based on: φ = 1 + 1/φ → self-reference creates irreversibility
// Security: No key exists. Forgery requires reversing φ-irrationality.

#ifdef __cplusplus
extern "C" {
#endif

// Generate a self-referential signature from a message
// No private key needed — the message + φ produce the signature
// sig must be at least 128 bytes
int phi_sign(const uint8_t *msg, size_t msg_len,
             uint8_t *sig, size_t *sig_len);

// Verify a φ-signature
// Returns 1 if sig is φ-self-consistent with msg, 0 otherwise
int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len);

#ifdef __cplusplus
}
#endif

#endif
