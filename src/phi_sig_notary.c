#include "phi_sig.h"
#include "phi_sig_notary.h"
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

static uint64_t get_timestamp(void) {
    return (uint64_t)time(NULL);
}

static void chain_hash(const uint8_t *data, size_t len, uint8_t *hash_out) {
    SHA256(data, len, hash_out);
}

int phi_notarize(const uint8_t *msg, size_t msg_len,
                 PhiNotaryEntry *entry) {
    if (!msg || !entry) return -1;
    memset(entry, 0, sizeof(*entry));
    
    size_t core_len = 64;
    if (phi_sign(msg, msg_len, entry->core_sig, &core_len) != 0) return -1;
    
    entry->timestamp = get_timestamp();
    
    uint8_t seal_input[64 + 8];
    memcpy(seal_input, entry->core_sig, 64);
    memcpy(seal_input + 64, &entry->timestamp, 8);
    
    size_t seal_len = 64;
    if (phi_sign(seal_input, 72, entry->notary_seal, &seal_len) != 0) return -1;
    
    return 0;
}

int phi_notary_verify(const uint8_t *msg, size_t msg_len,
                      const PhiNotaryEntry *entry) {
    if (!msg || !entry) return 0;
    if (!phi_verify(msg, msg_len, entry->core_sig, 64)) return 0;
    
    uint8_t seal_input[64 + 8];
    memcpy(seal_input, entry->core_sig, 64);
    memcpy(seal_input + 64, &entry->timestamp, 8);
    
    return phi_verify(seal_input, 72, entry->notary_seal, 64);
}

int phi_chain_append(const uint8_t *prev_chain_hash,
                     const uint8_t *msg, size_t msg_len,
                     PhiChainEntry *entry) {
    if (!msg || !entry) return -1;
    memset(entry, 0, sizeof(*entry));
    
    // Chain hash = hash of previous entry (or zeros for genesis)
    if (prev_chain_hash) {
        memcpy(entry->chain_hash, prev_chain_hash, 64);
    }
    
    // Notarize the message
    if (phi_notarize(msg, msg_len, &entry->entry) != 0) return -1;
    
    // Chain seal = Φ-SIG(chain_hash || notary_entry)
    uint8_t seal_input[64 + 136];
    memcpy(seal_input, entry->chain_hash, 64);
    memcpy(seal_input + 64, &entry->entry, 136);
    
    size_t seal_len = 64;
    if (phi_sign(seal_input, 200, entry->chain_seal, &seal_len) != 0) return -1;
    
    return 0;
}

int phi_chain_verify(const uint8_t *prev_chain_hash,
                     const uint8_t *msg, size_t msg_len,
                     const PhiChainEntry *entry) {
    if (!msg || !entry) return 0;
    
    // Verify chain hash links to previous
    if (prev_chain_hash && memcmp(entry->chain_hash, prev_chain_hash, 64) != 0) return 0;
    
    // Verify notary entry
    if (!phi_notary_verify(msg, msg_len, &entry->entry)) return 0;
    
    // Verify chain seal
    uint8_t seal_input[64 + 136];
    memcpy(seal_input, entry->chain_hash, 64);
    memcpy(seal_input + 64, &entry->entry, 136);
    
    return phi_verify(seal_input, 200, entry->chain_seal, 64);
}

int phi_chain_verify_all(const PhiChainEntry *chain, size_t count,
                         const uint8_t **messages, const size_t *msg_lens) {
    if (!chain || !messages || !msg_lens || count == 0) return 0;
    
    // Genesis: prev_hash = NULL
    const uint8_t *prev_hash = NULL;
    
    for (size_t i = 0; i < count; i++) {
        if (!phi_chain_verify(prev_hash, messages[i], msg_lens[i], &chain[i])) {
            return 0;
        }
        // FIXED: Use chain_seal as the hash link for next entry
        // This matches what phi_chain_append uses as prev_chain_hash
        prev_hash = chain[i].chain_seal;
    }
    
    return 1;
}
