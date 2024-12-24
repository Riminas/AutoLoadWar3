#include "pch.h"
#include "OpenIrinaBot.h"
#include "Global.h"
#include "DataWarcraft.h"

OpenIrinaBot::OpenIrinaBot() {
    InitializeWindow();
    InitializeWebView();
}

void OpenIrinaBot::InitializeWindow() {
    // Установка размеров окна
    int windowWidth = static_cast<int>((static_cast<float>(G_DATA_WARCRAFT.m_DataRect.size.x / 3)) * 2.5f);
    int windowHeight = static_cast<int>((static_cast<float>(G_DATA_WARCRAFT.m_DataRect.size.y / 3)) * 2.5f);

    window.create(sf::VideoMode(windowWidth, windowHeight), "IrinaBot Browser",
        sf::Style::Resize | sf::Style::Close);

    // Получение handle окна
    HWND hwnd = window.getSystemHandle();

    // Установка стилей окна
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);

    // Позиционирование окна
    int posX = (G_DATA_WARCRAFT.m_DataRect.center.x - windowWidth/2);
    int posY = (G_DATA_WARCRAFT.m_DataRect.center.y - windowHeight/2);
    window.setPosition(sf::Vector2i(posX, posY));

    // Установка поверх всех окон
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void OpenIrinaBot::InitializeWebView() {
    CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                webViewEnvironment = env;
                
                env->CreateCoreWebView2Controller(
                    (HWND)window.getSystemHandle(),
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            webViewController = controller;
                            controller->get_CoreWebView2(&webView);
                            
                            RECT bounds;
                            GetClientRect((HWND)window.getSystemHandle(), &bounds);
                            controller->put_Bounds(bounds);
                            
                            // Навигация на сайт IrinaBot
                            webView->Navigate(L"https://irinabot.ru");
                            return S_OK;
                        }).Get());
                return S_OK;
            }).Get());
}

void OpenIrinaBot::Run() {
    while (window.isOpen() && G_WINDOW.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    // Обновляем размер WebView при изменении размера окна
                    RECT bounds = { 0, 0, 
                        static_cast<LONG>(event.size.width), 
                        static_cast<LONG>(event.size.height) };
                    if (webViewController != nullptr) {
                        webViewController->put_Bounds(bounds);
                    }
                    break;
            }
        }

        window.clear();
        window.display();
    }
}
