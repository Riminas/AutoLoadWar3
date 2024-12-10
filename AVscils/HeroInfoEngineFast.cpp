#include <filesystem>
#include "DataPath.h"
#include "LogError.h"

#include "HeroInfoEngineFast.h"

bool HeroInfoEngineFast::retrieveHeroDataFast(const std::wstring& saveCodePath) {

    const std::wstring fullSavePath = G_DATA_PATH.warPathDirectSave + saveCodePath;

    if (!std::filesystem::is_directory(fullSavePath)) {
        LogError().logErrorW(L"directory - (" + fullSavePath + L")");
        return false;
    }

    parseHeroData(fullSavePath);
    return true;
}

void HeroInfoEngineFast::parseHeroData(const std::wstring& folderPath) {
    G_HERO_INFO.resize(1);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == L".txt") {
            auto fileTime = std::filesystem::last_write_time(entry);
            if (fileTime > G_HERO_INFO[0].latestTime) {
                G_HERO_INFO[0].latestTime = fileTime;
                G_HERO_INFO[0].path = entry.path().wstring();
            }
        }
    }
    G_HERO_INFO[0].nameChar = "FastLoad";
}