#pragma once
#include <string>

class AutoUpdate
{
public:
	std::string GetLatestReleaseInfo(const std::wstring& owner, const std::wstring& repo);
	//bool DownloadUpdate(const std::wstring& downloadUrl, const std::wstring& savePath);
	bool DownloadFile(const std::string& url, const std::wstring& savePath);
};

