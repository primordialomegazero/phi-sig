#include "phi_sig.h"
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <string.h>

// True Schnorr Σ-Protocol on secp256k1
// Verification: s·G == R + c·Y

int phi_keygen(uint8_t *pk, uint8_t *sk) {
    EC_KEY *ec = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec) return -1;
    EC_KEY_generate_key(ec);
    
    const BIGNUM *priv = EC_KEY_get0_private_key(ec);
    const EC_POINT *pub = EC_KEY_get0_public_key(ec);
    const EC_GROUP *g = EC_KEY_get0_group(ec);
    
    BN_bn2binpad(priv, sk, 32);
    EC_POINT_point2oct(g, pub, POINT_CONVERSION_COMPRESSED, pk, 33, NULL);
    
    EC_KEY_free(ec);
    return 0;
}

int phi_sign(const uint8_t *msg, size_t msg_len,
              uint8_t *sig, size_t *sig_len) {
    // Generate ephemeral key for signing
    uint8_t sk[32], pk[33];
    phi_keygen(pk, sk);
    
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, ctx);
    
    BIGNUM *priv = BN_new();
    BN_bin2bn(sk, 32, priv);
    
    // R = k·G
    BIGNUM *k = BN_new();
    BN_rand_range(k, order);
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
    SHA256_Update(&sha, msg, msg_len);
    unsigned char c_hash[32];
    SHA256_Final(c_hash, &sha);
    
    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_mod(c, c, order, ctx);
    
    // s = k + c·priv
    BIGNUM *s = BN_new();
    BIGNUM *cx = BN_new();
    BN_mod_mul(cx, c, priv, order, ctx);
    BN_mod_add(s, k, cx, order, ctx);
    BN_bn2binpad(s, sig + 33, 32);
    
    // Copy public key into signature
    memcpy(sig + 65, pk, 33);
    *sig_len = 98;  // 65 (sig) + 33 (pk)
    
    BN_free(priv); BN_free(k); BN_free(c); BN_free(s); BN_free(cx);
    BN_free(order); EC_POINT_free(R); EC_POINT_free(Y);
    BN_CTX_free(ctx); EC_GROUP_free(g);
    return 0;
}

int phi_verify(const uint8_t *msg, size_t msg_len,
               const uint8_t *sig, size_t sig_len) {
    if (sig_len < 98) return 0;  // REJECT invalid size
    
    EC_GROUP *g = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *order = BN_new();
    EC_GROUP_get_order(g, order, ctx);
    
    // Parse R from signature
    EC_POINT *R = EC_POINT_new(g);
    if (!EC_POINT_oct2point(g, R, sig, 33, ctx)) {
        EC_POINT_free(R); BN_free(order); BN_CTX_free(ctx); EC_GROUP_free(g);
        return 0;  // REJECT invalid point
    }
    
    // Parse s from signature
    BIGNUM *s = BN_new();
    BN_bin2bn(sig + 33, 32, s);
    
    // Parse Y (public key) from signature
    EC_POINT *Y = EC_POINT_new(g);
    if (!EC_POINT_oct2point(g, Y, sig + 65, 33, ctx)) {
        EC_POINT_free(R); EC_POINT_free(Y); BN_free(s); BN_free(order);
        BN_CTX_free(ctx); EC_GROUP_free(g);
        return 0;  // REJECT invalid public key
    }
    
    // c = SHA256(R || Y || msg)
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, sig, 33);        // R
    SHA256_Update(&sha, sig + 65, 33);    // Y (pk)
    SHA256_Update(&sha, msg, msg_len);    // message
    unsigned char c_hash[32];
    SHA256_Final(c_hash, &sha);
    
    BIGNUM *c = BN_new();
    BN_bin2bn(c_hash, 32, c);
    BN_mod(c, c, order, ctx);
    
    // Verify: s·G == R + c·Y
    EC_POINT *sG = EC_POINT_new(g);
    EC_POINT_mul(g, sG, s, NULL, NULL, ctx);
    
    EC_POINT *cY = EC_POINT_new(g);
    EC_POINT_mul(g, cY, NULL, Y, c, ctx);
    
    EC_POINT *RcY = EC_POINT_new(g);
    EC_POINT_add(g, RcY, R, cY, ctx);
    
    int result = (EC_POINT_cmp(g, sG, RcY, ctx) == 0) ? 1 : 0;
    
    BN_free(s); BN_free(c); BN_free(order);
    EC_POINT_free(R); EC_POINT_free(Y); EC_POINT_free(sG);
    EC_POINT_free(cY); EC_POINT_free(RcY);
    BN_CTX_free(ctx); EC_GROUP_free(g);
    
    return result;
}
