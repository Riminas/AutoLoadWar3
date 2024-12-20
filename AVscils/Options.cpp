#include <Windows.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <filesystem>

#include <functional>
#include <algorithm>
#include <fstream>
#include "StringConvector.h"

#include "Global.h"
#include "Options.h"
#include <unordered_set>
#include "ConfigMainEngine.h"
#include "DataWarcraft.h"
#include "ConfigMapsEngine.h"

#include "SelectingNewPathMap.h"
#include "UpdateRegionRect.h"

Options::Options()
{
    initializeWindow();
    numMenu = G_DATA_MAPS.m_NameMaps.empty() ? 3 : 2;
    if (numMenu == 3) {
        initializeSprite(topBackground, { m_Rect[0], m_Rect[1] - 32 }, { 0, 352, 512, 32 });
    }
}

void Options::options() {
    UpdateRegionRect().updateRegionOption();//   
    //            
    std::wstring folderPath = run();
    UpdateRegionRect().clearRegion();//    
}

inline void Options::initializeText(sf::Text& text, std::wstring textString, const sf::Vector2f& position,
    const sf::Color& color, const float& letterSpacing) const {
    text.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
    text.setString(std::move(textString));
    text.setPosition(position);
    text.setFillColor(color);
    text.setCharacterSize(15);
    text.setLetterSpacing(letterSpacing);
}

inline void Options::initializeRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color) const {
    rectangle.setSize(size);
    rectangle.setPosition(position);
    rectangle.setFillColor(color);
}

inline void Options::initializeSprite(sf::Sprite& rectangle, const sf::Vector2f& position, const sf::IntRect& rect) const {
    rectangle.setTexture(m_Texture);
    rectangle.setTextureRect(rect);
    rectangle.setPosition(position);
}


void Options::initializeWindow() {
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int windowHeight = desktop.height * 2 / 3 + 50;

    if (m_Rect[1] == 0) {
        m_Rect = {
            static_cast<float>(desktop.width / 2 - 256),   // left
            static_cast<float>(desktop.height / 2 - windowHeight / 2), // top
            static_cast<float>(desktop.width / 2 + 256),   // right
            static_cast<float>(desktop.height / 2 + windowHeight / 2 - 32) // bottom
        };
    }

    if (!m_Texture.loadFromFile("DataAutoLoad\\img\\Option.png")) {
        throw std::runtime_error("Failed to load Option.png");
    }

    initializeText(m_NameMap, G_DATA_MAPS.m_NameMapsFull, 
        {20 + m_Rect[0], m_Rect[1] - 27}, sf::Color::White, DEFAULT_LETTER_SPACING);
    m_NameMap.setStyle(sf::Text::Bold);

    initializeUIElements();
    initializeBackgrounds();
    initializeDataCommands();
    initializeSettings();

    G_WINDOW.setFramerateLimit(60);
}

void Options::initializeUIElements() {
    initializeSprite(titlAndClose, 
        {m_Rect[0], m_Rect[1] - 48}, 
        {0, 448, 512, 16});
}

void Options::initializeBackgrounds() {
    initializeSprite(background, 
        {m_Rect[0], m_Rect[1]}, 
        {0, 288, 512, 32});
    
    const float targetHeight = m_Rect[3] - m_Rect[1];
    background.setScale(1, targetHeight / 32);

    initializeSprite(topBackground, 
        {m_Rect[0], m_Rect[1] - 32}, 
        {0, 320, 512, 32});
        
    initializeSprite(bottomBackground, 
        {m_Rect[0], m_Rect[3]}, 
        {0, 416, 512, 32});
}

void Options::initializeDataCommands() {
    static constexpr float COMMAND_Y_OFFSET = 32.0f;
    
    initializeSprite(m_TopLineCommandsUI, 
        {m_Rect[0], m_Rect[1]}, 
        {0, 0, 512, 32});

    float positionY = m_Rect[1] + COMMAND_Y_OFFSET;

    // Инициализация основных команд
    for (size_t i = 0; auto& command : m_CommandsMain) {
        const auto& config = G_CONFIG_MAPS.mainConfig[i];
        command.initialize(config.start, config.cmd, m_Texture);
        command.setPosition(m_Rect[0], positionY);
        positionY += COMMAND_Y_OFFSET;
        ++i;
    }

    // Инициализация пользовательских команд
    for (size_t i = 0; auto& command : m_CommandsUsers) {
        const auto& config = G_CONFIG_MAPS.usersConfig[i];
        command.initialize(config.isVisibleButton, config.start, config.cmd, m_Texture);
        command.setPosition(m_Rect[0], positionY);
        positionY += COMMAND_Y_OFFSET;
        ++i;
    }

    m_PathWar3.initialize(G_CONFIG_MAPS.path, m_Texture);
    m_PathWar3.setPosition(m_Rect[0], positionY + COMMAND_Y_OFFSET);
}

void Options::initializeSettings() {
    static constexpr float OPTION_Y_OFFSET = 32.0f;
    static constexpr float INITIAL_Y = 10.0f;
    
    const auto& config = G_CONFIG_MAIN.optionsConfig;
    const std::array<std::pair<const bool&, const wchar_t*>, 6> options = {{
        {config.autoClickerKey, L"Авто кликер клавиши"},
        {config.autoClickerMouse, L"Авто кликер правой кнопки мыши"},
        {config.blackColor, L"Темная тема"},
        {config.autoUpdate, L"Автоматическое обновление"},
        {config.writeLogs, L"Запись логов"},
        {config.autoExit, L"Авто выход после быстрой загрузки"}
    }};

    for (size_t i = 0; i < options.size(); ++i) {
        const float yPos = m_Rect[1] + INITIAL_Y + (i * OPTION_Y_OFFSET);
        m_Options[i].initialize(options[i].first, options[i].second, m_Texture);
        m_Options[i].setPosition(m_Rect[0], yPos);
    }
}


void Options::drawWindow() {
    const auto& clearColor = G_CONFIG_MAIN.optionsConfig.blackColor ? BLACK_COLOR : WHITE_COLOR;
    G_WINDOW.clear(clearColor);

    G_WINDOW.draw(titlAndClose);
    G_WINDOW.draw(background);

    if (numMenu == MENU_COMMANDS) {
        drawCommandsMenu();
    }
    else if (numMenu == MENU_OPTIONS) {
        drawOptionsMenu();
    }

    G_WINDOW.draw(topBackground);
    G_WINDOW.draw(m_NameMap);
    G_WINDOW.draw(bottomBackground);
    G_WINDOW.display();
}

inline void Options::drawCommandsMenu() {
    m_PathWar3.draw(G_WINDOW);
    G_WINDOW.draw(m_TopLineCommandsUI);

    for (auto& cmd : m_CommandsMain) {
        cmd.draw(G_WINDOW);
    }

    for (auto& cmd : m_CommandsUsers) {
        cmd.draw(G_WINDOW);
    }
}

inline void Options::drawOptionsMenu() {
    for (auto& option : m_Options) {
        option.draw(G_WINDOW);
    }
}


std::wstring Options::handleMousePress(const sf::Event& event) {
    if (event.mouseButton.button != sf::Mouse::Left)
        return L"";

    const sf::Vector2f mouseButton{
        static_cast<float>(event.mouseButton.x),
        static_cast<float>(event.mouseButton.y)
    };

    if (topBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        const sf::Vector2f mouseButton2{ mouseButton.x - topBackground.getPosition().x, mouseButton.y - topBackground.getPosition().y };
        if (!G_DATA_MAPS.m_NameMaps.empty() && numMenu == 3 &&/*mouseButton2.x >= 0 && */mouseButton2.x < 384) {// �������� �����
            initializeSprite(topBackground, sf::Vector2f{ m_Rect[0], m_Rect[1] - 32 }, { 0, 320, 512, 32 });
            numMenu = 2;
            return L"";
        }
        else if (numMenu == 2 && /*mouseButton2.x >= 448 && */mouseButton2.x < 512) {// ��������
            initializeSprite(topBackground, sf::Vector2f{ m_Rect[0], m_Rect[1] - 32 }, { 0, 352, 512, 32 });
            numMenu = 3;
            return L"";
        }
    }

    if (bottomBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        //const sf::Vector2f mouseButton2{ mouseButton.x - bottomBackground.getPosition().x, mouseButton.y - bottomBackground.getPosition().y };
        //if (mouseButton2.x >= 480/* && mouseButton2.x < 512*/)
        return L"Exit";
    }
    if (titlAndClose.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        const sf::Vector2f mouseButton2{ mouseButton.x - titlAndClose.getPosition().x, mouseButton.y - titlAndClose.getPosition().y };
        if (mouseButton2.x >= 480/* && mouseButton2.x < 512*/)
            return L"Exit";
    }

    if (numMenu == 2) {

        if (bottomBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
            //const sf::Vector2f mouseButton2{ mouseButton.x - bottomBackground.getPosition().x, mouseButton.y - bottomBackground.getPosition().y };
            //if (mouseButton2.x >= 480/* && mouseButton2.x < 512*/)
            return L"Exit";
        }
        if (m_PathWar3.isClicked(mouseButton) && m_PathWar3.isClickedButton(mouseButton)) {
            SelectingNewPathMap().selectingNewPathMap();
            UpdateRegionRect().updateRegionOption();//�������� ����� �������� ������
            m_PathWar3.setString(G_CONFIG_MAPS.path);
        }

        //if (m_TopLineCommandsUI.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        //    const sf::Vector2f mouseButton2{ mouseButton.x - m_TopLineCommandsUI.getPosition().x, mouseButton.y - m_TopLineCommandsUI.getPosition().y };
        //    if (mouseButton2.x >= 0 && mouseButton2.x < 32) {
        //        //updateConfigMaps();
        //        SelectingNewPathMap().selectingNewPathMap();
        //        UpdateRegionRect().updateRegion(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar, 2);//�������� ����� �������� ������
        //    }
        //}

        bool isSquare = false;
        int numDataCommands = -1;
        for (int i = 0; CommandsUI_0 & command : m_CommandsMain) {
            if (command.isClicked(mouseButton)) {
                if (command.isClickedCheckBox(mouseButton)) {
                    G_CONFIG_MAPS.mainConfig[i].start = command.getValue();
                    break;
                }
                else if (command.isClickedEdit(mouseButton)) {
                    updateConfigMaps();
                    break;
                }
            }
            i++;
        }
        if (isSquare == false) {
            for (int i = 0; CommandsUI_1 & command : m_CommandsUsers) {
                if (command.isClicked(mouseButton)) {
                    if (command.isClickedCheckBox1(mouseButton)) {
                        G_CONFIG_MAPS.usersConfig[i].isVisibleButton = command.getValueBox1();
                        isSquare = true;
                        break;
                    }
                    else if (command.isClickedCheckBox2(mouseButton)) {
                        G_CONFIG_MAPS.usersConfig[i].start = command.getValueBox2();
                        isSquare = true;
                        break;
                    }
                    else if (command.isClickedEdit(mouseButton)) {
                        updateConfigMaps();
                        break;
                    }
                }
                i++;
            }
        }
        if (isSquare) {
            ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).saveConfigMaps();
        }
    }
    else if (numMenu == 3) {

        const sf::Vector2i mousePosition = sf::Mouse::getPosition(G_WINDOW);
        auto checkAndToggle = [&](const int& index, bool& option, bool& isSave) {
            if (m_Options[index].isClicked(mouseButton)) {
                if (m_Options[index].isClickedCheckBox(mouseButton)) {
                    option = m_Options[index].getValue();
                    isSave = true;
                }
            }
            };

        bool isSave = false;
        checkAndToggle(0, G_CONFIG_MAIN.optionsConfig.autoClickerKey, isSave);
        if (!isSave) checkAndToggle(1, G_CONFIG_MAIN.optionsConfig.autoClickerMouse, isSave);
        if (!isSave) checkAndToggle(2, G_CONFIG_MAIN.optionsConfig.blackColor, isSave);
        if (!isSave) checkAndToggle(3, G_CONFIG_MAIN.optionsConfig.autoUpdate, isSave);
        if (!isSave) checkAndToggle(4, G_CONFIG_MAIN.optionsConfig.writeLogs, isSave);
        if (!isSave) checkAndToggle(5, G_CONFIG_MAIN.optionsConfig.autoExit, isSave);

        if (isSave) {
            ConfigMainEngine ConfigMainEngine_;
            ConfigMainEngine_.saveConfigMain();
        }
    }

    return L"";
}


void Options::updateConfigMaps() {
    UpdateRegionRect().clearRegion();

    const auto usersConfigLast = G_CONFIG_MAPS.usersConfig;
    
    ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).loadConfigMaps();

    for (uint8_t i = 0; auto& command : G_CONFIG_MAPS.usersConfig) {
        if (command.cmd != usersConfigLast[i].cmd) {
            command.isVisibleButton = !command.cmd.empty();
            m_CommandsUsers[i].updateSpriteIsChecBoxBool(
                command.isVisibleButton, 
                command.start
            );
        }
        ++i;
    }

    initializeDataCommands();
    UpdateRegionRect().updateRegionOption();
}

inline bool Options::IsWarcraftInFocus(const HWND& hWnd) {
    static thread_local std::wstring windowTitle(256, L'\0');
    if (!GetWindowTextW(hWnd, windowTitle.data(), static_cast<int>(windowTitle.size()))) {
        return false;
    }
    return (windowTitle.compare(0, wcslen(WARCRAFT_WINDOW_TITLE), WARCRAFT_WINDOW_TITLE) == 0);
}

std::wstring Options::handleEvents() {
    sf::Event event;
    while (G_WINDOW.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            G_WINDOW.close();
            return L"Exit";

        case sf::Event::MouseButtonPressed:
            if (auto result = handleMousePress(std::move(event)); !result.empty())
                return result;
            break;

        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape) {
                return L"Exit";
            }
            break;
        }
    }
    return L"";
}

std::wstring Options::run() {
    bool isActive = true;
    while (G_WINDOW.isOpen()) {
        HWND hWndWindow = GetForegroundWindow();

        if (!IsWarcraftInFocus(hWndWindow)) {
            if (isActive) {
                UpdateRegionRect().clearRegion();
                isActive = false;
            }
            handleInactiveWindow();
            continue;
        }

        if (!isActive) {
            if (G_DATA_WARCRAFT.m_DataPath.hWndWindowWar != hWndWindow)
                return L"";
            isActive = true;
            UpdateRegionRect().updateRegionOption();
        }

        auto result = handleEvents();
        if (!result.empty())
            return std::move(result);

        drawWindow();
    }
    return L"";
}

inline void Options::handleInactiveWindow() {
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