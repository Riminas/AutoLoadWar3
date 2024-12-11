#include <Windows.h>

#include <filesystem>

#include "Global.h"
#include "getMapOpen.h"
#include "LoadManager.h"
#include "EngineFileTip1.h"
#include "Options.h"
#include "SkillsUpgradeStart.h"
#include "HeroInfoEngine.h"
#include "HeroInfoEngineFast.h"
#include "ConfigMain.h"
#include "ConfigMapsEngine.h"
#include "LogError.h"
#include "DataWarcraft.h"
#include "DataMaps.h"

void EngineFileTip1::engineFile()
{
    if (!HeroInfoEngineFast().retrieveHeroDataFast(G_CONFIG_MAPS.path)) return;

    if (G_HERO_INFO.size() == 0) {
        MessageBox(NULL, L"Неудалась найти профы", L"Error", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    engineTip1();
    return;
}

void EngineFileTip1::engineTip1()
{
    std::wstring patc;
    std::string nameChar;

    if (G_HERO_INFO.size() > 1) {
        auto latestTime = std::filesystem::file_time_type();
        for (const auto& p : G_HERO_INFO) {
            if (p.latestTime > latestTime) {
                latestTime = p.latestTime;
                patc = p.path;
                nameChar = p.nameChar;
            }
        }
    }
    else{
        patc = G_HERO_INFO[0].path;// patc = G_HERO_INFO.front().path
    }

    LogError().logMessage("Герой: " + nameChar);

    LoadManager LoadManager_(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
    LoadManager_.executeLoad(patc);

    //if (G_CONFIG_MAIN.optionsConfig.autoSkillsUpgrade) {
    //    const std::wstring& nameMaps(G_DATA_MAPS.m_NameMaps);
    //    SkillsUpgradeStart ScilsUpgradeStart_(nameMaps, G_DATA_WARCRAFT.m_DataPath.versionWarcraft);
    //    ScilsUpgradeStart_.skillsUpgradeStart();
    //}
    ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
    ConfigMapsEngine_.startGameCoutCmd(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
}


