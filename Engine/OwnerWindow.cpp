#include "pch.h"
#include <Windows.h>

#include <wtypes.h>

#include <filesystem>
#include <chrono>

#include "Global.h"
#include "OwnerWindow.h"
#include "LoadDataFail.h"
#include "LoadManager.h"
#include "EngineFileTip1.h"
#include "getMapOpen.h"
#include "Options.h"
#include "SkillsUpgradeStart.h"
#include "ConfigMaps.h"
#include "ConfigMain.h"
#include "StringConvector.h"
#include "DataAll.h"
#include "SearchAfterCreatedFile.h"
#include "ConfigMapsEngine.h"
#include "CoutGuide.h"
#include "SelectingNewPathMap.h"
#include "LogManager.h"
#include "BoolVisibleMenu.h"
#include "UpdateRegionRect.h"
#include <sfml/OpenGL.hpp>
#include <gl/GL.h>

void OwnerWindow::initialize() {

    setupWindow();
    if (G_CONFIG_MAIN.optionsConfig.blackColor) m_TextureButton.loadFromFile("img\\ButtonBlack.png");
    else m_TextureButton.loadFromFile("img\\Button.png");

    m_Buttons.initialize({ 128, 0, 640, 128 }, m_TextureButton);
    initializeButtonsUsersDataCommands();
    m_ButtonsMenu.initialize({ 0, 0, 128, 640 }, m_TextureButton);

    m_CoutUserName.initialize({ 256, 384, 128, 128 }, m_TextureButton);

    m_SpriteIsLoad.setTexture(m_TextureButton);
    m_SpriteIsLoad.setTextureRect(sf::IntRect(128, 512, 640, 128));//640x120
    m_SpriteIsLoad.setScale(0.3f, 0.3f);//213x40

    m_ShapeTrueVisibleMainMenu.initialize({ 128, 384, 128, 128 }, m_TextureButton);

    //m_ShapeFalseVisibleMainMenu;
    m_ShapeFalseVisibleMainMenu.setSize(sf::Vector2f(10.f, 8.f));
    m_ShapeFalseVisibleMainMenu.setOutlineColor(sf::Color::Black);
    m_ShapeFalseVisibleMainMenu.setOutlineThickness(2);

}

void OwnerWindow::draw(const bool isVisibleLoad) {
    if (isVisibleLoad) {
        G_WINDOW.draw(m_SpriteIsLoad);
        return;
    }

    if (!G_BOOL_VISIBLE.isVisibleMainMenu) {
        G_WINDOW.draw(m_ShapeFalseVisibleMainMenu);
        return;
    }
    m_Buttons.draw(G_WINDOW);

    if (G_CONFIG_MAPS.usersConfig[0].isVisibleButton) { m_ButtonsUsers[0].draw(G_WINDOW); }
    if (G_CONFIG_MAPS.usersConfig[1].isVisibleButton) { m_ButtonsUsers[1].draw(G_WINDOW); }
    if (G_CONFIG_MAPS.usersConfig[2].isVisibleButton) { m_ButtonsUsers[2].draw(G_WINDOW); }
    if (G_CONFIG_MAPS.usersConfig[3].isVisibleButton) { m_ButtonsUsers[3].draw(G_WINDOW); }
    if (G_CONFIG_MAPS.usersConfig[4].isVisibleButton) { m_ButtonsUsers[4].draw(G_WINDOW); }
    
    if (m_CoutGuide.isActive)
        m_CoutGuide.draw();
    
    if (!G_BOOL_VISIBLE.isVisibleMenu)
        return;

    m_ButtonsMenu.draw(G_WINDOW);
    m_ShapeTrueVisibleMainMenu.draw(G_WINDOW);
    m_CoutUserName.draw(G_WINDOW);
}

void OwnerWindow::processingButton(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible) {

    const int numButton = mouseButtonPressed(event, isWindow2Visible);

    if (numButton == -5) {
        LoadManager(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAIN.playerName }, false);
    }
    else if (numButton == -4) {
        G_BOOL_VISIBLE.isVisibleMainMenu = true;
        UpdateRegionRect().updateRegionMain();
    }
    else if (numButton == -3) {
        G_BOOL_VISIBLE.isVisibleMainMenu = false;
        UpdateRegionRect().updateRegionMiniButton();
    }
    else if (numButton == 0) {
        G_BOOL_VISIBLE.isVisibleMenu = !G_BOOL_VISIBLE.isVisibleMenu;
        UpdateRegionRect().updateRegionMain();//нужно передать позицию а ообще нужно все осноные позиции при старте программы записать
    }
    else if (numButton == 2) {

        LoadManager(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[1].cmd }, false);
        // так как неработает убрано
       /* if (G_CONFIG_MAPS.path.size() <= 1) {
            std::chrono::file_time now = std::chrono::file_clock::now();
            std::filesystem::file_time_type threshold_time = now - std::chrono::seconds(10);

            LoadManager(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[1].cmd }, false);

            Sleep(1000);
            G_CONFIG_MAPS.path = SearchAfterCreatedFile(threshold_time).searchAfterCreatedFile1();
            size_t pos = G_CONFIG_MAPS.path.find(G_DATA_WARCRAFT.m_DataPath.warPathDirectSave);
            if (pos != std::wstring::npos) {
                G_CONFIG_MAPS.path.erase(pos, G_DATA_WARCRAFT.m_DataPath.warPathDirectSave.length());
            }
            ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).saveConfigMaps();
        }
        else {
            LoadManager(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[1].cmd }, false);
        }*/
    }
    else if (numButton == 1 || numButton >= 3 && numButton < 10) {
        LoadManager LoadManager_(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
        LoadManager_.sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[numButton - 1].cmd }, false);
    }
    else if (numButton >= 10 && numButton < 20) {
        LoadManager LoadManager_(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar);
        LoadManager_.sendLoadDataCommands({ G_CONFIG_MAPS.usersConfig[numButton - 10].cmd }, false);
    }
}

inline int OwnerWindow::mouseButtonPressed(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible)
{
    sf::Vector2f mouseButton = { static_cast<float>(event.x), static_cast<float>(event.y) };
    if (m_Buttons.isClicked(mouseButton)) {
        mouseButton.x = mouseButton.x - m_ButtonsMenu.getPosition().x;
        if (mouseButton.x < 19.84) { return 0; }
        else if (mouseButton.x < 39.68) { return 1; }
        else if (mouseButton.x < 59.52) { return 2; }
        else if (mouseButton.x < 79.36) { return 3; }
        else if (mouseButton.x < 99.2) { return 4; }
    }
    else if (m_ButtonsUsers[0].isClicked(mouseButton, G_CONFIG_MAPS.usersConfig[0].isVisibleButton)) { return 10; }
    else if (m_ButtonsUsers[1].isClicked(mouseButton, G_CONFIG_MAPS.usersConfig[1].isVisibleButton)) { return 11; }
    else if (m_ButtonsUsers[2].isClicked(mouseButton, G_CONFIG_MAPS.usersConfig[2].isVisibleButton)) { return 12; }
    else if (m_ButtonsUsers[3].isClicked(mouseButton, G_CONFIG_MAPS.usersConfig[3].isVisibleButton)) { return 13; }
    else if (m_ButtonsUsers[4].isClicked(mouseButton, G_CONFIG_MAPS.usersConfig[4].isVisibleButton)) { return 14; }
    else if (m_ShapeTrueVisibleMainMenu.isClicked(mouseButton)) { return -3; }
    else if (m_ShapeFalseVisibleMainMenu.getGlobalBounds().contains(mouseButton)) { return -4; }
    else if (m_CoutUserName.isClicked(mouseButton)) { return -5; }

    return -2;
}

void OwnerWindow::processingButtonMenu(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible) {
    if (!G_BOOL_VISIBLE.isVisibleMenu)
        return;

    switch (mouseButtonMenuPressed(event, isWindow2Visible)) {
    case -2: return;
    case -1: G_WINDOW.close(); return;
    case 0: {//загрузка последнего сохранения
        if (G_CONFIG_MAPS.path.size() >= 3) {
            G_BOOL_VISIBLE.isVisibleMenu = !G_BOOL_VISIBLE.isVisibleMenu;
            if (isWindow2Visible[0]) {
                isWindow2Visible[0] = false;
            }
            EngineFileTip1 EngineFileTip1_;
            if (EngineFileTip1_.engineFile()) {
                UpdateRegionRect().updateRegionLoad();

                if (G_CONFIG_MAIN.optionsConfig.blackColor) G_WINDOW.clear(sf::Color(45, 45, 48));
                else G_WINDOW.clear(sf::Color(255, 255, 255));
                draw(true);
                G_WINDOW.display();

                EngineFileTip1_.engineTip1();

                UpdateRegionRect().updateRegionMain();

                if (G_CONFIG_MAIN.optionsConfig.autoExit)
                    G_WINDOW.close();
            }

        }
        else if (!G_DATA_MAPS.m_NameMaps.empty()) {
            LogManager::logger().log(LogManager::LogLevel::Error, L"Путь до папки с сохранениями не указон. карта: " + G_DATA_MAPS.m_NameMapsFull);

            SelectingNewPathMap().selectingNewPathMap();

            if (G_CONFIG_MAPS.path.empty()) {
                LogManager::logger().log(LogManager::LogLevel::Message, L"новый путь до папки с сохранениями: " + G_CONFIG_MAPS.path);
            }
            else {
                LogManager::logger().log(LogManager::LogLevel::Message, L"Пользоатель невыбрал нувый путь до папки с сохранениями");
            }

            G_BOOL_VISIBLE.isVisibleMenu = !G_BOOL_VISIBLE.isVisibleMenu;
            UpdateRegionRect().updateRegionMain();
        }

        break;
    }
    case 1: {//список героев
        if (G_CONFIG_MAPS.path.size() >= 3) {
            isWindow2Visible[0] = !isWindow2Visible[0];
        }
        else if (!G_DATA_MAPS.m_NameMaps.empty()) {
            SelectingNewPathMap().selectingNewPathMap();
            G_BOOL_VISIBLE.isVisibleMenu = !G_BOOL_VISIBLE.isVisibleMenu;
            UpdateRegionRect().updateRegionMain();
        }
        break;
    }
    case 2: {//натройки
        Options().options();

        G_BOOL_VISIBLE.isVisibleMenu = !G_BOOL_VISIBLE.isVisibleMenu;
        isWindow2Visible[0] = false;
        UpdateRegionRect().updateRegionMain();


        if (G_CONFIG_MAIN.optionsConfig.blackColor) m_TextureButton.loadFromFile("img\\ButtonBlack.png");
        else m_TextureButton.loadFromFile("img\\Button.png");

        break;
    }
    case 3: {//вывод гайда
        isWindow2Visible[0] = false;
        UpdateRegionRect().updateRegionMain();
        UpdateWinow2();

        //if (m_CoutGuide.isActive)
        //    m_CoutGuide = new CoutGuide();
        //else {
        //    m_CoutGuide->exitGuide();
        //    delete m_CoutGuide;
        //}
        //G_WINDOW.clear(sf::Color(0, 255, 0));
        //G_WINDOW.display();
        //CoutGuide().coutGuide();

        break;
    }
    };
}

inline int OwnerWindow::mouseButtonMenuPressed(const sf::Event::MouseButtonEvent& event, std::array<bool, 2>& isWindow2Visible)
{
    sf::Vector2f mouseButton = { static_cast<float>(event.x), static_cast<float>(event.y) };
    if (!m_ButtonsMenu.isClicked(mouseButton))
        return -2;

    mouseButton.y = mouseButton.y - m_ButtonsMenu.getPosition().y;
    if (mouseButton.y < 19.84) { return -1; }
    else if (mouseButton.y < 39.68) { return 3; }
    else if (mouseButton.y < 59.52) { return 2; }
    else if (mouseButton.y < 79.36) { return 1; }
    else if (mouseButton.y < 99.2) { return 0; }

    return -2;

}

void OwnerWindow::processingGuide() {
     m_CoutGuide.processEvents();
}

const bool OwnerWindow::getCoutGuideActive() const { return m_CoutGuide.isActive; }

inline void OwnerWindow::initializeButtonsUsersDataCommands() {
    m_ButtonsUsers[0].initialize({ 128, 128, 128, 128 }, m_TextureButton);
    m_ButtonsUsers[1].initialize({ 256, 128, 128, 128 }, m_TextureButton);
    m_ButtonsUsers[2].initialize({ 384, 128, 128, 128 }, m_TextureButton);
    m_ButtonsUsers[3].initialize({ 512, 128, 128, 128 }, m_TextureButton);
    m_ButtonsUsers[4].initialize({ 640, 128, 128, 128 }, m_TextureButton);
}

// Функция для установки окна поверх всех других окон
static void setAlwaysOnTop(const HWND& hwnd) {
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void OwnerWindow::setupWindow() {
    // Создаем окно без рамки
    G_WINDOW.create(sf::VideoMode(
        sf::VideoMode::getDesktopMode().width-2,
        sf::VideoMode::getDesktopMode().height-2),
        "AutoLoadWar3", sf::Style::None);

    // Получаем хэндл окна SFML
    HWND hwnd = G_WINDOW.getSystemHandle();
    activeGameFalse();

    G_WINDOW.setPosition(sf::Vector2i(1, 1));
    UpdateWindow(hwnd);
    // Устанавливаем окно "поверх всех окон" без активации
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE );

    //Устанавливаем окно поверх всех других окон
    setAlwaysOnTop(hwnd);

    // Запускаем таймер в отдельном потоке для периодического обновления окна
    std::thread([hwnd]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            setAlwaysOnTop(hwnd);
        }
        }).detach();
}

void OwnerWindow::activeGameTrue(const HWND& hWndWindow) {
    G_WINDOW.setFramerateLimit(20);
    if(G_BOOL_VISIBLE.isVisibleMainMenu)
        UpdateRegionRect().updateRegionMain();
    else
        UpdateRegionRect().updateRegionMiniButton();
    m_CoutGuide.m_Window.setVisible(true);
}

void OwnerWindow::updateRect(const HWND& hWndWindow) {

    const float x = G_DATA_WARCRAFT.m_DataRect.size.x / 2.f - 50;//-50-8
    const float y = (G_DATA_WARCRAFT.m_DataRect.size.y / 20.0f) * 15.75f - 10;//-10-31

    sf::Vector2f newPosition = sf::Vector2f(x, y);
    const sf::Vector2f windowPoition = sf::Vector2f(static_cast<float>(G_DATA_WARCRAFT.m_DataRect.position.x), static_cast<float>(G_DATA_WARCRAFT.m_DataRect.position.y));
    const sf::Vector2f windowWidthHeight = sf::Vector2f(static_cast<float>(G_DATA_WARCRAFT.m_DataRect.size.x), static_cast<float>(G_DATA_WARCRAFT.m_DataRect.size.y));
    updatePosition(newPosition, windowPoition, windowWidthHeight);
}

void OwnerWindow::activeGameFalse() {
    UpdateRegionRect().clearRegion();
    m_CoutGuide.m_Window.setVisible(false);
    G_WINDOW.setFramerateLimit(3);
}

void OwnerWindow::UpdateWinow2()
{
    if (m_CoutGuide.isActive) {
        m_CoutGuide.isActive = false;
        m_CoutGuide.m_Window.close();
    }
    else {
        m_CoutGuide.isActive = true;
        m_CoutGuide.initializeWindow();
        m_CoutGuide.newGame();
    }
}

void OwnerWindow::updatePosition(const sf::Vector2f& newPosition, const sf::Vector2f& windowPoition, const sf::Vector2f& windowWidthHeight)
{
    m_SpriteIsLoad.setPosition(sf::Vector2f(windowWidthHeight.x / 2 - 96.f, windowWidthHeight.y / 2 - 70.f));

    m_ShapeFalseVisibleMainMenu.setPosition(sf::Vector2f(windowWidthHeight.x / 2 - 5, 2.f) + windowPoition);

    sf::Vector2f sum = /*sf::Vector2f(0,-1) +*/ newPosition + windowPoition;
    m_Buttons.setPosition2(sum);

    for (unsigned i = 0; ButtonUI & button : m_ButtonsUsers) {
        //if (G_CONFIG_MAPS.usersConfig[i].isVisibleButton) {
            button.setPosition1(sum.x - (++i * m_ConstSize), sum.y);
        //}
    }

    {
        m_ButtonsMenu.setPosition1(sum.x, /*1.0f - */sum.y - 100.f);

        m_ShapeTrueVisibleMainMenu.setPosition2(sf::Vector2f(sum.x, 20.f + /*1.0f + */sum.y));

        m_CoutUserName.setPosition2(sf::Vector2f(sum.x, 40.f + /*1.0f + */sum.y));
    }

}




void OwnerWindow::setIsVisibleMenu(const bool t_IsVisibleMenu) { G_BOOL_VISIBLE.isVisibleMenu = t_IsVisibleMenu; }
const bool OwnerWindow::getIsVisibleMenu() const { return G_BOOL_VISIBLE.isVisibleMenu; }

