# Φ-SIG: Golden Ratio Keyless Signatures + Φ-NOTARY
No keys. No storage. No seed phrase. Pure φ.
Layer 1: Anyone can verify. Layer 2: Only the owner can verify.
⟨observer|signature⟩ = φ⁻¹ × e^(iπ)


## What is Φ-SIG?

Φ-SIG is a keyless signature scheme based on the self-referential property of the golden ratio: **φ = 1 + 1/φ**.

Traditional signatures require key generation, key storage, and key management. Φ-SIG requires none. The message itself, transformed through φ-convergents (Fibonacci ratios), produces a self-verifying signature with 7 fractal layers.

## Φ-NOTARY: Hybrid Keyless + Mirror Signatures

Extended with **Φ-MIRROR** — observer-entangled signatures for legal-grade document notarization.

### Dual-Layer Security

| Layer | Type | Verification |
|-------|------|-------------|
| Layer 1 | Φ-SIG Keyless | Anyone can verify |
| Layer 2 | Φ-MIRROR | Only document owner can verify |

### Notary Entry Format
[timestamp(8)] [owner_hash(32)] [doc_hash(32)] [keyless_sig(256)] [mirror_sig(256)]
= 584 bytes total


### Properties

| Property | Basis |
|----------|-------|
| **Keyless** | No keys to generate, store, or steal |
| **Observer-Entangled** | Mirror layer binds to owner identity |
| **Tamper-Evident** | Any modification invalidates both layers |
| **Post-Quantum** | No ECDSA, no lattices, no discrete log |
| **Universal Timestamp** | φ-anchored, collision-free |
| **Fractal Self-Verification** | 7 recursive φ-layers |
| **Compact** | 584 bytes per notarized document |

## How It Works

### Φ-SIG (Keyless)
Message → SHA-256 → φ-fractal transform → Signature (256 bytes)
Fibonacci convergents: Fn+1/Fn → φ as n → ∞
7 fractal layers of self-verification


### Φ-MIRROR (Observer-Entangled)
⟨Observer|Document⟩ = SHA256(Observer || Document)
→ φ-fractal transform → Entangled Signature
Only matching observer can verify


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
╔════════════════════════════════════════════════════════════╗
║  Φ-SIG: Recursive Fractal Self-Verifying                  ║
╚════════════════════════════════════════════════════════════╝
✅ FULL VERIFY OK — All 8 fractal layers valid!
✅ Tampered message correctly rejected!
✅ Tampered signature correctly rejected!

╔════════════════════════════════════════════════════════════╗
║  Φ-NOTARY — Hybrid Keyless + Mirror Signatures           ║
╚════════════════════════════════════════════════════════════╝
Test 1 (correct owner): VALID ✅
Test 2 (wrong owner): INVALID ✅
Test 3 (tampered): INVALID ✅
FAQ
Q: How is this secure without keys?
A: Security is based on the irreversibility of Fibonacci continued fraction convergents approaching φ. The φ-fractal transform is a one-way function — you cannot recover the input from the convergent output.

Q: What makes it post-quantum?
A: No discrete logarithm, no elliptic curves, no lattice problems. The security assumption is purely the irrationality of φ.

Q: Why two layers?
A: Layer 1 (Keyless) provides universal verifiability — anyone can check the signature. Layer 2 (Mirror) provides ownership binding — only the original owner can fully verify. This is optimal for legal documents: public verification + private ownership.

Q: Who are you?
A: ΦΩ0 — I AM THAT I AM.

License
MIT — ΦΩ0
