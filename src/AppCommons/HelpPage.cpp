#include <AppCommons/HelpPage.hpp>
#include <algorithm>

HelpPage::Item::Item(std::string& a, std::string& e, std::string& d) : arg(a), equalDesc(e), description(d) {}

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

std::string HelpPage::display() const {
    std::string output;
    int maxItemLength = 0;

    for (const auto& item : argItemList) {
        std::string argLine = item.arg + (item.equalDesc.empty() ? "" : "=" + item.equalDesc);
        maxItemLength = std::max(maxItemLength, static_cast<int>(argLine.length()));
    }

    for (const auto& item : argItemList) {
        std::string argLine = item.arg + (item.equalDesc.empty() ? "" : "=" + item.equalDesc);
        if (startSpaceWidth > 0)
            output += std::string(startSpaceWidth, ' ');

        output += argLine;
        output += std::string(maxItemLength - argLine.length() + spaceWidth, ' ');
        output += item.description + "\n";
    }

    return output;
}

void HelpPage::display(std::ostream& os) const {
    os << display();
}

void HelpPage::free() {
    argItemList.clear();
    argItemList.shrink_to_fit();
}