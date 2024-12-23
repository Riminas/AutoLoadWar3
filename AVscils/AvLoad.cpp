#include <windows.h>
#include <memory>
#include <stdexcept>
#include "AutoUpdate.h"

int main()
{
    try {
        // Проверка на уже запущенный экземпляр
        if (FindWindow(nullptr, L"AutoLoadWar3") != nullptr) {
            MessageBoxA(nullptr, "Программа уже запущена.", "Сообщение", MB_OK | MB_ICONERROR);
            return 2;
        }

        // Запуск автообновления
        AutoUpdate().autoUpdate1();

        typedef void (*StartEngineFn)();
        HMODULE hModule = LoadLibrary(L"Engine.dll");
        if (hModule) {
            StartEngineFn startEngine = (StartEngineFn)GetProcAddress(hModule, "StartEngine");
            if (startEngine) {
                startEngine();
            }
        }
        
        return 0;
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
}