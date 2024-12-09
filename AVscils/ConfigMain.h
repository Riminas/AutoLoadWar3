#pragma once
#include <string>
#include <vector>
#include <array>


struct OpenConfig {
    std::wstring appPath{ L"" };
    bool isAuto{ false };
    bool isVisibleButton{ true };
};

struct OptionsConfig {
    bool autoClickerKey{ true };
    bool autoClickerMouse{ true };
    bool autoExit{ false };
    bool writeLogs{ true };

    //float timeDelay{ 600000 };
};

struct ConfigMain {
    OptionsConfig optionsConfig; // содержит информацию что включено а что выключено
    std::string playerName{ "PlayerName" }; //имя игрока


    ConfigMain() {
        
    }
};

extern ConfigMain G_CONFIG_MAIN;