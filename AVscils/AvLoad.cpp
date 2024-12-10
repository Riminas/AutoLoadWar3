#include <windows.h>
#include <iostream>
#include "Engine.h"
#include "AutoUpdate.h"
#include "json.hpp"
#include "LogError.h"
#include "StringConvector.h"
#include "ConfigMainEngine.h"
using json = nlohmann::json;

void update() {
    AutoUpdate AutoUpddate_;
    std::string releaseInfo = AutoUpddate_.GetLatestReleaseInfo(L"Riminas", L"AutoLoadWar3");

    if (!releaseInfo.empty())
    {
        json jsonData = json::parse(releaseInfo);

        std::string latestVersion = jsonData["tag_name"];

        // Предположим, что currentVersion - это строка с текущей версией вашей программы
        std::string currentVersion = "AutoLoad_1_18_16";

        if (latestVersion != currentVersion)
        {

            std::string downloadUrl;
            for (const auto& asset : jsonData["assets"])
            {
                std::string name = asset["name"];
                if (name == "AvLoad.exe") // или другое имя вашего файла(в гит хаб)
                {
                    downloadUrl = asset["browser_download_url"];
                    break;
                }
            }

            wchar_t modulePath[MAX_PATH];
            GetModuleFileName(NULL, modulePath, MAX_PATH);
            std::wstring savePath = modulePath;
            savePath.erase(savePath.size() - 11);
            savePath += L"\\AutoLoad.exe";//куда будет сохранена программа и с каким именем


            LogError().logMessageW(L"savePath " + savePath);

            if (AutoUpddate_.DownloadFile(downloadUrl, savePath))
            {
                // Перейдите к установке обновления
                ShellExecute(NULL, L"open", L"update.exe", NULL, NULL, SW_SHOWNORMAL);
            }
            else
            {
                LogError().logError("Ошибка при загрузке файла обновления.");
                LogError().logMessage("Доступно обновление до версии " + latestVersion);
                LogError().logMessage("адрес " + downloadUrl);
            }


        }
        //else
        //{
        //    LogError().logMessage("У вас установлена последняя версия.");
        //}
    }
    else
    {
        LogError().logMessage("Не удалось получить информацию о последнем релизе.");
    }
}

//с++20 SFML MVS2022 как сделать авто обновление программы через GitHab с использоанием #pragma comment(lib, "winhttp.lib")
int main()
{
    SetConsoleOutputCP(CP_UTF8); // Устанавливаем кодировку консоли на UTF-8

    std::locale::global(std::locale("Russian"));
    std::cout.imbue(std::locale());

    if (FindWindow(NULL, L"AutoLoads") == NULL) {

        ConfigMainEngine().loadConfigMain();

        if (G_CONFIG_MAIN.optionsConfig.autoUpdate) {
            update();
        }
        Engine().engine1();
    }
    else
        MessageBox(NULL, L"Программа уже запущена.", L"Оповищение", MB_OK | MB_ICONEXCLAMATION);

    return 0;
}

//#include <SFML/Window.hpp>
//#include <SFML/Graphics.hpp>
//#include <windows.h> // Для работы с WinAPI
//
//void setWindowRegion(sf::Window& window, bool addVerticalBar, bool addLeftSide) {
//    // Получаем хендл окна SFML
//    HWND hwnd = static_cast<HWND>(window.getSystemHandle());
//
//    // Создаём основной регион в виде перевёрнутой буквы "Т"
//    HRGN mainHorizontalBar = CreateRectRgn(200, 0, 600, 200); // Горизонтальная часть "Т"
//    HRGN verticalBar = CreateRectRgn(350, 200, 450, 600);     // Вертикальная часть "Т"
//    HRGN leftSide = CreateRectRgn(0, 0, 200, 600);            // Дополнительная левая часть
//
//    // Основной регион
//    HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования
//    CombineRgn(region, region, mainHorizontalBar, RGN_OR); // Добавляем горизонтальную часть
//
//    // Добавляем вертикальную часть, если включена
//    if (addVerticalBar) {
//        CombineRgn(region, region, verticalBar, RGN_OR);
//    }
//
//    // Добавляем левую часть, если включена
//    if (addLeftSide) {
//        CombineRgn(region, region, leftSide, RGN_OR);
//    }
//
//    // Устанавливаем регион для окна
//    SetWindowRgn(hwnd, region, TRUE);
//
//    // Устанавливаем окно "поверх всех окон" без активации
//    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//
//    // Применяем WS_EX_NOACTIVATE для предотвращения активации
//    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
//    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);
//
//    // Удаляем временные регионы
//    DeleteObject(mainHorizontalBar);
//    DeleteObject(verticalBar);
//    DeleteObject(leftSide);
//}
//
//int main() {
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Custom Shape Window", sf::Style::None);
//
//    // Флаги для управления формой окна
//    bool addVerticalBar = true; // Включить вертикальную часть
//    bool addLeftSide = false;  // Включить левую часть
//
//    // Устанавливаем форму окна
//    setWindowRegion(window, addVerticalBar, addLeftSide);
//    
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//            else if (event.type == sf::Event::KeyPressed) {
//                // Управление формой окна
//                if (event.key.code == sf::Keyboard::V) {
//                    addVerticalBar = !addVerticalBar;
//                }
//                if (event.key.code == sf::Keyboard::L) {
//                    addLeftSide = !addLeftSide;
//                }
//                setWindowRegion(window, addVerticalBar, addLeftSide); // Обновляем форму окна
//            }
//        }
//
//        // Рисуем содержимое
//        window.clear(sf::Color::Blue);
//        window.display();
//    }
//
//    return 0;
//}