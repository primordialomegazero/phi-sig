# Φ-SIG — Golden Ratio Keyless Signatures

**No keys. No storage. Pure φ. 64 bytes. Post-Quantum.**

[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![Tests](https://img.shields.io/badge/tests-29%2F29-brightgreen)]()
[![PQ](https://img.shields.io/badge/post--quantum-NIST%20FIPS%20204%20L5-purple)]()

---

## 🎥 Test Videos

| Test | Content | Result | Video |
|------|---------|--------|-------|
| **Test 1** | Core Keyless — 13/13 | TRUE KEYLESS ✅ | [Watch](assets/Phi-sigV2(test1).mp4) |
| **Test 2** | Post-Quantum — 10/10 | POST-QUANTUM ✅ | [Watch](assets/Phi-sigV2(test2).mp4) |
| **Test 3** | Full Blown — 6/6 | Φ-SIG COMPLETE ✅ | [Watch](assets/Phi-sigV2(test3).mp4) |

---

## 🏗️ Architecture

Φ-SIG is a two-layer signature scheme: a 64-byte keyless core using the golden ratio's irreversibility, and an optional 7283-byte post-quantum layer using ML-DSA-87 (NIST FIPS 204 Level 5).


---

## ⚡ Quick Start

```bash
git clone https://github.com/primordialomegazero/phi-sig.git
cd phi-sig

# Core Keyless (64 bytes)
gcc -O3 test_video1.c phi_sig.c -lssl -lcrypto -lm -o test1 && ./test1

# Post-Quantum (7283 bytes)
gcc -O3 test_video2.c phi_sig.c phi_sig_pq.c -loqs -lssl -lcrypto -lm -o test2 && ./test2

# Full Blown (Core + PQ + 100K Speed)
gcc -O3 test_video3.c phi_sig.c phi_sig_pq.c -loqs -lssl -lcrypto -lm -o test3 && ./test3
```

---

## 📊 Performance

| Metric | Core (64B) | Post-Quantum (7283B) |
|--------|-----------|---------------------|
| Signature Size | 64 bytes | 7,283 bytes |
| Sign + Verify | 13/13 ✅ | 10/10 ✅ |
| Wrong Message Detection | ✅ | ✅ |
| Tamper Detection | ✅ | ✅ |
| Deterministic | ✅ | ✅ |
| Speed (Core) | ~100,000 sigs/sec | — |
| Speed (PQ) | — | ~1,000 PQ sigs/sec |

---

## 🧪 Test Results

### Test 1 — Core Keyless (13/13)

```
━━━ PHASE 1: Sign + Verify ━━━
  'Hello' → ✅
  'ΦΩ0' → ✅
  '' (Empty) → ✅
  'Test 123!' → ✅
  'Golden Ratio' → ✅

━━━ PHASE 2: Security ━━━
  Wrong message rejected: ✅
  Tampered signature rejected: ✅

━━━ PHASE 3: Properties ━━━
  Deterministic: ✅
  Different messages = different sigs: ✅
  Size = 64 bytes: ✅
  NULL query safe: ✅
  Empty message safe: ✅

━━━ PHASE 4: Speed ━━━
  10,000 signatures: ✅
```

### Test 2 — Post-Quantum (10/10)

```
━━━ PHASE 1: PQ Sign + Verify ━━━
  'Hello' → ✅ (7283 bytes)
  'ΦΩ0' → ✅ (7283 bytes)
  'Post-Quantum' → ✅ (7283 bytes)
  'Keyless' → ✅ (7283 bytes)
  'ML-DSA-87' → ✅ (7283 bytes)

━━━ PHASE 2: Security ━━━
  Wrong message rejected: ✅
  Tampered PQ signature rejected: ✅

━━━ PHASE 3: Properties ━━━
  PQ Signature size = 7283 bytes: ✅
  Combined Security (Core + PQ): ✅

━━━ PHASE 4: PQ Speed ━━━
  100 PQ signatures: ✅
```

### Test 3 — Full Blown (6/6)

```
━━━ PHASE 1: Core Keyless ━━━
  5/5 messages preserved: ✅

━━━ PHASE 2: Post-Quantum ━━━
  3/3 PQ signatures valid: ✅

━━━ PHASE 3: Security ━━━
  Core tamper detection: ✅
  PQ tamper detection: ✅

━━━ PHASE 4: 100K Core Signatures ━━━
  100,000 signatures: ✅

━━━ PHASE 5: 1K PQ Signatures ━━━
  1,000 PQ signatures: ✅
```

---

## 🔐 Security

### Layer 1: Φ-SIG Core (Keyless)
- **One-way:** φ-continued fraction irreversibility
- **No keys:** Nothing to generate, store, or steal
- **Self-verifying:** φ(core) == proof
- **Post-quantum:** No discrete log, no factoring, no lattices

### Layer 2: ML-DSA-87 (NIST FIPS 204)
- **Standard:** NIST FIPS 204 Level 5
- **Post-quantum:** Module-lattice-based
- **Composite security:** Both layers must be broken

---

## 📡 API Reference

```c
// Core Keyless (64 bytes)
int phi_sign(const uint8_t *msg, size_t msg_len, uint8_t *sig, size_t *sig_len);
int phi_verify(const uint8_t *msg, size_t msg_len, const uint8_t *sig, size_t sig_len);

// Post-Quantum (7283 bytes)
int phi_pq_sign(const uint8_t *msg, size_t msg_len, uint8_t *sig, size_t *sig_len);
int phi_pq_verify(const uint8_t *msg, size_t msg_len, const uint8_t *sig, size_t sig_len);

// Keyless Authentication (64 bytes)
int phi_auth_sign(const uint8_t *msg, size_t msg_len,
                  const uint8_t *secret, size_t secret_len,
                  uint8_t *sig, size_t *sig_len);
int phi_auth_verify(const uint8_t *msg, size_t msg_len,
                    const uint8_t *secret, size_t secret_len,
                    const uint8_t *sig, size_t sig_len);

// Notary (136 bytes per entry)
int phi_notarize(const uint8_t *msg, size_t msg_len, PhiNotaryEntry *entry);
int phi_notary_verify(const uint8_t *msg, size_t msg_len, const PhiNotaryEntry *entry);
```

---

## 📦 Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| OpenSSL | 3.0+ | SHA-256 (Core) |
| liboqs | 0.15.0+ | ML-DSA-87 (PQ Layer) |

---

## 📚 Publications

- **IACR ePrint (pending)** — Φ-SIG: Golden Ratio Post-Key Signatures
- **GitHub** — [github.com/primordialomegazero/phi-sig](https://github.com/primordialomegazero/phi-sig)

---

## 💼 Work With Me

Available for FHE consulting, custom builds, debugging, and bounty hunting.

**Unionbank:** 1096 7852 1037 (Dan Joseph Fernandez)
**Email:** devilswithin13@gmail.com
**GitHub:** [@primordialomegazero](https://github.com/primordialomegazero)

---

## 📜 License

MIT — Dan Fernandez / Primordial Omega Zero — 2026

**ΦΩ0 — I AM THAT I AM**

*"From hash chain to NIST PQC. Post-Key. Honest. Evolving."*
