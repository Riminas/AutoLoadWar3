#pragma once
#include "HeroInfo.h"
#include <vector>
#include <unordered_map>
#include <string_view>
#include <filesystem>

class HeroInfoEngine
{
public:
    bool retrieveHeroData(std::wstring_view saveCodePath);
private:
	static constexpr size_t INITIAL_RESERVE_SIZE = 1000;
	static constexpr std::string_view HERO_FILE_NAME = "Hero.ini";
	
	void removeSpecialCharacters(std::string& str);
	void sortList(std::vector<HeroInfo>& heroList);
	std::vector<HeroInfo> parseHeroData(std::wstring_view folderPath);
	void processDirectory(const std::filesystem::directory_entry& dirEntry, std::unordered_map<std::string, HeroInfo>& heroDataMap);
	void processFile(const std::filesystem::directory_entry& fileEntry, std::unordered_map<std::string, HeroInfo>& heroDataMap);
	
	[[nodiscard]] bool isTextFile(const std::filesystem::directory_entry& entry) const noexcept;
	[[nodiscard]] bool isHeroToken(std::string_view token) const noexcept;
	[[nodiscard]] bool isHeroTokenIsTrue(std::string_view token) const noexcept;
	[[nodiscard]] std::string extractHeroName(const std::vector<std::string>& tokens, size_t startIndex) const;
	[[nodiscard]] std::filesystem::file_time_type getMinFileTime() const;
	
	void updateHeroInfo(const std::filesystem::directory_entry& fileEntry, HeroInfo& heroInfo);
	[[nodiscard]] std::vector<HeroInfo> convertToHeroList(
		const std::unordered_map<std::string, HeroInfo>& heroDataMap) const;

	void saveDataHero(const std::vector<HeroInfo>& heroInfo) const;
	bool loadDataHero(std::vector<HeroInfo>& heroInfo) const;

	bool m_IsDataLoaded = false;
	std::vector<HeroInfo> m_HeroList;
};

