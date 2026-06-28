#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/obj_mac.h>
#include <openssl/rand.h>

#define PHI_SIG_PUBLICKEYBYTES 33
#define PHI_SIG_SECRETKEYBYTES 32
#define PHI_SIG_BYTES 98

// Deterministic nonce using SHA256(phi || msg || sk) — NO BN_mod needed
static void phi_deterministic_nonce(const uint8_t *msg, size_t msg_len,
                                     const uint8_t *sk, BIGNUM *k) {
    SHA256_CTX sha;
    unsigned char hash[32];
    
    SHA256_Init(&sha);
    const double phi = 1.6180339887498948482;
    SHA256_Update(&sha, &phi, sizeof(phi));
    SHA256_Update(&sha, msg, msg_len);
    SHA256_Update(&sha, sk, 32);
    SHA256_Final(hash, &sha);
    
    // Ensure k is non-zero and reasonable size
    hash[0] |= 1;  // Ensure non-zero
    BN_bin2bn(hash, 32, k);
}

int phi_keygen(uint8_t *pk, uint8_t *sk) {
    const double phi = 1.6180339887498948482;
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, &phi, sizeof(phi));
    SHA256_Final(sk, &sha);
    
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
    phi_deterministic_nonce(msg ? msg : (const uint8_t*)"", msg_len, sk, k);
    // Reduce k modulo order
    BN_nnmod(k, k, order, ctx);
    if (BN_is_zero(k)) BN_one(k);
    
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
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, sig, 33);
    SHA256_Update(&sha, Y_bytes, 33);
    if (msg && msg_len > 0) SHA256_Update(&sha, msg, msg_len);
    unsigned char c_hash[32];
    SHA256_Final(c_hash, &sha);

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
    
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, sig, 33);
    SHA256_Update(&sha, Y_bytes, 33);
    SHA256_Update(&sha, msg, msg_len);
    unsigned char c_hash[32];
    SHA256_Final(c_hash, &sha);

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
