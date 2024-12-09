#include <windows.h>

#include "Global.h"
#include "getMapOpen.h"
#include "NewDirectory.h"
#include "NewDataAll.h"
#include "ConfigMain.h"
#include "ConfigMapsEngine.h"
#include "DataPath.h"
#include "DataMaps.h"
#include "ConfigMainEngine.h"
#include "LoadDataFail.h"


NewDataAll::NewDataAll()
{
}

void NewDataAll::newMaps(const bool& isNoDataCommands, const bool& isOptions)
{
    
	getMapOpen getMapOpen_;
	const std::wstring newNameMaps = getMapOpen_.getMapOpen1(G_DATA_PATH.warPathDirectMaps);
    if (newNameMaps == L"error") { 
        G_DATA_MAPS.isNewMaps = false;
        //G_DATA_ALL.isMapsStart = true;
        G_DATA_ALL.isMapsStart = false;
        return;
    }

    G_DATA_ALL.isMapsStart = true;
    bool isNewMaps = false;
    const bool isNewNameMaps = G_DATA_MAPS.m_NameMapsFull != newNameMaps;
	if (isNoDataCommands || isOptions || isNewNameMaps) {
        if (isNewNameMaps) {
            G_DATA_MAPS.m_NameMapsFull = newNameMaps;
            G_DATA_MAPS.NameMaps();
            ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
            ConfigMapsEngine_.loadConfigMain();
        }

        const bool isNewDirectory{ (G_DATA_MAPS.isNewMaps == false && isNoDataCommands) || isOptions };
        if (isNewDirectory) {
            static int num = { -1 };
            if (num == -1) {
                num = PutSaveCode(G_DATA_PATH.warPathDirectSave);
            }
            if (num == 1 && !isOptions) {
                isNewMaps = true;
            }
            else if (isOptions || (isNoDataCommands && ( num == 0))) {

                NewDirectory().newDirectory();
                //const int num2 = app.newDirectory();
                //if (num2 == 3) { // if (utf8Text == "False") return 3; убрать выбор дириктории
                //    G_CONFIG_MAPS.path.clear();
                //    isNewMaps = false;
                //}
                //else if (num2 == 2) { // if (folderPath == L"Exet") return 2;
                //}
                //else if (num2 == 1) { //true
                //    //if (G_CONFIG_MAPS.path == L"False")
                //    //    G_CONFIG_MAPS.path = L"\0";
                //    isNewMaps = true;
                //}
                //else if (num2 == 0) {// false
                //    G_CONFIG_MAPS.path.clear();
                //    isNewMaps = false;
                //}
                //if (G_CONFIG_MAPS.lastPath != G_CONFIG_MAPS.path) {
                //    G_CONFIG_MAPS.lastPath = G_CONFIG_MAPS.path;
                //    G_DATA_MAPS.m_IsNewInfo = true;
                //}
                //else {
                //    isNewMaps = false;
                //}
            }
            //else if (isNoDataCommands == false && num == 2) {
            //    G_DATA_MAPS.m_IsNewInfo = true;
            //}
        }

        G_DATA_MAPS.isNewMaps = isNewMaps;
	}

	
}

bool NewDataAll::newWarcraft(const HWND& hWndWindow)
{
    RECT rectClient;
    RECT rectWindow;
    if (GetClientRect(hWndWindow, &rectClient) && GetWindowRect(hWndWindow, &rectWindow)) {
        const int width = rectClient.right - rectClient.left;
        const int height = rectClient.bottom - rectClient.top;
        if (height < 300 || width < 300) {
            return false;
        }
    }

    if (!G_DATA_PATH.initializePaths(hWndWindow)) {
        return false;
    }
    ConfigMainEngine ConfigMainEngine_;
    ConfigMainEngine_.loadConfigMain();
    return true;
}

int NewDataAll::PutSaveCode(const std::wstring& path) {
    //if (G_CONFIG_MAPS.path == L"False")
    //    return 2;

    if (loadDatFail(path)) {
        std::wcout << "Loads1: " << G_CONFIG_MAPS.path << std::endl;
        return 1;
    }

    if (PathDirectorySaveCode(path)) {
        std::wcout << "Loads2: " << G_CONFIG_MAPS.path << std::endl;
        return 1;
    }

    //G_CONFIG_MAPS.path = L"False";
    return 0;
}

bool NewDataAll::loadDatFail(const std::wstring& fullPath) {
    ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
    ConfigMapsEngine_.loadConfigMain();

    if (!G_CONFIG_MAPS.path.empty() && std::filesystem::is_directory((fullPath + G_CONFIG_MAPS.path)))
        return true;
    return false;
}

std::wstring NewDataAll::removePath(const std::wstring& filePath) {
    size_t start_pos = filePath.find(L"CustomMapData");
    if (start_pos == std::wstring::npos) {
        std::cerr << "Substring 'CustomMapData' not found in the path." << std::endl;
        return L"";
    }
    start_pos += std::wstring(L"CustomMapData").length() + 1;

    size_t end_pos = filePath.find_last_of(L"\\");
    if (end_pos == std::wstring::npos) {
        std::cerr << "Last backslash not found in the path." << std::endl;
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