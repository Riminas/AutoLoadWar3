#pragma once
//#include <SFML/Graphics.hpp>
//class CoutGuide
//{
//public:
//	void coutGuide();
//	void initialize();
//	void draw();
//	void processEvents();
//private:
//	sf::RenderWindow m_Windom2;
//};
//
//
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

class CoutGuide {
private:
    std::vector<sf::Text> m_TextLines; // Хранение строк текста как отдельных объектов
    std::wstring m_FilePath = { L"DataAutoLoad/DataMaps/" + G_DATA_MAPS.m_NameMaps/* + L"/"*/ };
    float m_ScrollOffset; // Смещение для скроллинга
    const float m_LineHeight = 30.f; // Высота строки текста
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
        m_FilePath = { L"DataAutoLoad/DataMaps/" + G_DATA_MAPS.m_NameMaps/* + L"/"*/ };
        initialize();
    }

    void draw() {
        if (G_CONFIG_MAIN.optionsConfig.blackColor) {
            m_Window.clear(sf::Color(45, 45, 48));
        }
        else {
            m_Window.clear(sf::Color::White);
        }

        // Отрисовка только тех строк, которые видимы в окне
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
        // Размер окна
        int windowWidth = 512;
        int windowHeight = sf::VideoMode::getDesktopMode().height / 3;

        m_Window.create(sf::VideoMode(windowWidth, windowHeight), "AutoLoadsGuide",
            sf::Style::Resize | sf::Style::Close);
        // Создаем окно с возможностью изменения размера и перемещения

        //m_Window.setVisible(false);

        // Получаем системный дескриптор окна
        HWND hwnd = m_Window.getSystemHandle();

        //LONG style = GetWindowLong(hwnd, GWL_STYLE);
        //style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // Убираем кнопки "Свернуть" и "Развернуть"
        //SetWindowLong(hwnd, GWL_STYLE, style);

        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);

        // Центрируем окно
        int screenWidth = sf::VideoMode::getDesktopMode().width;
        int screenHeight = sf::VideoMode::getDesktopMode().height;
        int posX = (screenWidth - windowWidth);
        int posY = (screenHeight - windowHeight) / 2;
        m_Window.setPosition(sf::Vector2i(posX, posY));

        // Устанавливаем окно поверх всех других окон
        setAlwaysOnTop2(hwnd);

        //// Запускаем таймер в отдельном потоке для периодического обновления окна
        //std::thread([hwnd]() {
        //    while (true) {
        //        std::this_thread::sleep_for(std::chrono::seconds(1));
        //        setAlwaysOnTop2(hwnd);
        //    }
        //    }).detach();
    }
private:
    // Функция для установки окна поверх всех других окон
    static void setAlwaysOnTop2(const HWND& hwnd) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }

    void initialize() {
        loadFileContent();
    }

    void loadFileContent() {
        // Открываем файл и читаем его содержимое
        std::string fileContent;
        for (const auto& entry : std::filesystem::directory_iterator(m_FilePath)) {
            if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".txt") {
                fileContent = LoadDataFail().loadDataFail(m_FilePath + L"/" + entry.path().filename().wstring());
                break;
            }
        }

        // Разбиваем текст на строки
        std::stringstream fileStream(fileContent);
        std::string line;
        float yPosition = 10.f; // Начальная позиция текста по Y

        while (std::getline(fileStream, line)) {
            sf::Text textLine;
            textLine.setFont(G_FONT_STANDART);
            textLine.setString(line);
            textLine.setCharacterSize(14);
            if (G_CONFIG_MAIN.optionsConfig.blackColor) {
                textLine.setFillColor(sf::Color::White);
            }
            else {
                textLine.setFillColor(sf::Color::Black);
            }
            textLine.setPosition(10.f, yPosition);
            m_TextLines.push_back(textLine);
            yPosition += m_LineHeight; // Смещение по высоте для каждой строки
        }
    }

    void handleScroll(float delta) {
        const float scrollSpeed = 40.f;
        m_ScrollOffset -= delta * scrollSpeed;

        // Ограничиваем прокрутку
        float minOffset = 0.f;
        float maxOffset = (std::max)(0.f, static_cast<float>(m_TextLines.size()) * m_LineHeight - m_Window.getSize().y);
        m_ScrollOffset = std::clamp(m_ScrollOffset, minOffset, maxOffset);

        // Обновляем позиции строк с учетом смещения
        for (size_t i = 0; i < m_TextLines.size(); ++i) {
            float yPosition = 10.f + i * m_LineHeight - m_ScrollOffset;
            m_TextLines[i].setPosition(10.f, yPosition);
        }
    }
};