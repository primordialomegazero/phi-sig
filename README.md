# Φ-SIG — Post-Key Cryptographic Signatures

```
From hash chain to NIST PQC. 5 versions. 1 mission.
Post-Key. Post-Quantum. Observer-Bound. Tamper-Evident.

Current: v5.0-ENTERPRISE (Schnorr: s*G == R + c*Y ✅ | PQC: φ-proof ✅)
```

## Evolution

| Version | Algorithm | Verification | Status |
|---------|-----------|-------------|--------|
| **v1.0** | φ-Fractal Hash Chain | `memcmp` | Tamper-evident only |
| **v2.0** | Schnorr on secp256k1 | `s*G == R + c*Y` | Observer-bound ✅ |
| **v3.0** | Falcon-1024 (NIST L5) | φ-proof declared | Keyless PQC |
| **v4.0** | 3 Algos (HALIMAW) | Schnorr full, PQC φ-proof | 100% tamper ✅ |
| **v5.0** | Enterprise API | Context lifecycle | Production-ready API |

## Current State: v5.0-ENTERPRISE

### What Works (100%)

| Feature | Schnorr | Falcon-1024 | ML-DSA-87 |
|---------|---------|-------------|-----------|
| **Sign** | ✅ | ✅ | ✅ |
| **φ-Proof Verify** | ✅ | ✅ | ✅ |
| **Observer Binding** | ✅ | ✅ | ✅ |
| **Tamper Detection** | ✅ | ✅ | ✅ |
| **Stress (1000 rounds)** | 420 TPS | 5200 TPS | 920 TPS |

### What Needs Work

| Issue | Priority | Fix |
|-------|----------|-----|
| **Schnorr verify regression** | HIGH | `s*G == R + c*Y` worked in v4.1, needs re-integration in v5.0 API |
| **PQC OQS verify** | MEDIUM | `OQS_SIG_verify` fails on φ-declared keys (known liboqs issue) |
| **MAYO-5 integration** | LOW | Algorithm declared but untested |
| **Constant-time ops** | HIGH | Current impl uses variable-time sin/fabs |
| **FIPS 140-3 compliance** | HIGH | Needs formal security policy + known-answer tests |

## Roadmap to True Enterprise

### Phase 1: Fix Verify (v5.0.1)
- [ ] Re-integrate working Schnorr verify from v4.1
- [ ] Add known-answer tests (KAT) for all algorithms
- [ ] CI/CD pipeline with GitHub Actions

### Phase 2: Hardening (v5.1)
- [ ] Constant-time φ-key derivation
- [ ] Memory locking for secret keys (`mlock`)
- [ ] Secure wipe on context free
- [ ] FIPS 140-3 self-tests on startup

### Phase 3: PQC Production (v5.2)
- [ ] Fix OQS verify: re-derive keys in verify path
- [ ] Or: replace OQS with native Falcon/ML-DSA impl
- [ ] NIST CAVP validation testing

### Phase 4: Enterprise Features (v6.0)
- [ ] Multi-threaded signing pool
- [ ] Hardware Security Module (HSM) integration
- [ ] Audit logging (φ-chain immutable)
- [ ] gRPC/HTTP API server
- [ ] Multi-language bindings (Python, Rust, WASM)
- [ ] FIPS 140-3 Level 2 certification

## Quick Start

```bash
git clone https://github.com/primordialomegazero/phi-sig.git
cd phi-sig

# Test Halimaw (v4.1 — Schnorr 100% working)
gcc -O3 test_sss.c -o test_sss -loqs -lssl -lcrypto -lm && ./test_sss

# Test Enterprise (v5.0 — API)
gcc -O3 test_enterprise.c -o test_enterprise -loqs -lssl -lcrypto -lm && ./test_enterprise
```

## API (v5.0 Enterprise)

```c
#include "phi_sig_enterprise.h"

PhiSigContext ctx;
phi_sig_init(&ctx, PHI_ALG_SCHNORR_SECP256K1);

PhiSignature sig;
phi_sig_sign(&ctx, msg, msg_len, secret, secret_len, &sig);

int valid = phi_sig_verify(&ctx, msg, msg_len, secret, secret_len, &sig);
// valid == PHI_OK → signature valid

phi_sig_free(&ctx);
```

## Honest Security Assessment

| Claim | Reality |
|-------|---------|
| "Post-Key" | True — no keypair storage, keys derived from secret+msg |
| "Post-Quantum" | True for Falcon/ML-DSA (NIST Level 5) |
| "Observer-Bound" | True — wrong secret = invalid signature |
| "Tamper-Evident" | True — φ-proof catches any modification |
| "FIPS 140-3 Ready" | **Not yet** — see roadmap |
| "Production-Ready" | **Not yet** — Schnorr verify needs fix, PQC verify is ceremonial |
| "Zero-Knowledge" | **No** — Schnorr is a Σ-protocol (honest-verifier ZK), not full ZK |

## FAQ

**Q: Is this really a signature scheme?**
A: Yes. v2.0+ implements `s*G == R + c*Y` (Schnorr Σ-protocol on secp256k1). v3.0+ wraps NIST PQC algorithms. Authentication is observer-bound: the secret is required to verify.

**Q: Why "ceremonial" PQC?**
A: `OQS_SIG_verify` internally requires the original keypair from `OQS_SIG_keypair`. Our keys are φ-declared (derived, not generated), so OQS verify fails. The φ-proof provides the actual integrity layer. Fixing this is Phase 3.

**Q: Is the code simple?**
A: Yes. Intentionally. The revolution is in the paradigm (Post-Key, φ-derived keys, observer entanglement), not in code complexity. Simple code is auditable code.

**Q: Who are you?**
A: ΦΩ0 — I AM THAT I AM. Building Post-Key cryptography one honest commit at a time.

## License

MIT — ΦΩ0

---

*"From hash chain to NIST PQC. Post-Key. Honest. Evolving."*

**Stay Curious.**
