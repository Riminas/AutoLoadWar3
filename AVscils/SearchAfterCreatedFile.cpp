#include "DataWarcraft.h"
#include <filesystem>
#include <map>
#include "SearchAfterCreatedFile.h"

std::wstring SearchAfterCreatedFile::searchAfterCreatedFile1() const
{
    // Use std::multimap to store directories with their modification times, automatically sorted
    std::multimap<std::filesystem::file_time_type, std::wstring> updated_dirs;

    recursiveDirectoryIteratorFor(G_DATA_WARCRAFT.m_DataPath.warPathDirectSave[G_DATA_WARCRAFT.m_DataPath.versionWarcraft], updated_dirs);
    //recursiveDirectoryIteratorFor(G_DATA_WARCRAFT.m_DataPath.warPathDirectSave[1], updated_dirs);

    // Traverse in reverse order to go from newest to oldest modified directory
    for (auto it = updated_dirs.rbegin(); it != updated_dirs.rend(); ++it) {
        const auto& dir_path = it->second;
        for (const auto& file : std::filesystem::directory_iterator(dir_path)) {
            if (std::filesystem::is_regular_file(file) && file.path().extension() == ".txt") {
                return dir_path;  // Return path to the first suitable directory
            }
        }
    }

    return L"00"; // Return "0" if no suitable directory is found
}

inline void SearchAfterCreatedFile::recursiveDirectoryIteratorFor(const std::wstring_view path, std::multimap<std::filesystem::file_time_type, std::wstring>& updated_dirs) const {
    if (!path.empty()) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(std::wstring(path))) {
            if (std::filesystem::is_directory(entry)) {
                auto last_write_time = std::filesystem::last_write_time(entry);
                if (last_write_time > start_time_) {
                    // Insert into multimap, key is the last_write_time, value is the directory path
                    updated_dirs.emplace(last_write_time, entry.path().wstring());
                }
            }
        }
    }
}




//#include "SFML/Graphics.hpp"
//#include <Windows.h>
//#include "DataWarcraft.h"
//#include <filesystem>
//#include <string>
//#include <vector>
//#include "SearchAfterCreatedFile.h"
//
//std::wstring SearchAfterCreatedFile::searchAfterCreatedFile1() const
//{
//    std::vector<std::pair<std::wstring, std::filesystem::file_time_type>> updated_dirs;
//    updated_dirs.reserve(4);
//
//    for (const auto& entry : std::filesystem::recursive_directory_iterator(G_DATA_WARCRAFT.m_DataPath.warPathDirectSave)) {
//        if (std::filesystem::is_directory(entry)) {
//            std::chrono::file_time last_write_time = std::filesystem::last_write_time(entry);
//            if (last_write_time > start_time_) {
//                updated_dirs.emplace_back(entry.path().string(), last_write_time);
//            }
//        }
//    }
//
//    // Сортируем по времени обновления, от самого нового к старому
//    std::sort(updated_dirs.begin(), updated_dirs.end(),
//        [](const auto& a, const auto& b) {
//            return a.second > b.second;
//        });
//
//    // Проверяем каждую папку на наличие файлов с расширением .txt
//    for (const auto& [dir_path, _] : updated_dirs) {
//        for (const auto& file : std::filesystem::directory_iterator(dir_path)) {
//            if (std::filesystem::is_regular_file(file) && file.path().extension() == ".txt") {
//                return dir_path;  // Возвращаем путь до первой подходящей папки
//            }
//        }
//    }
//
//    return L"0"; // Возвращаем пустую строку, если ничего не найдено
//}
