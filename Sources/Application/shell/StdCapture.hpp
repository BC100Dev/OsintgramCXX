#ifndef OSINTGRAMCXX_STDCAPTURE_HPP
#define OSINTGRAMCXX_STDCAPTURE_HPP

#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <cstdio>

#if defined(_WIN32)

#include <io.h>
#include <fcntl.h>
#define READ_FN          _read
#define WRITE_FN         _write
#define DUP              _dup
#define DUP2             _dup2
#define CLOSE            _close
#define FILENO           _fileno
#define MAKE_PIPE(fds)   (_pipe(fds, 4096, _O_BINARY) == 0)

#else

#include <unistd.h>

#define READ_FN          read
#define WRITE_FN         write
#define DUP              dup
#define DUP2             dup2
#define CLOSE            close
#define FILENO           fileno
#define MAKE_PIPE(fds)   (pipe(fds) == 0)

#endif

class StdCaptureError : public std::runtime_error {
public:
    explicit StdCaptureError(const std::string &s) : std::runtime_error(s) {}
};

class StdCapture {
public:
    StdCapture() {
        fflush(stdout);
        fflush(stderr);
        std::cout.flush();
        std::cerr.flush();

        if (!MAKE_PIPE(pipeOut))
            throw StdCaptureError("pipe for stdout creation failed");

        if (!MAKE_PIPE(pipeErr))
            throw StdCaptureError("pipe for stderr creation failed");

        savedOut = DUP(FILENO(stdout));
        savedErr = DUP(FILENO(stderr));

        DUP2(pipeOut[1], FILENO(stdout));
        DUP2(pipeErr[1], FILENO(stderr));
        CLOSE(pipeOut[1]);
        CLOSE(pipeErr[1]);

        stream = std::make_shared<std::string>();
        outReader = std::thread([this]() {
            char buf[4096];
            ssize_t n;
            while ((n = READ_FN(pipeOut[0], buf, sizeof(buf))) > 0) {
                // echo to real stdout
                WRITE_FN(savedOut, buf, n);
                // capture
                stream->append(buf, static_cast<size_t>(n));
            }
            CLOSE(pipeOut[0]);
        });

        errReader = std::thread([this]() {
            char buf[4096];
            ssize_t n;
            while ((n = READ_FN(pipeErr[0], buf, sizeof(buf))) > 0) {
                WRITE_FN(savedErr, buf, n);
                stream->append(buf, static_cast<size_t>(n));
            }
            CLOSE(pipeErr[0]);
        });
    }

    ~StdCapture() {
        fflush(stdout);
        fflush(stderr);
        std::cout.flush();
        std::cerr.flush();

        DUP2(savedOut, FILENO(stdout));
        DUP2(savedErr, FILENO(stderr));
        CLOSE(savedOut);
        CLOSE(savedErr);

        if (outReader.joinable())
            outReader.join();

        if (errReader.joinable())
            errReader.join();
    }

    // Returns the captured stdout & stderr themselves
    [[nodiscard]] std::string str() const { return *stream; }

private:
    int pipeOut[2]{-1,-1}, pipeErr[2]{-1,-1};
    int savedOut{-1}, savedErr{-1};
    std::shared_ptr<std::string> stream, errBuf;
    std::thread outReader, errReader;
};

#endif //OSINTGRAMCXX_STDCAPTURE_HPP
