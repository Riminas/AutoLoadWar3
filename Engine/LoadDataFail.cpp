#include "pch.h"
#include <filesystem>
#include <fstream>

#include "StringConvector.h"
#include "LoadDataFail.h"

std::string LoadDataFail::loadDataFail(const std::wstring& pathFail)
{
    // �������� ������������� �����
    if (!std::filesystem::exists(pathFail)) {
        return "\0";
    }

    // ��������� ������� �����
    auto fileSize = std::filesystem::file_size(pathFail);

    // �������� �����
    FILE* file = nullptr;
    std::string str2 = { pathFail.begin(),pathFail.end() };
    errno_t err = fopen_s(&file, str2.c_str(), "rb");
    if (err == 0 && file != nullptr) {
        // ������ ����� � �����
        std::vector<char> buffer(fileSize);
        fread(buffer.data(), 1, fileSize, file);
        fclose(file);

        // �������������� ������ � ������
        std::string str(buffer.begin(), buffer.end());

        return str;
    }
    return "\0";
}

std::vector<std::string> LoadDataFail::loadDataFailTokens(const std::wstring& pathFail) {
    const std::string fullLine = loadDataFail(pathFail);

    if (fullLine.empty())
        return std::vector<std::string>();

    std::istringstream iss(fullLine);
    std::vector<std::string> tokens((std::istream_iterator<std::string, char>(iss)), std::istream_iterator<std::string, char>());

    return tokens;
}

std::wstring LoadDataFail::loadDataFailW(const std::wstring& pathFail)
{
    // �������� ������������� �����
    if (!std::filesystem::exists(pathFail)) {
        return L"\0";
    }

    // ��������� ������� �����
    auto fileSize = std::filesystem::file_size(pathFail);

    // �������� �����
    FILE* file = nullptr;
    std::string str2 = { pathFail.begin(),pathFail.end() };
    errno_t err = fopen_s(&file, str2.c_str(), "rb");
    if (err == 0 && file != nullptr) {
        // ������ ����� � �����
        std::vector<char> buffer(fileSize);
        fread(buffer.data(), 1, fileSize, file);
        fclose(file);

        // �������������� ������ � ������
        std::string str(buffer.begin(), buffer.end());
        return StringConvector().utf8_to_utf16(str);
    }
    return L"\0";
}

std::vector<std::wstring> LoadDataFail::loadDataFailTokensW(const std::wstring& pathFail) {
    const std::wstring fullLine = loadDataFailW(pathFail);

    if (fullLine.empty())
        return std::vector<std::wstring>();

    std::wistringstream iss(fullLine);
    std::vector<std::wstring> tokens((std::istream_iterator<std::wstring, wchar_t>(iss)), std::istream_iterator<std::wstring, wchar_t>());

    return tokens;
}