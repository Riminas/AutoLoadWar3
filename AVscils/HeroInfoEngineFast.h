#pragma once
#include "HeroInfo.h"
class HeroInfoEngineFast
{
public:
	bool retrieveHeroDataFast(const std::wstring& saveCodePath);
private:
	void parseHeroData(const std::wstring& folderPath);
};

