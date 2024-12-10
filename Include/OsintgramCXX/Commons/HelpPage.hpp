#ifndef ABOAWT_HELPPAGE_H
#define ABOAWT_HELPPAGE_H

#include <string>
#include <iostream>
#include <vector>

class HelpPage {
private:
    struct Item {
        std::string arg;
        std::string equalDesc;
        std::string description;

        Item(std::string& a, std::string& e, std::string& d);
    };

    int spaceWidth, startSpaceWidth;
    std::vector<Item> argItemList;
    std::string descSep = "=";

public:
    HelpPage();

    void setSpaceWidth(int width);
    [[nodiscard]] int getSpaceWidth() const;

    void setStartSpaceWidth(int width);
    [[nodiscard]] int getStartSpaceWidth() const;

    void setDescSeparator(const std::string& sep);
    [[nodiscard]] std::string getDescSeparator() const;

    void addArg(std::string arg, std::string assignableDesc, std::string description);
    [[nodiscard]] std::string display() const;
    void display(std::ostream& os) const;

    void free();
};

#endif //ABOAWT_HELPPAGE_H
