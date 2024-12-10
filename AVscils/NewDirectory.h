#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <array>
#include <wtypes.h>
#include "CommandsUI_0.h"
#include "CommandsUI_1.h"
#include "OptionsUI.h"
#include "NewDataAll.h"
#include "OptionActionsUi.h"


class NewDirectory { // 8000 > 11000 > 12700
public:
    NewDirectory();
    void newDirectory();

private:
    void initializeText(sf::Text& text, const std::wstring& textString = L"", const sf::Vector2f& position = { 0,0 }, const sf::Color& color = sf::Color::White, const float& letterSpacing = 1.0f) const;
    void initializeRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size = { 10, 10 }, const sf::Vector2f& position = { 0,0 }, const sf::Color& color = sf::Color::White) const;
    void initializeSprite(sf::Sprite& rectangle, const sf::Vector2f& position, const sf::IntRect& rect) const;
    std::wstring run();
    void initializeWindow();
    void initializeDataCommands();
    void initializeSettings();
    void drawWindow();
    std::wstring handleMousePress(sf::Event& event);
    void updateRegionTrue();
    void updateRegionFalse();
    void updateConfigMaps();
    bool IsWindowInFocus(const HWND& hWnd) const;

    sf::Texture m_Texture;
    std::array<float, 4> m_Rect = {0, 0, 0, 0};//[0]left [1]top [2]right [3]bottom
    sf::Text m_NameMap;
    sf::Sprite topBackground;// содержит optionsButton DataCommandsButton 
    sf::Sprite bottomBackground;
    sf::Sprite background;//осташияся незаполненная область

    OptionActionUi m_PathWar3;

    sf::Sprite titlAndClose;

    sf::Sprite m_TopLineCommandsUI;
    bool m_IsRunning = false;
    int numMenu = 2;
    std::array<CommandsUI_0, 4> m_CommandsMain;
    std::array<CommandsUI_1, 5> m_CommandsUsers;
    std::array<OptionsUI, 6> m_Options;
};
