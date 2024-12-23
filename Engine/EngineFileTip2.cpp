#include "pch.h"
#include <wtypes.h>

#include <chrono>
#include <sstream>
#include <format>
#include <iomanip>

#include "Global.h"
#include "EngineFileTip2.h"
#include "StringConvector.h"
#include "HeroInfoEngine.h"
#include "ConfigMaps.h"
#include "ConfigMain.h"
#include "UpdateRegionRect.h"

short EngineFileTip2::initialize() {
    if (!engineFile() || G_HERO_INFO.empty()) {
        // MessageBox(NULL, L"Нет данных героев", L"Error", MB_OK | MB_ICONEXCLAMATION);
        return 0;
    }

    m_ListHeroUI.clear();
    m_ListHeroUI.resize(G_HERO_INFO.size() + 1);
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
    const uint8_t lastNum = (uint8_t)m_ListHeroUI.size() - 1;
    for (uint8_t i = 0; i < lastNum; ++i) {
        if (m_ListHeroUI[i].shape.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
            return i;
    }

    if (m_ListHeroUI[lastNum].shape.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)))
        return -2;

    return -1;
}

void EngineFileTip2::draw() {
    for (const ListHeroUI& list : m_ListHeroUI) {
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

    const float x = G_DATA_WARCRAFT.m_DataRect.size.x - 441.0f;
    float y = G_DATA_WARCRAFT.m_DataRect.size.y / 4.0f * 3.0f - 20 - (G_HERO_INFO.size() * 25.0f + 25.0f);

    if (y < 0) {
        y = (G_DATA_WARCRAFT.m_DataRect.size.y % 25) / 2.f;

        int num = (int)G_HERO_INFO.size() * 25;

        if (G_DATA_WARCRAFT.m_DataRect.size.y < num) {
            int num2 = (G_DATA_WARCRAFT.m_DataRect.size.y / 25);
            std::vector<HeroInfo> listHero{};
            listHero.resize(num2);
            std::vector<ListHeroUI> listHeroDraw{};
            listHeroDraw.resize(num2);
            int j = 0;
            for (size_t i = G_HERO_INFO.size() - num2; auto & p : listHero) {
                p = G_HERO_INFO[i];
                listHeroDraw[j] = m_ListHeroUI[i+1];
                i++;
                j++;
            }
            G_HERO_INFO = listHero;
            m_ListHeroUI = listHeroDraw;
        }


    }
    else {

    }

    sf::Vector2f newPosition = sf::Vector2f(x, y);

    if (isUpdataPosition)
        updatePosition(newPosition);
}

void EngineFileTip2::updatePosition(const sf::Vector2f& newPosition) {
    for (unsigned i = 0; i < m_ListHeroUI.size(); ++i) {
        m_ListHeroUI[i].shape.setPosition(sf::Vector2f(0, static_cast<float>(2 + 25 * i)) + newPosition);
        m_ListHeroUI[i].text.setPosition(sf::Vector2f(4, static_cast<float>(3 + 25 * i)) + newPosition);
        m_ListHeroUI[i].textData.setPosition(sf::Vector2f(m_ListHeroUI[i].shape.getSize().x-60, static_cast<float>(3 + 25 * i)) + newPosition);
        const float maxWidth = m_ListHeroUI[i].shape.getSize().x - 60;
        StringConvector StringConvector_;
        StringConvector_.adjustTextToFit(m_ListHeroUI[i].text, maxWidth);
    }
}

void EngineFileTip2::updateRegionTrue(const bool t_IsVisibleMenu) {
    UpdateRegionRect().updateRegionMain({
            static_cast<int>(m_ListHeroUI[0].shape.getPosition().x),
            static_cast<int>(m_ListHeroUI[0].shape.getPosition().y),
            static_cast<int>(m_ListHeroUI[0].shape.getPosition().x + m_ListHeroUI[0].shape.getSize().x),
            static_cast<int>(m_ListHeroUI.back().shape.getPosition().y + m_ListHeroUI[0].shape.getSize().y) 
        });
}

std::wstring EngineFileTip2::getPathListHero(const int& i) {
    return G_HERO_INFO[i].path;
}

void EngineFileTip2::createHeroDraw(int index, unsigned int characterSize) {
    ListHeroUI& currentDraw = m_ListHeroUI[index];
    bool isLastElement = (index == G_HERO_INFO.size());
    
    currentDraw.shape.setSize(sf::Vector2f(400, 23));
    if (G_CONFIG_MAIN.optionsConfig.blackColor) {
        currentDraw.shape.setFillColor(sf::Color(45, 45, 48));
        currentDraw.shape.setOutlineColor(sf::Color(28, 28, 28));
    }
    else {
        currentDraw.shape.setFillColor(sf::Color::White);
        currentDraw.shape.setOutlineColor(sf::Color::Black);
    }
    currentDraw.shape.setOutlineThickness(2);

    if (!isLastElement) {
        currentDraw.text.setString(G_HERO_INFO[index].nameChar);
        currentDraw.text.setFont(G_FONT.getFontForString(std::u32string(G_HERO_INFO[index].nameChar.begin(), G_HERO_INFO[index].nameChar.end())));
        currentDraw.text.setCharacterSize(characterSize);

        std::wstring strtData = file_time_to_wstring(G_HERO_INFO[index].latestTime);
        currentDraw.textData.setString(strtData);
        currentDraw.textData.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        currentDraw.textData.setCharacterSize(characterSize-6);
        currentDraw.textData.setLineSpacing(0.6f);

        if (G_CONFIG_MAIN.optionsConfig.blackColor) {
            currentDraw.text.setFillColor(sf::Color::White);
            currentDraw.textData.setFillColor(sf::Color::White);
        }
        else {
            currentDraw.text.setFillColor(sf::Color::Black);
            currentDraw.textData.setFillColor(sf::Color::Black);
        }
    }
    else {
        std::wstring str = std::format(L"{:>20}", L"Отмена");
        currentDraw.text.setString(str);
        currentDraw.text.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        currentDraw.text.setCharacterSize(characterSize);

        currentDraw.textData.setString(L"\0");

        if (G_CONFIG_MAIN.optionsConfig.blackColor) {
            currentDraw.text.setFillColor(sf::Color::White);
        }
        else {
            currentDraw.text.setFillColor(sf::Color::Black);
        }
    }
}