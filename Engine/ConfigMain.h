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
    bool blackColor{ true };
    bool autoUpdate{ false };
    bool writeLogs{ true };
    bool autoExit{ false };

    //float timeDelay{ 600000 };
};

struct ConfigMain {
    OptionsConfig optionsConfig; // �������� ���������� ��� �������� � ��� ���������
    std::string playerName{ "PlayerName" }; //��� ������
   

    ConfigMain() {
        
    }
};

extern ConfigMain G_CONFIG_MAIN;