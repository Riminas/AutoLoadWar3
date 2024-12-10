
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

#include "LogError.h"
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
            LogError().logMessage("путь до 1.36 " + filePath.string());
            versionWarcraft = 1;
        }
        else if (filename == L"war3.exe") {
            LogError().logMessage("путь до 1.26 " + filePath.string());
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
            LogError().logError("SHGetKnownFolderPath()");
            return false;
        }

        std::wstring str = path;
        
        // Освобождаем память, выделенную SHGetKnownFolderPath
        CoTaskMemFree(path);

        str += L"\\Warcraft III";

        if (!std::filesystem::is_directory(str)) {
            LogError().logMessageW(L"directory Warcraft III(" + str + L")");
            return false;
        }

        warPathDirectMaps = str + L"\\Maps";
        LogError().logMessageW(L"Путь до папки с картами:(" + warPathDirectMaps + L")");
        warPathDirectSave = str + L"\\CustomMapData";
        LogError().logMessageW(L"путь до папки с сохранениями:" + warPathDirectSave + L")");
    }
    else if (versionWarcraft == 2) {
        warPathDirectMaps = pathWar + L"\\Maps";
        LogError().logMessageW(L"Путь до папки с картами:(" + warPathDirectMaps + L")");
        warPathDirectSave = pathWar;
        LogError().logMessageW(L"путь до папки с сохранениями:" + warPathDirectSave + L")");
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

        LogError().logError("неудалось открыть процесс");
        return L"\0";
    }

    WCHAR processName[MAX_PATH];
    if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
        LogError().logMessageW(L"Process name: " + std::wstring(processName));
        return processName;
    }
    else {
        LogError().logErrorW(L"Неудалось полчить имя процесса");
    }

    CloseHandle(hProcess);
    return L"\0";
}