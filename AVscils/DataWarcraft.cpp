
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
#include "DataWarcraft.h"

DataWarcraft G_DATA_WARCRAFT;


bool DataWarcraft::initializeDataWarcraft(const HWND hWndWindow)
{
    if(!m_DataPath.initializeDataPath(hWndWindow))
        return false;
    if(!m_DataRect.initializeDataRect(hWndWindow))
        return false;

    return true;
}


bool DataWarcraft::DataPath::initializeDataPath(const HWND hWndWindow)
{
    const std::wstring pathWstr = openWarcraft3(hWndWindow);

    if (pathWstr.empty() || pathWstr == L"C:\\Windows\\explorer.exe")
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

        LogError().logMessageW(L"------------------------------------------------------------------------");

        warPathDirectMaps = str + L"\\Maps";
        LogError().logMessageW(L"Путь до папки с картами:(" + warPathDirectMaps + L")");
        warPathDirectSave = str + L"\\CustomMapData";
        LogError().logMessageW(L"путь до папки с сохранениями:" + warPathDirectSave + L")");
    }
    else if (versionWarcraft == 2) {
        LogError().logMessageW(L"------------------------------------------------------------------------");
        warPathDirectMaps = pathWar + L"\\Maps";
        LogError().logMessageW(L"Путь до папки с картами:(" + warPathDirectMaps + L")");
        warPathDirectSave = pathWar;
        LogError().logMessageW(L"путь до папки с сохранениями:" + warPathDirectSave + L")");
    }

    hWndWindowWar = hWndWindow;

    return true;
}

std::wstring DataWarcraft::DataPath::openWarcraft3(const HWND hWndWindow) {

    DWORD processId;
    GetWindowThreadProcessId(hWndWindow, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL) {
        std::wstring path;
        NewPathSaveCode NewPathSaveCode_;
        if (NewPathSaveCode_.newPathSaveCode(path)) {
            return path;
        }

        LogError().logError("неудалось открыть процесс: " + std::to_string(GetLastError()));
        return L"\0";
    }

    WCHAR processName[MAX_PATH];
    if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return processName;
    }
    else {
        LogError().logErrorW(L"Неудалось полчить имя процесса: " + std::to_wstring(GetLastError()));
    }

    CloseHandle(hProcess);
    return L"\0";
}



bool DataWarcraft::DataRect::initializeDataRect(const HWND hWndWindow)
{
    RECT rectClient;
    //RECT rectWindow;
    if(!GetClientRect(hWndWindow, &rectClient)/* || !GetWindowRect(hWndWindow, &rectWindow)*/)
        return false;

    size.x = rectClient.right - rectClient.left;
    size.y = rectClient.bottom - rectClient.top;

    if (size.x < 300 || size.y < 300)// а надо ли? вроде это чтобы окна ошибок варкрафта несщитались за окно игр аркрафта
        return false;

    position.x = rectClient.left;
    position.y = rectClient.top;


    center.x = size.x / 2;
    center.y = size.y / 2;

    return true;
}