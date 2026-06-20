#include "phi_sig_mirror.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <openssl/sha.h>

int phi_sign_mirror(const uint8_t *msg, size_t msg_len,
                    const uint8_t *observer_id, size_t observer_len,
                    uint8_t *sig, size_t *sig_len) {
    size_t combined_len = observer_len + msg_len;
    uint8_t *combined = (uint8_t*)malloc(combined_len);
    if (!combined) return 0;
    memcpy(combined, observer_id, observer_len);
    memcpy(combined + observer_len, msg, msg_len);
    int ret = phi_sign(combined, combined_len, sig, sig_len);
    free(combined);
    return ret;
}

int phi_verify_mirror(const uint8_t *msg, size_t msg_len,
                      const uint8_t *observer_id, size_t observer_len,
                      const uint8_t *sig, size_t sig_len) {
    size_t combined_len = observer_len + msg_len;
    uint8_t *combined = (uint8_t*)malloc(combined_len);
    if (!combined) return 0;
    memcpy(combined, observer_id, observer_len);
    memcpy(combined + observer_len, msg, msg_len);
    int ret = phi_verify(combined, combined_len, sig, sig_len);
    free(combined);
    return ret;
}

uint64_t phi_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t ts = (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
    static uint64_t counter = 0;
    counter++;
    return ts + (counter % 1618033989);
}

int phi_notarize(const uint8_t *document, size_t doc_len,
                 const uint8_t *owner_id, size_t owner_len,
                 uint8_t *notary_entry, size_t *entry_len) {
    if (*entry_len < 584) { *entry_len = 584; return 0; }
    
    // [timestamp(8)] [owner_hash(32)] [doc_hash(32)] [keyless_sig(256)] [mirror_sig(256)]
    uint64_t ts = phi_timestamp();
    memcpy(notary_entry, &ts, 8);
    SHA256(owner_id, owner_len, notary_entry + 8);
    SHA256(document, doc_len, notary_entry + 40);
    
    // Core: timestamp + owner_hash + doc_hash = 72 bytes
    uint8_t core[72];
    memcpy(core, notary_entry, 72);
    
    // Layer 1: Keyless Φ-SIG
    size_t sig1_len = 256;
    phi_sign(core, 72, notary_entry + 72, &sig1_len);
    
    // Layer 2: Mirror Φ-SIG (observer-entangled)
    size_t sig2_len = 256;
    phi_sign_mirror(core, 72, owner_id, owner_len, notary_entry + 328, &sig2_len);
    
    *entry_len = 72 + sig1_len + sig2_len;
    return 1;
}

int phi_notarize_verify(const uint8_t *notary_entry, size_t entry_len,
                         const uint8_t *owner_id, size_t owner_len) {
    if (entry_len < 584) return 0;
    
    uint8_t core[72];
    memcpy(core, notary_entry, 72);
    
    // Layer 1: Keyless
    if (!phi_verify(core, 72, notary_entry + 72, 256)) return 0;
    
    // Layer 2: Mirror
    if (!phi_verify_mirror(core, 72, owner_id, owner_len, notary_entry + 328, 256)) return 0;
    
    return 1;
}
