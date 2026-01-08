#include <dev_utils/commons/HelpPage.hpp>
#include <algorithm>
#include <sstream>

HelpPage::Item::Item(std::string &a, std::string &e, std::string &d) : arg(a), equalDesc(e), description(d) {}

HelpPage::HelpPage() : spaceWidth(5), startSpaceWidth(0) {}

void HelpPage::setSpaceWidth(int width) {
    spaceWidth = width;
}

int HelpPage::getSpaceWidth() const {
    return spaceWidth;
}

void HelpPage::setStartSpaceWidth(int width) {
    startSpaceWidth = width;
}

int HelpPage::getStartSpaceWidth() const {
    return startSpaceWidth;
}

void HelpPage::addArg(std::string arg, std::string assignableDesc, std::string description) {
    argItemList.emplace_back(arg, assignableDesc, description);
}

void HelpPage::setDescSeparator(const std::string &sep) {
    descSep = sep;
}

std::string HelpPage::getDescSeparator() const {
    return descSep;
}

std::string HelpPage::display() const {
    std::ostringstream oss;
    int maxItemLength = 0;

    for (const auto &item: argItemList) {
        std::string argLine = item.arg + (item.equalDesc.empty() ? "" : descSep + item.equalDesc);
        maxItemLength = std::max(maxItemLength, static_cast<int>(argLine.length()));
    }

    for (const auto& item : argItemList) {
        std::string argLine = item.arg + (item.equalDesc.empty() ? "" : descSep + item.equalDesc);
        if (startSpaceWidth > 0)
            oss << std::string(startSpaceWidth, ' ');

        oss << argLine
            << std::string(maxItemLength - argLine.length() + spaceWidth, ' ')
            << item.description << "\n";
    }

    return oss.str();
}

void HelpPage::display(std::ostream &os) const {
    os << display();
}

void HelpPage::free() {
    argItemList.clear();
    argItemList.shrink_to_fit();
}