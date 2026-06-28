#ifndef PHI_SIG_H
#define PHI_SIG_H

#include <stdint.h>
#include <stddef.h>

#define PHI_SIG_PUBLICKEYBYTES 33
#define PHI_SIG_SECRETKEYBYTES 32
#define PHI_SIG_BYTES 98  // 65 (sig) + 33 (pk)

int phi_keygen(uint8_t *pk, uint8_t *sk);
int phi_sign(const uint8_t *msg, size_t msg_len, uint8_t *sig, size_t *sig_len);
int phi_verify(const uint8_t *msg, size_t msg_len, const uint8_t *sig, size_t sig_len);

#endif
