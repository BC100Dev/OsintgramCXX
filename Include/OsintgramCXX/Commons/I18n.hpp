#ifndef OSINTGRAMCXX_I18N_HPP
#define OSINTGRAMCXX_I18N_HPP

#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class I18n {
public:
    I18n(const std::string& lang, const std::string& ns);

    ~I18n();

    std::string GetText(const std::string& key);
private:
    std::string lang;
    std::string ns;

    json data;
};

#endif //OSINTGRAMCXX_I18N_HPP
