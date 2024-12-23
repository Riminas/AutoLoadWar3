#include "AutoUpdate.h"
#include <windows.h>
#include <shlobj.h>
#include <winhttp.h>
#include <string>
#include <filesystem>
#include <fstream>

#include "json.hpp"
using json = nlohmann::json;

#pragma comment(lib, "winhttp.lib")

void AutoUpdate::autoUpdate1()
{
    if (!isUpdate())
        return;

    const std::wstring currentVersion = getCurrentVersionFromFile(); // Получаем версию из файла

    
    // Получаем информацию о последнем релизе
    std::string releaseInfo = GetLatestReleaseInfo(L"Riminas", L"AutoLoadWar3");
    
    try {
        // Парсим JSON ответ
        json release = json::parse(releaseInfo);
        
        // Проверяем наличие тега версии
        if (!release.contains("tag_name")) {
            MessageBoxA(nullptr, "No version tag found in release", 
                       "Error", MB_OK | MB_ICONERROR);
            return;
        }
        
        // Получаем версию как строку
        std::string latestVersion = release["tag_name"].get<std::string>();
        std::wstring wlatestVersion(latestVersion.begin(), latestVersion.end());
        
        // Проверяем assets
        if (!release.contains("assets") || !release["assets"].is_array() || release["assets"].empty()) {
            std::string debugInfo = "Received JSON: " + releaseInfo;
            MessageBoxA(nullptr, debugInfo.c_str(),
                       "Debug Info", MB_OK | MB_ICONINFORMATION);
            return;
        }
        
        // Добавляем отладочный вывод

        if (!release.contains("assets") || !release["assets"].is_array()) {
            std::string debugInfo = "Received JSON: " + releaseInfo;
            MessageBoxA(nullptr, debugInfo.c_str(),
                "Debug Info", MB_OK | MB_ICONINFORMATION);
            return;
        }
        
        // Сравниваем версии
        if (wlatestVersion != currentVersion) {
            {
                std::wstring wstr = L"Update. your " + currentVersion + L"\n  new" + wlatestVersion;
                MessageBoxW(nullptr, wstr.c_str(),
                    L"Update", MB_OK | MB_ICONINFORMATION);
            }

            // Получаем URL для скачивания Engine.dll
            std::string downloadUrl;
            for (const auto& asset : release["assets"]) {
                if (asset["name"] == "Engine.dll") {
                    downloadUrl = asset["browser_download_url"];
                    break;
                }
            }
            
            if (!downloadUrl.empty()) {
                const std::wstring enginePath = L"Engine.dll";
                
                // Пробуем удалить существующий файл
                if (std::filesystem::exists(enginePath)) {
                    try {
                        std::filesystem::remove(enginePath);
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        MessageBoxA(nullptr, "Couldn't delete the old version", 
                                  "Error Update", MB_OK | MB_ICONERROR);
                        return;
                    }
                }
                
                // Скачиваем новую версию
                if (DownloadFile(downloadUrl, enginePath)) {
                    saveCurrentVersion(wlatestVersion); // Сохраняем новую версию
                    MessageBoxW(nullptr, L"Successfully updated", 
                              L"Update", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    MessageBoxW(nullptr, L"Couldn't download the new version", 
                              L"Error Update", MB_OK | MB_ICONERROR);
                }
            }
        }
    }
    catch (const json::parse_error& e) {
        MessageBoxA(nullptr, e.what(),
                   "Error when parsing version information", MB_OK | MB_ICONERROR);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Update error", MB_OK | MB_ICONERROR);
    }
}

bool AutoUpdate::isUpdate() {
    {
        //G_PATH_APP_DATA = L"DataAutoLoad\\";
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
            m_PathAppData = std::wstring(path) + L"\\DataAutoLoad\\";
        }
        else {
            return true;
        }

        // Проверка наличия папки DataAutoLoad
        DWORD attrib = GetFileAttributesW(m_PathAppData.c_str());
        if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            if (!CreateDirectoryW(m_PathAppData.c_str(), NULL)) {
                return true;
            }
        }
    }
    {
        const std::filesystem::path m_FilePath{ m_PathAppData + L"DataAutoLoad.ini" };
        
        // Проверяем существование файла
        if (!std::filesystem::exists(m_FilePath)) {
            return true; // Если файл не существует, разрешаем обновление
        }

        // Читаем файл
        std::wifstream file(m_FilePath);
        if (!file.is_open()) {
            return true;
        }

        std::wstring line;
        while (std::getline(file, line)) {
            if (line.find(L"autoUpdate=") == 0) {
                std::wstring value = line.substr(11); // Длина "autoUpdate="
                return (value == L"true");
            }
        }
        return true; // Если параметр не найден, разрешаем обновление
    }
}

std::string AutoUpdate::GetLatestReleaseInfo(const std::wstring& owner, const std::wstring& repo)
{
    HINTERNET hSession = WinHttpOpen(L"MyApp/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    std::string result;

    if (hSession)
    {
        HINTERNET hConnect = WinHttpConnect(hSession, L"api.github.com",
            INTERNET_DEFAULT_HTTPS_PORT, 0);

        if (hConnect)
        {
            std::wstring path = L"/repos/" + owner + L"/" + repo + L"/releases/latest";

            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
                NULL, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES,
                WINHTTP_FLAG_SECURE);

            if (hRequest)
            {
                // ���������� ��������� User-Agent, ����� GitHub API ����� ������� ������
                const wchar_t* szHeaders = L"User-Agent: MyApp\r\n";
                BOOL bResult = WinHttpSendRequest(hRequest, szHeaders, -1L,
                    WINHTTP_NO_REQUEST_DATA, 0,
                    0, 0);

                if (bResult)
                {
                    bResult = WinHttpReceiveResponse(hRequest, NULL);

                    if (bResult)
                    {
                        DWORD dwSize = 0;
                        do
                        {
                            DWORD dwDownloaded = 0;
                            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                                break;

                            if (dwSize == 0)
                                break;

                            char* buffer = new char[dwSize + 1];
                            ZeroMemory(buffer, dwSize + 1);

                            if (!WinHttpReadData(hRequest, (LPVOID)buffer,
                                dwSize, &dwDownloaded))
                            {
                                delete[] buffer;
                                break;
                            }

                            result.append(buffer, dwDownloaded);
                            delete[] buffer;

                        } while (dwSize > 0);
                    }
                }
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }
    return result;
}

//
//bool AutoUpdate::DownloadUpdate(const std::wstring& downloadUrl, const std::wstring& savePath)
//{
//    bool downloadSuccess = false;
//
//    // ������� ������ WinHTTP
//    HINTERNET hSession = WinHttpOpen(L"Updater/1.0",
//        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
//        WINHTTP_NO_PROXY_NAME,
//        WINHTTP_NO_PROXY_BYPASS, 0);
//    if (hSession) {
//        // ��������� URL
//        URL_COMPONENTS urlComp;
//        ZeroMemory(&urlComp, sizeof(urlComp));
//        urlComp.dwStructSize = sizeof(urlComp);
//
//        wchar_t hostName[256];
//        wchar_t urlPath[256];
//        urlComp.lpszHostName = hostName;
//        urlComp.dwHostNameLength = _countof(hostName);
//        urlComp.lpszUrlPath = urlPath;
//        urlComp.dwUrlPathLength = _countof(urlPath);
//
//        if (WinHttpCrackUrl(downloadUrl.c_str(), (DWORD)downloadUrl.length(), 0, &urlComp)) {
//            // ��������� ����������
//            HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
//                urlComp.nPort, 0);
//            if (hConnect) {
//                // ������� ������
//                HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
//                    urlComp.lpszUrlPath,
//                    NULL, WINHTTP_NO_REFERER,
//                    WINHTTP_DEFAULT_ACCEPT_TYPES,
//                    (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
//                if (hRequest) {
//                    // ���������� ������
//                    BOOL bResult = WinHttpSendRequest(hRequest,
//                        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
//                        WINHTTP_NO_REQUEST_DATA, 0,
//                        0, 0);
//                    if (bResult) {
//                        bResult = WinHttpReceiveResponse(hRequest, NULL);
//                        if (bResult) {
//                            HANDLE hFile = CreateFile(savePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//                            if (hFile != INVALID_HANDLE_VALUE) {
//                                DWORD dwSize = 0;
//                                do {
//                                    dwSize = 0;
//                                    if (WinHttpQueryDataAvailable(hRequest, &dwSize)) {
//                                        BYTE* buffer = new BYTE[dwSize];
//                                        ZeroMemory(buffer, dwSize);
//                                        DWORD dwDownloaded = 0;
//                                        if (WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded)) {
//                                            DWORD dwWritten = 0;
//                                            WriteFile(hFile, buffer, dwDownloaded, &dwWritten, NULL);
//                                        }
//                                        delete[] buffer;
//                                    }
//                                } while (dwSize > 0);
//                                CloseHandle(hFile);
//                                downloadSuccess = true;
//                            }
//                        }
//                    }
//                    WinHttpCloseHandle(hRequest);
//                }
//                WinHttpCloseHandle(hConnect);
//            }
//        }
//        WinHttpCloseHandle(hSession);
//    }
//
//    return downloadSuccess;
//}

bool AutoUpdate::DownloadFile(const std::string& url, const std::wstring& savePath)
{
    HINTERNET hSession = WinHttpOpen(L"MyApp/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        MessageBoxA(nullptr, "hSession", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    URL_COMPONENTS urlComp;
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);

    wchar_t hostName[256];
    wchar_t urlPath[1024];
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = _countof(hostName);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = _countof(urlPath);

    std::wstring wideUrl = std::wstring(url.begin(), url.end());
    WinHttpCrackUrl(wideUrl.c_str(), 0, 0, &urlComp);

    HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
        urlComp.nPort, 0);

    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        MessageBoxA(nullptr, "hConnect", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    DWORD dwFlags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath,
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        dwFlags);

    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        MessageBoxA(nullptr, "hRequest", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // ���������� ��������� User-Agent
    const wchar_t* szHeaders = L"User-Agent: MyApp\r\n";
    BOOL bResult = WinHttpSendRequest(hRequest, szHeaders, -1L,
        WINHTTP_NO_REQUEST_DATA, 0,
        0, 0);

    if (bResult)
    {
        bResult = WinHttpReceiveResponse(hRequest, NULL);

        if (bResult)
        {
            HANDLE hFile = CreateFileW(savePath.c_str(), GENERIC_WRITE, 0, NULL,
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                MessageBoxA(nullptr, "INVALID_HANDLE_VALUE", "Error", MB_OK | MB_ICONERROR);
                return false;
            }

            DWORD dwSize = 0;
            do
            {
                DWORD dwDownloaded = 0;
                BYTE buffer[8192];

                if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                    break;

                if (dwSize == 0)
                    break;

                if (!WinHttpReadData(hRequest, buffer, min(dwSize, sizeof(buffer)), &dwDownloaded))
                    break;

                DWORD dwWritten = 0;
                WriteFile(hFile, buffer, dwDownloaded, &dwWritten, NULL);

            } while (dwSize > 0);

            CloseHandle(hFile);
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return bResult == TRUE;
}

// Добавляем новые функции для работы с версией
std::wstring AutoUpdate::getCurrentVersionFromFile() const {
    std::wstring versionPath = m_PathAppData + L"version.txt";

    // Проверяем существование файла
    if (!std::filesystem::exists(versionPath)) {
        std::wofstream file(versionPath);
        if (file.is_open()) {
            file << L"v1_19_1";
            file.close();
        }
        return L"v1_19_1"; // Возвращаем дефолтную версию если файл не существует
    }

    // Читаем версию из файла
    std::wifstream file(versionPath);
    if (!file.is_open()) {
        return L"v1_19_1";
    }

    std::wstring version;
    std::getline(file, version);
    file.close();
    
    return version.empty() ? L"v1_19_1" : version;
}

void AutoUpdate::saveCurrentVersion(const std::wstring& version) const {
    std::wstring versionPath = m_PathAppData + L"version.txt";

    // Записываем новую версию в файл
    std::wofstream file(versionPath);
    if (file.is_open()) {
        file << version;
        file.close();
    }
}