#include "SelectingNewPathMap.h"
#include "Global.h"
#include <functional>
#include <filesystem>
#include "DataWarcraft.h"

#include "ConfigMapsEngine.h"
#include "DataMaps.h"
#include <unordered_set>
#include "LogError.h"
#include "ConfigMain.h"
#include "UpdateRegionRect.h"




SelectingNewPathMap::SelectingNewPathMap()
    : rootDirectory(L"CustomMapData", G_DATA_WARCRAFT.m_DataPath.warPathDirectSave),
    scrollOffset(0),
    isScrolling(false),
    scrollSpeed(1.0f),
    openDirectoryEntry(rootDirectory)
{
}

void SelectingNewPathMap::selectingNewPathMap() {
    // Открытие диалогового окна для выбора папки и получение пути к выбранной папке
    initialize();
    UpdateRegionRect().updateRegion(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar, 2);//пернести перед очисткой экрана
    std::wstring folderPath = run();
    UpdateRegionRect().updateRegion(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar, -1);// перенести ближе к выводу
    // Проверка, пуст ли путь
    if (folderPath == L"Exit") {
        return;//2
    }

    // Удаление подстроки "C:\\Users\\Aio\\Documents\\Warcraft III\\CustomMapData" из пути
    size_t pos = folderPath.find(rootDirectory.fullPath);
    if (pos != std::wstring::npos) {
        folderPath.erase(pos, rootDirectory.fullPath.length());
    }

    // Проверка, пуст ли оставшийся путь
    if (folderPath.empty()) {
        if(G_CONFIG_MAIN.optionsConfig.writeLogs)
            LogError().logMessage("Новый путь к папке с сохранениями пуст");
        G_CONFIG_MAPS.path.clear();
        return;//0
    }

    G_CONFIG_MAPS.path = folderPath;
    ConfigMapsEngine(G_DATA_MAPS.m_NameMaps).saveConfigMaps();
    return; //1 Успешное выполнение
}


inline void SelectingNewPathMap::initializeText(sf::Text& text, const std::wstring& textString, const sf::Vector2f& position, const sf::Color& color, const float& letterSpacing) const {
    text.setFont(G_FONT_STANDART);
    text.setCharacterSize(16);
    text.setLetterSpacing(letterSpacing);;

    text.setString(textString);
    text.setPosition(position);
    text.setFillColor(color);
}

inline void SelectingNewPathMap::initializeRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color) const {
    rectangle.setSize(size);
    rectangle.setPosition(position);
    rectangle.setFillColor(color);
}

inline void SelectingNewPathMap::initializeSprite(sf::Sprite& rectangle, const sf::Vector2f& position, const sf::IntRect& rect) const {
    rectangle.setTexture(m_Texture);
    rectangle.setTextureRect(rect);
    rectangle.setPosition(position);
}

void SelectingNewPathMap::initialize() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float windowWidth = 512;
    float windowHeight = desktop.height * 2.f / 3.f + 50;

    if (m_Rect[1] == 0) {
        m_Rect[0]/*.left*/ = static_cast<float>(desktop.width / 2 - 256);
        m_Rect[1]/*.top*/ = static_cast<float>(desktop.height / 2.f - windowHeight / 2);
        m_Rect[2]/*.right*/ = static_cast<float>(desktop.width / 2 + 256);
        m_Rect[3]/*.bottom*/ = static_cast<float>(desktop.height / 2.f + windowHeight / 2 - 32);
    }

    m_Texture.loadFromFile("DataAutoLoad\\img\\Option.png");
    // Общее меню
    initializeSprite(titleAndClose, sf::Vector2f{ m_Rect[0],  m_Rect[1] - 48 }, { 0, 448, 512, 16 });
    //initializeText(title, L"Maps: " + G_DATA_MAPS.m_NameMapsFull, newPosition + sf::Vector2f{ 20, -24 });

    {
        initializeSprite(background, sf::Vector2f{ m_Rect[0], m_Rect[1] }, { 0, 288, 512, 32 });
        sf::Vector2f targetSize(512, m_Rect[3] - m_Rect[1]);
        background.setScale(1, targetSize.y / 32);
    }

    initializeSprite(topBackground, sf::Vector2f{ m_Rect[0], m_Rect[1] - 32 }, { 0, 480, 512, 32 });
    initializeSprite(bottomBackground, sf::Vector2f{ m_Rect[0],  m_Rect[3] }, { 0, 384, 512, 32 });

    // Меню выбора директории
    initializeScrollbar(windowWidth, windowHeight);

    bool isPathSaveCodeFalse = { !G_CONFIG_MAPS.path.empty() };
    loadDirectories(rootDirectory.fullPath, rootDirectory, isPathSaveCodeFalse);

    updateDirectoryTexts();

    G_WINDOW.setFramerateLimit(60);
}

void SelectingNewPathMap::initializeScrollbar(const float& windowWidth, const float& windowHeight)
{
    scrollbar.setSize(sf::Vector2f(5, windowHeight - 32));
    scrollbar.setPosition(sf::Vector2f(m_Rect[2] - 9, m_Rect[1]));
    scrollbarThumb.setFillColor(sf::Color::White);//sf::Color(28, 28, 28)
    scrollbarThumb.setPosition(scrollbar.getPosition());

    backgroundDirectory.setFillColor(sf::Color(0, 60, 160, 80));
    backgroundDirectory.setSize(sf::Vector2f(windowWidth, 30));

    rootDirectory.isOpen = true;
}

void SelectingNewPathMap::loadDirectories(const std::wstring& directoryPath, DirectoryEntry& parent, bool& isPathSaveCodeFalse)
{
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_directory() && isPushDirectory(entry.path().wstring(), entry.path().filename().wstring())) {
            std::wstring name = entry.path().filename().wstring();

            //StringConvector StringConvector_;
            //const float maxWidth = background.getSize().x - 38; // Максимальная ширина текста
            //StringConvector_.adjustTextToFit(dirText, maxWidth);


            if (name.size() > 35) {
                name.erase(35);
                name += L" ...";
            }
            std::wstring fullPath = entry.path().wstring();
            DirectoryEntry subDir(name, fullPath);
            parent.subDirectories.push_back(subDir);

            if (isPathSaveCodeFalse) {
                const std::wstring& wstr = G_CONFIG_MAPS.path;
                std::wstring wstr2 = subDir.fullPath;
                // Удаление подстроки "C:\\Users\\Aio\\Documents\\Warcraft III\\CustomMapData" из пути
                size_t pos = wstr2.find(rootDirectory.fullPath);
                if (pos != std::wstring::npos) {
                    wstr2.erase(pos, rootDirectory.fullPath.length());
                }
                if (wstr2 == wstr) {
                    isPathSaveCodeFalse = false;
                    openDirectoryEntry = subDir;
                    isBackgroundDirectory = true;
                }
            }

            loadDirectories(fullPath, parent.subDirectories.back(), isPathSaveCodeFalse);
        }
    }
}


void SelectingNewPathMap::updateDirectoryTexts() {
    isBackgroundDirectory = false;
    directoryTexts.clear();
    directoryTexts.reserve(20);
    directoryTriangles.clear();
    directoryTriangles.reserve(20);

    float yOffset = 8.f + scrollOffset; // Применяем scrollOffset при расчете yOffset
    std::function<void(const DirectoryEntry&, float)> addTexts = [&](const DirectoryEntry& dir, float indent) {
        if (dir.isOpen) {
            for (const auto& subDir : dir.subDirectories) {
                sf::Text dirText(subDir.name, G_FONT_STANDART, 14);
                dirText.setFillColor(sf::Color::White);
                dirText.setPosition(sf::Vector2f(38 + indent, yOffset + m_Rect[1])); // Смещение текста для размещения треугольника
                directoryTexts.push_back(dirText);

                if (!subDir.subDirectories.empty()) { // Проверяем, есть ли подкаталоги
                    sf::Sprite triangle; // Создаем треугольник
                    triangle.setPosition(sf::Vector2f(20 + indent, 2 + yOffset + m_Rect[1]));//index * 20
                    triangle.setTexture(m_Texture);
                    triangle.setScale(0.4f, 0.4f);

                    if (subDir.isOpen) {
                        triangle.setTextureRect(sf::Rect{ 32, 608, 32, 32 });
                    }
                    else {
                        triangle.setTextureRect(sf::Rect{ 0, 608, 32, 32 });
                    }
                    directoryTriangles.push_back(triangle); // Добавляем треугольник
                }

                if (isBackgroundDirectory == false && subDir.fullPath == openDirectoryEntry.fullPath && (yOffset + m_Rect[1] >= 60 && yOffset <= m_Rect[3] - 60)) {
                    backgroundDirectory.setPosition(sf::Vector2f(m_Rect[0], m_Rect[1] + yOffset - 6));
                    isBackgroundDirectory = true;
                }

                yOffset += 30.f;
                addTexts(subDir, indent + 20.f);//inex + 1
            }
        }
        };
    addTexts(rootDirectory, m_Rect[0]);
    adjustScrollbar();
}

void SelectingNewPathMap::adjustScrollbar() {
    float visibleArea = (m_Rect[3] - m_Rect[1] - 72) / 30;//120 > 72
    float thumbSize = scrollbar.getSize().y * (visibleArea / directoryTexts.size());
    thumbSize = (std::max)(thumbSize, 20.0f);
    scrollbarThumb.setSize(sf::Vector2f(5, thumbSize));

    // Пересчитываем позицию thumb после изменения его размера
    float scrollbarRange = scrollbar.getSize().y - scrollbarThumb.getSize().y;
    float scrollableArea = 30.0f * (directoryTexts.size() - visibleArea);
    float thumbPosition = (std::abs(scrollOffset) / scrollableArea) * scrollbarRange;
    thumbPosition = std::clamp(thumbPosition, 0.0f, scrollbarRange);
    scrollbarThumb.setPosition(sf::Vector2f(scrollbar.getPosition().x, scrollbar.getPosition().y + thumbPosition));
}

void SelectingNewPathMap::drawWindow() {
    G_WINDOW.clear(sf::Color(255, 255, 255));
    G_WINDOW.draw(background);

    for (const auto& dirText : directoryTexts) { // Сначала рисуем тексты
        if (dirText.getPosition().y >= m_Rect[1] - 32 && dirText.getPosition().y <= m_Rect[3]) {
            G_WINDOW.draw(dirText);
        }
    }

    for (const auto& triangle : directoryTriangles) { // Затем рисуем треугольники
        if (triangle.getPosition().y >= m_Rect[1] - 32 && triangle.getPosition().y + 16 <= m_Rect[3]) {
            G_WINDOW.draw(triangle);
        }
    }

    if (isBackgroundDirectory)
        if (backgroundDirectory.getPosition().y >= m_Rect[1] - 30 && backgroundDirectory.getPosition().y <= m_Rect[3])
            G_WINDOW.draw(backgroundDirectory);


    //G_WINDOW.draw(scrollbar);
    G_WINDOW.draw(scrollbarThumb);
    G_WINDOW.draw(topBackground);
    G_WINDOW.draw(bottomBackground);
    G_WINDOW.draw(titleAndClose);

    G_WINDOW.display();
}

std::wstring SelectingNewPathMap::handleMousePress(sf::Event& event) {
    const sf::Vector2f mouseButton = { static_cast<float>(sf::Mouse::getPosition(G_WINDOW).x), static_cast<float>(sf::Mouse::getPosition(G_WINDOW).y) };

    if (scrollbarThumb.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        isScrolling = true;
        mouseOffset = mouseButton.y - scrollbarThumb.getPosition().y;
        return L"";
    }
    if (bottomBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        const sf::Vector2f mousePositions = mouseButton - bottomBackground.getPosition();
        if (openDirectoryEntry.fullPath.empty() || mouseButton.x <= 256)
            return L"Exit";
        else {
            return newPath();
        }
    }
    if (titleAndClose.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        const sf::Vector2f mouseButton2{ mouseButton.x - bottomBackground.getPosition().x, mouseButton.y - bottomBackground.getPosition().y };
        if (mouseButton2.x >= 480/* && mouseButton2.x < 512*/)
            return L"Exit";
    }

    float yOffset = 8.f + scrollOffset;

    std::function<bool(DirectoryEntry&, int)> processClick = [&](DirectoryEntry& dir, int indent) {
        if (dir.isOpen) {
            const sf::Vector2f mouseButton2{ mouseButton.x, mouseButton.y };
            for (auto& subDir : dir.subDirectories) {
                sf::FloatRect textBounds(m_Rect[0], yOffset + m_Rect[1], m_Rect[2] - m_Rect[0] - 60, 30);

                if (!subDir.subDirectories.empty()) {
                    sf::FloatRect triangleBounds(10 + indent * 20 + m_Rect[0], yOffset + m_Rect[1], 25, 25);
                    if (triangleBounds.contains(mouseButton2.x, mouseButton2.y)) {
                        toggleDirectory(subDir);
                        return true;
                    }
                }
                if (mouseButton2.y <= bottomBackground.getPosition().y && textBounds.contains(mouseButton2.x, mouseButton2.y)) {
                    openDirectoryEntry = subDir;
                    subDir.isOpen = true;
                    updateDirectoryTexts();
                    return true;
                }

                yOffset += 30;
                if (processClick(subDir, indent + 1)) return true;
            }
        }
        return false;
        };

    processClick(rootDirectory, 0);

    return L"";
}

void SelectingNewPathMap::handleScroll(sf::Event& event) {
    float delta = event.mouseWheelScroll.delta;
    scrollOffset += delta * scrollSpeed * 30;
    scrollOffset = std::clamp(scrollOffset, { -30.0f * (directoryTexts.size() - (m_Rect[3] - m_Rect[1] - 72) / 30) }, 0.0f);//120 > 72
    updateDirectoryTexts();
}

void SelectingNewPathMap::handleMouseMove(sf::Event& event) {
    if (isScrolling) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(G_WINDOW);

        float scrollbarRange = scrollbar.getSize().y - scrollbarThumb.getSize().y;
        float scrollableArea = 30.0f * directoryTexts.size() - (m_Rect[3] - m_Rect[1] - 72);//120 > 72

        // Используем сохраненное смещение при расчете позиции
        float thumbPosition = mousePosition.y - scrollbar.getPosition().y - mouseOffset;
        thumbPosition = std::clamp(thumbPosition, 0.0f, scrollbarRange);
        scrollbarThumb.setPosition(sf::Vector2f(scrollbar.getPosition().x + m_Rect[0], scrollbar.getPosition().y + thumbPosition + m_Rect[1]));

        scrollOffset = -(thumbPosition / scrollbarRange) * scrollableArea;
        updateDirectoryTexts();
    }
}

std::wstring SelectingNewPathMap::run() {
    bool isActive = true;
    while (G_WINDOW.isOpen()) {

        // Получаем дескриптор активного окна
        HWND hWndWindow = GetForegroundWindow();

        //HWND hWndWindow = FindWindow(NULL, L"Warcraft III");
        if (IsWindowInFocus(hWndWindow)) {
            SetWindowPos(G_WINDOW.getSystemHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            if (isActive == false) {
                if (G_DATA_WARCRAFT.m_DataPath.hWndWindowWar != hWndWindow)
                    return std::wstring();
                isActive = true;
                UpdateRegionRect().updateRegion(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar, 2);//пернести перед очисткой экрана
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
                case sf::Event::MouseWheelScrolled:
                    handleScroll(event);
                    break;
                case sf::Event::MouseMoved:
                    handleMouseMove(event);
                    break;
                case sf::Event::MouseButtonReleased:
                    isScrolling = false;
                    break;
                default:
                    break;
                }
            }
            drawWindow();
        }
        else {
            if (isActive) {
                UpdateRegionRect().updateRegion(G_DATA_WARCRAFT.m_DataPath.hWndWindowWar, -1);// перенести ближе к выводу
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

void SelectingNewPathMap::toggleDirectory(DirectoryEntry& directory) {
    directory.isOpen = !directory.isOpen;
    updateDirectoryTexts();
}

std::wstring SelectingNewPathMap::newPath() {
    std::wstring ownerPath = L"\0";
    ownerPath = openDirectoryEntry.fullPath;

    if (openDirectoryEntry.subDirectories.size() == 1) {
        bool isWhile = true;
        while (isWhile) {
            for (const auto& entry : std::filesystem::directory_iterator(ownerPath)) {
                if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt") {
                    isWhile = false;
                    break;
                }
                else if (std::filesystem::is_directory(entry)) {
                    for (const auto& entry : std::filesystem::directory_iterator(entry)) {
                        if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt") {
                            isWhile = false;
                            break;
                        }
                    }
                }

            }
            if (isWhile) {
                openDirectoryEntry = openDirectoryEntry.subDirectories[0];
                ownerPath = openDirectoryEntry.fullPath;
            }
        }
    }
    return ownerPath;
}




bool SelectingNewPathMap::IsWindowInFocus(const HWND& hWnd) const {
    if (hWnd == NULL) {
        //std::wcout << L"Нет активного окна!" << std::endl;
        return 0;
    }

    // Буфер для имени окна
    wchar_t windowTitle[256];

    // Получаем имя окна
    int length = GetWindowText(hWnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
    if (length == 0) {
        //std::wcout << L"Не удалось получить имя окна!" << std::endl;
        return 0;
    }

    // Сравниваем имя окна с ожидаемым
    if (wcscmp(windowTitle, L"Warcraft III") != 0) {
        //std::wcout << L"Имя активного окна не совпадает с 'Warcraft III'" << windowTitle << std::endl;
        return 0;
    }

    //std::wcout << windowTitle << std::endl;


    return true;
}

bool SelectingNewPathMap::isPushDirectory(const std::wstring& fullPath, const std::wstring& nameDirectory) const {
    const std::unordered_set<std::wstring> infoSkipDirectory = { L"Errors", L"unifont" };
    for (const auto& entry : std::filesystem::recursive_directory_iterator(fullPath)) {
        if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt" && !infoSkipDirectory.contains(nameDirectory))
            return true;
    }
    return false;
}