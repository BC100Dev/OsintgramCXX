#include "../../Include/OsintgramCXX/Networking/Networking.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace OsintgramCXX::Networking {

    static size_t WriteStringCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        size_t totalSize = size * nmemb;
        userp->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    static size_t WriteBinaryCallback(void* contents, size_t size, size_t nmemb, std::vector<unsigned char>* userp) {
        size_t totalSize = size * nmemb;
        userp->insert(userp->end(), static_cast<unsigned char*>(contents), static_cast<unsigned char*>(contents) + totalSize);
        return totalSize;
    }

    static void ApplyHeaders(CURL* curl, const std::map<std::string, std::string>& headers) {
        struct curl_slist* headerList = nullptr;

        for (const auto& [key, value] : headers) {
            std::stringstream headerStream;
            headerStream << key << ": " << value;

            headerList = curl_slist_append(headerList, headerStream.str().c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }

    bool ShouldInterpretBinary(const std::string& contentType) {
        if (contentType.find("text/") != std::string::npos)
            return false;

        std::vector<std::string> knownTextAppFormats = {
                "application/json",
                "application/xml",
                "application/xhtml+xml",
                "application/javascript",
                "application/atom+xml",
                "application/rss+xml",
                "application/ld+json",
                "application/x-www-form-urlencoded",
                "application/sql"
        };

        return std::any_of(knownTextAppFormats.begin(), knownTextAppFormats.end(),
                           [&contentType](const std::string& knownEntry) {
            return contentType.find(knownEntry) != std::string::npos;
        });
    }

    ResponseData GetRequest(const std::string& url, const HeaderMap& headers) {
        CURL* curl;
        CURLcode res;
        ResponseData response;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            response.isBinary = false;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.stringData);

            ApplyHeaders(curl, headers);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                response.error << "curl_easy_perform() failed: " << curl_easy_strerror(res);

            curl_easy_cleanup(curl);
        }

        return response;
    }

    ResponseData PostRequestS(const std::string& url, const std::map<std::string, std::string>& headers, const std::string& data) {
        CURL* curl;
        CURLcode res;
        ResponseData response;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

            response.isBinary = false;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.stringData);

            ApplyHeaders(curl, headers);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                response.error << "curl_easy_perform() failed: " << curl_easy_strerror(res);

            curl_easy_cleanup(curl);
        }

        return response;
    }

    ResponseData PostRequestB(const std::string& url, const std::map<std::string, std::string>& headers, const std::vector<unsigned char>& data) {
        CURL* curl;
        CURLcode res;
        ResponseData response;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, reinterpret_cast<const char*>(data.data()));
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

            response.isBinary = true;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBinaryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.binaryData);

            ApplyHeaders(curl, headers);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                response.error << "curl_easy_perform() failed: " << curl_easy_strerror(res);

            curl_easy_cleanup(curl);
        }

        return response;
    }

    ResponseData DeleteRequest(const std::string& url, const std::map<std::string, std::string>& headers) {
        CURL* curl;
        CURLcode res;
        ResponseData response;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

            response.isBinary = false;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.stringData);

            ApplyHeaders(curl, headers);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                response.error << "curl_easy_perform() failed: " << curl_easy_strerror(res);

            curl_easy_cleanup(curl);
        }

        return response;
    }

}