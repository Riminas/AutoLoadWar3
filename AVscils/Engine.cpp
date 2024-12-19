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
#include "InputHandler.h"

#include "UpdateRegionRect.h"

NOTIFYICONDATA nid;

void Engine::engine1() {
    if (initialize()) {
        return;
    }

    InputHandler inputHandler;
    HWND hWndWindow = nullptr;

    while (G_WINDOW.isOpen()) {
        hWndWindow = GetForegroundWindow();

        if (hWndWindow != nullptr && updateWindowVisibility(hWndWindow)) {
            processEvents();
            
            const auto& config = G_CONFIG_MAIN.optionsConfig;
            if (config.autoClickerKey || config.autoClickerMouse) {
                inputHandler.handleInput();
            }
            
            draw();
        } else {
            handleInactiveWindow();
        }
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

bool Engine::updateWindowVisibility(const HWND& hWndWindow) {
    const bool isWarcraft = IsWarcraftInFocus(hWndWindow);
    
    if (!isWarcraft) {
        if (!m_IsVisibleOwnerWindow) {
            return false;
        }
        m_OwnerWindow.activeGameFalse();
        m_IsVisibleOwnerWindow = false;
        return false;
    }

    if (m_IsVisibleOwnerWindow) {
        return true;
    }

    // Обновление данных только при необходимости
    if (G_DATA_WARCRAFT.m_DataPath.hWndWindowWar != hWndWindow) {
        if (!NewDataAll().newWarcraft(hWndWindow)) {
            G_WINDOW.setVisible(false);
            m_IsVisibleOwnerWindow = false;
            return false;
        }
        G_WINDOW.setVisible(true);
        m_OwnerWindow.updateRect(hWndWindow);
        if (G_BOOL_VISIBLE.isVisibleEngineFile) {
            m_EngineFileTip2.updateRect(hWndWindow, false);
        }
    }

    NewDataAll().newMaps();
    m_OwnerWindow.activeGameTrue(hWndWindow);

    if (G_DATA_MAPS.m_IsNewInfo) {
        G_BOOL_VISIBLE.isVisibleEngineFile = false;
        G_DATA_MAPS.m_IsNewInfo = false;
        G_CONFIG_MAPS.path = G_CONFIG_MAPS.lastPath;
    }

    m_IsVisibleOwnerWindow = true;
    return true;
}

void Engine::processEvents() {
    sf::Event event;
    while (G_WINDOW.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                G_WINDOW.close();
                break;
            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left && 
                    m_Clock.getElapsedTime() >= sf::milliseconds(50)) {
                    handleMouseButtonPressed(event);
                }
                break;
        }
    }

    if (m_OwnerWindow.getCoutGuideActive()) {
        m_OwnerWindow.processingGuide();
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

void Engine::handleMouseButtonPressed(const sf::Event& event) {
    std::array<bool, 2> lastIsActiveWindow = { 
        G_BOOL_VISIBLE.isVisibleEngineFile, 
        G_BOOL_VISIBLE.isVisibleGuide 
    };

    m_OwnerWindow.processingButtonMenu(event.mouseButton, lastIsActiveWindow);
    m_OwnerWindow.processingButton(event.mouseButton, lastIsActiveWindow);

    isInitialize(lastIsActiveWindow);

    if (G_BOOL_VISIBLE.isVisibleEngineFile) {
        const int16_t numLainListHero = m_EngineFileTip2.mouseButtonPressed(event);
        if (numLainListHero >= 0) {
            handlePositiveHeroSelection(numLainListHero);
        }
        else if (numLainListHero == -2) {
            handleNegativeHeroSelection();
        }
    }
    m_Clock.restart();
}

void Engine::handlePositiveHeroSelection(int16_t numLainListHero) {
    G_BOOL_VISIBLE.isVisibleEngineFile = false;
    m_OwnerWindow.setIsVisibleMenu(false);
    UpdateRegionRect().updateRegionLoad();

    draw(true);

    const auto& hwnd = G_DATA_WARCRAFT.m_DataPath.hWndWindowWar;
    auto&& loadManager = LoadManager(hwnd);
    loadManager.executeLoad(m_EngineFileTip2.getPathListHero(numLainListHero));
    
    ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).startGameCoutCmd(hwnd);
    
    if (G_CONFIG_MAIN.optionsConfig.autoExit) {
        G_WINDOW.close();
    } else {
        UpdateRegionRect().updateRegionMain();
    }
}

inline void Engine::isInitialize(const std::array<bool, 2>& lastIsActiveWindow) {
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
    m_OwnerWindow.initialize();

    m_OwnerWindow.activeGameFalse();
    m_IsVisibleOwnerWindow = false;

    m_TrayManager = std::make_unique<TrayManager>(G_WINDOW);
    m_TrayManager->initialize();

    return false;
}
