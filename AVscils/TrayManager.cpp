#include "TrayManager.h"
#include "StringConvector.h"
#include "LogManager.h"
#include <fstream>
#include <shellapi.h>
#include "Global.h"

TrayManager* TrayManager::s_instance = nullptr;

TrayManager::TrayManager(sf::Window& window) 
    : m_sfmlWindow(window) {
    s_instance = this;
}

TrayManager::~TrayManager() {
    cleanup();
    s_instance = nullptr;
}

void TrayManager::initialize() {
    // Регистрация класса окна
    std::wstring className = L"AutoLoad_Tree";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className.c_str();
    
    if (!RegisterClassEx(&wc)) {
        LogManager::logger().log(LogManager::LogLevel::Error, L"Failed to register window class");
        return;
    }

    // Создание скрытого окна
    m_hwnd = CreateWindow(className.c_str(), L"AutoLoad Tree",
        WS_OVERLAPPEDWINDOW, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, wc.hInstance, NULL);

    if (!m_hwnd) {
        LogManager::logger().log(LogManager::LogLevel::Error, L"Failed to create window");
        return;
    }

    // Загрузка конфигурации
    if (!loadConfigData(G_PATH_APP_DATA+L"StartPath.ini")) {
        m_buttonPaths.clear();
    }

    setupTrayIcon();
}

void TrayManager::cleanup() {
    if (m_nid.cbSize) {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
        ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));
    }
    
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

void TrayManager::setupTrayIcon() {
    ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = ID_TRAY_APP_ICON;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon = loadIconFromFile(L"DataAutoLoad\\img\\icon.ico");
    wcscpy_s(m_nid.szTip, L"AutoLoad tree");
    
    if (!Shell_NotifyIcon(NIM_ADD, &m_nid)) {
        LogManager::logger().log(LogManager::LogLevel::Error, L"Failed to create tray icon");
    }
}

LRESULT CALLBACK TrayManager::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (s_instance) {
        switch (uMsg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);

                HMENU hMenu = CreatePopupMenu();
                if (!s_instance->m_buttonPaths.empty()) {
                    HMENU hWarcraftMenu = CreatePopupMenu();
                    int buttonId = ID_WARCRAFT_MENU_START;
                    
                    for (const auto& [buttonName, programPath] : s_instance->m_buttonPaths) {
                        AppendMenu(hWarcraftMenu, MF_STRING, buttonId++, buttonName.c_str());
                    }
                    
                    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hWarcraftMenu, L"Warcraft");
                    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                }
                
                AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                    pt.x, pt.y + 35, 0, hwnd, NULL);
                
                PostMessage(hwnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                s_instance->cleanup();
                s_instance->m_sfmlWindow.close();
            }
            else if (LOWORD(wParam) >= ID_WARCRAFT_MENU_START) {
                int buttonIndex = LOWORD(wParam) - ID_WARCRAFT_MENU_START;
                if (buttonIndex >= 0 && buttonIndex < s_instance->m_buttonPaths.size()) {
                    auto it = std::next(s_instance->m_buttonPaths.begin(), buttonIndex);
                    s_instance->launchProgramWithAdminRights(it->second);
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            s_instance->m_sfmlWindow.close();
            break;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool TrayManager::loadConfigData(const std::wstring& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();

    StringConvector converter;
    std::wstring wcontent = converter.utf8_to_utf16(content);

    if (wcontent[wcontent.size() - 1] != L'\n') {
        wcontent += L'\n';
    }

    size_t pos = 0;
    while ((pos = wcontent.find(L'\n')) != std::wstring::npos) {
        std::wstring line = wcontent.substr(0, pos);
        wcontent.erase(0, pos + 1);

        size_t bracketPos = line.find(L']');
        if (bracketPos != std::wstring::npos) {
            std::wstring buttonName = line.substr(1, bracketPos - 1);
            std::wstring programPath = line.substr(bracketPos + 2);
            
            if (!programPath.empty()) {
                if (programPath.back() == L'\r') {
                    programPath.pop_back();
                }
                if (!iswalpha(programPath[0])) {
                    programPath.erase(0, 1);
                }
                m_buttonPaths[buttonName] = programPath;
            }
        }
    }

    return !m_buttonPaths.empty();
}

void TrayManager::launchProgramWithAdminRights(const std::wstring& programPath) const {
    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas";
    sei.lpFile = programPath.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        if (GetLastError() == ERROR_CANCELLED) {
            MessageBox(NULL, L"Операция отменена пользователем.", L"", MB_OK | MB_ICONINFORMATION);
        }
    }
}

HICON TrayManager::loadIconFromFile(const std::wstring& filename) const {
    return (HICON)LoadImage(NULL, filename.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}
