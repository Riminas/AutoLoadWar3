#include "pch.h"
#include <Windows.h>

#include <filesystem>

#include "LoadManager.h"
#include "EngineFileTip1.h"
#include "SkillsUpgradeStart.h"
#include "HeroInfoEngineFast.h"
#include "ConfigMapsEngine.h"
#include "LogManager.h"
#include "DataWarcraft.h"
#include "DataMaps.h"

bool EngineFileTip1::engineFile()
{
    if (!HeroInfoEngineFast().retrieveHeroDataFast(G_CONFIG_MAPS.path)) return false;

    if (G_HERO_INFO.size() == 0) {
        //MessageBox(NULL, L"��������� ����� �����", L"Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
    }

    return true;
}

void EngineFileTip1::engineTip1()
{
    LogManager::logger().log(LogManager::LogLevel::Message, "�����: " + G_HERO_INFO[0].nameChar);

    LoadManager LoadManager_(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
    LoadManager_.executeLoad(G_HERO_INFO[0].path);

    //if (G_CONFIG_MAIN.optionsConfig.autoSkillsUpgrade) {
    //    const std::wstring& nameMaps(G_DATA_MAPS.m_NameMaps);
    //    SkillsUpgradeStart ScilsUpgradeStart_(nameMaps, G_DATA_WARCRAFT.m_DataPath.versionWarcraft);
    //    ScilsUpgradeStart_.skillsUpgradeStart();
    //}
    ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
    ConfigMapsEngine_.startGameCoutCmd(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
}


