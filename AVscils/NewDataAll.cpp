#include <windows.h>

#include "Global.h"
#include "getMapOpen.h"
#include "Options.h"
#include "NewDataAll.h"
#include "ConfigMain.h"
#include "ConfigMapsEngine.h"
#include "DataWarcraft.h"
#include "DataMaps.h"
#include "ConfigMainEngine.h"
#include "LoadDataFail.h"
#include "LogError.h"


NewDataAll::NewDataAll()
{
}

void NewDataAll::newMaps()//должнен обновить информацию об карте и все что заисит от нее
{
	const std::wstring newNameMaps = getMapOpen().getMapOpen1(G_DATA_WARCRAFT.m_DataPath.warPathDirectMaps);
    
    LogError().logMessageW(L"newNameMaps: " + newNameMaps);
    LogError().logMessageW(L"G_DATA_MAPS.m_NameMapsFull: " + G_DATA_MAPS.m_NameMapsFull);

    if (newNameMaps == L"error") { 
        G_DATA_MAPS.isNewMaps = false;
        //G_DATA_ALL.isMapsStart = true;
        G_DATA_ALL.isMapsStart = false;
        return;
    }

    G_DATA_ALL.isMapsStart = true;
    bool isNewMaps = false;
    const bool isNewNameMaps = G_DATA_MAPS.m_NameMapsFull != newNameMaps;
	if (isNewNameMaps) {
        G_DATA_MAPS.m_NameMapsFull = newNameMaps;
        G_DATA_MAPS.NameMaps();
        ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
        ConfigMapsEngine_.loadConfigMaps();

        if (G_DATA_MAPS.isNewMaps == false) {
            static int num = { -1 };
            if (num == -1) {
                num = PutSaveCode(G_DATA_WARCRAFT.m_DataPath.warPathDirectSave);
            }
            if (num == 1) {
                isNewMaps = true;
            }
            else if (num == 0) { }
        }

        G_DATA_MAPS.isNewMaps = isNewMaps;
	}

	
}

bool NewDataAll::newWarcraft(const HWND& hWndWindow)//должнен обновить информацию об Warcrft 3 и все что заисит от нее
{
    return G_DATA_WARCRAFT.initializeDataWarcraft(hWndWindow);
}

int NewDataAll::PutSaveCode(const std::wstring& path) {
    //if (G_CONFIG_MAPS.path == L"False")
    //    return 2;

    if (loadDatFail(path)) {
        LogError().logMessageW(L"Loads1: " + G_CONFIG_MAPS.path);
        return 1;
    }

    if (PathDirectorySaveCode(path)) {
        LogError().logMessageW(L"Loads2: " + G_CONFIG_MAPS.path);
        return 1;
    }

    //G_CONFIG_MAPS.path = L"False";
    return 0;
}

bool NewDataAll::loadDatFail(const std::wstring& fullPath) {
    ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
    ConfigMapsEngine_.loadConfigMaps();

    if (!G_CONFIG_MAPS.path.empty() && std::filesystem::is_directory((fullPath + G_CONFIG_MAPS.path)))
        return true;
    return false;
}

std::wstring NewDataAll::removePath(const std::wstring& filePath) {
    size_t start_pos = filePath.find(L"CustomMapData");
    if (start_pos == std::wstring::npos) {
        LogError().logError("Substring 'CustomMapData' not found in the path.");
        return L"";
    }
    start_pos += std::wstring(L"CustomMapData").length() + 1;

    size_t end_pos = filePath.find_last_of(L"\\");
    if (end_pos == std::wstring::npos) {
        LogError().logError("Last backslash not found in the path.");
        return L"";
    }

    return L"\\" + filePath.substr(start_pos, end_pos - start_pos);
}

bool NewDataAll::PathDirectorySaveCode(const std::wstring& path) {
    std::wstring substr = G_DATA_MAPS.m_NameMaps;
    if (substr.size() > 4 && substr.substr(substr.size() - 4) == L".txt") {
        substr.erase(substr.size() - 4);
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (std::filesystem::is_directory(entry.path())) {
            std::wstring result(L"\0");
            std::wstring str = entry.path().filename().wstring();
            std::transform(str.begin(), str.end(), std::back_inserter(result), [](char c) {
                return std::tolower(c);
                });


            if (result == substr) {
                G_CONFIG_MAPS.path = removePath(entry.path().wstring());
                return !G_CONFIG_MAPS.path.empty();
            }
        }
    }
    return false;
}