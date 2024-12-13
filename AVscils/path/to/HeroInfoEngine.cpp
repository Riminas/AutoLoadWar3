#include "HeroInfoEngine.h"
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include "LoadDataFail.h"
#include "StringConvector.h"
#include "DataWarcraft.h"
#include "ConfigMapsEngine.h"
#include "DataMaps.h"
#include "LogError.h"
#include <codecvt>

bool HeroInfoEngine::retrieveHeroData(const std::wstring& saveCodePath) {

    const std::wstring fullSavePath = G_DATA_WARCRAFT.m_DataPath.warPathDirectSave + saveCodePath;
    LogError().logError("Thil_3_1_1");

    if (!std::filesystem::is_directory(fullSavePath)) {
        LogError().logErrorW(L"Directory - (" + fullSavePath + L")");
        return false;
    }

    // Определяем путь к файлу сохранённых данных
    std::wstring heroDataFilePath = L"DataAutoLoad/DataMaps/" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini";

    // Пытаемся загрузить данные из файла
    if (std::filesystem::exists(heroDataFilePath)) {
        LogError().logErrorW(L"Loading hero data from file: " + heroDataFilePath);
        if (loadDataHero(G_HERO_INFO)) {
            LogError().logError("Thil_3_1_2_Загрузка данных успешна");
            sortList(G_HERO_INFO);
            LogError().logError("Thil_3_1_3_Сортировка данных выполнена");
            return true;
        }
        else {
            LogError().logErrorW(L"Не удалось загрузить данные из файла: " + heroDataFilePath);
        }
    }

    // Если загрузка не удалась, парсим данные из папки
    LogError().logError("Thil_3_1_2_Начало парсинга данных");
    G_HERO_INFO = parseHeroData(fullSavePath);
    LogError().logError("Thil_3_1_3_Парсинг данных завершён");

    sortList(G_HERO_INFO);
    LogError().logError("Thil_3_1_4_Сортировка данных выполнена");

    // Сохраняем данные после успешного парсинга
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
    LogError().logError("Thil_3_2_1_1");
}

std::vector<HeroInfo> HeroInfoEngine::parseHeroData(const std::wstring& folderPath) {
    std::unordered_map<std::string, HeroInfo> heroDataMap;

    LogError().logErrorW(L"Thil_3_1_1_1 " + folderPath);
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_directory()) {
            LogError().logError("Thil_9 " + entry.path().string());
            processDirectory(entry, heroDataMap);
        }
        else if (isTextFile(entry)) {
            LogError().logError("Thil_8 " + entry.path().string());
            processFile(entry, heroDataMap);
        }
        LogError().logError("Thil_3_1_1_2");
    }
    LogError().logError("Thil_3_1_1_7");

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

inline bool HeroInfoEngine::isTextFile(const std::filesystem::directory_entry& entry) const {
    return std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt";
}

bool HeroInfoEngine::isHeroToken(const std::string& token) const {
    static const std::unordered_set<std::string> heroTokens = { "hero", "job", "char", "character", "герой" };
    return heroTokens.find(token) != heroTokens.end();
}

bool HeroInfoEngine::isHeroTokenIsTrue(const std::string& token) const {
    static const std::unordered_set<std::string> excludeTokens = { "information", "inventory" };
    return excludeTokens.find(token) == excludeTokens.end();
}

std::string HeroInfoEngine::extractHeroName(const std::vector<std::string>& tokens, size_t startIndex) const {
    std::string heroName = tokens[startIndex];
    for (size_t i = startIndex + 1; i < tokens.size(); ++i) {
        if (tokens[i] == "call" || tokens[i] == "level")
            break;
        heroName += " " + tokens[i];
    }
    return heroName;
}

void HeroInfoEngine::updateHeroInfo(const std::filesystem::directory_entry& fileEntry, HeroInfo& heroInfo) {
    std::filesystem::file_time_type fileTime = fileEntry.last_write_time();
    if (fileTime > heroInfo.latestTime) {
        heroInfo.latestTime = fileTime;
        heroInfo.path = fileEntry.path().wstring();
    }
}

std::vector<HeroInfo> HeroInfoEngine::convertToHeroList(const std::unordered_map<std::string, HeroInfo>& heroDataMap) const {
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
    std::wstring heroDataFilePath = L"DataAutoLoad/DataMaps/" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini";

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

        // Конвертация std::wstring к UTF-8
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string pathStr = converter.to_bytes(hero.path);
        file << "path=" << pathStr << "\n";

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
    std::wstring heroDataFilePath = L"DataAutoLoad/DataMaps/" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini";

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
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            currentHero.path = converter.from_bytes(line.substr(5));
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

    file.close();
    LogError().logErrorW(L"Данные героев успешно загружены из: " + heroDataFilePath);
    return true;
} 