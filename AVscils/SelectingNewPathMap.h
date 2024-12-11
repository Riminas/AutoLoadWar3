#pragma once
#include <vector>
#include <string>
#include <wtypes.h>
#include <array>
#include <SFML/Graphics.hpp>

struct DirectoryEntry {
    std::wstring name;
    std::wstring fullPath;
    bool isOpen;
    std::vector<DirectoryEntry> subDirectories;

    DirectoryEntry(const std::wstring& name, const std::wstring& fullPath)
        : name(name), fullPath(fullPath), isOpen(false) {
    }
};

class SelectingNewPathMap
{
public:
    SelectingNewPathMap();
    void selectingNewPathMap();
private:
    void initialize();
    void initializeText(sf::Text& text, const std::wstring& textString, const sf::Vector2f& position, const sf::Color& color, const float& letterSpacing) const;
    void initializeRectangle(sf::RectangleShape& rectangle, const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color) const;
    void initializeSprite(sf::Sprite& rectangle, const sf::Vector2f& position, const sf::IntRect& rect) const;
    std::wstring run();
    void initializeScrollbar(const float& windowWidth, const float& windowHeight);
    void loadDirectories(const std::wstring& directoryPath, DirectoryEntry& parent, bool& isPathSaveCodeFalse);
    void updateDirectoryTexts();
    void adjustScrollbar();
    void drawWindow();
    std::wstring handleMousePress(sf::Event& event);
    void handleScroll(sf::Event& event);
    void handleMouseMove(sf::Event& event);

    void toggleDirectory(DirectoryEntry& directory);
    std::wstring newPath();
    // ѕќћ≈Ќя“№ посто€нный вызов фукции определени€ размеров монитора на positionLeftTop
    bool IsWindowInFocus(const HWND& hWnd) const;

    bool isPushDirectory(const std::wstring& fullPath, const std::wstring& nameDirectory) const;
    

    std::array<float, 4> m_Rect{ 0, 0, 0, 0 };//newPoition

    sf::Texture m_Texture;
    sf::Sprite titleAndClose;
    sf::Text title;
    sf::RectangleShape scrollbar;
    sf::RectangleShape scrollbarThumb;

    sf::Sprite background;//осташи€с€ незаполненна€ область
    sf::Sprite topBackground;// содержит optionsButton DataCommandsButton 
    sf::Sprite bottomBackground;
    DirectoryEntry rootDirectory;
    std::vector<sf::Text> directoryTexts;
    std::vector<sf::Sprite> directoryTriangles; // треугольники попробовать перенести их в текст
    float scrollOffset = 0;
    bool isScrolling = false;
    float scrollSpeed = 1;
    float mouseOffset = 0;
    bool isBackgroundDirectory = false;
    sf::RectangleShape backgroundDirectory;// подсвечиает текущию дерикторию
    DirectoryEntry openDirectoryEntry;
};

