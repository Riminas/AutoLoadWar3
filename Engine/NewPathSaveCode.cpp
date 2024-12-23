#include "pch.h"
#include <ShlObj.h>
#include <filesystem>
#include <windows.h>
#include <shobjidl.h>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <fstream>
#include <cstdio>

#include "StringConvector.h"
#include "LoadDataFail.h"
#include "NewPathSaveCode.h"
#include <Psapi.h>

void NewPathSaveCode::saveFile(const std::wstring& t_Path, std::ofstream& outFile) {
    StringConvector StringConvector_;
    std::string utf8Text = StringConvector_.utf16_to_utf8(t_Path);
    outFile.write(utf8Text.c_str(), utf8Text.size());
    outFile << std::endl;
    outFile.close();
}
bool NewPathSaveCode::isPathUnique(const std::wstring& path) {
    std::wifstream inFile(m_PathOptionDirectory);
    if (!inFile) {
        return true;
    }

    std::wstring line;
    while (std::getline(inFile, line)) {
        if (line == path) {
            return false;
        }
    }
    return true;
}
bool NewPathSaveCode::newPathSaveCode(std::wstring& t_Path) {
    {
        LoadDataFail LoadDataFail_;
        std::wstring fileContent = LoadDataFail_.loadDataFailW(m_PathOptionDirectory);
        std::wistringstream pathStream(fileContent);
        std::wstring line;

        // Читаем все пути из строки файла
        while (std::getline(pathStream, line)) {
            if (line.empty()) continue;

            std::filesystem::path path{ line };
            std::wstring wstr = path.parent_path().wstring();

            if (std::filesystem::is_directory(wstr)) {
                // Пробуем открыть файл для записи
                std::fstream testFile(line);
                if (!testFile.is_open()) {
                    testFile.close();
                    t_Path.erase(std::remove(t_Path.begin(), t_Path.end(), L'\r'), t_Path.end());
                    t_Path = line;
                    return true;
                }
            }
        }
    }
    {
        while (G_WINDOW.isOpen()) {
            t_Path = BrowseFileDialog();
            if (!t_Path.empty()) {
                t_Path.erase(std::remove(t_Path.begin(), t_Path.end(), L'\r'), t_Path.end());
                std::fstream testFile(t_Path);
                if (!testFile.is_open() && isPathUnique(t_Path)) {
                    std::ofstream outFile(m_PathOptionDirectory, std::ios::app | std::ios::binary);
                    saveFile(t_Path, outFile);
                    break;
                }
            }
        }
    }
    return true;
}

std::wstring NewPathSaveCode::BrowseFileDialog() const {
    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = L"Warcraft III Executable (war3.exe, w3l.exe, Warcrafit III.exe)\0war3.exe;w3l.exe;Warcrafit III.exe\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        std::wstring filePath(ofn.lpstrFile);
        return filePath;
    }

    return L"";
}
