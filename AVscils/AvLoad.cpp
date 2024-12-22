#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include "Engine.h"
#include "LogManager.h"
#include "ConfigMainEngine.h"
#include "Global.h"

// mainCRTStartup
int main()
{
    if (FindWindow(NULL, L"AutoLoadWar3") == NULL) {
        //G_PATH_APP_DATA = L"DataAutoLoad\\";
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
            G_PATH_APP_DATA = std::wstring(path) + L"\\DataAutoLoad\\";
        }
        else {
            LogManager::logger().log(LogManager::LogLevel::Error, L"Не удалось получить путь к AppData.");
            return -1;
        }

        // Проверка наличия папки DataAutoLoad
        DWORD attrib = GetFileAttributesW(G_PATH_APP_DATA.c_str());
        if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            if (!CreateDirectoryW(G_PATH_APP_DATA.c_str(), NULL)) {
                LogManager::logger().log(LogManager::LogLevel::Error, L"Не удалось создать папку DataAutoLoad.");
                return -1;
            }
            else {
                LogManager::logger().log(LogManager::LogLevel::Message, L"Папка DataAutoLoad успешно создана.");
            }
        }

        ConfigMainEngine().loadConfigMain(); // должно выполняться только 1 раз при старте программы

        LogManager::logger().log(LogManager::LogLevel::Message, L"\n\n------------------------------------------------------------------------");
        Engine().engine1();
    }
    else
        MessageBox(NULL, L"Программа уже запущена.", L"Оповещение", MB_OK | MB_ICONEXCLAMATION);
    
    return 0;
}