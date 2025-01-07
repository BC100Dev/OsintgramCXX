#ifndef OSINTGRAMCXX_FILEENCRYPTION_HPP
#define OSINTGRAMCXX_FILEENCRYPTION_HPP

// this blueprint will eventually require OpenSSL...
// fucking another library

// TODO: Implement AES-256 encryption/decryption using OpenSSL
// Step 1: Write the functions.
// Step 2: ask ChatGPT to write the damn code because you clearly don't want to waste hours on OpenSSL and complex methods
// Step 3: Link the motherfucking library.
// Step 4: Pray it compiles.
// Step 5: Did it compile? Okay, great, now rewrite the entire fucking thing because you remembered, you don't have a life (at least mine...)
// Step 6: Does it still work? Then rage quit this project because you clearly don't want to do programming anymore

// now, onto the fucking code

#include <vector>
#include <string>

namespace OsintgramCXX::FileEncryption {

    struct EncryptionData {
        std::vector<unsigned char> uCharData;
        std::vector<char> sCharData;
    };

    EncryptionData encrypt(const std::string &filePath, const std::string &password);

    EncryptionData decrypt(const std::string &filePath, const std::string &password);

    void encryptToFile(const std::string &iFilePath, const std::string &pass, const std::string &oFilePath);

    void decryptToFile(const std::string &iFilePath, const std::string &pass, const std::string &oFilePath);

}

#endif //OSINTGRAMCXX_FILEENCRYPTION_HPP
