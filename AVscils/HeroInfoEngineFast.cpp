#include <filesystem>
#include "DataWarcraft.h"
#include "LogManager.h"
#include <format>
#include <ranges>
#include <string_view>

#include "HeroInfoEngineFast.h"

bool HeroInfoEngineFast::retrieveHeroDataFast(const std::wstring& saveCodePath) {
    static auto& logger = LogManager::instance();
    
    HeroInfo tempHeroInfo;
    tempHeroInfo.nameChar = "FastLoad";

    bool foundAnyPath = false;
    std::filesystem::file_time_type globalLatestTime{};
    const std::filesystem::path* globalLatestPath = nullptr;

    for (const auto& warPathDirectSave : G_DATA_WARCRAFT.m_DataPath.warPathDirectSave) {
        const std::filesystem::path fullSavePath = warPathDirectSave + saveCodePath;
        
        if (std::filesystem::is_directory(fullSavePath)) {
            auto [found, latestTime, latestPath] = parseHeroData(fullSavePath);
            if (found && latestTime > globalLatestTime) {
                globalLatestTime = latestTime;
                globalLatestPath = latestPath;
                foundAnyPath = true;
            }
        }
    }

    if (foundAnyPath && globalLatestPath) {
        tempHeroInfo.latestTime = globalLatestTime;
        tempHeroInfo.path = globalLatestPath->wstring();
    } else {
        logger.log(LogManager::LogLevel::Warning, L"Не найдено ни одной валидной папки сохранений");
    }

    G_HERO_INFO.push_back(std::move(tempHeroInfo));

    return !G_HERO_INFO[0].path.empty();
}

[[nodiscard]] std::tuple<bool, std::filesystem::file_time_type, const std::filesystem::path*> 
HeroInfoEngineFast::parseHeroData(const std::filesystem::path& folderPath) noexcept {
    static auto& logger = LogManager::instance();
    static constexpr wchar_t txtExtension[] = L".txt";
    
    try {
        std::filesystem::file_time_type latestTime{};
        const std::filesystem::path* latestPath = nullptr;

        for (const auto& entry : std::filesystem::directory_iterator(
            folderPath, 
            std::filesystem::directory_options::skip_permission_denied
        )) {
            if (!entry.is_regular_file()) continue;
            
            const auto& path = entry.path();
            if (path.extension() != txtExtension) continue;

            const auto fileTime = std::filesystem::last_write_time(path);
            if (fileTime > latestTime) {
                latestTime = fileTime;
                latestPath = &path;
            }
        }

        if (latestPath) {
            return {true, latestTime, latestPath};
        }
    } catch (const std::filesystem::filesystem_error&) {
        logger.log(LogManager::LogLevel::Error, 
            L"Ошибка при сканировании: " + folderPath.wstring());
    }
    
    return {false, std::filesystem::file_time_type{}, nullptr};
}