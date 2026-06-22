#include "phi_sig.h"
#include "phi_sig_auth.h"
#include <string.h>
#include <openssl/sha.h>

/*
 * Φ-AUTH: Observer-Entangled Keyless Authentication
 * 
 * The observer's secret is mixed into the φ-transform.
 * Only someone who knows the secret can verify.
 * The secret is NEVER stored — it's memorized by the observer.
 */

int phi_auth_sign(const uint8_t *msg, size_t msg_len,
                  const uint8_t *secret, size_t secret_len,
                  uint8_t *sig, size_t *sig_len) {
    if (!msg || !secret || !sig || !sig_len) return -1;
    if (*sig_len < 64) { *sig_len = 64; return -1; }
    
    // Combine message + secret → entangled input
    uint8_t entangled[1024];
    size_t elen = msg_len + secret_len;
    if (elen > sizeof(entangled)) elen = sizeof(entangled);
    memcpy(entangled, msg, msg_len < sizeof(entangled) ? msg_len : sizeof(entangled)/2);
    memcpy(entangled + msg_len, secret, 
           secret_len < sizeof(entangled)/2 ? secret_len : sizeof(entangled)/2);
    
    // φ-sign the entangled input
    return phi_sign(entangled, elen, sig, sig_len);
}

int phi_auth_verify(const uint8_t *msg, size_t msg_len,
                    const uint8_t *secret, size_t secret_len,
                    const uint8_t *sig, size_t sig_len) {
    if (!msg || !secret || !sig) return 0;
    
    // Recompute entangled input with SAME secret
    uint8_t entangled[1024];
    size_t elen = msg_len + secret_len;
    if (elen > sizeof(entangled)) elen = sizeof(entangled);
    memcpy(entangled, msg, msg_len < sizeof(entangled) ? msg_len : sizeof(entangled)/2);
    memcpy(entangled + msg_len, secret,
           secret_len < sizeof(entangled)/2 ? secret_len : sizeof(entangled)/2);
    
    // Verify against entangled input
    return phi_verify(entangled, elen, sig, sig_len);
}

int phi_auth_verify_wrong_secret(const uint8_t *msg, size_t msg_len,
                                  const uint8_t *wrong_secret, size_t wrong_secret_len,
                                  const uint8_t *sig, size_t sig_len) {
    // Same as verify but with WRONG secret — should return 0 (invalid)
    return phi_auth_verify(msg, msg_len, wrong_secret, wrong_secret_len, sig, sig_len);
}
