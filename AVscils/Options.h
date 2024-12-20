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


class Options { // 8000 > 11000 > 12700
public:
    Options();
    void options();

private:
    inline static const sf::Color BLACK_COLOR{45, 45, 48};
    inline static const sf::Color WHITE_COLOR{255, 255, 255};
    inline static const sf::Vector2f DEFAULT_POSITION{0.0f, 0.0f};
    inline static const wchar_t* WARCRAFT_WINDOW_TITLE = L"Warcraft III";
    
    static constexpr float DEFAULT_LETTER_SPACING = 1.0f;
    static constexpr int MENU_COMMANDS = 2;
    static constexpr int MENU_OPTIONS = 3;
    static constexpr float COMMAND_OFFSET_Y = 32.0f;
    
    void initializeText(sf::Text& text, std::wstring textString, const sf::Vector2f& position, const sf::Color& color, const float& letterSpacing) const;
    void initializeRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size = { 10, 10 }, const sf::Vector2f& position = { 0,0 }, const sf::Color& color = sf::Color::White) const;
    void initializeSprite(sf::Sprite& rectangle, const sf::Vector2f& position, const sf::IntRect& rect) const;
    std::wstring run();
    void handleInactiveWindow();
    void initializeWindow();
    void initializeDataCommands();
    void initializeSettings();
    void drawWindow();
    void drawCommandsMenu();
    void drawOptionsMenu();
    std::wstring handleMousePress(const sf::Event& event);
    void updateConfigMaps();
    bool IsWarcraftInFocus(const HWND& hWnd);

    std::wstring handleEvents();

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

    // Новые вспомогательные методы
    void initializeUIElements();
    void initializeBackgrounds();
    
    // Константы для размеров и отступов
    static constexpr float INITIAL_OFFSET = 10.0f;
    static constexpr float WINDOW_WIDTH = 512.0f;
    static constexpr int FRAMERATE_LIMIT = 60;
};
