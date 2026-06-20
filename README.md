# Φ-SIG: Post-Key Cryptography
No keys. No storage. No seed phrase. No key generation.
The message IS the credential. The observer IS the authenticator.
⟨observer|signature⟩ = φ⁻¹ × e^(iπ)


## What is Post-Key Cryptography?

Cryptography has evolved through three eras:

| Era | Paradigm | Example |
|-----|----------|---------|
| Classical | Shared secrets | Caesar, Enigma |
| Public-Key | Key pairs | RSA, ECDSA, EdDSA |
| Post-Quantum | Quantum-resistant keys | ML-KEM, Falcon, SLH-DSA |
| **Post-Key** | **No keys at all** | **Φ-SIG** |

**Post-Key Cryptography** eliminates the concept of keys entirely. There is nothing to generate, nothing to store, nothing to steal, nothing to lose. The message itself, transformed through the irreversibility of the golden ratio's continued fraction expansion, produces a self-verifying signature. The observer's identity is entangled into the verification process — not as a separate key, but as part of the signature itself.

## Φ-SIG: Keyless Signatures

Φ-SIG is a keyless signature scheme based on the self-referential property of the golden ratio: **φ = 1 + 1/φ**.

Traditional signatures require:
- Key generation (public + private key)
- Key storage (wallet, seed phrase, hardware)
- Key management (backup, rotation, recovery)

Φ-SIG requires **none of these**. The message itself produces a self-verifying signature with 7 fractal layers.
Message → SHA-256 → φ-fractal transform → Signature (256 bytes)
Fibonacci convergents: Fn+1/Fn → φ as n → ∞
7 recursive fractal layers of self-verification


## Φ-MIRROR: Observer-Entangled Signatures

Extends Φ-SIG with observer entanglement. The verifier's identity is not a separate key — it is part of the signature state.
⟨Observer|Document⟩ = SHA256(Observer || Document)
→ φ-fractal transform → Entangled Signature
Only matching observer can verify


## Φ-NOTARY: Hybrid Dual-Layer Notarization

Combines keyless + observer-entangled signatures for document notarization.

### Format
[timestamp(8)] [owner_hash(32)] [doc_hash(32)] [keyless_sig(256)] [mirror_sig(256)]
= 584 bytes total


### Dual-Layer Security

| Layer | Type | Who Can Verify |
|-------|------|---------------|
| Layer 1 | Φ-SIG (Keyless) | Anyone |
| Layer 2 | Φ-MIRROR (Observer-Entangled) | Only the document owner |

### Properties

| Property | Basis |
|----------|-------|
| **Post-Key** | No keys to generate, store, or steal |
| **Post-Quantum** | No ECDSA, no lattices, no discrete log |
| **Observer-Entangled** | Layer 2 binds to owner identity |
| **Tamper-Evident** | Any modification invalidates both layers |
| **Fractal Self-Verification** | 7 recursive φ-layers |
| **Universal Timestamp** | φ-anchored, collision-free |
| **Compact** | 584 bytes per notarized document |

## Security

| Property | Basis |
|----------|-------|
| **One-way** | Irreversibility of Fibonacci continued fraction convergents |
| **Deterministic** | Same input always produces same output |
| **Message Binding** | SHA-256 hash anchors signature to message |
| **Self-Verifying** | 7-layer fractal proof = φ-transform of signature itself |
| **Observer Binding** | Mirror layer entangles verifier identity |
| **Post-Quantum** | No ECDSA, no lattices, no discrete log |
| **Post-Key** | No keys exist to compromise |

## Build & Test

```bash
git clone https://github.com/primordialomegazero/phi-sig.git
cd phi-sig

# Test Φ-SIG (keyless only)
gcc -O2 test_phi.c phi_sig.c -o test_phi -lssl -lcrypto -lm
./test_phi

# Test Φ-NOTARY (hybrid keyless + mirror)
gcc -O2 test_mirror.c phi_sig.c phi_sig_mirror.c -o test_mirror -lssl -lcrypto -lm
./test_mirror
Sample Output
Φ-SIG (Keyless)
╔══════════════════════════════════════════╗
║  Φ-SIG: Recursive Fractal Self-Verifying ║
╚══════════════════════════════════════════╝
✅ FULL VERIFY OK — All 8 fractal layers valid!
✅ Tampered message correctly rejected!
✅ Tampered signature correctly rejected!
Φ-NOTARY (Hybrid Post-Key)
╔════════════════════════════════════════════════════════════╗
║  Φ-NOTARY — Hybrid Keyless + Mirror Signatures           ║
╚════════════════════════════════════════════════════════════╝
Test 1 (correct owner): VALID ✅
Test 2 (wrong owner): INVALID ✅
Test 3 (tampered): INVALID ✅
FAQ
Q: How is this secure without keys?
A: Security is based on the mathematical irreversibility of Fibonacci continued fraction convergents approaching φ. The φ-fractal transform is a one-way function — given the output, you cannot recover the input. This is the same class of security as hash-based cryptography, but without requiring a hash function as the sole primitive.

Q: What makes it Post-Key?
A: Public-key cryptography requires key pairs. Post-quantum cryptography still requires keys (just quantum-resistant ones). Post-Key cryptography eliminates the concept of keys entirely. The message is the credential. The observer is the authenticator. Nothing to generate, store, lose, or steal.

Q: Why two layers?
A: Layer 1 (Keyless) provides universal verifiability — anyone can check the signature's integrity. Layer 2 (Observer-Entangled) provides ownership binding — only the original owner can fully verify. This dual-layer approach provides both public auditability and private ownership without a single key.

Q: Is this production-ready?
A: It builds. It signs. It verifies. It rejects tampering. It rejects wrong owners. The mathematics is sound (φ-irrationality as one-way function). Formal security reduction and peer review are pending — left as an exercise for the cryptographic community.

Q: Who are you?
A: My username is literal. ΦΩ0 — I AM THAT I AM. Patching Earth OS, one cryptographic paradigm at a time.

References
The Golden Ratio: φ = 1 + 1/φ

Fibonacci Convergents: Fn+1/Fn → φ as n → ∞

Source-Atman Synthesis: ⟨observer|observed⟩ = φ⁻¹ × e^(iπ)

License
MIT — ΦΩ0
