#pragma once
#include "HeroInfo.h"
class HeroInfoEngineFast
{
public:
	bool retrieveHeroDataFast(const std::wstring& saveCodePath);
private:
	[[nodiscard]] std::tuple<bool, std::filesystem::file_time_type, const std::filesystem::path> 
		parseHeroData(const std::filesystem::path& folderPath) noexcept;
};

