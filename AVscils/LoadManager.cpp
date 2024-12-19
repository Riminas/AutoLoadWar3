#include <windows.h>
#include <vector>

#include <fstream>
#include <filesystem>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <future>
#include <sstream>
#include <regex>
#include <array>

#include "LogManager.h"
#include "LoadManager.h"
#include "key.h"
#include "StringConvector.h"

LoadManager::LoadManager(const HWND& hWnd)
    : m_TargetWindow(hWnd)
{
}
LoadManager::LoadManager()
{
}

bool LoadManager::executeLoad(const std::wstring& path) {
    m_FilePath = path;
    std::array<std::string, 5> codes{};// сделать определение дленны массива

    if (loadCodesFromFile(codes)) {
        if (!switchToTargetWindow() || m_TargetWindow == NULL) return false;

        bool isLastLoad = false;
        for (int i = 0; std::string & p : codes) {
            if (i >= 5) {
                isLastLoad = true;
                break;
            }

            if (!p.empty()) {
                i++;
            }
            else
                break;
        }

        if (m_LoadType == 1 && IsWindowInFocus(m_TargetWindow)) {
            displayCommand("-load");
        }

        sendLoadDataCommands(codes, true, isLastLoad);

        if (isLastLoad) {
            Sleep(5000);
            displayCommand("-load");
        }
        Sleep(200);

        return true;
    }

    return false;
}

bool LoadManager::loadCodesFromFile(std::array<std::string, 5>& codes) {
    isFistLoad = true;
    std::ifstream file(m_FilePath);
    if (!file.is_open()) return false;

    short saveCodeType = 0;
    std::string line;
    std::regex codeRegex(R"(Code[1-9]:\s*([^ ]+))");
    std::regex preloadRegex(R"((?:-load|-load)\s*(.*[\"]))");
    std::regex preloadRegex2(R"((?:-load|-load)\s*(.*[^]))");
    std::regex loadRegex(R"(-Load[0-9]+\s*([^\s]+))");
    std::regex callPreloadRegex(R"(call Preload\( \"([^ ]+)\" \))");
    std::regex lRegex(R"(-l[0-9]\s*([^ ]+))");

    int j = 0;
    while (std::getline(file, line)) {
        std::smatch match;

        if ((saveCodeType == 0 || saveCodeType == 1) && std::regex_search(line, match, codeRegex)) {
            saveCodeType = 1;
            m_LoadType = true;
            for (size_t i = 0; i < codes.size(); ++i) {
                std::string prefix = "Code" + std::to_string(i + 1) + ":";
                if (match.str().find(prefix) != std::string::npos) {
                    processSingleCode(codes[i], match, prefix);
                }
            }
        }
        else if ((saveCodeType == 0 || saveCodeType == 2) && std::regex_search(line, match, preloadRegex)) {
            saveCodeType = 2;
            processSingleCode(codes[j++], match, "-load");
        }
        else if ((saveCodeType == 0 || saveCodeType == 6) && std::regex_search(line, match, preloadRegex2)) {
            saveCodeType = 6;
            processSingleCode(codes[j++], match, "-load");
        }
        else if ((saveCodeType == 0 || saveCodeType == 3) && std::regex_search(line, match, loadRegex)) {
            saveCodeType = 3;
            for (size_t i = 0; i < codes.size(); ++i) {
                std::string prefix = "-Load" + std::to_string(i + 1) + " ";
                if (match.str(0).find(prefix) != std::string::npos) {
                    processSingleCode(codes[i], match, prefix);
                }
            }
        }
        else if ((saveCodeType == 0 || saveCodeType == 4) && std::regex_search(line, match, callPreloadRegex)) {
            saveCodeType = 4;
            processSingleCode(codes[0], match, "-load");
        }
        else if ((saveCodeType == 0 || saveCodeType == 5) && std::regex_search(line, match, lRegex)) {
            saveCodeType = 5;
            for (size_t i = 0; i < codes.size(); ++i) {
                std::string prefix = "-" + std::to_string(i + 1);
                if (match.str().find(prefix) != std::string::npos) {
                    processSingleCode(codes[i], match, prefix);
                }
            }
        }
    }

    for (int i = 0; i < codes.size() - 1; i++) {
        const int i1 = i + 1;
        if (codes[i1].size() == 0 && codes[i].size() == 111)
            codes[i1] = "A";
    }

    return true;
}

void LoadManager::processSingleCode(std::string& code, const std::smatch& match, const std::string& prefixToRemove) const {
    std::string extractedCode = match.str(1);

    if (!prefixToRemove.empty()) {
        if (prefixToRemove == "-load") {
            if (extractedCode.size() <= 20 && !isFistLoad) {
                code = "";
                return;
            }
            extractedCode = "-load " + extractedCode;
        }
        else if (prefixToRemove[0] == L'-' && prefixToRemove[1] == L'l') {
            extractedCode = prefixToRemove + extractedCode;
        }
        else if (prefixToRemove[0] == L'-' && prefixToRemove[1] == L'L') {
            extractedCode = prefixToRemove + extractedCode;
        }
        else
        {
            size_t pos = extractedCode.find(prefixToRemove);
            if (pos != std::string::npos) {
                extractedCode.erase(pos, prefixToRemove.length());
            }
        }
    }

    if (extractedCode.back() == L'"' && extractedCode.size() > 1) {
        extractedCode.pop_back();
        if (extractedCode.front() == L'"') {
            extractedCode.erase(0, 1);
        }
    }
    else if (extractedCode.size() > 1 && extractedCode.back() == L')' && extractedCode[extractedCode.size() - 2] == L'"') {
        extractedCode.erase(extractedCode.size() - 2, 2);
    }

    code = extractedCode;
}

void LoadManager::simulateKeyStrokes(const std::string& code) {
    // Записываем строку в буфер обмена
    setClipboardText(code);

    // Симулируем вставку текста из буфера обмена
    key().simulatePaste();
}


void LoadManager::setClipboardText(const std::string& text) {

    if (!OpenClipboard(nullptr)) {
        throw std::runtime_error("Unable to open clipboard");
    }

    // Очищаем буфер обмена
    if (!EmptyClipboard()) {
        CloseClipboard();
        throw std::runtime_error("Unable to empty clipboard");
    }

    // Определяем размер буфера
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hGlobal) {
        CloseClipboard();
        throw std::runtime_error("Unable to allocate global memory");
    }

    // Копируем текст в глобальную память
    memcpy(GlobalLock(hGlobal), text.c_str(), text.size() + 1);
    GlobalUnlock(hGlobal);

    // Устанавливаем текст в буфер обмена
    if (!SetClipboardData(CF_TEXT, hGlobal)) {//неставить CF_UNICODETEXT иначе будет "???????????"
        GlobalFree(hGlobal);
        CloseClipboard();
        throw std::runtime_error("Unable to set clipboard data");
    }

    CloseClipboard();
}

void LoadManager::displayCommand(const std::string& command) {
    key key_;
    key_.key1(0x0D); // Enter to open chat
    simulateKeyStrokes(command);
    key_.key1(0x0D); // Enter to close chat
}

bool LoadManager::switchToTargetWindow() const {
    SetForegroundWindow(m_TargetWindow);
    Sleep(100);
    return true;
}

bool LoadManager::IsWindowInFocus(HWND hWnd) {
    HWND hForegroundWnd = GetForegroundWindow();
    return hWnd == hForegroundWnd;
}

void LoadManager::sendLoadDataCommands(const std::array<std::string, 5>& codes, const bool& isDelay, bool isLastLoad) {
    for (const auto& code : codes) {
        if (code.empty()) break;

        if (isDelay) {
            Sleep(100);
            if (!switchToTargetWindow() || m_TargetWindow == NULL) return;

            Sleep(500);
        }
        while (checkKeyPressed()) {
            Sleep(100);
        }
        if (!IsWindowInFocus(m_TargetWindow)) return;

        displayCommand(code);

        Sleep(10);
        LogManager::logger().log(LogManager::LogLevel::Message, "codes: (" + code + ")");
    }
}

void LoadManager::sendLoadDataCommandFast(const std::string& codes) {
    if (codes.empty()) return;
    displayCommand(codes);
}

inline bool LoadManager::checkKeyPressed() {
    return (GetAsyncKeyState(16) & 0x8000 || GetAsyncKeyState(17) & 0x8000 || GetAsyncKeyState(18) & 0x8000);
}
