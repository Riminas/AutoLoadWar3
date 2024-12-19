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

    // �������� ���� � ����������� �������� Warcraft
    const std::wstring pathWstr = openWarcraft3(hWndWindow);
    if (pathWstr.empty() || pathWstr == L"C:\\Windows\\explorer.exe")
        return false;

    // ���������� ������ �� ����� ������������ �����
    std::filesystem::path filePath(pathWstr);
    if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
        LogManager::logger().log(LogManager::LogLevel::Error, "�� ������� ���������� ������ Warcraft");
        return false;
    }

    std::wstring filename = filePath.filename().wstring();
    if (filename == L"Warcraft III.exe") {
        LogManager::logger().log(LogManager::LogLevel::Message, "������ Warcraft 1.36 " + filePath.string());
        versionWarcraft = 0;
    }
    else if (filename == L"war3.exe") {
        LogManager::logger().log(LogManager::LogLevel::Message, "������ Warcraft 1.26 " + filePath.string());
        versionWarcraft = 1;
    }
    else {
        LogManager::logger().log(LogManager::LogLevel::Error, "����������� ������ Warcraft");
        versionWarcraft = -1;
        return false;
    }

    {//1.36
        // ������������� ���� � ����� � �������
        PWSTR documentsPath = nullptr;
        if (FAILED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &documentsPath))) {
            LogManager::logger().log(LogManager::LogLevel::Error, "SHGetKnownFolderPath()");
            return false;
        }

        std::wstring basePath = documentsPath;
        CoTaskMemFree(documentsPath);
        basePath += L"\\Warcraft III";

        if (std::filesystem::exists(basePath)) {
            if(versionWarcraft == 0)
                warPathDirectMaps = basePath + L"\\Maps";
            warPathDirectSave[0] = basePath + L"\\CustomMapData";
        }
    }

    {//1.26
        size_t pos = pathWstr.find_last_of(L"\\");
        warPathDirectMaps = pathWstr.substr(0, pos) + L"\\Maps";

        std::wstring fileContent = LoadDataFail().loadDataFailW(L"DataAutoLoad\\PathWar3.txt");
        if (!fileContent.empty()) {
            std::wistringstream pathStream(std::move(fileContent));
            std::wstring line;
            int pathIndex = 1; // �������� � ������� 1
            while (std::getline(pathStream, line) && pathIndex < 10) {
                if (!line.empty()) {
                    size_t pos = line.find(L"\\war3.exe");
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

    

    // �������� ��� ����
    LogManager::logger().log(LogManager::LogLevel::Message, L"------------------------------------------------------------------------");
    LogManager::logger().log(LogManager::LogLevel::Message, L"���� �� ����� � �������: (" + warPathDirectMaps + L")");
    LogManager::logger().log(LogManager::LogLevel::Message, L"���� �� ����� � ������������ Ref: (" + warPathDirectSave[0] + L")");
    for (int i = 1; i < warPathDirectSave.size(); i++) {
        LogManager::logger().log(LogManager::LogLevel::Message, L"���� �� ����� � ������������ " + std::to_wstring(i) + L": (" + warPathDirectSave[i] + L")");
    }
    return true;
}

std::wstring DataWarcraft::DataPath::openWarcraft3(const HWND hWndWindow) {

    DWORD processId = 0;
    GetWindowThreadProcessId(hWndWindow, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        std::wstring path;
        LogManager::logger().log(LogManager::LogLevel::Error, "�� ������� ������� �������: " + std::to_string(GetLastError()));

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
        LogManager::logger().log(LogManager::LogLevel::Error, L"�� ������� �������� ��� ��������: " + std::to_wstring(GetLastError()));
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

    if (size.x < 300 || size.y < 300)// �������� ������������ ������� ����
        return false;

    position.x = rectClient.left;
    position.y = rectClient.top;


    center.x = size.x / 2;
    center.y = size.y / 2;

    return true;
}