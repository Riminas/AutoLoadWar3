#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <string>

#include "DataMaps.h"
#include "Global.h"
#include "ConfigMain.h"
//#include <cstdio>  // для _popen/_pclose

class CoutGuide {
private:
    std::vector<sf::Text> m_TextLines; //      
    std::wstring m_FilePath = { G_PATH_APP_DATA+L"DataMaps/" + G_DATA_MAPS.m_NameMaps/* + L"/"*/ };
    //std::string m_GuideUrl; // Добавляем переменную для хранения URL
    float m_ScrollOffset; // 
public:
    bool isActive = false;
    sf::RenderWindow m_Window;
    CoutGuide();
    void newGame();
    void draw();
    void processEvents();
    void initializeWindow();
private:
    void initialize();
    void loadFileContent();
    void handleScroll(float delta);
};