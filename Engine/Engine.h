#pragma once
#include "pch.h"
#include <memory>
#include <SFML/Graphics.hpp>
#include "OwnerWindow.h"
#include "EngineFileTip2.h"
#include "DataAll.h"
#include "TrayManager.h"
#include "BoolVisibleMenu.h"
#include "UpdateRegionRect.h"
#include "ConfigMain.h"

// Экспортируем класс Engine
#ifdef ENGINE_EXPORTS
    #define ENGINE_API __declspec(dllexport)
#else
    #define ENGINE_API __declspec(dllimport)
#endif

class ENGINE_API Engine {
public:
    static Engine& getInstance() {
        static Engine instance;
        return instance;
    }
    void RunEngineDll();
private:
    Engine() = default;
    ~Engine() = default;
    
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    inline bool IsWarcraftInFocus(const HWND& hWnd);
    inline void handleNegativeHeroSelection();
    inline void handleInactiveWindow();
    
    bool updateWindowVisibility(const HWND& hWndWindow);
    void processEvents();
    void draw(bool isVisibleLoad = false);
    void handleMouseButtonPressed(const sf::Event& event);
    void handlePositiveHeroSelection(int16_t numLainListHero);
    void isInitialize(const std::array<bool, 2>& lastIsActiveWindow);

    bool initialize();

    std::unique_ptr<TrayManager> m_TrayManager;
    OwnerWindow m_OwnerWindow;
    EngineFileTip2 m_EngineFileTip2;
    sf::Clock m_Clock;
    bool m_IsVisibleOwnerWindow{false};
};

inline bool Engine::IsWarcraftInFocus(const HWND& hWnd) {
    static const wchar_t* TARGET_TITLE = L"Warcraft III";
    wchar_t windowTitle[256];
    
    if (!GetWindowTextW(hWnd, windowTitle, 256)) {
        return false;
    }
    return (wcscmp(windowTitle, TARGET_TITLE) == 0);
}

inline void Engine::handleNegativeHeroSelection() {
    G_BOOL_VISIBLE.isVisibleEngineFile = false;
    UpdateRegionRect().updateRegionMain();
}

inline void Engine::handleInactiveWindow() {
    sf::Event event;
    while (G_WINDOW.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            G_WINDOW.close();
        }
    }

    const auto& config = G_CONFIG_MAIN.optionsConfig;
    const auto clearColor = config.blackColor ? 
        sf::Color(45, 45, 48) : 
        sf::Color(255, 255, 255);
    
    G_WINDOW.clear(clearColor);
    G_WINDOW.display();
}