#include "ConfigMainEngine.h"
#include "LoadDataFail.h"
#include "LogError.h"

ConfigMainEngine::ConfigMainEngine()
{
}
bool ConfigMainEngine::loadConfigMain() {
    LoadDataFail LoadDataFail_;
    const std::string fullLine = LoadDataFail_.loadDataFail(m_FilePath);

    if (fullLine.empty()) {
        //std::wcerr << L"Unable to loadConfigMain data from file: " << m_FilePath << std::endl;
        //return false;
        saveConfigMain();
    }

    std::stringstream fileStream(fullLine);
    std::string line;
    std::string section;

    while (std::getline(fileStream, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (line.empty() || line[0] == L';' || line[0] == L'#') {
            continue;
        }

        if (line[0] == L'[') {
            section = line.substr(1, line.find(L']') - 1);
        }
        else if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

            if (section == "Options") {
                if (key == "autoClickerKey") {
                    G_CONFIG_MAIN.optionsConfig.autoClickerKey = parseBool(value);
                }
                else if (key == "autoClickerMouse") {
                    G_CONFIG_MAIN.optionsConfig.autoClickerMouse = parseBool(value);
                }
                else if (key == "autoExit") {
                    G_CONFIG_MAIN.optionsConfig.autoExit = parseBool(value);
                }
                else if (key == "blackColor") {
                    G_CONFIG_MAIN.optionsConfig.blackColor = parseBool(value);
                }
                else if (key == "writeLogs") {
                    G_CONFIG_MAIN.optionsConfig.writeLogs = parseBool(value);
                }
                else if (key == "autoUpdate") {
                    G_CONFIG_MAIN.optionsConfig.autoUpdate = parseBool(value);
                }
            }
            else if (section == "PlayerName") {
                if (key == "name") {
                    G_CONFIG_MAIN.playerName = value;
                }
            }
        }
    }

    return true;
}
bool ConfigMainEngine::saveConfigMain() const {
    std::ofstream file(m_FilePath);
    if (!file.is_open()) {
        LogError().logError("Unable to open file: " + m_FilePath.string());
        return false;
    }
    // Сохранение секции Options
    file << "[Options]" << std::endl;
    file << "autoClickerKey=" << (G_CONFIG_MAIN.optionsConfig.autoClickerKey ? "true" : "false") << std::endl;
    file << "autoClickerMouse=" << (G_CONFIG_MAIN.optionsConfig.autoClickerMouse ? "true" : "false") << std::endl;
    file << "autoExit=" << (G_CONFIG_MAIN.optionsConfig.autoExit ? "true" : "false") << std::endl;
    file << "blackColor=" << (G_CONFIG_MAIN.optionsConfig.blackColor ? "true" : "false") << std::endl;
    file << "writeLogs=" << (G_CONFIG_MAIN.optionsConfig.writeLogs ? "true" : "false") << std::endl;
    file << "autoUpdate=" << (G_CONFIG_MAIN.optionsConfig.autoUpdate ? "true" : "false") << std::endl;

    // Сохранение секции PlayerName
    file << "[PlayerName]" << std::endl;
    file << "name=" << std::string(G_CONFIG_MAIN.playerName.begin(), G_CONFIG_MAIN.playerName.end()) << std::endl;

    file.close();
    return true;
}

bool ConfigMainEngine::parseBool(const std::string& value) {
    return value == "true" || value == "1";
}

int ConfigMainEngine::parseInt(const std::string& value) {
    return std::stoi(value);
}