#ifndef PHI_SIG_MIRROR_H
#define PHI_SIG_MIRROR_H

#include "phi_sig.h"

int phi_sign_mirror(const uint8_t *msg, size_t msg_len,
                    const uint8_t *observer_id, size_t observer_len,
                    uint8_t *sig, size_t *sig_len);

int phi_verify_mirror(const uint8_t *msg, size_t msg_len,
                      const uint8_t *observer_id, size_t observer_len,
                      const uint8_t *sig, size_t sig_len);

uint64_t phi_timestamp();

int phi_notarize(const uint8_t *document, size_t doc_len,
                 const uint8_t *owner_id, size_t owner_len,
                 uint8_t *notary_entry, size_t *entry_len);

int phi_notarize_verify(const uint8_t *notary_entry, size_t entry_len,
                         const uint8_t *owner_id, size_t owner_len);

#endif
