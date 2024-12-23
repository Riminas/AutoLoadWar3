#include "pch.h"
#include "getMapOpen.h"
#include "LogManager.h"
#include <regex>

namespace {
    constexpr char MAP_30_MINUTES[] = "30 minutes";
    constexpr char DEFAULT_MAP_NAME[] = "Just another Warcraft III map";
    constexpr char WAR3_HEADER[] = "HM3W";
}

std::wstring getMapOpen::getMapOpen1(const std::wstring& folder_path)
{
    if (!std::filesystem::is_directory(folder_path.c_str())) {
        LogManager::logger().log(LogManager::LogLevel::Error, L"Put(not \"Maps\" directory) - (" + folder_path + L")");
        return L"error";
    }

    try {
        if (!findUsedMapFile(folder_path)) {
            return L"error";
        }
        return processMapFile();
    }
    catch (const std::exception& e) {
        LogManager::logger().log(LogManager::LogLevel::Error, std::string("Error processing map: ") + e.what());
        return L"error";
    }

    return L"error";
}

bool getMapOpen::findUsedMapFile(const std::wstring& folder_path)
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path)) {
        if (std::filesystem::is_directory(entry.path())) {
            continue; // Пропускаем директории, если это необходимо
        }
        const std::filesystem::path path = entry.path();
        if (checkFile(path, m_nameFile))
            return true;
    }
    return false;
}

bool getMapOpen::checkFile(const std::filesystem::path& filePath, std::wstring& nameFile) {
    std::fstream file;
    file.open(filePath);
    if (!file.is_open()) {
        ////std::lock_guard<std::mutex> lock(m_Mtx);
        nameFile = filePath.wstring();
        return true;
    }
    file.close();
    return false;
}

std::string getMapOpen::removeColorCodes(std::string str)
{
    std::string result;
    result.reserve(str.size());

    for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '|') {
            if (i + 1 < str.size()) {
                if (str[i + 1] == 'r' || str[i + 1] == 'R') {
                    i += 1;
                    continue;
                }
                if ((str[i + 1] == 'c' || str[i + 1] == 'C') && i + 9 < str.size()) {
                    i += 9;
                    continue;
                }
            }
        }
        result += str[i];
    }

    return result;
}

std::wstring getMapOpen::processMapFile()
{
    std::ifstream file(m_nameFile);
    if (!file) {
        throw std::runtime_error("Cannot open map file");
    }

    std::string content;
    std::getline(file, content);
    file.close();

    // Удаляем WAR3 заголовок если есть
    if (content.compare(0, 4, WAR3_HEADER) == 0) {
        content.erase(0, 4);
    }

    // Удаляем начальные нулевые байты
    content.erase(0, content.find_first_not_of('\0'));

    // Проверяем специальную карту
    if (content.compare(0, sizeof(MAP_30_MINUTES) - 1, MAP_30_MINUTES) == 0) {
        return L"30minutes";
    }

    // Обрезаем по первому нулевому байту
    std::size_t pos = content.find('\0');
    if (pos != std::string::npos) {
        content.erase(pos);
    }

    content = removeColorCodes(std::move(content));

    // Возвращаем имя карты или имя файла
    if (containsOnlyEnglishCharacters(content) && content != DEFAULT_MAP_NAME) {
        return std::wstring(content.begin(), content.end());
    }

    return std::filesystem::path(m_nameFile).filename().wstring();
}

bool getMapOpen::containsOnlyEnglishCharacters(const std::string& text)
{
    static const std::regex englishRegex(R"(^[A-Za-z0-9 .\$;@!^+=/<>#?*,\"'&:_~()\-\[\]]*$)");
    return std::regex_match(text, englishRegex);
}