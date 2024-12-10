#include "AutoUpdate.h"
#include <windows.h>
#include <winhttp.h>
#include <string>


#include "LogError.h"
#include "json.hpp"
using json = nlohmann::json;

#pragma comment(lib, "winhttp.lib")

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
                // Установите заголовок User-Agent, иначе GitHub API может вернуть ошибку
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
//    // Создаем сессию WinHTTP
//    HINTERNET hSession = WinHttpOpen(L"Updater/1.0",
//        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
//        WINHTTP_NO_PROXY_NAME,
//        WINHTTP_NO_PROXY_BYPASS, 0);
//    if (hSession) {
//        // Разбираем URL
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
//            // Открываем соединение
//            HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
//                urlComp.nPort, 0);
//            if (hConnect) {
//                // Создаем запрос
//                HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
//                    urlComp.lpszUrlPath,
//                    NULL, WINHTTP_NO_REFERER,
//                    WINHTTP_DEFAULT_ACCEPT_TYPES,
//                    (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
//                if (hRequest) {
//                    // Отправляем запрос
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
        LogError().logError("hSession");
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
        LogError().logError("hConnect");
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
        LogError().logError("hRequest");
        return false;
    }

    // Установите заголовок User-Agent
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
                LogError().logError("INVALID_HANDLE_VALUE");
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