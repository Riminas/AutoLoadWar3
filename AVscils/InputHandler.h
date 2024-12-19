#pragma once
#include <chrono>
#include <array>
#include <windows.h>
#include "key.h"
#include "Global.h"

class InputHandler {
public:
    void handleInput();

private:
    void handleKeyboardInput();
    void handleMouseInput();
    void processInput(int keyNum, const std::chrono::high_resolution_clock::time_point& now);
    void processMouseInput(const std::chrono::high_resolution_clock::time_point& now);
    
    static constexpr std::array<int, 46> INPUT_CODES = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, // 0-9
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, // numpad 0-9
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, // A-J
        0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, // K-T
        0x55, 0x56, 0x57, 0x58, 0x59, 0x5A  // U-Z
    };

    int m_lastInputCode = -1;
    std::chrono::high_resolution_clock::time_point m_currentTime{};
    key m_key;

    static constexpr int KEYBOARD_DELAY_MS = 250;
    static constexpr int MOUSE_DELAY_MS = 400;
    static constexpr int MOUSE_CLICK_INTERVAL_MS = 100;
};

