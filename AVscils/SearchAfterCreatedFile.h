#pragma once
#include <string>
#include <filesystem>

class SearchAfterCreatedFile {
public:
    SearchAfterCreatedFile(const std::filesystem::file_time_type& start_time)
        : start_time_(start_time) {}

    std::wstring searchAfterCreatedFile1() const;


private:
    void recursiveDirectoryIteratorFor(const std::wstring_view path, std::multimap<std::filesystem::file_time_type, std::wstring>& updated_dirs) const;
    std::filesystem::file_time_type start_time_;
};

