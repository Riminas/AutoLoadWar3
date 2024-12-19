#include "InputHandler.h"
#include <thread>
#include "DataWarcraft.h"
#include "ConfigMain.h"

void InputHandler::handleInput() {
    auto now = std::chrono::high_resolution_clock::now();

    // Обработка клавиатуры только для Warcraft 1.26
    if (G_CONFIG_MAIN.optionsConfig.autoClickerKey && 
        G_DATA_WARCRAFT.m_DataPath.versionWarcraft == 1) {
        handleKeyboardInput();
    }

    if (G_CONFIG_MAIN.optionsConfig.autoClickerMouse) {
        handleMouseInput();
    }
}

void InputHandler::handleKeyboardInput() {
    auto now = std::chrono::high_resolution_clock::now();

    for (int inputCode : INPUT_CODES) {
        if (GetAsyncKeyState(inputCode) & 0x8000) {
            processInput(inputCode, now);
            return;
        }
        else if (inputCode == m_lastInputCode) {
            m_lastInputCode = -1;
        }
    }
}

void InputHandler::handleMouseInput() {
    auto now = std::chrono::high_resolution_clock::now();

    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        processMouseInput(now);
    }
    else if (VK_RBUTTON == m_lastInputCode) {
        m_lastInputCode = -1;
    }
}

void InputHandler::processInput(int keyNum, const std::chrono::high_resolution_clock::time_point& now) {
    if (m_lastInputCode != keyNum) {
        m_lastInputCode = keyNum;
        m_currentTime = now;
    }
    else {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_currentTime).count();
            
        if (elapsed >= KEYBOARD_DELAY_MS) {
            while (GetAsyncKeyState(keyNum) & 0x8000) {
                m_key.key1(keyNum);
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
            m_lastInputCode = -1;
        }
    }
}

void InputHandler::processMouseInput(const std::chrono::high_resolution_clock::time_point& now) {
    if (m_lastInputCode != VK_RBUTTON) {
        m_lastInputCode = VK_RBUTTON;
        m_currentTime = now;
    }
    else {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_currentTime).count();
            
        if (elapsed >= MOUSE_DELAY_MS) {
            while (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                m_key.keyVK(VK_RBUTTON, true);
                std::this_thread::sleep_for(std::chrono::milliseconds(MOUSE_CLICK_INTERVAL_MS));
            }
            
            m_key.keyVK(VK_RBUTTON, false);
            m_lastInputCode = -1;
        }
    }
}
