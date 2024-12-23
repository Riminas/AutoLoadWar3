#pragma once
#include <SFML/Graphics.hpp>
#include <wrl.h>
#include <WebView2.h>

using namespace Microsoft::WRL;

class OpenIrinaBot
{
private:
    sf::RenderWindow window;
    ComPtr<ICoreWebView2Environment> webViewEnvironment;
    ComPtr<ICoreWebView2Controller> webViewController;
    ComPtr<ICoreWebView2> webView;

    void InitializeWindow();
    void InitializeWebView();

public:
    OpenIrinaBot();
    void Run();
};

