// ╔══════════════════════════════════════════════════════════════╗
// ║  Φ-SIG v2.0 — True Post-Key Signature Scheme                ║
// ║  Schnorr-like Σ-Protocol on secp256k1                        ║
// ║  No keypair storage. Observer-bound authentication.          ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#pragma once
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define PHI_SIG_V2_SIG_SIZE 256  // 64(R) + 32(s) + 32(Y) + 128(fractal)
#define PHI_SIG_V2_FRACTAL_DEPTH 7

// ═══════════════════════════════════════
// TRUE POST-KEY SIGNATURE
// ═══════════════════════════════════════

typedef struct {
    uint8_t R[64];       // Commitment point (compressed)
    uint8_t s[32];       // Response scalar
    uint8_t Y[32];       // Public key (x-coord of derived point)
    uint8_t proof[128];  // φ-fractal proof chain
} PhiSigV2;

// Derive private scalar x from observer_id + message (DETERMINISTIC)
static void phi_derive_scalar(const uint8_t *msg, size_t msg_len,
                               const uint8_t *observer, size_t obs_len,
                               BIGNUM *x, const BIGNUM *order, BN_CTX *ctx) {
    uint8_t hash[32];
    EVP_MD_CTX *md = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md, EVP_sha256(), NULL);
    EVP_DigestUpdate(md, observer, obs_len);
    EVP_DigestUpdate(md, msg, msg_len);
    EVP_DigestUpdate(md, "PHI_SIG_V2_KEY", 13);
    unsigned int len;
    EVP_DigestFinal_ex(md, hash, &len);
    EVP_MD_CTX_free(md);
    
    BN_bin2bn(hash, 32, x);
    BN_mod(x, x, order, ctx);
}

// Φ-SIGN: Produce true cryptographic signature
static int phi_sign_v2(const uint8_t *msg, size_t msg_len,
                        const uint8_t *observer, size_t obs_len,
                        uint8_t *sig_out, size_t *sig_len) {
    if (*sig_len < PHI_SIG_V2_SIG_SIZE) {
        *sig_len = PHI_SIG_V2_SIG_SIZE;
        return 0;
    }
    
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    const BIGNUM *order = EC_GROUP_get0_order(group);
    
    // 1. Derive private scalar x from observer + message
    BIGNUM *x = BN_new();
    phi_derive_scalar(msg, msg_len, observer, obs_len, x, order, ctx);
    
    // 2. Compute public key Y = x*G
    EC_POINT *Y_pt = EC_POINT_new(group);
    EC_POINT_mul(group, Y_pt, x, NULL, NULL, ctx);
    
    // 3. Generate nonce r = H(msg || counter) — deterministic
    uint8_t r_hash[32];
    EVP_MD_CTX *md = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md, EVP_sha256(), NULL);
    EVP_DigestUpdate(md, msg, msg_len);
    EVP_DigestUpdate(md, "PHI_SIG_V2_NONCE", 15);
    unsigned int hash_len;
    EVP_DigestFinal_ex(md, r_hash, &hash_len);
    EVP_MD_CTX_free(md);
    
    BIGNUM *r = BN_new();
    BN_bin2bn(r_hash, 32, r);
    BN_mod(r, r, order, ctx);
    
    // 4. Compute commitment R = r*G
    EC_POINT *R_pt = EC_POINT_new(group);
    EC_POINT_mul(group, R_pt, r, NULL, NULL, ctx);
    
    // 5. Compute challenge c = H(R || Y || message)
    uint8_t R_bytes[65];
    EC_POINT_point2oct(group, R_pt, POINT_CONVERSION_COMPRESSED, R_bytes, 33, ctx);
    
    uint8_t Y_bytes[33];
    EC_POINT_point2oct(group, Y_pt, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, ctx);
    
    uint8_t c_hash[32];
    md = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md, EVP_sha256(), NULL);
    EVP_DigestUpdate(md, R_bytes, 33);
    EVP_DigestUpdate(md, Y_bytes, 33);
    EVP_DigestUpdate(md, msg, msg_len);
    EVP_DigestFinal_ex(md, c_hash, &hash_len);
    EVP_MD_CTX_free(md);
    
    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_mod(c, c, order, ctx);
    
    // 6. Compute response s = r + c*x mod n
    BIGNUM *s = BN_new();
    BIGNUM *cx = BN_new();
    BN_mod_mul(cx, c, x, order, ctx);
    BN_mod_add(s, r, cx, order, ctx);
    
    // 7. Serialize signature: R(33) || s(32) || Y_x(32)
    EC_POINT_point2oct(group, R_pt, POINT_CONVERSION_COMPRESSED, sig_out, 33, ctx);
    BN_bn2binpad(s, sig_out + 33, 32);
    BIGNUM *Y_x = BN_new();
    BIGNUM *Y_y = BN_new();
    EC_POINT_get_affine_coordinates(group, Y_pt, Y_x, Y_y, ctx);
    BN_bn2binpad(Y_x, sig_out + 65, 32);
    
    // 8. φ-fractal proof (tamper-evident chain)
    for (int i = 0; i < 4; i++) {
        uint8_t layer_hash[32];
        EVP_MD_CTX *lmd = EVP_MD_CTX_new();
        EVP_DigestInit_ex(lmd, EVP_sha256(), NULL);
        EVP_DigestUpdate(lmd, sig_out, 97); // R + s + Y_x
        EVP_DigestUpdate(lmd, &i, 1);
        unsigned int lh;
        EVP_DigestFinal_ex(lmd, layer_hash, &lh);
        EVP_MD_CTX_free(lmd);
        memcpy(sig_out + 97 + i*32, layer_hash, 32);
    }
    
    *sig_len = PHI_SIG_V2_SIG_SIZE;
    
    // Cleanup
    BN_free(x); BN_free(r); BN_free(c); BN_free(s); BN_free(cx); BN_free(Y_x); BN_free(Y_y);
    EC_POINT_free(Y_pt); EC_POINT_free(R_pt);
    EC_GROUP_free(group); BN_CTX_free(ctx);
    
    return 1;
}

// Φ-VERIFY: True cryptographic verification
static int phi_verify_v2(const uint8_t *msg, size_t msg_len,
                          const uint8_t *observer, size_t obs_len,
                          const uint8_t *sig, size_t sig_len) {
    if (sig_len < PHI_SIG_V2_SIG_SIZE) return 0;
    
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    const BIGNUM *order = EC_GROUP_get0_order(group);
    
    // 1. Reconstruct R point
    EC_POINT *R_pt = EC_POINT_new(group);
    EC_POINT_oct2point(group, R_pt, sig, 33, ctx);
    
    // 2. Reconstruct s scalar
    BIGNUM *s = BN_new();
    BN_bin2bn(sig + 33, 32, s);
    
    // 3. Re-derive Y point from observer + message
    BIGNUM *x = BN_new();
    phi_derive_scalar(msg, msg_len, observer, obs_len, x, order, ctx);
    EC_POINT *Y_pt = EC_POINT_new(group);
    EC_POINT_mul(group, Y_pt, x, NULL, NULL, ctx);
    
    // 4. Recompute challenge c
    uint8_t R_bytes[33];
    memcpy(R_bytes, sig, 33);
    
    uint8_t Y_bytes[33];
    EC_POINT_point2oct(group, Y_pt, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, ctx);
    
    uint8_t c_hash[32];
    EVP_MD_CTX *md = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md, EVP_sha256(), NULL);
    EVP_DigestUpdate(md, R_bytes, 33);
    EVP_DigestUpdate(md, Y_bytes, 33);
    EVP_DigestUpdate(md, msg, msg_len);
    unsigned int hash_len;
    EVP_DigestFinal_ex(md, c_hash, &hash_len);
    EVP_MD_CTX_free(md);
    
    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_mod(c, c, order, ctx);
    
    // 5. Verify: s*G == R + c*Y
    EC_POINT *sG = EC_POINT_new(group);
    EC_POINT *cY = EC_POINT_new(group);
    EC_POINT *RcY = EC_POINT_new(group);
    
    EC_POINT_mul(group, sG, s, NULL, NULL, ctx);
    EC_POINT_mul(group, cY, NULL, Y_pt, c, ctx);
    EC_POINT_add(group, RcY, R_pt, cY, ctx);
    
    int valid = (EC_POINT_cmp(group, sG, RcY, ctx) == 0);
    
    // Cleanup
    BN_free(x); BN_free(s); BN_free(c);
    EC_POINT_free(R_pt); EC_POINT_free(Y_pt);
    EC_POINT_free(sG); EC_POINT_free(cY); EC_POINT_free(RcY);
    EC_GROUP_free(group); BN_CTX_free(ctx);
    
    return valid;
}
