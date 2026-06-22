#ifndef PHI_SIG_AUTH_H
#define PHI_SIG_AUTH_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PHI_AUTH_BYTES 64

/*
 * Φ-SIG AUTH — True Keyless Authentication
 * 
 * The observer has a MEMORIZED secret (never stored).
 * The signature can only be verified by someone who knows the secret.
 * This proves the observer's identity without any stored key.
 * 
 * φ-observer entanglement: sig = φ(hash(msg || observer_secret))
 * Verification: recompute with same secret → must match
 */

/*
 * Sign with observer secret
 * secret: the observer's memorized secret (any length, never stored)
 * Returns 0 on success, signature is 64 bytes
 */
int phi_auth_sign(const uint8_t *msg, size_t msg_len,
                  const uint8_t *secret, size_t secret_len,
                  uint8_t *sig, size_t *sig_len);

/*
 * Verify with observer secret
 * Only someone who knows the SAME secret can verify
 * Returns 1 if valid (correct observer + untampered message)
 */
int phi_auth_verify(const uint8_t *msg, size_t msg_len,
                    const uint8_t *secret, size_t secret_len,
                    const uint8_t *sig, size_t sig_len);

/*
 * Verify with WRONG secret — MUST fail
 * Proves that the original observer signed it
 */
int phi_auth_verify_wrong_secret(const uint8_t *msg, size_t msg_len,
                                  const uint8_t *wrong_secret, size_t wrong_secret_len,
                                  const uint8_t *sig, size_t sig_len);

#ifdef __cplusplus
}
#endif

#endif
