#pragma once
#include <string>

class AutoUpdate
{
public:
    bool CheckForUpdates(const std::wstring& serverUrl, const std::wstring& currentVersion);
    bool DownloadUpdate(const std::wstring& downloadUrl, const std::wstring& savePath);
};

