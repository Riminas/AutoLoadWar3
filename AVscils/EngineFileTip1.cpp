#include <Windows.h>
#include <iostream>
#include <filesystem>

#include "Global.h"
#include "getMapOpen.h"
#include "LoadManager.h"
#include "EngineFileTip1.h"
#include "NewDirectory.h"
#include "SkillsUpgradeStart.h"
#include "HeroInfoEngine.h"
#include "ConfigMain.h"
#include "ConfigMapsEngine.h"
#include "DataPath.h"
#include "DataMaps.h"

void EngineFileTip1::engineFile()
{
    HeroInfoEngine HeroInfoEngine_;
    if (!HeroInfoEngine_.retrieveHeroData(G_CONFIG_MAPS.path)) return;

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
    else {
        patc = G_HERO_INFO.front().path;
    }
    std::cout << "Hero:  " << nameChar << std::endl;

    LoadManager LoadManager_(G_DATA_PATH.hWndWindowWar);
    LoadManager_.executeLoad(patc);

    //if (G_CONFIG_MAIN.optionsConfig.autoSkillsUpgrade) {
    //    const std::wstring& nameMaps(G_DATA_MAPS.m_NameMaps);
    //    SkillsUpgradeStart ScilsUpgradeStart_(nameMaps, G_DATA_PATH.versionWarcraft);
    //    ScilsUpgradeStart_.skillsUpgradeStart();
    //}
    ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
    ConfigMapsEngine_.startGameCoutCmd(G_DATA_PATH.hWndWindowWar);
}


