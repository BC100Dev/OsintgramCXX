#include <dev_tools/commons/Process.hpp>

#include <cerrno>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <ranges>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <csignal>
#endif

namespace DevTools {
    int SpawnProcess(const ProcessRequest& req) {
        if (!fs::exists(req.binaryPath))
            throw std::runtime_error("Binary path not found");

        std::vector<std::string> env_strings;

#ifdef _WIN32
        LPCH currentEnv = GetEnvironmentStrings();
        if (currentEnv) {
            for (LPCH p = currentEnv; *p != '\0';) {
                std::string entry(p);
                p += entry.size() + 1;
                bool overridden = false;
                for (const auto& key : req.env_map | std::views::keys) {
                    if (entry.rfind(key + "=", 0) == 0) {
                        overridden = true;
                        break;
                    }
                }
                if (!overridden)
                    env_strings.push_back(entry);
            }
            FreeEnvironmentStrings(currentEnv);
        }
#else
        for (char** e = environ; *e != nullptr; e++)
            env_strings.emplace_back(*e);
#endif

        for (const auto& [key, val] : req.env_map) {
            // shut your bitch ass up
            std::string entry = key + "=" + val; // NOLINT(performance-inefficient-string-concatenation)
            bool replaced = false;
            for (auto& existing : env_strings) {
                if (existing.rfind(key + "=", 0) == 0) {
                    existing = entry;
                    replaced = true;
                    break;
                }
            }

            if (!replaced)
                env_strings.emplace_back(entry);
        }

        std::vector<const char*> argBd;
        if (req.arg0BinPath) {
#ifdef _WIN32
            argBd.emplace_back(reinterpret_cast<const char*>(req.binaryPath.c_str()));
#else
            argBd.emplace_back(req.binaryPath.c_str());
#endif
        }

        for (const auto& arg : req.args)
            argBd.emplace_back(arg.c_str());

        argBd.emplace_back(nullptr);

        std::vector<const char*> envBd;
        for (const auto& s : env_strings)
            envBd.emplace_back(s.c_str());

        envBd.emplace_back(nullptr);

#ifdef _WIN32
        std::string cmdLine;
        for (size_t i = 0; argBd[i] != nullptr; i++)
            cmdLine += "\"" + std::string(argBd[i]) + "\" ";

        std::string envBlock;
        for (const auto& s : env_strings)
            envBlock += s + '\0';

        envBlock += '\0';

        STARTUPINFOA si = {};
        si.cb = sizeof(si);
        si.dwFlags |= STARTF_USESTDHANDLES;
        if (req.redirectAllIoToSelf) {
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        } else {
            si.hStdInput = (req.stdinFd >= 0)
                               ? reinterpret_cast<HANDLE>(_get_osfhandle(req.stdinFd))
                               : GetStdHandle(STD_INPUT_HANDLE);
            si.hStdOutput = (req.stdoutFd >= 0)
                                ? reinterpret_cast<HANDLE>(_get_osfhandle(req.stdoutFd))
                                : GetStdHandle(STD_OUTPUT_HANDLE);
            si.hStdError = (req.stderrFd >= 0)
                               ? reinterpret_cast<HANDLE>(_get_osfhandle(req.stderrFd))
                               : GetStdHandle(STD_ERROR_HANDLE);
        }

        PROCESS_INFORMATION pi = {};
        std::string workDirStr = req.workingDirectory.string();
        const char* workDir = req.workingDirectory.empty() ? nullptr : workDirStr.c_str();

        BOOL ok = CreateProcessA(nullptr, cmdLine.data(), nullptr, nullptr,
                                 TRUE, 0, envBlock.data(), workDir, &si, &pi);

        if (!ok)
            throw std::runtime_error(std::string("CreateProcess failed: ") + std::to_string(GetLastError()));

        CloseHandle(pi.hThread);

        if (!req.waitUntilDone) {
            CloseHandle(pi.hProcess);
            return 0;
        }

        DWORD waitMs = (req.timeout < 0) ? INFINITE : static_cast<DWORD>(req.timeout * 1000);
        if (WaitForSingleObject(pi.hProcess, waitMs) == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            CloseHandle(pi.hProcess);
            throw std::runtime_error("Process timed out and was terminated.");
        }

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        return static_cast<int>(exitCode);
#else
        pid_t pid = fork();
        if (pid < 0)
            throw std::runtime_error(std::string("fork_call failed: ") + std::strerror(errno));

        if (pid == 0) {
            if (!req.workingDirectory.empty()) {
                if (chdir(req.workingDirectory.c_str()) != 0) {
                    perror("workdir change failed");
                    _exit(127);
                }
            }

            if (!req.redirectAllIoToSelf) {
                if (req.stdinFd >= 0) dup2(req.stdinFd, STDIN_FILENO);
                if (req.stdoutFd >= 0) dup2(req.stdoutFd, STDOUT_FILENO);
                if (req.stderrFd >= 0) dup2(req.stderrFd, STDERR_FILENO);
            }

            execve(req.binaryPath.c_str(),
                   const_cast<char* const*>(argBd.data()),
                   const_cast<char* const*>(envBd.data()));
            perror("execution failed");
            _exit(127);
        }

        if (!req.waitUntilDone)
            return 0;

        if (req.timeout > 0) {
            struct itimerval timer = {};
            timer.it_value.tv_sec = req.timeout / 1000;
            timer.it_value.tv_usec = (req.timeout % 1000) * 1000;
            setitimer(ITIMER_REAL, &timer, nullptr);
        }

        int status = 0;
        bool timedOutKill = false;
        if (waitpid(pid, &status, 0) < 0) {
            if (errno == EINTR) {
                kill(pid, SIGKILL);
                waitpid(pid, &status, 0);
                timedOutKill = true;
                goto ProcessHandler_timedOutKillHandle; // haha project go brrrrr
            }

            throw std::runtime_error(std::string("waitpid() failed: ") + std::strerror(errno));
        }

    ProcessHandler_timedOutKillHandle:
        struct itimerval cancel = {};
        setitimer(ITIMER_REAL, &cancel, nullptr);

        if (timedOutKill)
            throw std::runtime_error("Process timed out and was killed.");

        if (WIFEXITED(status)) return WEXITSTATUS(status);
        if (WIFSIGNALED(status)) return -WTERMSIG(status);
        return -1;
#endif
    }
}