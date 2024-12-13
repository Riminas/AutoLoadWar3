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
    // Получаем путь к запущенному процессу Warcraft
    const std::wstring pathWstr = openWarcraft3(hWndWindow);
    if (pathWstr.empty() || pathWstr == L"C:\\Windows\\explorer.exe")
        return false;

    // Определяем версию по имени исполняемого файла
    std::filesystem::path filePath(pathWstr);
    if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
        LogError().logError("Не удалось определить версию Warcraft");
        return false;
    }

    std::wstring filename = filePath.filename().wstring();
    if (filename == L"Warcraft III.exe") {
        LogError().logMessage("Версия Warcraft 1.36 " + filePath.string());
        versionWarcraft = 0;
    }
    else if (filename == L"war3.exe") {
        LogError().logMessage("Версия Warcraft 1.26 " + filePath.string());
        versionWarcraft = 1;
    }
    else {
        LogError().logError("Неизвестная версия Warcraft");
        versionWarcraft = -1;
        return false;
    }

    {//1.36
        // Инициализация пути к папке с картами
        PWSTR documentsPath = nullptr;
        if (FAILED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &documentsPath))) {
            LogError().logError("SHGetKnownFolderPath()");
            return false;
        }

        std::wstring basePath = documentsPath;
        CoTaskMemFree(documentsPath);
        basePath += L"\\Warcraft III";

        if (std::filesystem::exists(basePath)) {
            if(versionWarcraft == 0)
                warPathDirectMaps = basePath + L"\\Maps";
            warPathDirectSave[0] = basePath + L"\\Warcraft III\\CustomMapData";
        }
    }

    {//1.26
        if (versionWarcraft == 1) {
            if (!std::filesystem::exists(L"DataAutoLoad\\PathWar3.dat")) {
                std::wofstream pathFile(L"DataAutoLoad\\PathWar3.dat");
                pathFile << filePath.wstring();
                pathFile.close();
            }
            warPathDirectSave[1] = filePath.wstring();
            warPathDirectMaps += warPathDirectSave[1] + L"\\Maps";
        }
        else if (versionWarcraft == 0) {
            // Загружаем дополнительные пути из файла конфигурации
            std::wifstream pathFile(L"DataAutoLoad\\PathWar3.dat");
            if (pathFile.is_open()) {
                std::wstring line;
                std::getline(pathFile, line);
                if (!line.empty()) {
                    size_t pos = line.find(L"\\war3.exe");
                    if (pos != std::wstring::npos) {
                        line = line.substr(0, pos);
                    }
                    if (std::filesystem::exists(line)) {
                        warPathDirectSave[1] = line;
                    }
                }
                pathFile.close();
            }
        }
    }

    

    // Логируем все пути
    LogError().logMessageW(L"------------------------------------------------------------------------");
    LogError().logMessageW(L"Путь до папки с картами: (" + warPathDirectMaps + L")");
    LogError().logMessageW(L"Путь до папки с сохранениями 1.36: (" + warPathDirectSave[0] + L")");
    LogError().logMessageW(L"Путь до папки с сохранениями 1.26: (" + warPathDirectSave[1] + L")");

    hWndWindowWar = hWndWindow;
    return true;
}

std::wstring DataWarcraft::DataPath::openWarcraft3(const HWND hWndWindow) {

    DWORD processId = 0;
    GetWindowThreadProcessId(hWndWindow, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        std::wstring path;
        NewPathSaveCode newPathSaveCode_;
        if (newPathSaveCode_.newPathSaveCode(path)) {
            return path;
        }

        LogError().logError("Не удалось открыть процесс: " + std::to_string(GetLastError()));
        return L"";
    }

    WCHAR processName[MAX_PATH];
    if (GetModuleFileNameEx(hProcess, nullptr, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return processName;
    }
    else {
        LogError().logErrorW(L"Не удалось получить имя процесса: " + std::to_wstring(GetLastError()));
    }

    CloseHandle(hProcess);
    return L"";
}



bool DataWarcraft::DataRect::initializeDataRect(const HWND hWndWindow)
{
    RECT rectClient;
    //RECT rectWindow;
    if(!GetClientRect(hWndWindow, &rectClient)/* || !GetWindowRect(hWndWindow, &rectWindow)*/)
        return false;

    size.x = static_cast<float>(rectClient.right - rectClient.left);
    size.y = static_cast<float>(rectClient.bottom - rectClient.top);

    if (size.x < 300 || size.y < 300)// Проверка минимального размера окна
        return false;

    position.x = rectClient.left;
    position.y = rectClient.top;


    center.x = size.x / 2;
    center.y = size.y / 2;

    return true;
}