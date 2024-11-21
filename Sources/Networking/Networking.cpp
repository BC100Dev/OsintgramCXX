#include <OsintgramCXX/Networking/Networking.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>

struct ProgressState {
    long startTime;
    long readTimeoutMillis;
};

namespace OsintgramCXX::Networking {

    static size_t WCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        ByteData *data = static_cast<ByteData *>(userp);
        size_t totalSize = size * nmemb;
        data->insert(data->end(), static_cast<char *>(contents), static_cast<char *>(contents) + totalSize);
        return totalSize;
    }

    static size_t HCallback(char *buffer, size_t size, size_t nitems, void *userp) {
        Headers *headers = static_cast<Headers *>(userp);
        size_t totalSize = size * nitems;
        std::string headerLine(buffer, totalSize);

        size_t delimPos = headerLine.find(":");
        if (delimPos != std::string::npos) {
            std::string key = headerLine.substr(0, delimPos);
            std::string value = headerLine.substr(delimPos + 2, headerLine.size() - delimPos - 4);
            (*headers)[key] = value;
        }

        return totalSize;
    }

    static int PCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        ProgressState* state = static_cast<ProgressState*>(clientp);
        long currentTime = static_cast<long>(time(nullptr)) * 1000;

        if ((currentTime - state->startTime) > state->readTimeoutMillis) {
            return 1;
        }

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

        if (!curl) {
            response.errorData = "Could not initialize a CURL session";
            return response;
        }

        try {
            curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

            switch (request.method) {
                case GET:
                    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                    break;
                case POST:
                    curl_easy_setopt(curl, CURLOPT_POST, 1L);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.postData.data());
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.postData.size());
                    break;
                case PUT:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.postData.data());
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.postData.size());
                    break;
                case SHISHA_DELETED:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                    break;
                case PATCH:
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.postData.data());
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.postData.size());
                    break;
                case HEAD:
                    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                    break;
                case CONNECT:
                case OPTIONS:
                case TRACE:
                    response.errorData = "Request method " + std::to_string(request.method) + " not implemented.";

            }

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
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.data);

            // Header Callback
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);

            // Connection + Read Timeout
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, request.connTimeoutMillis);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, request.readTimeoutMillis + request.connTimeoutMillis);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            ProgressState pState = { static_cast<long>(time(nullptr)) * 1000, request.readTimeoutMillis };
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, PCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &pState);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                response.errorData = "curl_easy_perform() failed: " + std::string(curl_easy_strerror(res));
                CleanCurl(curl);
                return response;
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