#pragma once
#include <string>

class AutoUpdate
{
public:
	std::string GetLatestReleaseInfo(const std::wstring& owner, const std::wstring& repo);
	bool DownloadFile(const std::string& url, const std::wstring& savePath);
};

