#include <string>
#include <fstream>
#include <filesystem>
#include <mutex>


#pragma once
class getMapOpen
{
public:
	std::wstring getMapOpen1(const std::wstring& folder_path);
private:
	bool checkFile(const std::filesystem::path& filePath, std::string& nameFile);

	bool containsOnlyEnglishCharacters(const std::string& text);

	std::mutex m_Mtx;
	std::string m_nameFile;
};

