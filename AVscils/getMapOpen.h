#include <string>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <Windows.h>


#pragma once
class getMapOpen
{
public:
	std::wstring getMapOpen1(const std::wstring& folder_path);

private:
	bool findUsedMapFile(const std::wstring& folder_path);
	bool isFileUsedByProcess(const std::filesystem::path& path, DWORD processId);
	std::wstring processMapFile();
	static std::string removeColorCodes(std::string str);
	static bool containsOnlyEnglishCharacters(const std::string& text);

	std::wstring m_nameFile;
};

