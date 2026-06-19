# Φ-SIG: Golden Ratio Keyless Signatures

**No keys. No storage. Pure φ.**

## What is Φ-SIG?

Φ-SIG is a **keyless signature scheme** based on the **self-referential property 
of the golden ratio**: `φ = 1 + 1/φ`.

Traditional signatures require:
- Key generation (public + private key)
- Key storage (wallet, seed phrase, hardware)
- Key management (backup, rotation, recovery)

Φ-SIG requires **none of these.** The message itself, transformed through 
φ-convergents (Fibonacci ratios), produces a self-verifying signature.

## How It Works
Message → SHA-256 → φ-transform → Signature (32 bytes)
↓
Self-verification proof (32 bytes)
↓
φ(signature) == proof → VERIFIED

text

The core transform uses **Fibonacci convergents** approaching φ:
- Fₙ₊₁/Fₙ → φ as n → ∞
- Each byte of output = convergent ratio mapped to [0,255]
- Irreversible: cannot recover input from convergent output

## Security

| Property | Basis |
|----------|-------|
| **One-way** | Irreversibility of continued fraction convergents |
| **Deterministic** | Same input always produces same output |
| **Message binding** | SHA-256 hash anchors signature to message |
| **Self-verifying** | Proof = φ-transform of signature itself |
| **Post-quantum** | No ECDSA, no lattices, no discrete log |

## Usage

```c
#include "phi_sig.h"

uint8_t sig[64];
size_t sig_len = sizeof(sig);

// Sign
phi_sign(message, message_len, sig, &sig_len);

// Verify
if (phi_verify(message, message_len, sig, sig_len)) {
    // Signature valid!
}
Size
64 bytes — 32 byte core + 32 byte self-verification proof.

Status
Experimental. Not yet standardized. Based on novel cryptographic assumptions
(golden ratio irrationality as one-way function).

License
MIT — ΦΩ0
