#include "pch.h"
#include "CoutGuide.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "LoadDataFail.h"
#include <filesystem>
#include <wtypes.h>
#include <Windows.h>
#include <thread>
#include "DataWarcraft.h"

constexpr float INITIAL_Y_POSITION = 10.f;
constexpr float INITIAL_X_POSITION = 10.f;
constexpr float LINE_HEIGHT = 30.f;

CoutGuide::CoutGuide()
    : m_ScrollOffset(0.f) {
}


void CoutGuide::newGame() {
    m_TextLines.clear();
    m_FilePath = { G_PATH_APP_DATA + L"DataMaps/" + G_DATA_MAPS.m_NameMaps/* + L"/"*/ };
    initialize();
}

void CoutGuide::draw() {
    m_Window.clear({ G_CONFIG_MAIN.optionsConfig.blackColor ? sf::Color(45, 45, 48) : sf::Color::White });

    for (const auto& text : m_TextLines) {
        if (text.getPosition().y + LINE_HEIGHT >= 0 && text.getPosition().y <= m_Window.getSize().y) {
            m_Window.draw(text);
        }
    }

    m_Window.display();
}

void CoutGuide::processEvents() {
    sf::Event event;
    while (m_Window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                m_Window.close();
                break;
            case sf::Event::MouseWheelScrolled:
                handleScroll(event.mouseWheelScroll.delta);
                break;
            case sf::Event::Resized:
                m_Window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height))));
                break;
            default:
                break;
        }
    }
}

static void setAlwaysOnTop2(const HWND& hwnd) {
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CoutGuide::initializeWindow() {
    int windowWidth = static_cast<int>((static_cast<float>(G_DATA_WARCRAFT.m_DataRect.size.x / 3)) * 2.5f);
    int windowHeight = static_cast<int>((static_cast<float>(G_DATA_WARCRAFT.m_DataRect.size.y / 3)) * 2.5f);

    m_Window.create(sf::VideoMode(windowWidth, windowHeight), "WarGuide",
        sf::Style::Resize | sf::Style::Close);

    HWND hwnd = m_Window.getSystemHandle();

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);

    int posX = (G_DATA_WARCRAFT.m_DataRect.center.x - windowWidth / 2);
    int posY = (G_DATA_WARCRAFT.m_DataRect.center.y - windowHeight / 2);
    m_Window.setPosition(sf::Vector2i(posX, posY));

    setAlwaysOnTop2(hwnd);
}


void CoutGuide::initialize() {
    loadFileContent();
}

void CoutGuide::loadFileContent() {
    m_TextLines.clear();
    m_TextLines.reserve(100);
    std::string fileContent;
    bool contentLoaded = false;

    if (!contentLoaded) {
        for (const auto& entry : std::filesystem::directory_iterator(m_FilePath)) {
            if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt") {
                fileContent = LoadDataFail().loadDataFail(entry.path().wstring());
                contentLoaded = true;
                break;
            }
        }
    }

    if (!contentLoaded || fileContent.empty()) {
        sf::Text emptyText;
        emptyText.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        emptyText.setString(L"Гайд пуст или отсутствует");
        emptyText.setCharacterSize(14);
        emptyText.setFillColor(G_CONFIG_MAIN.optionsConfig.blackColor ? sf::Color::White : sf::Color::Black);

        sf::FloatRect textBounds = emptyText.getLocalBounds();
        emptyText.setPosition(
            (m_Window.getSize().x - textBounds.width) / 2.f,
            (m_Window.getSize().y - textBounds.height) / 2.f
        );

        m_TextLines.emplace_back(std::move(emptyText));
        return;
    }

    std::stringstream fileStream(fileContent);
    std::string line;
    float yPosition = INITIAL_Y_POSITION;

    while (std::getline(fileStream, line)) {
        if (line.empty()) continue;

        sf::Text textLine;
        textLine.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        textLine.setString(std::move(line));
        textLine.setCharacterSize(14);
        textLine.setFillColor(G_CONFIG_MAIN.optionsConfig.blackColor ? sf::Color::White : sf::Color::Black);
        textLine.setPosition(INITIAL_X_POSITION, yPosition);
        m_TextLines.emplace_back(std::move(textLine));
        yPosition += LINE_HEIGHT;
    }
}

//void CoutGuide::handleScroll(float delta) {
//    const float scrollSpeed = 40.f;
//    m_ScrollOffset -= delta * scrollSpeed;
//
//     
//    float minOffset = 0.f;
//    float maxOffset = (std::max)(0.f, static_cast<float>(m_TextLines.size()) * m_LineHeight - m_Window.getSize().y);
//    m_ScrollOffset = std::clamp(m_ScrollOffset, minOffset, maxOffset);
//
//     
//    for (size_t i = 0; i < m_TextLines.size(); ++i) {
//        float yPosition = 10.f + i * m_LineHeight - m_ScrollOffset;
//        m_TextLines[i].setPosition(10.f, yPosition);
//    }
//}
void CoutGuide::handleScroll(float delta) {
    const float scrollSpeed = 40.f;
    m_ScrollOffset = std::clamp(m_ScrollOffset - delta * scrollSpeed, 0.f,
        (std::max)(0.f, static_cast<float>(m_TextLines.size()) * LINE_HEIGHT - m_Window.getSize().y));

    float newYPosition = INITIAL_Y_POSITION - m_ScrollOffset;
    for (auto& text : m_TextLines) {
        text.setPosition(INITIAL_X_POSITION, newYPosition);
        newYPosition += LINE_HEIGHT;
    }
}