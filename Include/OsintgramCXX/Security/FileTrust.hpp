#ifndef OSINTGRAMCXX_FILETRUST_HPP
#define OSINTGRAMCXX_FILETRUST_HPP

#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <cstdint>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <openssl/sha.h>

namespace SecurityModule::FileTrust {

    enum TrustLevel {
        NO_TRUST = -1,
        LOW_TRUST = 0,
        NEUTRAL_TRUST = 1,
        MEDIUM_TRUST = 2,
        HIGH_TRUST = 3,
        MAX_TRUST = 4,
        ULTIMATE_TRUST = 5
    };

    struct FT_Record {
        std::array<uint8_t, 32> nativeLibraryHash;
        std::array<uint8_t, 32> jsonExecutorHash;
        uint64_t libLastModified;
        uint64_t exeLastModified;
        TrustLevel trustLevel;
        uint8_t flags;
    };

    struct FT_Struct {
        uint32_t magicNumber = 0x465E5E5C;
        uint16_t version = 0x0001;
        uint16_t reserved = 0x0000;
        std::vector<FT_Record> records;
    };

    void FT_SerializeRecord(const FT_Record &record, std::ofstream &out);

    void FT_DeserializeRecord(FT_Record &record, std::ifstream &in);

    void FT_SerializeFile(const FT_Struct &trusts, const std::string &file);

    void FT_DeserializeFile(FT_Struct &fileTrust, const std::string &file);

    std::array<uint8_t, 32> SHA_256_Compute(const std::string &data);

}

#endif //OSINTGRAMCXX_FILETRUST_HPP
