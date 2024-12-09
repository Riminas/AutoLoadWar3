#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include "AutoUpdate.h"

bool AutoUpdate::CheckForUpdates(const std::wstring& serverUrl, const std::wstring& currentVersion)
{
    bool updateAvailable = false;

    // Создаем сессию WinHTTP
    HINTERNET hSession = WinHttpOpen(L"Updater/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession) {
        URL_COMPONENTS urlComp;
        ZeroMemory(&urlComp, sizeof(urlComp));
        urlComp.dwStructSize = sizeof(urlComp);

        // Устанавливаем размеры буферов
        wchar_t hostName[256];
        wchar_t urlPath[256];
        urlComp.lpszHostName = hostName;
        urlComp.dwHostNameLength = _countof(hostName);
        urlComp.lpszUrlPath = urlPath;
        urlComp.dwUrlPathLength = _countof(urlPath);

        // Разбираем URL
        if (WinHttpCrackUrl(serverUrl.c_str(), (DWORD)serverUrl.length(), 0, &urlComp)) {
            // Открываем соединение
            HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
                urlComp.nPort, 0);
            if (hConnect) {
                // Создаем запрос
                HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
                    urlComp.lpszUrlPath,
                    NULL, WINHTTP_NO_REFERER,
                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                    (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
                if (hRequest) {
                    // Отправляем запрос
                    BOOL bResult = WinHttpSendRequest(hRequest,
                        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                        WINHTTP_NO_REQUEST_DATA, 0,
                        0, 0);
                    if (bResult) {
                        bResult = WinHttpReceiveResponse(hRequest, NULL);
                        if (bResult) {
                            // Читаем данные
                            DWORD dwSize = 0;
                            WinHttpQueryDataAvailable(hRequest, &dwSize);
                            if (dwSize > 0) {
                                wchar_t* buffer = new wchar_t[dwSize / sizeof(wchar_t) + 1];
                                ZeroMemory(buffer, dwSize + sizeof(wchar_t));
                                DWORD dwDownloaded = 0;
                                WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded);

                                std::wstring latestVersion(buffer);
                                delete[] buffer;

                                // Сравниваем версии
                                if (latestVersion != currentVersion) {
                                    updateAvailable = true;
                                }
                            }
                        }
                    }
                    WinHttpCloseHandle(hRequest);
                }
                WinHttpCloseHandle(hConnect);
            }
        }
        WinHttpCloseHandle(hSession);
    }

    return updateAvailable;

}

bool AutoUpdate::DownloadUpdate(const std::wstring& downloadUrl, const std::wstring& savePath)
{
    bool downloadSuccess = false;

    // Создаем сессию WinHTTP
    HINTERNET hSession = WinHttpOpen(L"Updater/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession) {
        // Разбираем URL
        URL_COMPONENTS urlComp;
        ZeroMemory(&urlComp, sizeof(urlComp));
        urlComp.dwStructSize = sizeof(urlComp);

        wchar_t hostName[256];
        wchar_t urlPath[256];
        urlComp.lpszHostName = hostName;
        urlComp.dwHostNameLength = _countof(hostName);
        urlComp.lpszUrlPath = urlPath;
        urlComp.dwUrlPathLength = _countof(urlPath);

        if (WinHttpCrackUrl(downloadUrl.c_str(), (DWORD)downloadUrl.length(), 0, &urlComp)) {
            // Открываем соединение
            HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
                urlComp.nPort, 0);
            if (hConnect) {
                // Создаем запрос
                HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
                    urlComp.lpszUrlPath,
                    NULL, WINHTTP_NO_REFERER,
                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                    (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
                if (hRequest) {
                    // Отправляем запрос
                    BOOL bResult = WinHttpSendRequest(hRequest,
                        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                        WINHTTP_NO_REQUEST_DATA, 0,
                        0, 0);
                    if (bResult) {
                        bResult = WinHttpReceiveResponse(hRequest, NULL);
                        if (bResult) {
                            HANDLE hFile = CreateFile(savePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                            if (hFile != INVALID_HANDLE_VALUE) {
                                DWORD dwSize = 0;
                                do {
                                    dwSize = 0;
                                    if (WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                                        BYTE* buffer = new BYTE[dwSize];
                                        ZeroMemory(buffer, dwSize);
                                        DWORD dwDownloaded = 0;
                                        if (WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded)) {
                                            DWORD dwWritten = 0;
                                            WriteFile(hFile, buffer, dwDownloaded, &dwWritten, NULL);
                                        }
                                        delete[] buffer;
                                    }
                                } while (dwSize > 0);
                                CloseHandle(hFile);
                                downloadSuccess = true;
                            }
                        }
                    }
                    WinHttpCloseHandle(hRequest);
                }
                WinHttpCloseHandle(hConnect);
            }
        }
        WinHttpCloseHandle(hSession);
    }

    return downloadSuccess;
}
