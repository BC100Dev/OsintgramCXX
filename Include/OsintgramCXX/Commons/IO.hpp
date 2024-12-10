#ifndef OSINTGRAMCXX_IO_HPP
#define OSINTGRAMCXX_IO_HPP

#include <chrono>
#include <string>
#include <vector>
#include <any>

namespace OsintgramCXX::IO {

    struct IO_Out {
        std::any* out;
        bool success;
        std::string error;
    };

    struct DirEntry {
        std::string filename;
        std::string path;
        std::chrono::system_clock::time_point lastModifiedDate;
        std::chrono::system_clock::time_point creationDate;
    };

    IO_Out ListDirectory(const std::string& dirPath);

    IO_Out isValidZipFile(const std::string& filePath);

    IO_Out createFile(const std::string& filePath);

    IO_Out createDirectory(const std::string& dirPath, bool createSubDirs);

    IO_Out deleteDiskEntry(const std::string& path);

}

#endif //OSINTGRAMCXX_IO_HPP
