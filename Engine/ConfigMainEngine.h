#pragma once
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "ConfigMain.h"
#include "Global.h"

class ConfigMainEngine {
public:
    ConfigMainEngine();
    //void initializeDefaultConfig();
    bool loadConfigMain();
    bool saveConfigMain() const;
private:
    const std::filesystem::path m_FilePath{ G_PATH_APP_DATA + L"DataAutoLoad.ini" };

    bool parseBool(const std::string& value);
    int parseInt(const std::string& value);
};

