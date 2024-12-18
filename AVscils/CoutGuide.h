#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "SFML/Graphics.hpp"
#include "LoadDataFail.h"
#include "DataMaps.h"
#include "Global.h"
#include <filesystem>
#include <wtypes.h>
#include <Windows.h>
#include <thread>
#include "FontLoader.h"
#include "ConfigMain.h"
//#include <cstdio>  // для _popen/_pclose

class CoutGuide {
private:
    std::vector<sf::Text> m_TextLines; //      
    std::wstring m_FilePath = { L"DataWarAssist/DataMaps/" + G_DATA_MAPS.m_NameMaps/* + L"/"*/ };
    //std::string m_GuideUrl; // Добавляем переменную для хранения URL
    float m_ScrollOffset; // 
    const float m_LineHeight = 30.f; // 
public:
    bool isActive = false;
    sf::RenderWindow m_Window;

    CoutGuide()
        : m_ScrollOffset(0.f) {
    }


    //void coutGuide() {
    //    if (m_TextLines.empty())
    //        m_Window.close();

    //    while (m_Window.isOpen()) {
    //        processEvents();
    //        draw();
    //    }
    //}
    void newGame() {
        m_TextLines.clear();
        m_FilePath = { L"DataWarAssist/DataMaps/" + G_DATA_MAPS.m_NameMaps/* + L"/"*/ };
        initialize();
    }

    void draw() {
        if (G_CONFIG_MAIN.optionsConfig.blackColor) {
            m_Window.clear(sf::Color(45, 45, 48));
        }
        else {
            m_Window.clear(sf::Color::White);
        }

        // 
        for (const auto& text : m_TextLines) {
            if (text.getPosition().y + m_LineHeight >= 0 && text.getPosition().y <= m_Window.getSize().y) {
                m_Window.draw(text);
            }
        }

        m_Window.display();
    }

    void processEvents() {
        sf::Event event;
        while (m_Window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_Window.close();
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                handleScroll(event.mouseWheelScroll.delta);
            }
            else if (event.type == sf::Event::Resized) {
                m_Window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));
            }
        }
    }

    void initializeWindow() {
        // 
        int windowWidth = 512;
        int windowHeight = sf::VideoMode::getDesktopMode().height / 3;

        m_Window.create(sf::VideoMode(windowWidth, windowHeight), "WarGuide",
            sf::Style::Resize | sf::Style::Close);
        // 

        //m_Window.setVisible(false);

        // 
        HWND hwnd = m_Window.getSystemHandle();

        //LONG style = GetWindowLong(hwnd, GWL_STYLE);
        //style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // 
        //SetWindowLong(hwnd, GWL_STYLE, style);

        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);

        // 
        int screenWidth = sf::VideoMode::getDesktopMode().width;
        int screenHeight = sf::VideoMode::getDesktopMode().height;
        int posX = (screenWidth - windowWidth);
        int posY = (screenHeight - windowHeight) / 2;
        m_Window.setPosition(sf::Vector2i(posX, posY));

        // 
        setAlwaysOnTop2(hwnd);

        //// 
        //std::thread([hwnd]() {
        //    while (true) {
        //        std::this_thread::sleep_for(std::chrono::seconds(1));
        //        setAlwaysOnTop2(hwnd);
        //    }
        //    }).detach();
    }
private:
    // 
    static void setAlwaysOnTop2(const HWND& hwnd) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }

    void initialize() {
        loadFileContent();
    }

    void loadFileContent() {
        m_TextLines.clear();
        std::string fileContent;
        bool contentLoaded = false;

        // Сначала пробуем загрузить из URL если он задан
        //if (!m_GuideUrl.empty()) {
        //    fileContent = loadFromUrl(m_GuideUrl);
        //    contentLoaded = !fileContent.empty();
        //}

        // Если не удалось загрузить из URL, пробуем локальный файл
        if (!contentLoaded) {
            for (const auto& entry : std::filesystem::directory_iterator(m_FilePath)) {
                if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt") {
                    fileContent = LoadDataFail().loadDataFail(m_FilePath + L"/" + entry.path().filename().wstring());
                    contentLoaded = true;
                    break;
                }
            }
        }

        if (!contentLoaded || fileContent.empty()) {
            sf::Text emptyText;
            emptyText.setFont(G_FONT_STANDART);
            emptyText.setString("Гайд пуст или отсутствует");
            emptyText.setCharacterSize(14);
            emptyText.setFillColor(G_CONFIG_MAIN.optionsConfig.blackColor ? sf::Color::White : sf::Color::Black);
            
            sf::FloatRect textBounds = emptyText.getLocalBounds();
            emptyText.setPosition(
                (m_Window.getSize().x - textBounds.width) / 2.f,
                (m_Window.getSize().y - textBounds.height) / 2.f
            );
            
            m_TextLines.push_back(std::move(emptyText));
            return;
        }

        // Остальной код загрузки текста остается без изменений
        std::stringstream fileStream(fileContent);
        std::string line;
        float yPosition = 10.f;

        while (std::getline(fileStream, line)) {
            if (line.empty()) continue;

            sf::Text textLine;
            textLine.setFont(G_FONT_STANDART);
            textLine.setString(line);
            textLine.setCharacterSize(14);
            textLine.setFillColor(G_CONFIG_MAIN.optionsConfig.blackColor ? sf::Color::White : sf::Color::Black);
            textLine.setPosition(10.f, yPosition);
            m_TextLines.push_back(std::move(textLine));
            yPosition += m_LineHeight;
        }
    }

    //std::string loadFromUrl(const std::string& url) {
    //    // Используем curl для загрузки содержимого
    //    std::string command = "curl -s \"" + url + "\"";
    //    std::string result;
    //    
    //    FILE* pipe = _popen(command.c_str(), "r");
    //    if (!pipe) return "";
    //    
    //    char buffer[128];
    //    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    //        result += buffer;
    //    }
    //    
    //    _pclose(pipe);
    //    return result;
    //}

    void handleScroll(float delta) {
        const float scrollSpeed = 40.f;
        m_ScrollOffset -= delta * scrollSpeed;

        // 
        float minOffset = 0.f;
        float maxOffset = (std::max)(0.f, static_cast<float>(m_TextLines.size()) * m_LineHeight - m_Window.getSize().y);
        m_ScrollOffset = std::clamp(m_ScrollOffset, minOffset, maxOffset);

        // 
        for (size_t i = 0; i < m_TextLines.size(); ++i) {
            float yPosition = 10.f + i * m_LineHeight - m_ScrollOffset;
            m_TextLines[i].setPosition(10.f, yPosition);
        }
    }

    //void setGuideUrl(const std::string& url) {
    //    m_GuideUrl = url;
    //    initialize();
    //}
};