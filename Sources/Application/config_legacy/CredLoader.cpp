#include "CredentialsIni.hpp"

#include <IGApi/Session.hpp>
#include <IGApi/HikerAPI.hpp>

#include <dev_tools/commons/Terminal.hpp>

namespace LegacyCreds {
    static void PerformUserLogin(const std::string& uval, const std::string& pval) {
        Terminal::println(std::cout, Terminal::TermColor::RED,
                          "Warning: You are using the unsecure login ability by using a credentials.ini file.", false);
        Terminal::println(std::cout, Terminal::TermColor::RED,
                          "It is strongly recommended to use \"userctl login\" to secure your details,", false);
        Terminal::println(std::cout, Terminal::TermColor::RED,
                          "as you won't be prone to malicious applications / modifications obtaining your login details.",
                          false);
        Terminal::println(std::cout, Terminal::TermColor::RED,
                          "\"userctl\" makes sure that your login details are encrypted.\n", false);
        Terminal::print(std::cout, Terminal::TermColor::CYAN, "Attempting to log in as ", false);
        Terminal::print(std::cout, Terminal::TermColor::PURPLE, uval, false);
        Terminal::println(std::cout, Terminal::TermColor::CYAN, "...", true);

        try {
            IG::Session::User user(uval);
            user.Login(pval.c_str());
        } catch (const IG::Session::AuthenticationError& e) {
            std::cerr << "PerformUserLogin#Authentication failed: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "PerformUserLogin failed: " << e.what() << std::endl;
        }
    }

    static void PerformTokenLogin(const std::string& token) {
        try {
            IG::Session::InitializeSharedHikerInstance(token);
        } catch (const IG::Session::AuthenticationError& e) {
            std::cerr << "PerformTokenLogin#Authentication failed: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "PerformTokenLogin failed: " << e.what() << std::endl;
        }
    }

    void InsertCredentials(const Credentials& creds) {
        if (!creds.username.empty() && !creds.password.empty())
            PerformUserLogin(creds.username, creds.password);

        if (!creds.hikerApi.empty())
            PerformTokenLogin(creds.hikerApi);
    }
}
