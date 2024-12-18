#include <filesystem>
#include <unordered_set>
#include <algorithm>
#include <fstream>

#include "LoadDataFail.h"
#include "StringConvector.h"
#include "HeroInfoEngine.h"
#include "DataWarcraft.h"
#include "DataMaps.h"
#include "LogError.h"
#include <codecvt>

namespace {
    constexpr size_t BUFFER_SIZE = 8192;
    const std::unordered_set<std::string_view> HERO_TOKENS = { "hero", "job", "char", "character", "герой" };
    const std::unordered_set<std::string_view> EXCLUDE_TOKENS = { "information", "inventory" };
}

bool HeroInfoEngine::retrieveHeroData(std::wstring_view saveCodePath) {
    bool foundValidPath = false;
    for (const auto& basePath : G_DATA_WARCRAFT.m_DataPath.warPathDirectMaps) {
        const std::wstring fullSavePath = basePath + std::wstring(saveCodePath);

        if (!std::filesystem::is_directory(fullSavePath)) {
            continue;
        }

        foundValidPath = true;
        G_HERO_INFO = parseHeroData(fullSavePath);
        if (!G_HERO_INFO.empty()) {
            break;
        }
    }

    if (!foundValidPath) {
        LogError().logErrorW(L"Directory not found in any of the specified paths");
        return false;
    }

    if (G_HERO_INFO.empty()) {
        LogError().logError("No hero data found");
        return false;
    }

    sortList(G_HERO_INFO);
    saveDataHero(G_HERO_INFO);
    return true;
}

// Утилита для удаления специальных символов из строки
void HeroInfoEngine::removeSpecialCharacters(std::string& str) {
    std::string temp;
    temp.reserve(str.size());
    for (char c : str) {
        if (c != '(' && c != ')' && c != ':' && c != '\"') {
            temp += c;
        }
    }
    str = temp;
}

void HeroInfoEngine::sortList(std::vector<HeroInfo>& heroList) {
    std::sort(heroList.begin(), heroList.end(), [](const HeroInfo& a, const HeroInfo& b) {
        return a.latestTime > b.latestTime; // Сортировка по latestTime в порядке убывания
        });
}

std::vector<HeroInfo> HeroInfoEngine::parseHeroData(std::wstring_view folderPath) {
    std::unordered_map<std::string, HeroInfo> heroDataMap;
    std::filesystem::path path{ std::wstring(folderPath) };

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            processDirectory(entry, heroDataMap);
        }
        else if (isTextFile(entry)) {
            processFile(entry, heroDataMap);
        }
    }

    return convertToHeroList(heroDataMap);
}

void HeroInfoEngine::processDirectory(const std::filesystem::directory_entry& dirEntry, std::unordered_map<std::string, HeroInfo>& heroDataMap) {
    HeroInfo& heroInfo = heroDataMap[dirEntry.path().filename().string()];

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirEntry.path())) {
        if (isTextFile(entry)) {
            updateHeroInfo(entry, heroInfo);
        }
    }
}

void HeroInfoEngine::processFile(const std::filesystem::directory_entry& fileEntry, std::unordered_map<std::string, HeroInfo>& heroDataMap) {
    // Пропускаем файл если данные были загружены и его время изменения меньше минимального
    if (m_IsDataLoaded && !m_HeroList.empty())
    {
        auto minTime = getMinFileTime();
        if (fileEntry.last_write_time() < minTime)
        {
            return;
        }
    }

    LoadDataFail LoadDataFail_;
    std::vector<std::string> tokens = LoadDataFail_.loadDataFailTokens(fileEntry.path());

    if (tokens.size() <= 1)
        return;

    StringConvector StringConvector_;
    // Получаем время последнего изменения файла
    const auto fileTime = fileEntry.last_write_time();

    // Обрабатываем токены для нахождения имени героя и других данных
    for (size_t i = 0; i < tokens.size(); ++i) {
        removeSpecialCharacters(tokens[i]);
        std::string lowerToken = StringConvector_.toLowerStr(tokens[i]);

        if (isHeroToken(lowerToken) && (i + 1) < tokens.size() && isHeroTokenIsTrue(StringConvector_.toLowerStr(tokens[i + 1]))) {
            std::string heroName = extractHeroName(tokens, i + 1);
            removeSpecialCharacters(heroName);

            // Удаление '|' и последующих символов
            size_t pos = heroName.find('|');
            while (pos != std::string::npos) {
                if (pos + 1 < heroName.size()) {
                    if (heroName[pos + 1] == 'r' || heroName[pos + 1] == 'R') {
                        heroName.erase(pos, 2);
                    }
                    else if ((heroName[pos + 1] == 'c' || heroName[pos + 1] == 'C') && pos + 9 < heroName.size()) {
                        heroName.erase(pos, 10);
                    }
                }
                pos = heroName.find('|', pos);
            }

            // Добавляем или обновляем информацию о герое
            HeroInfo& heroInfo = heroDataMap[heroName];
            if (fileTime > heroInfo.latestTime) {
                heroInfo.latestTime = fileTime;
                heroInfo.path = fileEntry.path().wstring();
            }
            return;
        }
    }

    // Если не найдено соответствующих токенов, обновляем только время и путь
    HeroInfo& heroInfo = heroDataMap[fileEntry.path().filename().string()];
    if (fileTime > heroInfo.latestTime) {
        heroInfo.latestTime = fileTime;
        heroInfo.path = fileEntry.path().wstring();
    }
}

[[nodiscard]] bool HeroInfoEngine::isTextFile(const std::filesystem::directory_entry& entry) const noexcept {
    try {
        return std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt";
    }
    catch (...) {
        return false;
    }
}

[[nodiscard]] bool HeroInfoEngine::isHeroToken(std::string_view token) const noexcept {
    try {
        return HERO_TOKENS.contains(token);
    }
    catch (...) {
        return false;
    }
}

[[nodiscard]] bool HeroInfoEngine::isHeroTokenIsTrue(std::string_view token) const noexcept {
    try {
        return !EXCLUDE_TOKENS.contains(token);
    }
    catch (...) {
        return false;
    }
}

[[nodiscard]] std::string HeroInfoEngine::extractHeroName(
    const std::vector<std::string>& tokens, 
    size_t startIndex) const {
    std::string heroName = tokens[startIndex];
    for (size_t i = startIndex + 1; i < tokens.size(); ++i) {
        if (tokens[i] == "call" || tokens[i] == "level")
            break;
        heroName += " " + tokens[i];
    }
    return heroName;
}

[[nodiscard]] std::filesystem::file_time_type HeroInfoEngine::getMinFileTime() const {
    if (m_HeroList.empty()) {
        return std::filesystem::file_time_type::min();
    }

    return std::min_element(m_HeroList.begin(), m_HeroList.end(),
        [](const HeroInfo& a, const HeroInfo& b) {
            return a.latestTime < b.latestTime;
        })->latestTime;
}

void HeroInfoEngine::updateHeroInfo(const std::filesystem::directory_entry& fileEntry, HeroInfo& heroInfo) {
    std::filesystem::file_time_type fileTime = fileEntry.last_write_time();
    if (fileTime > heroInfo.latestTime) {
        heroInfo.latestTime = fileTime;
        heroInfo.path = fileEntry.path().wstring();
    }
}

[[nodiscard]] std::vector<HeroInfo> HeroInfoEngine::convertToHeroList(
    const std::unordered_map<std::string, HeroInfo>& heroDataMap) const {
    std::vector<HeroInfo> heroList;
    heroList.reserve(heroDataMap.size());

    for (const auto& [name, info] : heroDataMap) {
        heroList.emplace_back(info); // Используем конструктор копирования
    }

    return heroList;
}

// Метод для сохранения данных героев в файл
void HeroInfoEngine::saveDataHero(const std::vector<HeroInfo>& heroInfo) const {
    // Определяем путь к файлу сохранения
    std::wstring heroDataFilePath = L"DataWarAssist/DataMaps/" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini";

    // Создаём директорию, если её нет
    std::filesystem::create_directories(std::filesystem::path(heroDataFilePath).parent_path());

    std::ofstream file(heroDataFilePath, std::ios::out);
    if (!file.is_open()) {
        LogError().logErrorW(L"Не удалось открыть файл для сохранения данных: " + heroDataFilePath);
        return;
    }

    // Записываем количество героев
    file << "size=" << heroInfo.size() << "\n";

    // Записываем минимальное время для потенциальной сортировки или других целей
    if (!heroInfo.empty()) {
        auto minTime = std::min_element(heroInfo.begin(), heroInfo.end(), [](const HeroInfo& a, const HeroInfo& b) {
            return a.latestTime < b.latestTime;
            })->latestTime;
        auto duration = minTime.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        file << "minTime=" << seconds << "\n";
    }
    else {
        file << "minTime=0\n";
    }

    // Записываем данные каждого героя
    for (const auto& hero : heroInfo) {
        file << "name=" << hero.nameChar << "\n";

        file << "path=" << StringConvector().utf16_to_utf8(hero.path) << "\n";

        // Записываем время в секундах с эпохи
        auto duration = hero.latestTime.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        file << "time=" << seconds << "\n";
    }

    file.close();
    LogError().logErrorW(L"Данные героев успешно сохранены в: " + heroDataFilePath);
}

// Метод для загрузки данных героев из файла
bool HeroInfoEngine::loadDataHero(std::vector<HeroInfo>& heroInfo) const {
    heroInfo.clear();

    // Определяем путь к файлу сохранения
    std::wstring heroDataFilePath = L"DataWarAssist/DataMaps/" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini";

    std::ifstream file(heroDataFilePath, std::ios::in);
    if (!file.is_open()) {
        LogError().logErrorW(L"Не удалось открыть файл для загрузки данных: " + heroDataFilePath);
        return false;
    }

    std::string line;
    size_t size = 0;
    int64_t minTime = 0;

    // Чтение размера списка
    if (std::getline(file, line)) {
        if (line.find("size=") == 0) {
            size = std::stoul(line.substr(5));
        }
        else {
            LogError().logErrorW(L"Неожиданный формат файла (отсутствует размер).");
            return false;
        }
    }

    // Чтение минимального времени
    if (std::getline(file, line)) {
        if (line.find("minTime=") == 0) {
            minTime = std::stoll(line.substr(8));
        }
        else {
            LogError().logErrorW(L"Неожиданный формат файла (отсутствует минимальное время).");
            return false;
        }
    }

    HeroInfo currentHero;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        if (line.find("name=") == 0) {
            currentHero = HeroInfo();
            currentHero.nameChar = line.substr(5);
        }
        else if (line.find("path=") == 0) {
            std::wstring wstr = StringConvector().utf8_to_utf16(line.substr(5));
            currentHero.path = wstr;
        }
        else if (line.find("time=") == 0) {
            try {
                int64_t seconds_since_epoch = std::stoll(line.substr(5));
                currentHero.latestTime = std::filesystem::file_time_type{} + std::chrono::seconds(seconds_since_epoch);
            }
            catch (const std::invalid_argument& e) {
                LogError().logErrorW(L"Неверный формат времени: " + std::wstring(e.what(), e.what() + strlen(e.what())));
                continue;
            }
            catch (const std::out_of_range& e) {
                LogError().logErrorW(L"Время вне допустимого диапазона: " + std::wstring(e.what(), e.what() + strlen(e.what())));
                continue;
            }
            heroInfo.push_back(currentHero);
        }
    }
}
