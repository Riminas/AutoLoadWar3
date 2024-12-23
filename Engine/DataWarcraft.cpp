#include "pch.h"
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

#include "LogManager.h"
#include "DataWarcraft.h"
#include "Global.h"

DataWarcraft G_DATA_WARCRAFT;


bool DataWarcraft::initializeDataWarcraft(const HWND hWndWindow)
{
    if(!m_DataPath.initializeDataPath(hWndWindow))
        return false;
    if(!m_DataRect.initializeDataRect(hWndWindow))
        return false;

    m_DataPath.hWndWindowWar = hWndWindow;
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
        LogManager::logger().log(LogManager::LogLevel::Error, "Не удалось определить версию Warcraft");
        return false;
    }

    std::wstring filename = filePath.filename().wstring();
    if (filename == L"Warcraft III.exe") {
        LogManager::logger().log(LogManager::LogLevel::Message, "Версия Warcraft 1.36 " + filePath.string());
        versionWarcraft = 0;
    }
    else if (filename == L"war3.exe") {
        LogManager::logger().log(LogManager::LogLevel::Message, "Версия Warcraft 1.26 " + filePath.string());
        versionWarcraft = 1;
    }
    else if (filename == L"Warcraft III.exe") {
        LogManager::logger().log(LogManager::LogLevel::Message, "Версия Warcraft 1.27b " + filePath.string());
        versionWarcraft = 1;
    }
    else if (filename == L"w3l.exe") {
        LogManager::logger().log(LogManager::LogLevel::Message, "Версия Warcraft 1.28. " + filePath.string());
        versionWarcraft = 1;
    }
    else {
        LogManager::logger().log(LogManager::LogLevel::Error, "Неизвестная версия Warcraft");
        versionWarcraft = -1;
        return false;
    }

    {//1.36
        // Инициализация пути к папке с картами
        PWSTR documentsPath = nullptr;
        if (FAILED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &documentsPath))) {
            LogManager::logger().log(LogManager::LogLevel::Error, "SHGetKnownFolderPath()");
            return false;
        }

        std::wstring basePath = documentsPath;
        CoTaskMemFree(documentsPath);
        basePath += L"\\Warcraft III";
        const std::wstring mapsPath = basePath + L"\\Maps";
        const std::wstring customMapPath = basePath + L"\\CustomMapData";

        if (std::filesystem::exists(basePath)) {
            if (versionWarcraft == 0 && std::filesystem::exists(mapsPath)) {
                warPathDirectMaps = mapsPath;
            }
            else {
                warPathDirectMaps.clear();
            }
            if (std::filesystem::exists(customMapPath)) {
                warPathDirectSave[0] = customMapPath;
            }
            else {
                warPathDirectSave[0].clear();
            }
        }
        else {
            warPathDirectMaps.clear();
            warPathDirectSave[0].clear();
        }
    }

    {//1.26
        if (versionWarcraft == 1) {
            size_t pos = pathWstr.find_last_of(L"\\");
            warPathDirectMaps = pathWstr.substr(0, pos) + L"\\Maps";
        }

        std::wstring fileContent = LoadDataFail().loadDataFailW(G_PATH_APP_DATA+L"\\PathWar3.txt");
        if (!fileContent.empty()) {
            std::wistringstream pathStream(std::move(fileContent));
            std::wstring line;
            int pathIndex = 1; // Начинаем с индекса 1
            while (std::getline(pathStream, line) && pathIndex < 10) {
                if (!line.empty()) {
                    static const std::wstring_view endings[] = { L"\\war3.exe", L"\\w3l.exe", L"\\Warcraft III.exe" };
                    size_t pos = std::wstring::npos;

                    for (const auto& ending : endings) {
                        if (size_t current = line.find(ending); current != std::wstring::npos) {
                            if (pos == std::wstring::npos || current < pos) {
                                pos = current;
                                if (pos == 0) break; // Выходим, если нашли в начале строки
                            }
                        }
                    }
                    /*size_t pos = line.find(L"\\war3.exe" || L"\\w3l.exe" || L"\\Warcraft III.exe");*/
                    if (pos != std::wstring::npos) {
                        line = line.substr(0, pos);
                    }
                    if (std::filesystem::exists(line)) {
                        if (pathIndex++ == 1)
                            warPathDirectSave[1] = line;
                        else
                            warPathDirectSave.push_back(line);
                    }
                }
            }
        }
    }

    

    // Логируем все пути
    LogManager::logger().log(LogManager::LogLevel::Message, L"------------------------------------------------------------------------");
    LogManager::logger().log(LogManager::LogLevel::Message, L"Путь до папки с картами: (" + warPathDirectMaps + L")");
    LogManager::logger().log(LogManager::LogLevel::Message, L"Путь до папки с сохранениями Ref: (" + warPathDirectSave[0] + L")");
    for (int i = 1; i < warPathDirectSave.size(); i++) {
        LogManager::logger().log(LogManager::LogLevel::Message, L"Путь до папки с сохранениями " + std::to_wstring(i) + L": (" + warPathDirectSave[i] + L")");
    }
    return true;
}

std::wstring DataWarcraft::DataPath::openWarcraft3(const HWND hWndWindow) {

    DWORD processId = 0;
    GetWindowThreadProcessId(hWndWindow, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        std::wstring path;
        LogManager::logger().log(LogManager::LogLevel::Error, "Не удалось открыть процесс: " + std::to_string(GetLastError()));

        NewPathSaveCode newPathSaveCode_;
        if (newPathSaveCode_.newPathSaveCode(path)) {
            return path;
        }

        return L"";
    }

    WCHAR processName[MAX_PATH];
    if (GetModuleFileNameEx(hProcess, nullptr, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return processName;
    }
    else {
        LogManager::logger().log(LogManager::LogLevel::Error, L"Не удалось получить имя процесса: " + std::to_wstring(GetLastError()));
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

    size.x = rectClient.right - rectClient.left;
    size.y = rectClient.bottom - rectClient.top;

    if (size.x < 300 || size.y < 300)// Проверка минимального размера окна
        return false;

    position.x = rectClient.left;
    position.y = rectClient.top;


    center.x = size.x / 2;
    center.y = size.y / 2;

    return true;
}