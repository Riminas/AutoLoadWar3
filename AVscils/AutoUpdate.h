#pragma once
#include <string>

class AutoUpdate
{
public:
	void autoUpdate1();
private:
	std::wstring m_PathAppData{};

	bool isUpdate();
	std::string GetLatestReleaseInfo(const std::wstring& owner, const std::wstring& repo);
	//bool DownloadUpdate(const std::wstring& downloadUrl, const std::wstring& savePath);
	bool DownloadFile(const std::string& url, const std::wstring& savePath);
	std::wstring getCurrentVersionFromFile() const;
	void saveCurrentVersion(const std::wstring& version) const;
};

