#include <Windows.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <functional>
#include <algorithm>
#include <fstream>
#include "StringConvector.h"

#include "Global.h"
#include "NewDirectory.h"
#include <unordered_set>
#include "ConfigMainEngine.h"
#include "DataPath.h"
#include "ConfigMapsEngine.h"
#include "FontSelector.h"
#include "SelectingNewPathMap.h"

NewDirectory::NewDirectory()
{
    initializeWindow();
}

inline void NewDirectory::initializeText(sf::Text& text, const std::wstring& textString, const sf::Vector2f& position, const sf::Color& color, const float& letterSpacing) const {
    text.setFont(G_FONT_STANDART);
    text.setCharacterSize(16);
    text.setLetterSpacing(letterSpacing);;

    text.setString(textString);
    text.setPosition(position);
    text.setFillColor(color);
}

inline void NewDirectory::initializeRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color) const {
    rectangle.setSize(size);
    rectangle.setPosition(position);
    rectangle.setFillColor(color);
}

inline void NewDirectory::initializeSprite(sf::Sprite& rectangle, const sf::Vector2f& position, const sf::IntRect& rect) const {
    rectangle.setTexture(m_Texture);
    rectangle.setTextureRect(rect);
    rectangle.setPosition(position);
}


void NewDirectory::initializeWindow() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    //int windowWidth = 512;
    int windowHeight = desktop.height * 2 / 3 + 50;
    
    if (m_Rect[1] == 0) {
        m_Rect[0]/*.left*/ = static_cast<float>(desktop.width / 2 - 256);
        m_Rect[1]/*.top*/ = static_cast<float>(desktop.height / 2 - windowHeight / 2);
        m_Rect[2]/*.right*/ = static_cast<float>(desktop.width / 2 + 256);
        m_Rect[3]/*.bottom*/ = static_cast<float>(desktop.height / 2 + windowHeight / 2 - 32);
    }
    m_Texture.loadFromFile("DataAutoLoad\\img\\Option.png");

    initializeText(m_NameMap, /*L"Maps: " + */G_DATA_MAPS.m_NameMapsFull, sf::Vector2f{ 20 + m_Rect[0], m_Rect[1] - 26 });
    m_NameMap.setStyle(sf::Text::Bold);

    initializeSprite(titlAndClose, sf::Vector2f{ m_Rect[0],  m_Rect[1] - 48 }, { 0, 448, 512, 16 });
    
    {
        initializeSprite(background, sf::Vector2f{ m_Rect[0], m_Rect[1] }, { 0, 288, 512, 32 });
        sf::Vector2f targetSize(512, m_Rect[3] - m_Rect[1]);
        background.setScale(1, targetSize.y / 32);
    }
    
    initializeSprite(topBackground, sf::Vector2f{ m_Rect[0], m_Rect[1] - 32 }, { 0, 320, 512, 32 });
    initializeSprite(bottomBackground, sf::Vector2f{ m_Rect[0],  m_Rect[3]  }, { 0, 416, 512, 32 });
    
    
    // ���� ��������� ������
    initializeDataCommands();
    
    // ���� ��������
    initializeSettings();

    G_WINDOW.setFramerateLimit(60);
}



float NewDirectory::initializeDataCommands() {
    const float OFFSET_X = 32;

    initializeSprite(m_TopLineCommandsUI, sf::Vector2f{ m_Rect[0], m_Rect[1] }, sf::Rect(0, 0, 512, 32));
    float positionY = 32;
    for (size_t i = 0; CommandsUI_0& commands : m_CommandsMain) {
        commands.initialize(G_CONFIG_MAPS.mainConfig[i].start, G_CONFIG_MAPS.mainConfig[i].cmd, m_Texture);
        commands.setPosition(m_Rect[0], m_Rect[1] + positionY);
        positionY += OFFSET_X;

        ++i;
    }
    for (size_t i = 0; CommandsUI_1& commands : m_CommandsUsers) {
        commands.initialize(G_CONFIG_MAPS.usersConfig[i].isVisibleButton, G_CONFIG_MAPS.usersConfig[i].start, G_CONFIG_MAPS.usersConfig[i].cmd, m_Texture);
        commands.setPosition(m_Rect[0], m_Rect[1] + positionY);
        positionY += OFFSET_X;

        ++i;
    }
    return positionY;
}

void NewDirectory::initializeSettings() {
    m_Options[0].initialize(G_CONFIG_MAIN.optionsConfig.autoClickerKey, "���� ������� ������", m_Texture, false);
    m_Options[0].setPosition(m_Rect[0], m_Rect[1] + 10);

    m_Options[1].initialize(G_CONFIG_MAIN.optionsConfig.autoClickerMouse, "���� ������� ������ ������ ����", m_Texture, false);
    m_Options[1].setPosition(m_Rect[0], m_Rect[1] + 42);

    m_Options[2].initialize(G_CONFIG_MAIN.optionsConfig.autoExit, "���� ����� ����� ������� ��������", m_Texture, false);
    m_Options[2].setPosition(m_Rect[0], m_Rect[1] + 74);
}

void NewDirectory::newDirectory() {
    updateRegionTrue();
    // �������� ����������� ���� ��� ������ ����� � ��������� ���� � ��������� �����
    std::wstring folderPath = run();
    updateRegionFalse();
}

void NewDirectory::drawWindow() {
    G_WINDOW.clear(sf::Color(255, 255, 255));//sf::Color(45, 45, 48)
    G_WINDOW.draw(titlAndClose);
    G_WINDOW.draw(background);

    if (numMenu == 2) {
        G_WINDOW.draw(m_TopLineCommandsUI);

        for (CommandsUI_0& p : m_CommandsMain)
            p.draw(G_WINDOW);

        for (CommandsUI_1& p : m_CommandsUsers)
            p.draw(G_WINDOW);

    }
    else if (numMenu == 3) {
        for (OptionsUI& option : m_Options)
            option.draw(G_WINDOW);
    }


    G_WINDOW.draw(topBackground);

    G_WINDOW.draw(m_NameMap);

    G_WINDOW.draw(bottomBackground);

    G_WINDOW.display();
}


std::wstring NewDirectory::handleMousePress(sf::Event& event) {
    if (event.mouseButton.button != sf::Mouse::Left)
        return L"";
    
    const sf::Vector2f mouseButton{ static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };

    if (topBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        const sf::Vector2f mouseButton2{ mouseButton.x - topBackground.getPosition().x, mouseButton.y - topBackground.getPosition().y };
        if (numMenu == 3 &&/*mouseButton2.x >= 0 && */mouseButton2.x < 384) {// �������� �����
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
        const sf::Vector2f mouseButton2{ mouseButton.x - bottomBackground.getPosition().x, mouseButton.y - bottomBackground.getPosition().y };
        if (mouseButton2.x >= 480/* && mouseButton2.x < 512*/)
            return L"Exit";
    }

    if (numMenu == 2) {
        if (m_TopLineCommandsUI.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
            const sf::Vector2f mouseButton2{ mouseButton.x - m_TopLineCommandsUI.getPosition().x, mouseButton.y - m_TopLineCommandsUI.getPosition().y };
            if (mouseButton2.x >= 0 && mouseButton2.x < 32) {
                //updateConfigMaps();
                SelectingNewPathMap().selectingNewPathMap();
                updateRegionTrue();
            }
        }

        short isSquare = 0;
        int numDataCommands = -1;
        for (int i = 0; CommandsUI_0& command : m_CommandsMain) {
            if (command.isClicked(mouseButton) && command.isClickedCheckBox(mouseButton))
                G_CONFIG_MAPS.mainConfig[i].start = command.getValue();
            i++;
        }

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
            }
            i++;
        }
        if (isSquare != 0) {

            ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
            ConfigMapsEngine_.saveConfigMain();
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
        checkAndToggle(1, G_CONFIG_MAIN.optionsConfig.autoClickerMouse, isSave);
        checkAndToggle(2, G_CONFIG_MAIN.optionsConfig.autoExit, isSave);

        if (isSave) {
            ConfigMainEngine ConfigMainEngine_;
            ConfigMainEngine_.saveConfigMain();
        }
    }

    return L"";
}

void NewDirectory::updateRegionTrue() {
    HWND hwnd = G_WINDOW.getSystemHandle();

    HRGN region = CreateRectRgn(0, 0, 0, 0); // ������ ������ ��� ��������������
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int windowHeight = desktop.height * 2 / 3 + 50;
    HRGN OptionBar = CreateRectRgn(
        desktop.width / 2 - 256,
        desktop.height / 2 - windowHeight / 2 - 48,
        desktop.width / 2 + 256,
        desktop.height / 2 + windowHeight / 2);
    CombineRgn(region, region, OptionBar, RGN_OR);
    DeleteObject(OptionBar);

    SetWindowRgn(hwnd, region, TRUE);
}

void NewDirectory::updateRegionFalse() {
    HWND hwnd = G_WINDOW.getSystemHandle();

    HRGN region = CreateRectRgn(0, 0, 0, 0); // ������ ������ ��� ��������������
    SetWindowRgn(hwnd, region, TRUE);
}

void NewDirectory::updateConfigMaps() {
    updateRegionFalse();

    G_WINDOW.clear(sf::Color(255, 255, 255));
    G_WINDOW.display();

    const std::array<CmdEntry, 5> usersConfigLast = G_CONFIG_MAPS.usersConfig;

    ConfigMapsEngine ConfigMapsEngine_(G_DATA_MAPS.m_NameMaps);
    ConfigMapsEngine_.openConfigMain();
    ConfigMapsEngine_.loadConfigMain();

    for (uint8_t i{ 0 }; CmdEntry& comand : G_CONFIG_MAPS.usersConfig) {
        if (comand.cmd != usersConfigLast[i].cmd) {
            if (comand.cmd.empty()) { comand.isVisibleButton = false; }
            else {  comand.isVisibleButton = true; }
            m_CommandsUsers[i].updateSpriteIsChecBoxBool(comand.isVisibleButton, comand.start);
        }
        i++;
    }

    initializeDataCommands();
    updateRegionTrue();
}

bool NewDirectory::IsWindowInFocus(const HWND& hWnd) const {
    if (hWnd == NULL) {
        //std::wcout << L"��� ��������� ����!" << std::endl;
        return 0;
    }

    // ����� ��� ����� ����
    wchar_t windowTitle[256];
    
    // �������� ��� ����
    int length = GetWindowText(hWnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
    if (length == 0) {
        //std::wcout << L"�� ������� �������� ��� ����!" << std::endl;
        return 0;
    }

    // ���������� ��� ���� � ���������
    if (wcscmp(windowTitle, L"Warcraft III") != 0) {
        //std::wcout << L"��� ��������� ���� �� ��������� � 'Warcraft III'" << windowTitle << std::endl;
        return 0;
    }

    //std::wcout << windowTitle << std::endl;


    return true;
}

std::wstring NewDirectory::run() {
    bool isActive = true;
    while (G_WINDOW.isOpen()) {

        // �������� ���������� ��������� ����
        HWND hWndWindow = GetForegroundWindow();

        //HWND hWndWindow = FindWindow(NULL, L"Warcraft III");
        if (IsWindowInFocus(hWndWindow)) {

            if (isActive == false) {
                if (G_DATA_PATH.hWndWindowWar != hWndWindow)
                    return std::wstring();
                isActive = true;
                updateRegionTrue();
            }

            sf::Event event;
            while (G_WINDOW.pollEvent(event)) {
                switch (event.type) {
                case sf::Event::MouseButtonPressed:
                {
                    std::wstring wstr = handleMousePress(event);
                    if (wstr != L"\0")
                        return wstr;
                }
                break;
                }
            }
            drawWindow();
        }
        else {
            if (isActive) {
                updateRegionFalse();
            }
            isActive = false;
            sf::Event event;
            while (G_WINDOW.pollEvent(event)) {

            }
            G_WINDOW.clear(sf::Color(255, 255, 255));
            G_WINDOW.display();
        }
    }
    return std::wstring();
}