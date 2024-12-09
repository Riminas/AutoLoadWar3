#include <windows.h>
#include <iostream>
#include "Engine.h"
#include "AutoUpdate.h"
 // точка входа: mainCRTStartup

const std::wstring CURRENT_VERSION = L"1.0.0";
const std::wstring VERSION_URL = L"https://yourserver.com/latest_version.txt";
const std::wstring UPDATE_URL = L"https://yourserver.com/yourapp_update.exe";
const std::wstring UPDATE_FILE_PATH = L"update.exe";

int main()
{
    SetConsoleOutputCP(CP_UTF8); // Устанавливаем кодировку консоли на UTF-8

    std::locale::global(std::locale("Russian"));
    std::cout.imbue(std::locale());

    if (FindWindow(NULL, L"AutoLoads") == NULL) {
        AutoUpdate AutoUpddate_;
        // Проверяем наличие обновлений
        if (AutoUpddate_.CheckForUpdates(VERSION_URL, CURRENT_VERSION)) {
            // Сообщаем пользователю
            MessageBox(NULL, L"Доступно обновление. Приложение будет обновлено.", L"Обновление", MB_OK);

            // Скачиваем обновление
            if (AutoUpddate_.DownloadUpdate(UPDATE_URL, UPDATE_FILE_PATH)) {
                // Запускаем обновление и выходим из приложения
                ShellExecute(NULL, L"open", UPDATE_FILE_PATH.c_str(), NULL, NULL, SW_SHOWNORMAL);
                return 0; // Завершаем текущий процесс
            }
            else {
                MessageBox(NULL, L"Не удалось скачать обновление.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
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