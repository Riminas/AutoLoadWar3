#pragma once
#include "Global.h"
#include <wtypes.h>
#include "ConfigMaps.h"
#include "BoolVisibleMenu.h"
#include "DataWarcraft.h"
#include "EngineFileTip2.h"

static std::array<int, 4> G_RECT;

class UpdateRegionRect
{
public:
    void updateRegionMain(const std::array<int, 4>& t_rect = { 0, 0, 0, 0 }) const {
        if (t_rect[0] != 0)
            G_RECT = t_rect;

        HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

        const auto& cmd = G_CONFIG_MAPS.usersConfig;
        const int sum = 20 * (cmd[0].isVisibleButton + cmd[1].isVisibleButton + cmd[2].isVisibleButton + cmd[3].isVisibleButton + cmd[4].isVisibleButton);
        const int x = G_DATA_WARCRAFT.m_DataRect.center.x - 50;
        const int y = static_cast<int>((G_DATA_WARCRAFT.m_DataRect.size.y / static_cast<float>(20)) * 15.75f - 9);
        const int sumX = x + G_DATA_WARCRAFT.m_DataRect.position.x;
        const int sumY = y + G_DATA_WARCRAFT.m_DataRect.position.y;

        {
            HRGN mainHorizontalBar = CreateRectRgn(
                sumX - sum,
                sumY,
                sumX + 100,
                (G_BOOL_VISIBLE.isVisibleMenu ? (sumY + 40) : (sumY + 20)));
            CombineRgn(region, region, mainHorizontalBar, RGN_OR); // Добавляем горизонтальную часть
            DeleteObject(mainHorizontalBar);
        }

        if (G_BOOL_VISIBLE.isVisibleMenu) {
            HRGN mainVerticalBar = CreateRectRgn(
                sumX,
                sumY - 100,
                sumX + 20,
                sumY/* + 80*/);
            CombineRgn(region, region, mainVerticalBar, RGN_OR); // Добавляем вертикальную часть
            DeleteObject(mainVerticalBar);
        }

        if (G_BOOL_VISIBLE.isVisibleEngineFile) {
            HRGN HeroBar = CreateRectRgn(
                G_RECT[0],
                G_RECT[1],
                G_RECT[2],
                G_RECT[3]);
            CombineRgn(region, region, HeroBar, RGN_OR);
            DeleteObject(HeroBar);
        }

        HWND hwnd = G_WINDOW.getSystemHandle();
        SetWindowRgn(hwnd, region, TRUE);
    }

    void updateRegionLoad() const {
        HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

        HRGN loadBar = CreateRectRgn(
            G_DATA_WARCRAFT.m_DataRect.center.x - 96,
            G_DATA_WARCRAFT.m_DataRect.center.y - 70,
            G_DATA_WARCRAFT.m_DataRect.center.x + 96,
            G_DATA_WARCRAFT.m_DataRect.center.y - 32);
        CombineRgn(region, region, loadBar, RGN_OR);
        DeleteObject(loadBar);

        HWND hwnd = G_WINDOW.getSystemHandle();
        SetWindowRgn(hwnd, region, TRUE);
    }

    void updateRegionOption() const {
        HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        int windowHeight = desktop.height * 2 / 3 + 50;
        HRGN OptionBar = CreateRectRgn(
            desktop.width / 2 - 256,
            desktop.height / 2 - windowHeight / 2 - 48,
            desktop.width / 2 + 256,
            desktop.height / 2 + windowHeight / 2);
        CombineRgn(region, region, OptionBar, RGN_OR);
        DeleteObject(OptionBar);

        HWND hwnd = G_WINDOW.getSystemHandle();
        SetWindowRgn(hwnd, region, TRUE);
    }

    void updateRegionMiniButton() const {
        HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

        const int width = G_DATA_WARCRAFT.m_DataRect.position.x + G_DATA_WARCRAFT.m_DataRect.center.x;
        HRGN loadBar = CreateRectRgn(
            width - 5,
            G_DATA_WARCRAFT.m_DataRect.position.y + 2,
            width + 5,
            G_DATA_WARCRAFT.m_DataRect.position.y + 10);
        CombineRgn(region, region, loadBar, RGN_OR);
        DeleteObject(loadBar);

        HWND hwnd = G_WINDOW.getSystemHandle();
        SetWindowRgn(hwnd, region, TRUE);
    }
    inline void clearRegion() {
        HRGN region = CreateRectRgn(0, 0, 0, 0); // Пустой регион для комбинирования

        HWND hwnd = G_WINDOW.getSystemHandle();
        SetWindowRgn(hwnd, region, TRUE);
    }
};