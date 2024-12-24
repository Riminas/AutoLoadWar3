#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <string>
#include <wrl.h>
#include <WebView2.h>

#include "DataMaps.h"
#include "Global.h"
#include "ConfigMain.h"

using namespace Microsoft::WRL;

class CoutGuide {
private:
    std::vector<sf::Text> m_TextLines;
    std::wstring m_FilePath = { G_PATH_APP_DATA+L"DataMaps/" + G_DATA_MAPS.m_NameMaps};
    std::wstring m_HttpLink{};
    float m_ScrollOffset;

    ComPtr<ICoreWebView2Environment> webViewEnvironment;
    ComPtr<ICoreWebView2Controller> webViewController;
    ComPtr<ICoreWebView2> webView;
    sf::RenderWindow m_Window;
public:
    CoutGuide();
    void Run();
private:
    void RunTxt();
    void RunWeb();

    void draw();
    void processEvents();
    void initializeWindow();    
    void InitializeWebView(const std::wstring& wstrUrlWeb);
    void initialize();
    void loadFileContent();
    void handleScroll(float delta);
};