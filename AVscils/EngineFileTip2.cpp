#include <wtypes.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <format>
#include <iomanip>
#include <tlhelp32.h>

#include "Global.h"
#include "getMapOpen.h"
#include "LoadManager.h"
#include "EngineFileTip2.h"
#include "EngineFileTip1.h"
#include "NewDirectory.h"
#include "SkillsUpgradeStart.h"
#include "StringConvector.h"
#include "HeroInfoEngine.h"
#include "ConfigMaps.h"

short EngineFileTip2::initialize() {
    if (!engineFile()) {
        return 0;
    }

    m_ListHeroDraw.resize(G_HERO_INFO.size() + 1);
    unsigned int characterSize = 16; // Размер шрифта

    m_MaxNameWidth = 0;
    for (const auto& hero : G_HERO_INFO) {
        if (hero.nameChar.length() > m_MaxNameWidth) {
            m_MaxNameWidth = hero.nameChar.length();
        }
    }

    for (int i = 0; i < G_HERO_INFO.size(); ++i) {
        createHeroDraw(i, characterSize);
    }

    createHeroDraw(static_cast<int>(G_HERO_INFO.size()), characterSize);

    isInitialize = true;
    return 1;
}

inline bool EngineFileTip2::engineFile() {
    return HeroInfoEngine().retrieveHeroData(G_CONFIG_MAPS.path);
}

int16_t EngineFileTip2::mouseButtonPressed(const sf::Event& event) const {
    const uint8_t lastNum = (uint8_t)m_ListHeroDraw.size() - 1;
    for (uint8_t i = 0; i < lastNum; ++i) {
        if (m_ListHeroDraw[i].shape.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
            return i;
    }

    if (m_ListHeroDraw[lastNum].shape.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
        return -2;

    return -1;
}

void EngineFileTip2::draw() {
    for (const ListHeroDraw& list : m_ListHeroDraw) {
        G_WINDOW.draw(list.shape);
        G_WINDOW.draw(list.text);
        G_WINDOW.draw(list.textData);
    }
}

std::wstring EngineFileTip2::file_time_to_wstring(const std::filesystem::file_time_type& ftime) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );

    std::time_t time = std::chrono::system_clock::to_time_t(sctp);
    std::tm tm;
    localtime_s(&tm, &time);

    std::wostringstream oss;
    oss << std::put_time(&tm, L"%d-%m-%Y\n    %H:%M");
    return oss.str();
}

void EngineFileTip2::updateRect(const HWND& hWndWindow, const bool& isUpdataPosition) {
    RECT rect;
    if (GetClientRect(hWndWindow, &rect)) {
        const int width = rect.right - rect.left;
        const int height = rect.bottom - rect.top;
        const float x = width - 441.0f;
        float y = height / 4.0f * 3.0f - 20 - (G_HERO_INFO.size() * 25.0f + 25.0f);

        if (y < 0) {
            y = (height % 25) / 2.f;

            int num = (int)G_HERO_INFO.size() * 25;

            if (height < num) {
                int num2 = (height / 25);
                std::vector<HeroInfo> listHero{};
                listHero.resize(num2);
                std::vector<ListHeroDraw> listHeroDraw{};
                listHeroDraw.resize(num2);
                int j = 0;
                for (size_t i = G_HERO_INFO.size() - num2; auto & p : listHero) {
                    p = G_HERO_INFO[i];
                    listHeroDraw[j] = m_ListHeroDraw[i+1];
                    i++;
                    j++;
                }
                G_HERO_INFO = listHero;
                m_ListHeroDraw = listHeroDraw;
            }


        }
        else {

        }

        sf::Vector2f newPosition = sf::Vector2f(x, y);

        if (isUpdataPosition)
            updatePosition(newPosition);
    }
}

void EngineFileTip2::updatePosition(const sf::Vector2f& newPosition) {
    for (unsigned i = 0; i < m_ListHeroDraw.size(); ++i) {
        m_ListHeroDraw[i].shape.setPosition(sf::Vector2f(0, static_cast<float>(2 + 25 * i)) + newPosition);
        m_ListHeroDraw[i].text.setPosition(sf::Vector2f(4, static_cast<float>(3 + 25 * i)) + newPosition);
        m_ListHeroDraw[i].textData.setPosition(sf::Vector2f(m_ListHeroDraw[i].shape.getSize().x-60, static_cast<float>(3 + 25 * i)) + newPosition);
        const float maxWidth = m_ListHeroDraw[i].shape.getSize().x - 60;
        StringConvector StringConvector_;
        StringConvector_.adjustTextToFit(m_ListHeroDraw[i].text, maxWidth);
    }
}

void EngineFileTip2::updateRegionTrue(const bool t_IsVisibleMenu) {

    HWND hwnd = G_WINDOW.getSystemHandle();
    HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

    {
        HRGN HeroBar = CreateRectRgn(
            static_cast<int>(m_ListHeroDraw[0].shape.getPosition().x),
            static_cast<int>(m_ListHeroDraw[0].shape.getPosition().y),
            static_cast<int>(m_ListHeroDraw[0].shape.getPosition().x + m_ListHeroDraw[0].shape.getSize().x),
            static_cast<int>(m_ListHeroDraw.back().shape.getPosition().y + m_ListHeroDraw[0].shape.getSize().y));
        CombineRgn(region, region, HeroBar, RGN_OR);
        DeleteObject(HeroBar);
    }    
    RECT rectClient;
    RECT rectWindow;
    GetClientRect(G_DATA_PATH.hWndWindowWar, &rectClient);
    GetWindowRect(G_DATA_PATH.hWndWindowWar, &rectWindow);
    const int width = rectClient.right - rectClient.left;
    const int height = rectClient.bottom - rectClient.top;

    const auto& cmd = G_CONFIG_MAPS.usersConfig;
    const unsigned sum = 20 * (cmd[0].isVisibleButton + cmd[1].isVisibleButton + cmd[2].isVisibleButton + cmd[3].isVisibleButton + cmd[4].isVisibleButton);
    const float x = width / 2.f - 50;
    const float y = (height / 20.0f) * 15.75f - 9;
    const unsigned sumX = static_cast<unsigned>(x) + rectWindow.left;
    const unsigned sumY = static_cast<unsigned>(y) + rectWindow.top;

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
        if (t_IsVisibleMenu) {
            HRGN mainVerticalBar = CreateRectRgn(
                sumX,
                sumY - 100,
                sumX + 20,
                sumY + 60);
            CombineRgn(region, region, mainVerticalBar, RGN_OR); // Добавляем вертикальную часть
            DeleteObject(mainVerticalBar);
        }
    }


    SetWindowRgn(hwnd, region, TRUE);
}

std::wstring EngineFileTip2::getPathListHero(const int& i) {
    return G_HERO_INFO[i].path;
}

void EngineFileTip2::createHeroDraw(int index, unsigned int characterSize) {
    bool isLastElement = (index == G_HERO_INFO.size());
    m_ListHeroDraw[index].shape.setSize(sf::Vector2f(400, 23));
    m_ListHeroDraw[index].shape.setFillColor(sf::Color::White);
    m_ListHeroDraw[index].shape.setOutlineColor(sf::Color::Black);
    m_ListHeroDraw[index].shape.setOutlineThickness(2);

    if (!isLastElement) {
        m_ListHeroDraw[index].text.setString(G_HERO_INFO[index].nameChar);
        m_ListHeroDraw[index].text.setFont(G_FONT_STANDART);
        m_ListHeroDraw[index].text.setCharacterSize(characterSize);
        m_ListHeroDraw[index].text.setFillColor(sf::Color::Black);

        std::wstring strtData = file_time_to_wstring(G_HERO_INFO[index].latestTime);
        m_ListHeroDraw[index].textData.setString(strtData);
        m_ListHeroDraw[index].textData.setFont(G_FONT_STANDART);
        m_ListHeroDraw[index].textData.setCharacterSize(characterSize-6);
        m_ListHeroDraw[index].textData.setFillColor(sf::Color::Black);
        m_ListHeroDraw[index].textData.setLineSpacing(0.6f);
    }
    else {
        std::wstring str = std::format(L"{:>20}", L"Отменить");
        m_ListHeroDraw[index].text.setString(str);
        m_ListHeroDraw[index].text.setFont(G_FONT_STANDART);
        m_ListHeroDraw[index].text.setCharacterSize(characterSize);
        m_ListHeroDraw[index].text.setFillColor(sf::Color::Black);

        m_ListHeroDraw[index].textData.setString(L"\0");
    }
}