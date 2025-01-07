// this is just a test code to see, if this shit will even compile.
// it'll tell me, whether I can have OpenSSL linked via libcurl or not.
// if not, then damn, fuck CMake, am I right or am I right?

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <filesystem>


namespace OsintgramCXX::FileEncryption {

    constexpr int KEY_LENGTH = 32; // 256-bit key
    constexpr int IV_LENGTH = 16; // AES block size
    constexpr int SALT_LENGTH = 8;
    constexpr int PBKDF2_ITERATIONS = 65536;
    constexpr unsigned char DEFAULT_SALT[SALT_LENGTH] = {
            0x43, 0x76, 0x95, 0xc7, 0x5b, 0xd7, 0x45, 0x17
    };

    std::vector<unsigned char> generateKey(const std::string &password, const unsigned char *salt) {
        std::vector<unsigned char> key(KEY_LENGTH);
        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(), salt, SALT_LENGTH, PBKDF2_ITERATIONS, EVP_sha256(),
                              KEY_LENGTH, key.data()) != 1) {
            throw std::runtime_error("Failed to generate key using PBKDF2");
        }
        return key;
    }

    std::vector<unsigned char> encrypt(const std::string &filePath, const std::string &password) {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error("The source file does not exist: " + filePath);
        }
        if (!std::filesystem::is_regular_file(filePath)) {
            throw std::runtime_error("The source file is not a valid file: " + filePath);
        }

        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filePath);
        }

        // Read file contents
        std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        size_t paddedSize = fileData.size() + (IV_LENGTH - fileData.size() % IV_LENGTH);
        fileData.resize(paddedSize, static_cast<unsigned char>(IV_LENGTH - fileData.size() % IV_LENGTH));

        // Generate key and IV
        auto key = generateKey(password, DEFAULT_SALT);
        std::vector<unsigned char> iv(IV_LENGTH);
        if (RAND_bytes(iv.data(), IV_LENGTH) != 1) {
            throw std::runtime_error("Failed to generate IV");
        }

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
        }

        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize encryption");
        }

        std::vector<unsigned char> encryptedData(paddedSize + IV_LENGTH);
        int len = 0;
        int ciphertextLen = 0;

        if (EVP_EncryptUpdate(ctx, encryptedData.data(), &len, fileData.data(), fileData.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to encrypt data");
        }
        ciphertextLen += len;

        if (EVP_EncryptFinal_ex(ctx, encryptedData.data() + ciphertextLen, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize encryption");
        }
        ciphertextLen += len;

        EVP_CIPHER_CTX_free(ctx);

        encryptedData.resize(ciphertextLen);
        encryptedData.insert(encryptedData.begin(), iv.begin(), iv.end());
        return encryptedData;
    }

    std::vector<unsigned char> decrypt(const std::string &filePath, const std::string &password) {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error("The source file does not exist: " + filePath);
        }
        if (!std::filesystem::is_regular_file(filePath)) {
            throw std::runtime_error("The source file is not a valid file: " + filePath);
        }

        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filePath);
        }

        std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        if (fileData.size() < IV_LENGTH) {
            throw std::runtime_error("Invalid file format: missing IV");
        }

        std::vector<unsigned char> iv(fileData.begin(), fileData.begin() + IV_LENGTH);
        std::vector<unsigned char> ciphertext(fileData.begin() + IV_LENGTH, fileData.end());

        auto key = generateKey(password, DEFAULT_SALT);

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
        }

        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize decryption");
        }

        std::vector<unsigned char> decryptedData(ciphertext.size());
        int len = 0;
        int plaintextLen = 0;

        if (EVP_DecryptUpdate(ctx, decryptedData.data(), &len, ciphertext.data(), ciphertext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to decrypt data");
        }
        plaintextLen += len;

        if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + plaintextLen, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to finalize decryption");
        }
        plaintextLen += len;

        EVP_CIPHER_CTX_free(ctx);

        decryptedData.resize(plaintextLen);
        return decryptedData;
    }

} // namespace OsintgramCXX::FileEncryption
