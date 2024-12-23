#pragma once
#include <string>
#include <array>


enum class CmdEntryEnum
{
    RETURN,
    SAVE,
    CREATE,
    CAM,
};

struct CmdEntry {
    std::string cmd{ "" };
    bool start{ false };
    bool isVisibleButton{ false };
};

struct ConfigMaps {
    std::wstring path{ L"0" };
    std::wstring lastPath{ L"\0" };

    std::array<CmdEntry, 4> mainConfig = { {
        { "-return" },
        { "-save" },
        { "-create" },
        { "-cam 150" }
        } }; // содердит информацию об кнопках для гланого меню

    std::array<CmdEntry, 5> usersConfig{ {} }; // содердит информацию об кнопках пользовательских
};

extern ConfigMaps G_CONFIG_MAPS;