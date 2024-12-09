#pragma once
#include "HeroInfo.h"
#include <vector>
#include <unordered_map>
class HeroInfoEngine
{
public:
	bool retrieveHeroData(const std::wstring& saveCodePath);
private:
	void removeSpecialCharacters(std::string& wstr);
	void sortList(std::vector<HeroInfo>& heroList);
	std::vector<HeroInfo> parseHeroData(const std::wstring& folderPath);
	void processDirectory(const std::filesystem::directory_entry& dirEntry, std::unordered_map<std::string, HeroInfo>& heroDataMap);
	void processFile(const std::filesystem::directory_entry& fileEntry, std::unordered_map<std::string, HeroInfo>& heroDataMap);
	bool isTextFile(const std::filesystem::directory_entry& entry) const;
	bool isHeroToken(const std::string& token) const;
	bool isHeroTokenIsTrue(const std::string& token) const;
	std::string extractHeroName(const std::vector<std::string>& tokens, size_t startIndex) const;
	void updateHeroInfo(const std::filesystem::directory_entry& fileEntry, HeroInfo& heroInfo);
	std::vector<HeroInfo> convertToHeroList(const std::unordered_map<std::string, HeroInfo>& heroDataMap) const;

	//void saveDataHero(const std::vector<HeroInfo>& heroInfo) const;
	//void loadDataHero(std::unordered_map<std::string, HeroInfo>& heroInfo);

	std::vector<HeroInfo> m_HeroList{};

};

