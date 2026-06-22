#ifndef PHI_SIG_H
#define PHI_SIG_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PHI_SIG_BYTES 64

int phi_sign(const uint8_t *msg, size_t msg_len,
             uint8_t *sig, size_t *sig_len);

int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len);

#ifdef __cplusplus
}
#endif

#endif
