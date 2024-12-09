#include <iostream>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <unordered_map>
#include <Windows.h>
#include <ShlObj_core.h>
#include <unordered_set>
#include <shlobj.h>
#include <locale>
#include <codecvt>
#include <psapi.h>

#include "StringToString.h"
#include "StringConvector.h"
#include "NewPathSaveCode.h"
#include "LoadDataFail.h"
#include "ConfigMain.h"

#include "DataPath.h"

DataPath G_DATA_PATH;


bool DataPath::initializePaths(const HWND hWndWindow) {
    const std::wstring pathWstr = openWarcraft3(hWndWindow);

    if(pathWstr.empty() || pathWstr == L"C:\\Windows\\explorer.exe")
        return false;
    

    std::wstring pathWar = L"\0";
    std::filesystem::path filePath(pathWstr);
    if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)) {
        std::wstring filename = filePath.filename().wstring();

        if (filename == L"Warcraft III.exe") {
            std::wcout << L"Found: " << filePath << std::endl;
            versionWarcraft = 1;
        }
        else if (filename == L"war3.exe") {
            std::wcout << L"Found: " << filePath << std::endl;
            versionWarcraft = 2;
            pathWar = filePath.parent_path().wstring();
        }
        else {
            versionWarcraft = 0;
        }
    }

    if (versionWarcraft == 1) {
        PWSTR path = NULL;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path);
        if (!SUCCEEDED(hr)) {
            std::cout << "Error: SHGetKnownFolderPath()" << std::endl;
            return false;
        }

        std::wstring str = path;
        
        // Освобождаем память, выделенную SHGetKnownFolderPath
        CoTaskMemFree(path);

        str += L"\\Warcraft III";

        if (!std::filesystem::is_directory(str)) {
            std::wcout << "Error: directory Warcraft III(" << str << ")" << std::endl;
            return false;
        }

        warPathDirectMaps = str + L"\\Maps";
        std::wcout << "Warcraft Maps Path: " << warPathDirectMaps << std::endl;
        warPathDirectSave = str + L"\\CustomMapData";
        std::wcout << "Warcraft Save Path: " << warPathDirectSave << std::endl;
    }
    else if (versionWarcraft == 2) {

        warPathDirectMaps = pathWar + L"\\Maps";
        std::wcout << "Warcraft Maps Path: " << warPathDirectMaps << std::endl;
        warPathDirectSave = pathWar;
        std::wcout << "Warcraft Save Path: " << warPathDirectSave << std::endl;
    }

    hWndWindowWar = hWndWindow;

    return true;
}

std::wstring DataPath::openWarcraft3(const HWND hWndWindow) {

    DWORD processId;
    GetWindowThreadProcessId(hWndWindow, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL) {
        std::wstring path;
        NewPathSaveCode NewPathSaveCode_;
        if (NewPathSaveCode_.newPathSaveCode(path)) {
            return path;
        }

        std::wcout << L"Unable to open process!" << std::endl;
        return L"\0";
    }

    WCHAR processName[MAX_PATH];
    if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
        std::wcout << L"Process name: " << processName << std::endl;
        return processName;
    }
    else {
        std::wcout << L"Unable to get process name!" << std::endl;
    }

    CloseHandle(hProcess);
    return L"\0";
}