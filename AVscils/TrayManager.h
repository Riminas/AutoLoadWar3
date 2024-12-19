#pragma once
#include <windows.h>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

class TrayManager {
public:
    TrayManager(sf::Window& window);
    ~TrayManager();

    void initialize();
    void cleanup();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void setupTrayIcon();
    bool loadConfigData(const std::wstring& filename);
    void launchProgramWithAdminRights(const std::wstring& programPath) const;
    HICON loadIconFromFile(const std::wstring& filename) const;

    static constexpr UINT WM_TRAYICON = WM_USER + 1;
    static constexpr UINT ID_TRAY_APP_ICON = 5000;
    static constexpr UINT ID_TRAY_EXIT = 3000;
    static constexpr UINT ID_WARCRAFT_MENU_START = 5000;

    HWND m_hwnd;
    NOTIFYICONDATA m_nid{};
    std::map<std::wstring, std::wstring> m_buttonPaths;
    sf::Window& m_sfmlWindow;

    static TrayManager* s_instance; // Для доступа из статического WindowProc
};

