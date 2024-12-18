#include <filesystem>
#include "DataWarcraft.h"
#include "LogError.h"

#include "HeroInfoEngineFast.h"

bool HeroInfoEngineFast::retrieveHeroDataFast(const std::wstring& saveCodePath) {

    G_HERO_INFO.resize(1);
    G_HERO_INFO[0].nameChar = "FastLoad";

    for (const std::wstring& warPathDirectSave : G_DATA_WARCRAFT.m_DataPath.warPathDirectSave) {

        const std::wstring fullSavePath = warPathDirectSave + saveCodePath;
        if (std::filesystem::is_directory(fullSavePath)) {
            parseHeroData(fullSavePath);
            LogError().logMessageW(L"путь до папки  сохранениями (" + fullSavePath + L")");
        }
    }

    if (G_HERO_INFO[0].path.empty()) {
        return false;
    }

    return true;
}

void HeroInfoEngineFast::parseHeroData(const std::wstring& folderPath) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == L".txt") {
            auto fileTime = std::filesystem::last_write_time(entry);
            if (fileTime > G_HERO_INFO[0].latestTime) {
                G_HERO_INFO[0].latestTime = fileTime;
                G_HERO_INFO[0].path = entry.path().wstring();
            }
        }
    }
}