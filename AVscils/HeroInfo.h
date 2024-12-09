#pragma once
#include <string>
#include <filesystem>

struct HeroInfo
{
    std::string nameChar = "\0";
    std::wstring path = L"\0";
    std::filesystem::file_time_type latestTime = std::filesystem::file_time_type();

    HeroInfo() = default;
    HeroInfo(const std::string& name, const std::wstring& path, const std::filesystem::file_time_type& time)
        : nameChar(name), path(path), latestTime(time) {}
};

extern std::vector<HeroInfo> G_HERO_INFO;

