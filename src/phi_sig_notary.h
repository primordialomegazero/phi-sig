#ifndef PHI_SIG_NOTARY_H
#define PHI_SIG_NOTARY_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PHI_NOTARY_TIMESTAMP_SIZE 8
#define PHI_NOTARY_SIG_SIZE 64
#define PHI_NOTARY_ENTRY_SIZE 136  // 64 + 8 + 64

/*
 * Notarized Φ-SIG Entry
 * 
 * Layer 1: Φ-SIG(message) — 64 bytes, keyless integrity
 * Layer 2: Timestamp — 8 bytes, temporal proof
 * Layer 3: Φ-SIG(layer1 || layer2) — 64 bytes, notary seal
 * 
 * Total: 136 bytes per entry
 */
typedef struct {
    uint8_t core_sig[64];       // Φ-SIG(message)
    uint64_t timestamp;          // Unix timestamp
    uint8_t notary_seal[64];    // Φ-SIG(core || timestamp)
} PhiNotaryEntry;

/*
 * Notarize a message with timestamp
 * Returns 0 on success
 */
int phi_notarize(const uint8_t *msg, size_t msg_len,
                 PhiNotaryEntry *entry);

/*
 * Verify a notarized entry
 * Returns 1 if valid (both core and notary seal match)
 */
int phi_notary_verify(const uint8_t *msg, size_t msg_len,
                      const PhiNotaryEntry *entry);

/*
 * Immutable Audit Log
 * Each entry chains to the previous via φ-hash
 */
typedef struct {
    uint8_t chain_hash[64];     // Hash of previous entry
    PhiNotaryEntry entry;       // Notarized entry (136 bytes)
    uint8_t chain_seal[64];     // Φ-SIG(chain_hash || entry)
} PhiChainEntry;

#define PHI_CHAIN_ENTRY_SIZE (64 + 136 + 64)  // 264 bytes

/*
 * Append to immutable chain
 * Returns 0 on success
 */
int phi_chain_append(const uint8_t *prev_chain_hash,
                     const uint8_t *msg, size_t msg_len,
                     PhiChainEntry *entry);

/*
 * Verify chain entry against previous hash
 * Returns 1 if chain is intact
 */
int phi_chain_verify(const uint8_t *prev_chain_hash,
                     const uint8_t *msg, size_t msg_len,
                     const PhiChainEntry *entry);

/*
 * Verify entire chain
 * Returns 1 if ALL entries are valid and chained correctly
 */
int phi_chain_verify_all(const PhiChainEntry *chain, size_t count,
                         const uint8_t **messages, const size_t *msg_lens);

#ifdef __cplusplus
}
#endif

#endif
