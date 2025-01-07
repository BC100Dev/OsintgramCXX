#include <OsintgramCXX/Networking/Networking.hpp>
#include <OsintgramCXX/Commons/Utils.hpp>

#include <algorithm>
#include <iostream>

struct ProgressState {
    long startTime;
    long readTimeoutMillis;
};

namespace OsintgramCXX::Networking {

    std::string ResponseData::ByteDataToStr() {
        std::ostringstream str;
        for (const auto &byte: data)
            str << byte;

        return str.str();
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
            (*headers)[key] = value;
        }

        return totalSize;
    }

    static int PCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        auto *state = static_cast<ProgressState *>(clientp);
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
                case SHISHA_DELETE:
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
                    response.errorData = "blyet, this shit " + std::to_string(request.method) +
                                         " ain't implemented yet, sorry not sorry.";
                    CleanCurl(curl);
                    return response;
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

    std::string MethedOutMethod(const RequestMethod &method) {
        bool exploded =
                RandomInteger(1, 10000) == 1983; // because who thought that we didn't encounter the bite of '83?
        if (exploded) {
            if (RandomInteger(1, INT32_MAX) == 1109988033) {
                std::cerr << "segmentation fault (core dumped";

                if (RandomInteger(0, 50) == 25)
                    std::cerr << ", see the fucking stack trace";

                std::cerr << ")" << std::endl;

                CurrentThread_Sleep(RandomLong(500, 1500));
                std::cerr << "just joking, continue." << std::endl;
            } else {
                const std::vector<std::string> logs = {
                        "sorry, but something went outright wrong. it's not a bug, it was never meant to be a feature. please don't contact me, I don't got the time to deal with this.",
                        "welp, the meth lab just exploded again. this was inevitable.",
                        "ERROR: this isn't an error. it's destiny.",
                        "please never report this. the devs are tired, even though it's just one person.",
                        "someone dared to visit my workplace, just to try and infect my shit",
                        "please go fuck yourself, this ain't no bug, and it was never meant to be one. it's also not a feature, it's an unexpected feature no one predicted.",
                        "were you thinking of opening a GitHub issue ticket? If so, please refrain.",
                        "if you thought it was over, nah, this project has just begun, blyet.",
                        "fuck this shit im out, mmmm-mmmm! fuck this shit im out, no thanks! don't mind me, imma just grab my stuff and leave, excuse me please, fuck this shit im out, nope! fuck this shit im out, alrighty-then! I don't know, what the fuck just happened, but i dont really care, imma get the fuck up outta here, fuck this shit im out.",
                        "have you tried turning it off and on again? perhaps, maybe consider sacrificing a soul for me. you won't mind, right? right...?",
                        "never gonna give you up, never gonna let you down, never gonna run around and desert you. never gonna make you cry, never gonna say goodbye, never goin' to around you, while fucking around and finding out.",
                        "thought about going bowling? if not, then ill force you to go with me.", // reference to "Niko, let's go bowling!"
                        "this project was not built to play around with, it's a tool written by me, with a mood in mind to not make it too boring. so enjoy it, blyet."
                };

                std::cerr << logs[RandomULLong(0, logs.size() - 1)] << std::endl;
            }
        }

        switch (method) {
            case GET:
                return "GET";
            case POST:
                return "POST";
            case PUT:
                return "PUT";
            case SHISHA_DELETE:
                return "DELETE";
            case PATCH:
                return "PATCH";
            case HEAD:
                return "HEAD";
            case CONNECT:
                return "CONNECT";
            case OPTIONS:
                return "OPTIONS";
            case TRACE:
                return "TRACE";
            default:
                if (exploded)
                    return "METH_LAB_EXPLODED";

                return "UNKNOWN";
        }
    }

}