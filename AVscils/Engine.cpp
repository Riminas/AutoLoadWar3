#include <windows.h>
#include <SFML/Graphics.hpp>
#include <wtypes.h>

#include <TlHelp32.h>
#include <thread>
#include <fstream>
#include <unordered_map>
#include <chrono>

#include "Engine.h"
#include "Global.h"
#include "LoadManager.h"
#include "SkillsUpgradeStart.h"
#include "key.h"
#include "StringConvector.h"
#include "ConfigMain.h"
#include "ConfigMapsEngine.h"
#include "FontLoader.h"
#include "UpdateRegionRect.h"

NOTIFYICONDATA nid;

void Engine::engine1() {
    if (initialize())
        return;

    HWND hWndWindow = nullptr;
    while (G_WINDOW.isOpen()) {
        hWndWindow = GetForegroundWindow();

        if (hWndWindow != nullptr && updateWindowVisibility(hWndWindow)) {
            processEvents();//       
            draw();//
        }
        else {
            sf::Event event;
            while (G_WINDOW.pollEvent(event)) if (event.type == sf::Event::Closed) G_WINDOW.close();

            if (G_CONFIG_MAIN.optionsConfig.blackColor) G_WINDOW.clear(sf::Color(45, 45, 48));
            else G_WINDOW.clear(sf::Color(255, 255, 255));
            G_WINDOW.display();
        }
    }

     //     
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

bool Engine::updateWindowVisibility(const HWND& hWndWindow)
{
    if (IsWarcraftInFocus(hWndWindow)) {//      Warcraft III
        if (m_IsVisibleOwnerWindow) return true;

        if (G_DATA_WARCRAFT.m_DataPath.hWndWindowWar != hWndWindow) {//        
            //       
            if (!NewDataAll().newWarcraft(hWndWindow)) {
                G_WINDOW.setVisible(false);//   ,   
                m_IsVisibleOwnerWindow = false;
                return false;
            }
            G_WINDOW.setVisible(true);
            m_OwnerWindow.updateRect(hWndWindow);
            if (G_BOOL_VISIBLE.isVisibleEngineFile)
                m_EngineFileTip2.updateRect(hWndWindow, false);
        }
        //G_DATA_ALL.isMapsStart = true;
        NewDataAll().newMaps();//        

        //     
        m_OwnerWindow.activeGameTrue(hWndWindow);

        {//  updateWindowVisibility(Visibility)
            // ,      
            if (G_DATA_MAPS.m_IsNewInfo) {
                //    ,        
                G_BOOL_VISIBLE.isVisibleEngineFile = false;
                G_DATA_MAPS.m_IsNewInfo = false;
                G_CONFIG_MAPS.path = G_CONFIG_MAPS.lastPath;
            }
        }
        //     
        m_IsVisibleOwnerWindow = true;

        return true;
    }
    else {
        if (!m_IsVisibleOwnerWindow) return false;

        m_OwnerWindow.activeGameFalse();
        m_IsVisibleOwnerWindow = false;

        return false;
    }
    
}

//bool Engine::IsWindowInFocus(const HWND& hWnd) {
//    if (!hWnd) {
//        return false;
//    }
//
//    wchar_t windowTitle[256];
//
//    //          "Warcraft III"
//    if (GetWindowTextW(hWnd, windowTitle, (int)std::size(windowTitle)) > 0 &&
//        wcscmp(windowTitle, L"Warcraft III") == 0) {
//        return true;
//    }
//
//    return false;
//}
bool Engine::IsWarcraftInFocus(const HWND& hWnd) {
    wchar_t windowTitle[256];

    //   
    int length = GetWindowText(hWnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
    if (length == 0) {
        return 0;
    }

    //     
    if (wcscmp(windowTitle, L"Warcraft III") != 0) {
        return 0;
    }

    return true;
}

void Engine::processEvents()
{
    sf::Event event;
    while (G_WINDOW.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            G_WINDOW.close();
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && m_Clock.getElapsedTime() >= sf::milliseconds(50)) {
            handleMouseButtonPressed(event);
        }
    }

    if (m_OwnerWindow.getCoutGuideActive())
        m_OwnerWindow.processingGuide();
    
    if (G_CONFIG_MAIN.optionsConfig.autoClickerKey || G_CONFIG_MAIN.optionsConfig.autoClickerMouse) {
        checkInputState();
    }
}

void Engine::draw(const bool isVisibleLoad) {
    if (G_CONFIG_MAIN.optionsConfig.blackColor) G_WINDOW.clear(sf::Color(45, 45, 48));
    else G_WINDOW.clear(sf::Color(255, 255, 255));
    m_OwnerWindow.draw(isVisibleLoad);

    if (m_IsVisibleOwnerWindow) {

        if (G_BOOL_VISIBLE.isVisibleEngineFile)
            m_EngineFileTip2.draw();
    }

    G_WINDOW.display();
}

void Engine::handleMouseButtonPressed(sf::Event& event)
{
    bool lastIsActiveWindow[] ={ G_BOOL_VISIBLE.isVisibleEngineFile, G_BOOL_VISIBLE.isVisibleGuide };
    m_OwnerWindow.processingButtonMenu(event.mouseButton, lastIsActiveWindow);
    m_OwnerWindow.processingButton(event.mouseButton, lastIsActiveWindow);

    isInitialize(lastIsActiveWindow);

    if (G_BOOL_VISIBLE.isVisibleEngineFile) {
        const int16_t numLainListHero = m_EngineFileTip2.mouseButtonPressed(event);
        if (numLainListHero >= 0) {
            G_BOOL_VISIBLE.isVisibleEngineFile = false;
            m_OwnerWindow.setIsVisibleMenu(false);
            UpdateRegionRect().updateRegionLoad();

            draw(true);

            LoadManager(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar).executeLoad(m_EngineFileTip2.getPathListHero(numLainListHero));
            ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).startGameCoutCmd(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
            if (G_CONFIG_MAIN.optionsConfig.autoExit)
                G_WINDOW.close();
            else
            {
                UpdateRegionRect().updateRegionMain();
            }
        }
        else if (numLainListHero == -2) {
            G_BOOL_VISIBLE.isVisibleEngineFile = false;
            UpdateRegionRect().updateRegionMain();
        }
    }
    m_Clock.restart();
}

inline void Engine::isInitialize(bool lastIsActiveWindow[]) {
    if (lastIsActiveWindow[0] != G_BOOL_VISIBLE.isVisibleEngineFile) {
        G_BOOL_VISIBLE.isVisibleEngineFile = lastIsActiveWindow[0];
        if (m_EngineFileTip2.initialize() == 0) {
            G_BOOL_VISIBLE.isVisibleEngineFile = false;
            m_OwnerWindow.setIsVisibleMenu(false);
            UpdateRegionRect().updateRegionMain();
        }
        if (G_BOOL_VISIBLE.isVisibleEngineFile) {
            m_EngineFileTip2.updateRect(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar, true);
            m_EngineFileTip2.updateRegionTrue(m_OwnerWindow.getIsVisibleMenu());
        }
        else {
            UpdateRegionRect().updateRegionMain();
        }
    }
}

bool Engine::initialize() {
    FontLoader().loadPredefinedFonts();

    m_OwnerWindow.initialize();

    m_OwnerWindow.activeGameFalse();
    m_IsVisibleOwnerWindow = false;

    initializeTree();

    return false;
}

void Engine::checkInputState() {
    auto now = std::chrono::high_resolution_clock::now();

    // работает только если Warcraft 1.26
    if (G_CONFIG_MAIN.optionsConfig.autoClickerKey && G_DATA_WARCRAFT.m_DataPath.versionWarcraft == 1) {
        static const std::array<int, 46> inputCodes = {
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
            0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
            0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
            0x55, 0x56, 0x57, 0x58, 0x59, 0x5A
        };

        for (int inputCode : inputCodes) {
            if (GetAsyncKeyState(inputCode) & 0x8000) {
                processInput(inputCode, currentTime);
                return;
            }
            else if (inputCode == m_lastInputCode) {
                m_lastInputCode = -1;
            }
        }
    }

    if (G_CONFIG_MAIN.optionsConfig.autoClickerMouse) {
        //     (VK_RBUTTON)
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)//    
            checkMouseInput(now);
        else if (VK_RBUTTON == m_lastInputCode) {
            m_lastInputCode = -1;
        }
    }
}

void Engine::processInput(int keyNum, auto& now) {
    if (m_lastInputCode != keyNum) {//     
        m_lastInputCode = keyNum;
        currentTime = now;
    }
    else {
        int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTime).count();//     
        if (elapsed >= 250) {
            key key_;
            while (GetAsyncKeyState(keyNum) & 0x8000) {
                key_.key1(keyNum);
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }

            m_lastInputCode = -1;
            return;
        }
    }
}

void Engine::checkMouseInput(auto& now) {
    if (m_lastInputCode != VK_RBUTTON) {
        m_lastInputCode = VK_RBUTTON;
        currentTime = now;
    }
    else {
        int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentTime).count();
        if (elapsed >= 400) {
            key key_;
            while (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                key_.keyVK(VK_RBUTTON, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            key_.keyVK(VK_RBUTTON, false); //    
            m_lastInputCode = -1;
            return;
        }
    }
}

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_APP_ICON 5000
#define ID_TRAY_EXIT 3000
#define ID_TRAY_WARCRAFT 4000
#define ID_WARCRAFT_MENU_START 5000

HWND hWnd;
std::map<std::wstring, std::wstring> buttonPaths;

bool LoadConfigData(const wchar_t* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();

    StringConvector StringConvector_;
    std::wstring wcontent = StringConvector_.utf8_to_utf16(content);

    if (wcontent[wcontent.size() - 1] != L'\n')
        wcontent = wcontent + L'\n';

    size_t pos = 0;
    while ((pos = wcontent.find(L'\n')) != std::wstring::npos) {
        std::wstring line = wcontent.substr(0, pos);
        wcontent.erase(0, pos + 1);

        size_t bracketPos = line.find(L']');
        if (bracketPos != std::wstring::npos) {
            std::wstring buttonName = line.substr(1, bracketPos - 1);
            std::wstring programPath = line.substr(bracketPos + 2);
            if (programPath[programPath.size() - 1] == L'\r')
                programPath.erase(programPath.size() - 1);

            if (!programPath.empty() && !iswalpha(programPath[0])) {
                programPath.erase(0, 1);
            }

            buttonPaths[buttonName] = programPath;
        }
    }

    return !buttonPaths.empty();
}

void LaunchProgramWithAdminRights(LPCWSTR programPath) {
    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas";
    sei.lpFile = programPath;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_CANCELLED) {
            MessageBox(NULL, L"    .", L"", MB_OK | MB_ICONINFORMATION);
        }
    }
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            HMENU hMenu = CreatePopupMenu();

            if (!buttonPaths.empty()) {
                HMENU hWarcraftMenu = CreatePopupMenu();
                int buttonId = ID_WARCRAFT_MENU_START;
                for (const auto& [buttonName, programPath] : buttonPaths) {
                    AppendMenu(hWarcraftMenu, MF_STRING, buttonId++, buttonName.c_str());
                }
                AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hWarcraftMenu, TEXT("Warcraft"));
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            }

            AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y + 35, 0, hwnd, NULL);
            PostMessage(hwnd, WM_NULL, 0, 0);
            DestroyMenu(hMenu);
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_TRAY_EXIT) {
            //  
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            G_WINDOW.close();
        }
        else if (LOWORD(wParam) >= ID_WARCRAFT_MENU_START) {
            int buttonIndex = LOWORD(wParam) - ID_WARCRAFT_MENU_START;
            if (buttonIndex >= 0 && buttonIndex < buttonPaths.size()) {

                auto it = std::next(buttonPaths.begin(), buttonIndex);
                LaunchProgramWithAdminRights(it->second.c_str());
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        G_WINDOW.close();
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void Engine::initializeTree() {
    hWnd = G_WINDOW.getSystemHandle();
    std::wstring wstr = L"DataWarAssist_Tree";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, wstr.c_str(), NULL };
    RegisterClassEx(&wc);
    hWnd = CreateWindow(wstr.c_str(), L"DataWarAssist Tree", WS_OVERLAPPEDWINDOW, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL);

    //     StartPath.ini
    if (!LoadConfigData(L"DataWarAssist\\StartPath.ini")) {
        buttonPaths.clear();
    }

    MinimizeToTray();
}

HICON LoadIconFromFile(const wchar_t* filename) {
    return (HICON)LoadImage(NULL, filename, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}

void Engine::MinimizeToTray() {
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIconFromFile(L"DataWarAssist\\img\\icon.ico");
    wcscpy_s(nid.szTip, L"DataWarAssist tree");
    Shell_NotifyIcon(NIM_ADD, &nid);
}