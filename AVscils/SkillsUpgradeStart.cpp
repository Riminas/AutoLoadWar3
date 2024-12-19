#include <filesystem>
#include <fstream>

#include <optional>
#include <cctype>
#include <Windows.h>
#include <gdiplus.h>
#include "SkillsUpgradeStart.h"
#include "key.h"
#include "LogManager.h"

#pragma comment (lib, "Gdiplus.lib")

SkillsUpgradeStart::SkillsUpgradeStart(const std::wstring& t_NameMaps, const int& t_VersionWarcraft3)
    : m_NameMaps(t_NameMaps),
    m_VersionWarcraft3(t_VersionWarcraft3)
{
}


void mouseClick(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
    Sleep(1);
    mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
}



void SkillsUpgradeStart::skillsUpgradeStart(const bool& isDelay) {
    //bool isExistsFullPath = false;
    //m_FullPath = m_pathDatMap + L'\\' + m_NameMaps + m_pathFile;
    //if (std::filesystem::exists(m_FullPath)) {
    //    isExistsFullPath = true;

    //    if (!loadDataSkill())
    //        isExistsFullPath = false;

    //    for (std::pair<int, unsigned>& p : m_DataSkill)
    //        if (!charToAscii(p.first)) {
    //            isExistsFullPath = false;
    //            break;
    //        }
    //}
}

bool SkillsUpgradeStart::loadDataSkill()
{
    std::ifstream inFile(m_FullPath);

    std::string line;
    while (std::getline(inFile, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> tokens(std::istream_iterator<std::string, char>{iss}, std::istream_iterator<std::string, char>());

        if (tokens.size() != 2)
            return false;

        int idKey = 0;
        {
            if (tokens[0].size() != 1)
                return false;

            idKey = tokens[0][0];
        }

        unsigned numKey = 0;
        {
            try {
                numKey = std::stoul(tokens[1]);
            }
            catch (const std::invalid_argument& e) {
                LogManager::logger().log(LogManager::LogLevel::Error, "Invalid argument: " + std::string(e.what()));// Обработка ошибки: строка не является числом
            }
            catch (const std::out_of_range& e) {
                LogManager::logger().log(LogManager::LogLevel::Error, "Out of range: " + std::string(e.what()));// Обработка ошибки: число выходит за пределы типа unsigned int
            }
        }

        std::pair<int, unsigned> pair{ idKey, numKey };
        m_DataSkill.push_back(std::move(pair));
    }

    return true;
}

bool SkillsUpgradeStart::charToAscii(int& c) {
    // Преобразование символа в верхний регистр
    char upperC = std::toupper(static_cast<unsigned char>(c));

    // Проверка, является ли символ буквой
    if (std::isalpha(static_cast<unsigned char>(upperC))) {
        c = static_cast<int>(upperC);
        return true;
    }
    return false; // Возвращаем пустое значение для небуквенных символов
}