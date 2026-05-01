#include "PWEncrypt.hpp"

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include <ctime>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>

class PWEncryptError : public std::runtime_error {
public:
    explicit PWEncryptError(const std::string& msg) : std::runtime_error(msg) {
    }

    explicit PWEncryptError(const std::string& stage, const std::string& msg) : std::runtime_error(
        "[" + stage + "] " + msg) {
    }
};

PassEncryption encryptPassword(const std::string& pw, const EncryptionKey& key) {
    int keyId = std::stoi(key.keyId);

    // Attempt base64 decode first, fall back to plain PEM
    EVP_PKEY* pubKey = nullptr;
    {
        BIO* b64bio = BIO_new(BIO_f_base64());
        BIO* memBio = BIO_new_mem_buf(key.pubKey.c_str(), static_cast<int>(key.pubKey.size()));
        memBio = BIO_push(b64bio, memBio);
        BIO_set_flags(memBio, BIO_FLAGS_BASE64_NO_NL);

        std::vector<unsigned char> decoded(key.pubKey.size());
        int decodedLen = BIO_read(memBio, decoded.data(), static_cast<int>(decoded.size()));
        BIO_free_all(memBio);

        if (decodedLen > 0) {
            BIO* pemBio = BIO_new_mem_buf(decoded.data(), decodedLen);
            pubKey = PEM_read_bio_PUBKEY(pemBio, nullptr, nullptr, nullptr);
            BIO_free(pemBio);
        }

        if (!pubKey) {
            BIO* plainBio = BIO_new_mem_buf(key.pubKey.c_str(), static_cast<int>(key.pubKey.size()));
            pubKey = PEM_read_bio_PUBKEY(plainBio, nullptr, nullptr, nullptr);
            BIO_free(plainBio);
        }

        if (!pubKey)
            throw PWEncryptError("PreStage/OpenSSL", "Public key import failed");
    }

    unsigned char sessionKey[32];
    unsigned char iv[12];
    if (!RAND_bytes(sessionKey, sizeof(sessionKey)) || !RAND_bytes(iv, sizeof(iv))) {
        EVP_PKEY_free(pubKey);
        throw PWEncryptError("Init/OpenSSL", "RAND_bytes failed");
    }

    std::string timestamp = std::to_string(std::time(nullptr));

    EVP_PKEY_CTX* rsaCtx = EVP_PKEY_CTX_new(pubKey, nullptr);
    EVP_PKEY_free(pubKey);
    if (!rsaCtx)
        throw PWEncryptError("Init/OpenSSL", "EVP_PKEY_CTX_new failed");

    if (EVP_PKEY_encrypt_init(rsaCtx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_padding(rsaCtx, RSA_PKCS1_PADDING) <= 0) {
        EVP_PKEY_CTX_free(rsaCtx);
        throw PWEncryptError("Init/OpenSSL", "RSA init failed");
    }

    size_t rsaOutLen = 0;
    if (EVP_PKEY_encrypt(rsaCtx, nullptr, &rsaOutLen, sessionKey, sizeof(sessionKey)) <= 0) {
        EVP_PKEY_CTX_free(rsaCtx);
        throw PWEncryptError("Encryption/OpenSSL", "RSA length probe failed");
    }

    std::vector<unsigned char> rsaEncrypted(rsaOutLen);
    if (EVP_PKEY_encrypt(rsaCtx, rsaEncrypted.data(), &rsaOutLen, sessionKey, sizeof(sessionKey)) <= 0) {
        EVP_PKEY_CTX_free(rsaCtx);
        throw PWEncryptError("Encryption/OpenSSL", "RSA encrypt failed");
    }
    rsaEncrypted.resize(rsaOutLen);
    EVP_PKEY_CTX_free(rsaCtx);

    EVP_CIPHER_CTX* aesCtx = EVP_CIPHER_CTX_new();
    if (!aesCtx)
        throw PWEncryptError("Init/OpenSSL", "AES context failed");

    if (!EVP_EncryptInit_ex(aesCtx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) ||
        !EVP_CIPHER_CTX_ctrl(aesCtx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), nullptr) ||
        !EVP_EncryptInit_ex(aesCtx, nullptr, nullptr, sessionKey, iv)) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw PWEncryptError("Init/OpenSSL", "AES-GCM init failed");
    }

    int len = 0;
    if (!EVP_EncryptUpdate(aesCtx, nullptr, &len,
                           reinterpret_cast<const unsigned char*>(timestamp.c_str()),
                           static_cast<int>(timestamp.size()))) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw PWEncryptError("Encryption/OpenSSL", "AAD injection failed");
    }

    std::vector<unsigned char> aesEncrypted(pw.size());
    if (!EVP_EncryptUpdate(aesCtx, aesEncrypted.data(), &len,
                           reinterpret_cast<const unsigned char*>(pw.c_str()), static_cast<int>(pw.size()))) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw PWEncryptError("Encryption/OpenSSL", "AES encrypt failed");
    }
    int totalLen = len;

    int finalLen = 0;
    if (!EVP_EncryptFinal_ex(aesCtx, aesEncrypted.data() + totalLen, &finalLen)) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw PWEncryptError("Finalization/OpenSSL", "AES final block failed");
    }
    totalLen += finalLen;
    aesEncrypted.resize(totalLen);

    unsigned char tag[16];
    if (!EVP_CIPHER_CTX_ctrl(aesCtx, EVP_CTRL_GCM_GET_TAG, sizeof(tag), tag)) {
        EVP_CIPHER_CTX_free(aesCtx);
        throw PWEncryptError("Finalization/OpenSSL", "GCM tag retrieval failed");
    }
    EVP_CIPHER_CTX_free(aesCtx);

    std::vector<unsigned char> payload;
    payload.push_back(0x01);
    payload.push_back(static_cast<unsigned char>(keyId & 0xFF));
    payload.insert(payload.end(), iv, iv + sizeof(iv));
    payload.push_back(static_cast<unsigned char>(rsaOutLen & 0xFF));
    payload.push_back(static_cast<unsigned char>((rsaOutLen >> 8) & 0xFF));
    payload.insert(payload.end(), rsaEncrypted.begin(), rsaEncrypted.end());
    payload.insert(payload.end(), tag, tag + sizeof(tag));
    payload.insert(payload.end(), aesEncrypted.begin(), aesEncrypted.end());

    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, payload.data(), static_cast<int>(payload.size()));
    BIO_flush(b64);
    BUF_MEM* bptr = nullptr;
    BIO_get_mem_ptr(b64, &bptr);
    std::string b64Payload(bptr->data, bptr->length);
    BIO_free_all(b64);

    return {timestamp, b64Payload};
}

std::string pw2igLogin(const PassEncryption& pw) {
    return "#PWD_INSTAGRAM:4:" + pw.pw_time + ":" + pw.pw_encrypted;
}
