#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "OwnerWindow.h"
#include "EngineFileTip2.h"
#include "DataAll.h"


class Engine
{
public:
    void engine1();
private:
    bool IsWarcraftInFocus(const HWND& hWnd);
    bool updateWindowVisibility(const HWND& hWndWindow);
    void processEvents();
    void draw(const bool isVisibleLoad = false);
    void handleMouseButtonPressed(sf::Event& event);
    void isInitialize(bool lastIsActiveWindow[]);

    bool initialize();



    void initializeTree();
    void MinimizeToTray();

    void checkInputState();
    void processInput(int inputCode, auto& currentTime);
    void checkMouseInput(auto& now);


    OwnerWindow m_OwnerWindow;
    EngineFileTip2 m_EngineFileTip2;
    sf::Clock m_Clock;

    bool m_IsLastActive = true;
    bool m_IsVisibleOwnerWindow = true;
    bool m_NewIsVisible = true;


    int m_lastInputCode{ 0 };
    std::chrono::high_resolution_clock::time_point currentTime{};
    //std::unordered_map<int, std::chrono::high_resolution_clock::time_point> keyPressTimes;
    int currentKey = -1;  // Track the currently pressed key
};