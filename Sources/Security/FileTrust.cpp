#include <OsintgramCXX/Security/FileTrust.hpp>

namespace SecurityModule::FileTrust {

    void FT_SerializeRecord(const FT_Record &record, std::ofstream &out) {
        out.write(reinterpret_cast<const char *>(record.nativeLibraryHash.data()), record.nativeLibraryHash.size());
        out.write(reinterpret_cast<const char *>(record.jsonExecutorHash.data()), record.jsonExecutorHash.size());
        out.write(reinterpret_cast<const char *>(&record.libLastModified), sizeof(record.libLastModified));
        out.write(reinterpret_cast<const char *>(&record.exeLastModified), sizeof(record.exeLastModified));
        out.put(record.trustLevel);
        out.put(record.flags);
    }

    void FT_DeserializeRecord(FT_Record &record, std::ifstream &in) {
        in.read(reinterpret_cast<char *>(record.nativeLibraryHash.data()), record.nativeLibraryHash.size());
        in.read(reinterpret_cast<char *>(record.jsonExecutorHash.data()), record.jsonExecutorHash.size());
        in.read(reinterpret_cast<char *>(&record.libLastModified), sizeof(record.libLastModified));
        in.read(reinterpret_cast<char *>(&record.exeLastModified), sizeof(record.exeLastModified));
        int8_t trustLevelValue = in.get();
        record.trustLevel = static_cast<TrustLevel>(trustLevelValue);
        record.flags = in.get();
    }

    void FT_SerializeFile(const FT_Struct &trusts, const std::string &file) {
        std::ofstream out(file, std::ios::binary);
        if (!out.is_open())
            throw std::runtime_error("Unable to open file for writing");

        out.write(reinterpret_cast<const char *>(&trusts.magicNumber), sizeof(trusts.magicNumber));
        out.write(reinterpret_cast<const char *>(&trusts.version), sizeof(trusts.version));
        out.write(reinterpret_cast<const char *>(&trusts.reserved), sizeof(trusts.reserved));

        uint32_t recordCount = trusts.records.size();
        out.write(reinterpret_cast<const char *>(&recordCount), sizeof(recordCount));

        for (const auto &record : trusts.records)
            FT_SerializeRecord(record, out);

        out.close();
    }

    void FT_DeserializeFile(FT_Struct &fileTrust, const std::string &file) {
        std::ifstream in(file, std::ios::binary);
        if (!in.is_open())
            throw std::runtime_error("Unable to open file for reading");

        in.read(reinterpret_cast<char *>(&fileTrust.magicNumber), sizeof(fileTrust.magicNumber));
        in.read(reinterpret_cast<char *>(&fileTrust.version), sizeof(fileTrust.version));
        in.read(reinterpret_cast<char *>(&fileTrust.reserved), sizeof(fileTrust.reserved));

        uint32_t recordCount;
        in.read(reinterpret_cast<char *>(&recordCount), sizeof(recordCount));

        fileTrust.records.resize(recordCount);
        for (auto &record : fileTrust.records)
            FT_DeserializeRecord(record, in);

        in.close();
    }

    std::array<uint8_t, 32> SHA_256_Compute(const std::string &data) {
        std::array<uint8_t, 32> hash{};
        SHA256(reinterpret_cast<const unsigned char *>(data.c_str()), data.size(), hash.data());
        return hash;
    }

}