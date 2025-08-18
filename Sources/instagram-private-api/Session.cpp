#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/err.h>

#include <ctime>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>

const std::string dummyPubKey = R"(-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEApZsl5rCOpKRTOeFg6rUO
6MT5I79OdzB6nbLG/cOksx9O3ZjYkyK5Ef91EV+uWvK4dS2Ska5zrxYFg0YEKApW
rUr3UCFXGf+vRg5SnEFZ5Ub3Ma3AW1Qubz5JxUXjlQnU2uS50nfgRfQ7sPZRYkzT
/OYr1aZHYZpj4yl5YjzH4b5r+n41XeWn78Lhf0kF9AMNV1BQqIXyBx+uT1RjqwnN
RbeRnkIjBdLzM15GxYqzD7X93rWTMOZ0+8+MhnMZ9i0j5zyfWdsrfH1RBGKmYllg
DLkOx81XsR4h8A/KnOyUyo8UG1RjVu20bXH/5KOvPKlW7aTPDQXb5IklBdEdjOEg
1wIDAQAB
-----END PUBLIC KEY-----)";

struct PassEncryption {
    std::string time;
    std::string encrypted;
};

struct EncryptionKey {
    std::string pubKey;
    std::string keyId;
};

PassEncryption encryptPassword(const std::string& pw, const EncryptionKey& key) {
    int keyId = std::stoi(key.keyId);

    unsigned char randKey[32];
    unsigned char iv[12];
    if (!RAND_bytes(randKey, sizeof(randKey)) || !RAND_bytes(iv, sizeof(iv))) {
        throw std::runtime_error("Failed to generate secure random bytes");
    }

    BIO* bio = BIO_new_mem_buf(key.pubKey.c_str(), -1);
    EVP_PKEY* pubKey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pubKey) {
        throw std::runtime_error("Failed to load public key");
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pubKey, nullptr);
    if (!ctx) {
        EVP_PKEY_free(pubKey);
        throw std::runtime_error("Failed to create EVP_PKEY_CTX");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
        EVP_PKEY_free(pubKey);
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to init RSA encryption");
    }

    size_t rsaOutLen = 0;
    if (EVP_PKEY_encrypt(ctx, nullptr, &rsaOutLen, randKey, sizeof(randKey)) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pubKey);
        throw std::runtime_error("Failed to get RSA output length");
    }

    std::vector<unsigned char> rsaEncrypted(rsaOutLen);
    if (EVP_PKEY_encrypt(ctx, rsaEncrypted.data(), &rsaOutLen, randKey, sizeof(randKey)) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pubKey);
        throw std::runtime_error("RSA encryption failed");
    }

    rsaEncrypted.resize(rsaOutLen);
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pubKey);

    std::string timestamp = std::to_string(std::time(nullptr));

    EVP_CIPHER_CTX* aesCtx = EVP_CIPHER_CTX_new();
    if (!aesCtx) throw std::runtime_error("Failed to create AES context");

    std::vector<unsigned char> aesEncrypted(pw.size() + 16);
    unsigned char authTag[16];
    int len = 0, totalLen = 0;

    if (!EVP_EncryptInit_ex(aesCtx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) ||
    !EVP_CIPHER_CTX_ctrl(aesCtx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), nullptr) ||
    !EVP_EncryptInit_ex(aesCtx, nullptr, nullptr, randKey, iv) ||
    !EVP_EncryptUpdate(aesCtx, nullptr, &len, reinterpret_cast<const unsigned char*>(timestamp.c_str()), timestamp.size()) ||
    !EVP_EncryptUpdate(aesCtx, aesEncrypted.data(), &len, reinterpret_cast<const unsigned char*>(pw.c_str()), pw.size())) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw std::runtime_error("AES-GCM encryption failed");
    }

    totalLen = len;
    if (!EVP_EncryptFinal_ex(aesCtx, aesEncrypted.data() + len, &len)) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw std::runtime_error("AES-GCM final block failed");
    }

    totalLen += len;
    aesEncrypted.resize(totalLen);

    if (!EVP_CIPHER_CTX_ctrl(aesCtx, EVP_CTRL_GCM_GET_TAG, sizeof(authTag), authTag)) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw std::runtime_error("Failed to get GCM auth tag");
    }

    EVP_CIPHER_CTX_free(aesCtx);

    std::vector<unsigned char> pwFinal;
    pwFinal.push_back(1);
    pwFinal.push_back(static_cast<unsigned char>(keyId));
    pwFinal.insert(pwFinal.end(), iv, iv + sizeof(iv));
    pwFinal.push_back(static_cast<unsigned char>(rsaOutLen & 0xFF));
    pwFinal.push_back(static_cast<unsigned char>((rsaOutLen >> 8) & 0xFF));
    pwFinal.insert(pwFinal.end(), rsaEncrypted.begin(), rsaEncrypted.end());
    pwFinal.insert(pwFinal.end(), authTag, authTag + sizeof(authTag));
    pwFinal.insert(pwFinal.end(), aesEncrypted.begin(), aesEncrypted.end());

    // finally encode this shiw with Base64
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, pwFinal.data(), pwFinal.size());
    BIO_flush(b64);
    BUF_MEM* bptr = nullptr;
    BIO_get_mem_ptr(b64, &bptr);

    std::string b64Out(bptr->data, bptr->length);
    BIO_free_all(b64);

    return {timestamp, b64Out};
}