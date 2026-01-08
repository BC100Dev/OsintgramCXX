#include <dev_utils/network/Network.hpp>
#include <dev_utils/commons/Utils.hpp>

#include <algorithm>
#include <iostream>

#ifdef __ANDROID__

#include <filesystem>

#endif

struct ProgressState {
    long startTime;
    long readTimeoutMillis;
};

namespace DevUtils {

    std::string ReqMethodToStr(const RequestMethod &method) {
        switch (method) {
            case REQ_GET:
                return "GET";
            case REQ_POST:
                return "POST";
            case REQ_PUT:
                return "PUT";
            case REQ_DELETE:
                return "DELETE";
            case REQ_PATCH:
                return "PATCH";
            case REQ_HEAD:
                return "HEAD";
            case REQ_CONNECT:
                return "CONNECT";
            case REQ_OPTIONS:
                return "OPTIONS";
            case REQ_TRACE:
                return "TRACE";
            default:
                return "UNKNOWN";
        }
    }

    static size_t WCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        auto *response = static_cast<ResponseData *>(userp);
        size_t totalSize = size * nmemb;

        response->data.insert(response->data.end(),
                              static_cast<char *>(contents),
                              static_cast<char *>(contents) + totalSize);

        response->raw.insert(response->raw.end(),
                             static_cast<unsigned char *>(contents),
                             static_cast<unsigned char *>(contents) + totalSize);

        return totalSize;
    }

    static size_t HCallback(char *buffer, size_t size, size_t nitems, void *userp) {
        auto *headers = static_cast<Headers *>(userp);
        size_t totalSize = size * nitems;
        std::string headerLine(buffer, totalSize);

        size_t delimPos = headerLine.find(':');
        if (delimPos != std::string::npos) {
            std::string key = headerLine.substr(0, delimPos);
            std::string value = headerLine.substr(delimPos + 2, headerLine.size() - delimPos - 4);

            (*headers).emplace_back(key, value);
        }

        return totalSize;
    }

    static int PCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        auto *state = static_cast<ProgressState *>(clientp);
        long currentTime = static_cast<long>(time(nullptr)) * 1000;

        if ((currentTime - state->startTime) > state->readTimeoutMillis)
            return 1;

        return 0;
    }

    long MapHttpVersion(HttpVersion version) {
        switch (version) {
            case HttpVersion::HTTP_1_0:
                return CURL_HTTP_VERSION_1_0;
            case HttpVersion::HTTP_1_1:
                return CURL_HTTP_VERSION_1_1;
            case HttpVersion::HTTP_2_0:
                return CURL_HTTP_VERSION_2;
            case HttpVersion::HTTP_3_0:
                return CURL_HTTP_VERSION_3;
            default:
                return CURL_HTTP_VERSION_NONE;
        }
    }

    void CleanCurl(CURL *curl) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    ResponseData CreateRequest(const RequestData &request) {
        CURL *curl = curl_easy_init();
        ResponseData response;

        if (!curl)
            throw NetworkError("Could not initialize a Network Request Session");

        try {
            curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

            switch (request.method) {
                case REQ_GET:
                    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                    break;
                case REQ_POST:
                    curl_easy_setopt(curl, CURLOPT_POST, 1L);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.postData.data());
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.postData.size());
                    break;
                case REQ_PUT:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.postData.data());
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.postData.size());
                    break;
                case REQ_DELETE:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                    break;
                case REQ_PATCH:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.postData.data());
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.postData.size());
                    break;
                case REQ_HEAD:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");
                    break;
                case REQ_OPTIONS:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
                    break;
                case REQ_CONNECT:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "CONNECT");
                    break;
                case REQ_TRACE:
                    CleanCurl(curl);
                    throw NetworkError(ReqMethodToStr(request.method) + " not implemented");
            }

            // CA store check (if given)
            if (!request.ca_path.empty())
                curl_easy_setopt(curl, CURLOPT_CAINFO, request.ca_path.c_str());

            // HTTP Version
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, MapHttpVersion(request.version));

            // Headers
            struct curl_slist *curlHeaders = nullptr;
            for (const auto &[key, value]: request.headers) {
                std::string header = key + ": " + value;
                curlHeaders = curl_slist_append(curlHeaders, header.c_str());
            }

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);

            // SSL Verification
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, request.verifySSL ? 1L : 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, request.verifySSL ? 2L : 0L);

            // 3xx: Following Redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, request.followRedirects ? 1L : 0L);

            // Response Data Callback
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Header Callback
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);

            // Connection + Read Timeout
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, request.connTimeoutMillis);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, request.readTimeoutMillis + request.connTimeoutMillis);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            ProgressState pState = {static_cast<long>(time(nullptr)) * 1000, request.readTimeoutMillis};
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, PCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &pState);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                CleanCurl(curl);
                throw NetworkError("curl_easy_perform() failed, error " + std::string(curl_easy_strerror(res)));
            }

            long httpVer;
            curl_easy_getinfo(curl, CURLINFO_HTTP_VERSION, &httpVer);
            switch (httpVer) {
                case CURL_HTTP_VERSION_1_0:
                    response.version = HTTP_1_0;
                    break;
                case CURL_HTTP_VERSION_1_1:
                    response.version = HTTP_1_1;
                    break;
                case CURL_HTTP_VERSION_2:
                    response.version = HTTP_2_0;
                    break;
                case CURL_HTTP_VERSION_3:
                    response.version = HTTP_3_0;
                    break;
                default:
                    response.version = HTTP_1_1;
                    break;
            }

            long httpStatusCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
            response.statusCode = static_cast<int>(httpStatusCode);

            curl_slist_free_all(curlHeaders);
        } catch (const std::exception &ex) {
            response.errorData = ex.what();
            CleanCurl(curl);

            return response;
        }

        CleanCurl(curl);

        return response;
    }

}