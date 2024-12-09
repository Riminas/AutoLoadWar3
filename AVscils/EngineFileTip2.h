#pragma once
#include <SFML/Graphics.hpp>
#include "NewDataAll.h"
#include <filesystem>

class EngineFileTip2 {
public:
    int16_t mouseButtonPressed(const sf::Event& event) const;
    void draw();
    short initialize();

    void updateRect(const HWND& hWndWindow, const bool& isUpdataPosition = true);

    std::wstring getPathListHero(const int& i);
    void updateRegionTrue(const bool t_IsVisibleMenu);

    bool isInitialize = false;

private:
    bool engineFile();
    std::wstring file_time_to_wstring(const std::filesystem::file_time_type& ftime);

    void updatePosition(const sf::Vector2f& newPosition);
    void createHeroDraw(int index, unsigned int characterSize);

    struct ListHeroDraw {
        sf::RectangleShape shape;
        sf::Text text;
        sf::Text textData;
    };

    std::vector<ListHeroDraw> m_ListHeroDraw;

    size_t m_MaxNameWidth = 0;
};