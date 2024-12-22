#pragma once
#include <string>
#include <ShObjIdl.h>
#include "Global.h"

class NewPathSaveCode {
public:
    bool newPathSaveCode(std::wstring& t_Path);

private:
    void saveFile(const std::wstring& t_Path, std::ofstream& outFile);
    bool isPathUnique(const std::wstring& path);
    std::wstring BrowseFileDialog() const;

    std::wstring m_Path;
    const std::wstring m_PathOptionDirectory = G_PATH_APP_DATA+L"PathWar3.txt";
};
