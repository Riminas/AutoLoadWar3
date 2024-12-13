#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include "ListHeroDraw.h"
#include <wtypes.h>

class EngineFileTip2 {
public:
    short initialize();
    int16_t mouseButtonPressed(const sf::Event& event) const;
    void draw();
    void updateRect(const HWND& hWndWindow, const bool& isUpdatePosition);
    void updatePosition(const sf::Vector2f& newPosition);
    void updateRegionTrue(const bool t_IsVisibleMenu);
    std::wstring getPathListHero(const int& i);

private:
    bool engineFile();
    std::wstring file_time_to_wstring(const std::filesystem::file_time_type& ftime);
    void createHeroDraw(int index, unsigned int characterSize);

    std::vector<ListHeroDraw> m_ListHeroDraw;
    size_t m_MaxNameWidth = 0;
    bool isInitialize = false;
};