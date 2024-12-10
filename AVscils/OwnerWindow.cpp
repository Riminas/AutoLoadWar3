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
#include "NewDirectory.h"
#include "SkillsUpgradeStart.h"
#include "ConfigMaps.h"
#include "ConfigMain.h"
#include "StringConvector.h"
#include "DataAll.h"
#include "SearchAfterCreatedFile.h"
#include "ConfigMapsEngine.h"
#include "CoutGuide.h"
#include "SelectingNewPathMap.h"
#include "LogError.h"

void OwnerWindow::initialize() {

    setupWindow();
    if (G_CONFIG_MAIN.optionsConfig.blackColor) m_TextureButton.loadFromFile("DataAutoLoad\\img\\ButtonBlack.png");
    else m_TextureButton.loadFromFile("DataAutoLoad\\img\\Button.png");

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

void OwnerWindow::draw(const bool t_isVisibleLoad) {

    if (t_isVisibleLoad) {
        LogError().logError("Thil_1_1");
        G_WINDOW.draw(m_SpriteIsLoad);
        return;
    }

    if (!m_IsVisibleMainMenu) {
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
    
    if (!m_IsVisibleMenu)
        return;

    m_ButtonsMenu.draw(G_WINDOW);
    m_ShapeTrueVisibleMainMenu.draw(G_WINDOW);
    m_CoutUserName.draw(G_WINDOW);
}

void OwnerWindow::processingButton(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[]) {

    const int numButton = mouseButtonPressed(event, isWindow2Visible);

    if (numButton == -5) {
        LoadManager(G_DATA_PATH.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAIN.playerName }, false);
    }
    else if (numButton == -4) {
        m_IsVisibleMainMenu = true;
        updateRegion(G_DATA_PATH.hWndWindowWar, 0u);
    }
    else if (numButton == -3) {
        m_IsVisibleMainMenu = false;
        updateRegion(G_DATA_PATH.hWndWindowWar, 3u);
    }
    else if (numButton == 0) {
        m_IsVisibleMenu = !m_IsVisibleMenu;
        updateRegion(G_DATA_PATH.hWndWindowWar);
    }
    else if (numButton == 2) {
        if (G_CONFIG_MAPS.path.size() <= 1) {
            std::chrono::file_time now = std::chrono::file_clock::now();
            std::filesystem::file_time_type threshold_time = now - std::chrono::seconds(10);

            LoadManager(G_DATA_PATH.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[1].cmd }, false);

            Sleep(1000);
            G_CONFIG_MAPS.path = SearchAfterCreatedFile(threshold_time).searchAfterCreatedFile1();
            size_t pos = G_CONFIG_MAPS.path.find(G_DATA_PATH.warPathDirectSave);
            if (pos != std::wstring::npos) {
                G_CONFIG_MAPS.path.erase(pos, G_DATA_PATH.warPathDirectSave.length());
            }
            ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).saveConfigMain();
        }
        else {
            LoadManager(G_DATA_PATH.hWndWindowWar).sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[1].cmd }, false);
        }
    }
    else if (numButton == 1 || numButton >= 3 && numButton < 10) {
        LoadManager LoadManager_(G_DATA_PATH.hWndWindowWar);
        LoadManager_.sendLoadDataCommands({ G_CONFIG_MAPS.mainConfig[numButton - 1].cmd }, false);
    }
    else if (numButton >= 10 && numButton < 20) {
        LoadManager LoadManager_(G_DATA_PATH.hWndWindowWar);
        LoadManager_.sendLoadDataCommands({ G_CONFIG_MAPS.usersConfig[numButton - 10].cmd }, false);
    }
}

inline int OwnerWindow::mouseButtonPressed(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[])
{
    sf::Vector2f mouseButton = { static_cast<float>(event.x), static_cast<float>(event.y) };
    if (m_Buttons.isClicked(mouseButton)) {
        mouseButton.x = mouseButton.x - m_ButtonsMenu.getPositionX();
        if (mouseButton.x >= 0 && mouseButton.x < 19.84) { return 0; }
        else if (mouseButton.x >= 19.84 && mouseButton.x < 39.68) { return 1; }
        else if (mouseButton.x >= 39.68 && mouseButton.x < 59.52) { return 2; }
        else if (mouseButton.x >= 59.52 && mouseButton.x < 79.36) { return 3; }
        else if (mouseButton.x >= 79.36 && mouseButton.x < 99.2) { return 4; }
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

void OwnerWindow::processingButtonMenu(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[]) {
    if (!m_IsVisibleMenu)
        return;

    switch (mouseButtonMenuPressed(event, isWindow2Visible)) {
    case -2: return;
    case -1: G_WINDOW.close(); return;
    case 0: {//загрузка последнего сохранения
        if (!G_CONFIG_MAPS.path.empty()) {
            m_IsVisibleMenu = !m_IsVisibleMenu;
            if (isWindow2Visible[0]) {
                isWindow2Visible[0] = false;
            }

            LogError().logError("Thil_1");
            updateRegion(G_DATA_PATH.hWndWindowWar, 1u);

            LogError().logError("Thil_2");
            G_WINDOW.clear(sf::Color(255, 255, 255));
            draw(true);
            G_WINDOW.display();
            LogError().logError("Thil_3");

            EngineFileTip1().engineFile();
            LogError().logError("Thil_4");

            updateRegion(G_DATA_PATH.hWndWindowWar, 0u);
            LogError().logError("Thil_5");

            if (G_CONFIG_MAIN.optionsConfig.autoExit)
                G_WINDOW.close();
        }
        else {
            LogError().logError("G_CONFIG_MAPS.path.empty()");
            SelectingNewPathMap().selectingNewPathMap();
            m_IsVisibleMenu = !m_IsVisibleMenu;
            updateRegion(G_DATA_PATH.hWndWindowWar, 0u);
        }

        break;
    }
    case 1: {//список героев
        if (!G_CONFIG_MAPS.path.empty()) {
            isWindow2Visible[0] = !isWindow2Visible[0];
            updateRegion(G_DATA_PATH.hWndWindowWar, 0u);
        }
        else {
            SelectingNewPathMap().selectingNewPathMap();
            m_IsVisibleMenu = !m_IsVisibleMenu;
            updateRegion(G_DATA_PATH.hWndWindowWar, 0u);
        }
        break;
    }
    case 2: {//натройки
        NewDataAll().newMaps(true, true);
        m_IsVisibleMenu = !m_IsVisibleMenu;
        isWindow2Visible[0] = false;
        updateRegion(G_DATA_PATH.hWndWindowWar, 0u);

        if (G_CONFIG_MAIN.optionsConfig.blackColor) m_TextureButton.loadFromFile("DataAutoLoad\\img\\ButtonBlack.png");
        else m_TextureButton.loadFromFile("DataAutoLoad\\img\\Button.png");
        break;
    }
    case 3: {//вывод гайда
        isWindow2Visible[0] = false;
        updateRegion(G_DATA_PATH.hWndWindowWar, 0u);
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

inline int OwnerWindow::mouseButtonMenuPressed(const sf::Event::MouseButtonEvent& event, bool isWindow2Visible[])
{
    sf::Vector2f mouseButton = { static_cast<float>(event.x), static_cast<float>(event.y) };
    if (!m_ButtonsMenu.isClicked(mouseButton))
        return -2;

    mouseButton.y = mouseButton.y - m_ButtonsMenu.getPositionY();
    if (mouseButton.y >= 0 && mouseButton.y < 19.84) { return -1; }
    else if (mouseButton.y >= 19.84 && mouseButton.y < 39.68) { return 3; }
    else if (mouseButton.y >= 39.68 && mouseButton.y < 59.52) { return 2; }
    else if (mouseButton.y >= 59.52 && mouseButton.y < 79.36) { return 1; }
    else if (mouseButton.y >= 79.36 && mouseButton.y < 99.2) { return 0; }

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
static void setAlwaysOnTop(const HWND& hwnd)  {
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void OwnerWindow::setupWindow() {
    //G_WINDOW.create(sf::VideoMode(
    //    sf::VideoMode::getDesktopMode().width - 2,
    //    sf::VideoMode::getDesktopMode().height - 2),
    //    "AutoLoads", sf::Style::None);

    //HWND hwnd = G_WINDOW.getSystemHandle();

    //SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    //SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    //SetLayeredWindowAttributes(hwnd, RGB(0, 255, 0), 0, LWA_COLORKEY);

    // Создаем окно без рамки
    G_WINDOW.create(sf::VideoMode(
        sf::VideoMode::getDesktopMode().width-2,
        sf::VideoMode::getDesktopMode().height-2),
        "AutoLoads", sf::Style::None);

    // Получаем хэндл окна SFML
    HWND hwnd = G_WINDOW.getSystemHandle();
    activeGameFalse();

    G_WINDOW.setPosition(sf::Vector2i(1, 1));
    // Устанавливаем окно "поверх всех окон" без активации
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    // Применяем WS_EX_NOACTIVATE для предотвращения активации
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);

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

void OwnerWindow::updateRegion(const HWND& hWndWindow, const uint8_t t_NumMenu) const {
    HWND hwnd = G_WINDOW.getSystemHandle();

    HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

    RECT rectClient;
    RECT rectWindow;
    if (GetClientRect(hWndWindow, &rectClient) && GetWindowRect(hWndWindow, &rectWindow)) {
        const int width = rectClient.right - rectClient.left;
        const int height = rectClient.bottom - rectClient.top;

        switch (t_NumMenu) 
        {
            case 1:// сообщение об загрузке
            {
                const int widthCenter = width / 2;
                const int heightCenter = height / 2;
                HRGN loadBar = CreateRectRgn(
                    widthCenter - 96,
                    heightCenter - 70,
                    widthCenter + 96,
                    heightCenter - 32);
                CombineRgn(region, region, loadBar, RGN_OR);
                DeleteObject(loadBar);
                break;
            }
            //case 2:// меню настроек
            //{
            //    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
            //    float windowHeight = desktop.height * 2.f / 3.f + 50;
            //    HRGN OptionBar = CreateRectRgn(
            //        static_cast<float>(desktop.width) / 2.f - 256.f,
            //        static_cast<float>(desktop.height) / 2.f - windowHeight / 2.f-32,
            //        static_cast<float>(desktop.width) / 2.f + 256.f,
            //        static_cast<float>(desktop.height) / 2.f + windowHeight / 2.f);
            //    CombineRgn(region, region, OptionBar, RGN_OR);
            //    DeleteObject(OptionBar);
            //    break;
            //}
            case 3://кнопка(показачать оновные кнопки)(разернуть)
            {
                RECT rectWindow;
                GetWindowRect(hWndWindow, &rectWindow);
                const int width = rectWindow.left + (rectClient.right - rectClient.left) / 2;
                HRGN loadBar = CreateRectRgn(
                    width - 5,
                    rectWindow.top + 2,
                    width + 5,
                    rectWindow.top + 10);
                CombineRgn(region, region, loadBar, RGN_OR);
                DeleteObject(loadBar);
                break;
            }
            case 0:
            default:
            {
                const auto& cmd = G_CONFIG_MAPS.usersConfig;
                const int sum = 20 * (cmd[0].isVisibleButton + cmd[1].isVisibleButton + cmd[2].isVisibleButton + cmd[3].isVisibleButton + cmd[4].isVisibleButton);
                const int x = width / 2 - 50;
                const int y = static_cast<int>((height / static_cast<float>(20)) * 15.75f - 9);
                const int sumX = x + rectWindow.left;
                const int sumY = y + rectWindow.top;

                {
                    HRGN mainHorizontalBar = CreateRectRgn(
                        sumX - sum,
                        sumY,
                        sumX + 100,
                        sumY + 20);
                    CombineRgn(region, region, mainHorizontalBar, RGN_OR); // Добавляем горизонтальную часть
                    DeleteObject(mainHorizontalBar);
                }
                {
                    if (m_IsVisibleMenu) {
                        HRGN mainVerticalBar = CreateRectRgn(
                            sumX,
                            sumY - 100,
                            sumX + 20,
                            sumY + 60);
                        CombineRgn(region, region, mainVerticalBar, RGN_OR); // Добавляем вертикальную часть
                        DeleteObject(mainVerticalBar);
                    }
                }
                break;
            }
        };

        SetWindowRgn(hwnd, region, TRUE);
    }
    else {
        SetWindowRgn(hwnd, region, TRUE);
    }
}

void OwnerWindow::activeGameTrue(const HWND& hWndWindow) {
    G_WINDOW.setFramerateLimit(20);
    if(m_IsVisibleMainMenu)
        updateRegion(hWndWindow);
    else
        updateRegion(hWndWindow, 3);
}

void OwnerWindow::updateRect(const HWND& hWndWindow) {
    RECT rectClient;
    RECT rectWindow;
    if (GetClientRect(hWndWindow, &rectClient) && GetWindowRect(hWndWindow, &rectWindow)) {
        const int width = rectClient.right - rectClient.left;
        const int height = rectClient.bottom - rectClient.top;

        const float x = width / 2.f - 50;
        const float y = (height / 20.0f) * 15.75f - 10;

        sf::Vector2f newPosition = sf::Vector2f(x, y);
        const sf::Vector2f windowPoition = sf::Vector2f(static_cast<float>(rectWindow.left), static_cast<float>(rectWindow.top));
        const sf::Vector2f windowWidthHeight = sf::Vector2f(static_cast<float>(width), static_cast<float>(height));
        updatePosition(newPosition, windowPoition, windowWidthHeight);
    }
}

void OwnerWindow::activeGameFalse() {


    // Получаем хэндл окна SFML
    HWND hwnd = G_WINDOW.getSystemHandle();
    HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования
    SetWindowRgn(hwnd, region, TRUE);

    G_WINDOW.clear(sf::Color(255, 255, 255));
    G_WINDOW.display();

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




void OwnerWindow::setIsVisibleMenu(const bool t_IsVisibleMenu) { m_IsVisibleMenu = t_IsVisibleMenu; }
const bool OwnerWindow::getIsVisibleMenu() const { return m_IsVisibleMenu; }

