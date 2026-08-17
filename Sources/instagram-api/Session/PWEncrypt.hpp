#ifndef OSINTGRAMCXX_PWENCRYPT_HPP
#define OSINTGRAMCXX_PWENCRYPT_HPP

#include <string>

const std::string dummyKeyId = "246";
const std::string dummyPubKey = R"(LS0tLS1CRUdJTiBQVUJMSUMgS0VZLS0tLS0KTUlJQklqQU5CZ2txaGtpRzl3MEJBUUVGQUFPQ0FROEFNSUlCQ2dLQ0FRRUF5R21wRlJ4NVY4NmtDanJoZkoyNwprSGFSMmMyUWtqQlJYWDBIS0s2UnFyUUZjc2JlTDNVYlR6bGNNdjhXUDk3aWhrYUNxNityZ0thOTg5VU9YWVZVClhPNVpTZU14dHBxd2E3aEh1Vk5sNENIdHBaRWF2b0xBZmtqQ3pEcUtFUjRwMHljMG0xWkh3aUlWQW9qeWYzZkIKbGhUL1Q5RHhwbHNIcy9PbkZnMFAwYS8wcGQ4azVKTEJoVTFZNUhmR1cwRHNkb1c1eC9ZQUJNa1IwV3Q5UmcrRwpoSFI2ck01RHZFM1QwM3cveU5WaGg3Q2NxMmkxcTBLSGVVajFYSXRwVFJLT3JITDEzU1o0VmdCN0h5bGdwTEZGClRwOFdaaUg2Zy85Mk02NUJtUksxY1ppMkp6WkVxT1hYV0FGVmhYem1oQ3pCbGRxZ2RYU3h3dTlEWDZqaTJRMnkKYlFJREFRQUIKLS0tLS1FTkQgUFVCTElDIEtFWS0tLS0tCg==)";

struct PassEncryption {
    std::string pw_time;
    std::string pw_encrypted;
};

struct EncryptionKey {
    std::string pubKey;
    std::string keyId;
};

PassEncryption encryptPassword(const std::string& pw, const EncryptionKey& key);

std::string pw2igLogin(const PassEncryption& pw);

#endif //OSINTGRAMCXX_PWENCRYPT_HPP
