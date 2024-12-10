
#include <filesystem>
#include <unordered_set>

#include "LoadDataFail.h"
#include "StringConvector.h"
#include "HeroInfoEngine.h"
#include "DataPath.h"
#include "ConfigMapsEngine.h"
#include "DataMaps.h"
#include "LogError.h"
#include <codecvt>

bool HeroInfoEngine::retrieveHeroData(const std::wstring& saveCodePath) {

    const std::wstring fullSavePath = G_DATA_PATH.warPathDirectSave + saveCodePath;
    LogError().logError("Thil_3_1_1");

    if (!std::filesystem::is_directory(fullSavePath)) {
        LogError().logErrorW(L"directory - (" + fullSavePath + L")");
        return false;
    }

    LogError().logError("Thil_3_1_2");
    G_HERO_INFO = parseHeroData(fullSavePath);
    LogError().logError("Thil_3_1_3");

    sortList(G_HERO_INFO);
    LogError().logError("Thil_3_1_4");
    return true;
}

// Utility function to remove specific characters from a string
void HeroInfoEngine::removeSpecialCharacters(std::string& str) {
    std::string str2 = str;
    str = "\0";
    for (char& c : str2) {
        if (c == L'(' || c == L')' || c == L':' || c == L'"') {
        }
        else {
            str += c;
        }
    }
}

void  HeroInfoEngine::sortList(std::vector<HeroInfo>& heroList) {
    std::sort(heroList.begin(), heroList.end(), [](const HeroInfo& a, const HeroInfo& b) {
        return a.latestTime < b.latestTime; // Sort by latestTime in descending order
        });
    LogError().logError("Thil_3_2_1_1");
}

std::vector<HeroInfo> HeroInfoEngine::parseHeroData(const std::wstring& folderPath) {
    std::unordered_map<std::string, HeroInfo> heroDataMap;

    LogError().logErrorW(L"Thil_3_1_1_1 "+folderPath);
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
    LogError().logError("Thil_3_1_1_7");//Thil_3_1_1_2

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
    // Получение времени последнего изменения файла
    const auto fileTime = fileEntry.last_write_time();

    // Поиск имени героя в токенах
    for (size_t i = 0; i < tokens.size(); ++i) {//  " hero:ear test " >>> " heroear test "
        removeSpecialCharacters(tokens[i]);
        if (isHeroToken(StringConvector_.toLowerStr(tokens[i])) && isHeroTokenIsTrue(StringConvector_.toLowerStr(tokens[i + 1]))) {
            std::string heroName = extractHeroName(tokens, i + 1);
            removeSpecialCharacters(heroName);

            // Обработка символа '|' в имени героя
            for (size_t j = 0; j < heroName.size(); ++j) {
                if (heroName[j] == L'|') {
                    if (heroName[j + 1] == L'r' || heroName[j + 1] == L'R') {
                        heroName.erase(j, 2);
                        --j;
                    }
                    else if (heroName[j + 1] == L'c' || heroName[j + 1] == L'C') {
                        heroName.erase(j, 10);
                    }
                }
            }

            // Обновление информации о герое
            HeroInfo& heroInfo = heroDataMap[heroName];
            if (fileTime > heroInfo.latestTime) {
                heroInfo.latestTime = fileTime;
                heroInfo.path = fileEntry.path().wstring();
            }
            return;
        }
    }

    // Если имя героя не найдено, используем имя файла как имя героя
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
    static const std::unordered_set<std::string> heroTokens = { "hero", "job", "char", "character", "перс" };
    return heroTokens.count(token) > 0 || (token[0] == 'h' && token[1] == 'e' && token[2] == 'r' && token[3] == 'o');
}

bool HeroInfoEngine::isHeroTokenIsTrue(const std::string& token) const {
    static const std::unordered_set<std::string> heroTokens = { "information", "inventory" };
    return heroTokens.count(token) == 0;
}

std::string HeroInfoEngine::extractHeroName(const std::vector<std::string>& tokens, size_t startIndex) const {
    std::string heroName = tokens[startIndex];
    for (size_t i = startIndex + 1; i < tokens.size(); ++i) {
        if (tokens[i] == "call" || tokens[i] == "Level")
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
    //LogError().logError("Thil_3_1_1_3");
    for (const auto& [name, info] : heroDataMap) {
        heroList.push_back(HeroInfo(name, info.path, info.latestTime));
    }
    //LogError().logError("Thil_3_1_1_4");
    //saveDataHero(heroList);
    return heroList;
}

//void HeroInfoEngine::saveDataHero(const std::vector<HeroInfo>& heroInfo) const {
//    const std::wstring filePathFull = { L"DataAutoLoad/DataMaps" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini" };
//
//    std::ofstream file(filePathFull);
//    if (!file.is_open()) {
//        std::wcerr << L"Unable to open file: " << filePathFull << std::endl;
//        return;
//    }
//    file << "size=" << heroInfo.size() << std::endl;
//    file << "minTime=" << heroInfo[0].latestTime << std::endl;
//
//    file << "; Hero" << std::endl;
//    for (const HeroInfo& hero : heroInfo) {
//        file << "name" << "=" << std::string(hero.nameChar.begin(), hero.nameChar.end()) << std::endl;
//        file << "path" << "=" << std::string(hero.path.begin(), hero.path.end()) << std::endl;
//        file << "time" << "=" << hero.latestTime << std::endl;
//    }
//    file.close();
//}
//
//void HeroInfoEngine::loadDataHero(std::unordered_map<std::string, HeroInfo>& heroInfo, ) {
//    const std::wstring filePathFull = { L"DataAutoLoad/DataMaps" + G_DATA_MAPS.m_NameMaps + L"/Hero.ini" };
//
//    std::ifstream file(filePathFull);
//    if (!file.is_open()) {
//        std::wcerr << L"Unable to open file: " << filePathFull << std::endl;
//        return;
//    }
//
//    std::string line;
//    size_t size = 0;
//
//    // Считываем размер списка героев
//    if (std::getline(file, line)) {
//        if (line.find("size=") == 0) {
//            size = std::stoul(line.substr(5));
//        }
//        else { std::cerr << "Unexpected file format (missing size)." << std::endl; return; }
//    }
//    // Считываем минимальное время
//    if (std::getline(file, line)) {
//        if (line.find("size=") == 0) {
//            size = std::stoul(line.substr(5));
//        }
//        else { std::cerr << "Unexpected file format (missing size)." << std::endl; return; }
//    }
//
//    while (std::getline(file, line)) {
//        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
//
//        HeroInfo hero;
//
//        if (line.find("name=") == 0) {
//            hero.nameChar = line.substr(5);
//        }
//        else { std::cerr << "Unexpected file format (missing name)." << std::endl; break; }
//
//        if (std::getline(file, line) && line.find("path=") == 0) {
//            hero.path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(line.substr(5));
//        }
//        else {
//            std::cerr << "Unexpected file format (missing path)." << std::endl;
//            break;
//        }
//
//        if (std::getline(file, line) && line.find("time=") == 0) {
//            try {
//                auto seconds_since_epoch = std::stoll(line.substr(5));
//                hero.latestTime = std::filesystem::file_time_type{} + std::chrono::seconds(seconds_since_epoch);
//            }
//            catch (const std::invalid_argument& e) { std::cerr << "Invalid time format: " << e.what() << std::endl; break; }
//            catch (const std::out_of_range& e) { std::cerr << "Time value out of range: " << e.what() << std::endl; break; }
//        }
//        else { std::cerr << "Unexpected file format (missing time)." << std::endl; break; }
//
//        heroInfo[hero.nameChar] = hero;
//    }
//
//    file.close();
//}