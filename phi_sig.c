#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/obj_mac.h>

#define PHI_SIG_PUBLICKEYBYTES 33
#define PHI_SIG_SECRETKEYBYTES 32
#define PHI_SIG_BYTES 98

// ═══ SHA256 using EVP (OpenSSL 3.0 compatible) ═══
static void sha256_evp(const uint8_t *data, size_t len, uint8_t *hash) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, data, len);
    EVP_DigestFinal_ex(ctx, hash, NULL);
    EVP_MD_CTX_free(ctx);
}

// Concatenate multiple buffers and hash
static void sha256_multi(const uint8_t **bufs, const size_t *lens, int count, uint8_t *hash) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    for (int i = 0; i < count; i++) {
        EVP_DigestUpdate(ctx, bufs[i], lens[i]);
    }
    EVP_DigestFinal_ex(ctx, hash, NULL);
    EVP_MD_CTX_free(ctx);
}

// ═══ Deterministic φ-based nonce ═══
static void phi_deterministic_nonce(const uint8_t *msg, size_t msg_len,
                                     const uint8_t *sk, BIGNUM *k, BN_CTX *ctx, BIGNUM *order) {
    const double phi = 1.6180339887498948482;
    const uint8_t *bufs[] = {(const uint8_t*)&phi, msg, sk};
    const size_t lens[] = {sizeof(phi), msg_len, 32};
    uint8_t hash[32];
    sha256_multi(bufs, lens, 3, hash);
    
    hash[0] |= 1;  // Ensure non-zero
    BN_bin2bn(hash, 32, k);
    BN_nnmod(k, k, order, ctx);
    if (BN_is_zero(k)) BN_one(k);
}

// ═══ Key Generation (deterministic from φ) ═══
int phi_keygen(uint8_t *pk, uint8_t *sk) {
    const double phi = 1.6180339887498948482;
    sha256_evp((const uint8_t*)&phi, sizeof(phi), sk);
    
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *priv = BN_new();
    BN_bin2bn(sk, 32, priv);
    
    EC_POINT *pub = EC_POINT_new(g);
    EC_POINT_mul(g, pub, priv, NULL, NULL, ctx);
    EC_POINT_point2oct(g, pub, POINT_CONVERSION_COMPRESSED, pk, 33, ctx);
    
    BN_free(priv);
    EC_POINT_free(pub);
    BN_CTX_free(ctx);
    EC_GROUP_free(g);
    return 0;
}

// ═══ SIGN ═══
int phi_sign(const uint8_t *msg, size_t msg_len,
              uint8_t *sig, size_t *sig_len) {
    if (!sig) {
        if (sig_len) *sig_len = PHI_SIG_BYTES;
        return 0;
    }
    if (!sig_len) return -1;
    if (!msg && msg_len > 0) return -1;
    
    uint8_t sk[32], pk[33];
    phi_keygen(pk, sk);

    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, ctx);

    BIGNUM *priv = BN_new();
    BN_bin2bn(sk, 32, priv);

    // Deterministic nonce
    BIGNUM *k = BN_new();
    phi_deterministic_nonce(msg ? msg : (const uint8_t*)"", msg_len, sk, k, ctx, order);
    
    // R = k·G
    EC_POINT *R = EC_POINT_new(g);
    EC_POINT_mul(g, R, k, NULL, NULL, ctx);
    EC_POINT_point2oct(g, R, POINT_CONVERSION_COMPRESSED, sig, 33, ctx);

    // Y = priv·G
    EC_POINT *Y = EC_POINT_new(g);
    EC_POINT_mul(g, Y, priv, NULL, NULL, ctx);
    unsigned char Y_bytes[33];
    EC_POINT_point2oct(g, Y, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, ctx);

    // c = SHA256(R || Y || msg)
    uint8_t c_hash[32];
    {
        const uint8_t *bufs[] = {sig, Y_bytes, msg ? msg : (const uint8_t*)""};
        const size_t lens[] = {33, 33, msg_len};
        sha256_multi(bufs, lens, 3, c_hash);
    }

    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_nnmod(c, c, order, ctx);

    // s = k + c·priv
    BIGNUM *s = BN_new();
    BIGNUM *cx = BN_new();
    BN_mod_mul(cx, c, priv, order, ctx);
    BN_mod_add(s, k, cx, order, ctx);
    BN_bn2binpad(s, sig + 33, 32);

    memcpy(sig + 65, pk, 33);
    *sig_len = PHI_SIG_BYTES;

    BN_free(priv); BN_free(k); BN_free(c); BN_free(s); BN_free(cx);
    BN_free(order); EC_POINT_free(R); EC_POINT_free(Y);
    BN_CTX_free(ctx); EC_GROUP_free(g);
    return 0;
}

// ═══ VERIFY ═══
int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len) {
    if (!msg || !sig) return 0;
    if (sig_len < PHI_SIG_BYTES) return 0;

    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, ctx);

    EC_POINT *R = EC_POINT_new(g);
    if (!EC_POINT_oct2point(g, R, sig, 33, ctx)) {
        EC_POINT_free(R); EC_GROUP_free(g); BN_free(order); BN_CTX_free(ctx);
        return 0;
    }

    BIGNUM *s = BN_new();
    BN_bin2bn(sig + 33, 32, s);

    EC_POINT *Y = EC_POINT_new(g);
    if (!EC_POINT_oct2point(g, Y, sig + 65, 33, ctx)) {
        BN_free(s); EC_POINT_free(R); EC_POINT_free(Y);
        EC_GROUP_free(g); BN_free(order); BN_CTX_free(ctx);
        return 0;
    }

    unsigned char Y_bytes[33];
    EC_POINT_point2oct(g, Y, POINT_CONVERSION_COMPRESSED, Y_bytes, 33, ctx);
    
    // Recompute c = SHA256(R || Y || msg)
    uint8_t c_hash[32];
    {
        const uint8_t *bufs[] = {sig, Y_bytes, msg};
        const size_t lens[] = {33, 33, msg_len};
        sha256_multi(bufs, lens, 3, c_hash);
    }

    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_nnmod(c, c, order, ctx);

    EC_POINT *sG = EC_POINT_new(g);
    EC_POINT *cY = EC_POINT_new(g);
    EC_POINT_mul(g, sG, s, NULL, NULL, ctx);
    EC_POINT_mul(g, cY, NULL, Y, c, ctx);
    EC_POINT_add(g, cY, R, cY, ctx);

    int result = (EC_POINT_cmp(g, sG, cY, ctx) == 0) ? 1 : 0;

    BN_free(s); BN_free(c); EC_POINT_free(R); EC_POINT_free(Y);
    EC_POINT_free(sG); EC_POINT_free(cY);
    EC_GROUP_free(g); BN_free(order); BN_CTX_free(ctx);
    return result;
}
