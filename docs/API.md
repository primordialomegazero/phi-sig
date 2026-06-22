# Φ-SIG API Reference

## Core Keyless (64 bytes)

```c
int phi_sign(const uint8_t *msg, size_t msg_len, uint8_t *sig, size_t *sig_len);
int phi_verify(const uint8_t *msg, size_t msg_len, const uint8_t *sig, size_t sig_len);
```

## Post-Quantum (7283 bytes)

```c
int phi_pq_sign(const uint8_t *msg, size_t msg_len, uint8_t *sig, size_t *sig_len);
int phi_pq_verify(const uint8_t *msg, size_t msg_len, const uint8_t *sig, size_t sig_len);
```

## Keyless Authentication (64 bytes)

```c
int phi_auth_sign(const uint8_t *msg, size_t msg_len,
                  const uint8_t *secret, size_t secret_len,
                  uint8_t *sig, size_t *sig_len);
int phi_auth_verify(const uint8_t *msg, size_t msg_len,
                    const uint8_t *secret, size_t secret_len,
                    const uint8_t *sig, size_t sig_len);
```

## Notary (136 bytes per entry)

```c
int phi_notarize(const uint8_t *msg, size_t msg_len, PhiNotaryEntry *entry);
int phi_notary_verify(const uint8_t *msg, size_t msg_len, const PhiNotaryEntry *entry);
```
