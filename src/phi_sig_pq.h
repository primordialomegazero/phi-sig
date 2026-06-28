#ifndef PHI_SIG_PQ_H
#define PHI_SIG_PQ_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Pure-φ Post-Quantum layer — φ-chaotic irreversibility
// No external dependencies. No lattices. No OQS.
int phi_pq_sign(const uint8_t *msg, size_t msg_len,
                uint8_t *sig, size_t *sig_len);

int phi_pq_verify(const uint8_t *msg, size_t msg_len,
                  const uint8_t *sig, size_t sig_len);

#ifdef __cplusplus
}
#endif

#endif
