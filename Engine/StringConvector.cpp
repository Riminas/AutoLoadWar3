#include "pch.h"
#include <Windows.h>
#include "StringConvector.h"

std::string StringConvector::utf16_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring StringConvector::utf8_to_utf16(const std::string& str) {
    if (str.empty()) return {};

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void StringConvector::adjustTextToFit(sf::Text& text, const float maxWidth) {
    std::wstring str = text.getString().toWideString();
    if (str.empty()) return;

    const sf::Font& font = *text.getFont();
    const unsigned int characterSize = text.getCharacterSize();
    static const std::wstring ellipsis = L"...";
    
    sf::Text tempText;
    tempText.setFont(font);
    tempText.setCharacterSize(characterSize);
    tempText.setString(str);
    if (tempText.getLocalBounds().width <= maxWidth) {
        return;
    }

    while (!str.empty()) {
        str.pop_back();
        tempText.setString(str + ellipsis);
        if (tempText.getLocalBounds().width <= maxWidth) break;
    }

    text.setString(str + ellipsis);
}

std::wstring StringConvector::toLower(const std::wstring& str) {
    std::wstring result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

std::string StringConvector::toLowerStr(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}
