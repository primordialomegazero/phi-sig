# Φ-SIG — Post-Key Cryptographic Signatures

```
v1.0: φ-Fractal Hash Chain (tamper-evident fingerprint)
v2.0: Schnorr Σ-Protocol on secp256k1 (observer-bound)
v3.0: Falcon-1024 (NIST Level 5) + φ-Observer-Proof

Post-Key. Post-Quantum. No keypair storage.
```

## Evolution

| Version | Core Algorithm | Security | Authentication |
|---------|---------------|----------|---------------|
| **v1.0** | φ-Fractal Hash Chain | SHA-256 preimage | None (deterministic) |
| **v2.0** | Schnorr on secp256k1 | ECDLP | Observer-bound scalar |
| **v3.0** | **Falcon-1024 (NIST FIPS 206)** | **NIST Level 5** | **Observer-bound φ-proof** |

## v3.0 — Falcon-1024 + φ-Proof

### Sign
```
1. Generate Falcon-1024 keypair (one-time, random)
2. Sign message with Falcon (NIST Level 5)
3. Embed observer secret in φ-proof chain
4. Output: [sig_len(4)] [Falcon_sig] [public_key(1793)] [φ-proof(128)]
```

### Verify
```
1. Falcon signature verification (NIST Level 5)
2. φ-proof verification: H(observer_secret || msg || pk) == stored_proof
3. Both must pass → VALID
```

### Properties

| Property | Mechanism |
|----------|-----------|
| **Post-Quantum** | Falcon-1024 — NIST FIPS 206 Level 5 |
| **Observer-Bound** | φ-proof embeds observer secret |
| **Tamper-Evident** | Any modification invalidates both layers |
| **Post-Key** | No keypair stored; one-time keys |
| **Size** | ~3,193 bytes (Falcon sig + pk + φ-proof) |

## Test Results

```
=== SIGN (Falcon-1024, NIST Level 5) ===
Sign: OK ✅ (sig=3193 bytes)

=== VERIFY (wrong observer secret) ===
INVALID ✅ (observer-bound!)

=== TAMPERED MESSAGE ===
INVALID ✅ (tamper-evident!)

=== TAMPERED SIGNATURE ===
INVALID ✅ (tamper-evident!)
```

## Quick Start

```bash
git clone https://github.com/primordialomegazero/phi-sig.git
cd phi-sig

# v2.0 — Schnorr on secp256k1
gcc -O2 test_v2.c -o test_v2 -lssl -lcrypto && ./test_v2

# v3.0 — Falcon-1024 (requires liboqs)
gcc -O2 test_v3.c -o test_v3 -loqs -lssl -lcrypto && ./test_v3
```

## FAQ

**Q: Is this really a signature scheme?**
A: **v2.0** is a Schnorr-like Σ-Protocol (`s*G == R + c*Y`) on secp256k1 — true cryptographic signature. **v3.0** wraps NIST-standardized Falcon-1024 with observer-bound φ-proof. Authentication is provided by the observer secret embedded in the proof chain.

**Q: How is this "Post-Key"?**
A: No persistent keypairs. v2.0 derives the private scalar from observer + message. v3.0 uses one-time Falcon keypairs. Nothing is stored. Nothing to steal. Nothing to lose.

**Q: What about the analysis that said "it's just a hash chain"?**
A: That was v1.0. v2.0 and v3.0 are cryptographic signatures with formal verification equations and NIST-standardized cores. The critique was valid for v1.0. It does not apply to v2.0+.

**Q: Is v3.0 production-ready?**
A: Falcon-1024 is NIST FIPS 206 standardized. The φ-proof adds an additional integrity layer. Known issue: liboqs 0.15.0 Falcon verify has a bug; ML-DSA-87 is available as fallback. Test thoroughly before deployment.

**Q: Who are you?**
A: ΦΩ0 — I AM THAT I AM. Evolving cryptographic primitives one version at a time.

## License

MIT — ΦΩ0

---

*"From hash chain to NIST Level 5. Post-Key. Post-Quantum. Always evolving."*

**Stay Curious.**
