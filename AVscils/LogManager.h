#pragma once
#include <filesystem>
#include <fstream>
#include <string_view>
#include <chrono>
#include <iomanip>
#include <sstream>

class LogManager
{
public:
    enum class LogLevel {
        Error,
        Message,
        Warning
    };

    static LogManager& instance() {
        static LogManager instance;
        return instance;
    }
    static LogManager& logger() {
        return instance();
    }

    template<typename T>
    void log(LogLevel level, T&& message) noexcept {
        try {
            if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string_view> ||
                std::is_same_v<std::remove_cvref_t<T>, std::string>) {
                writeLog(getLevelPrefix(level), std::forward<T>(message));
            }
            else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::wstring_view> ||
                std::is_same_v<std::remove_cvref_t<T>, std::wstring>) {
                writeLogW(getWLevelPrefix(level), std::forward<T>(message));
            }
        }
        catch (...) {}
    }

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;

private:
    LogManager() = default;
    const std::filesystem::path nameFile{ getCurrentLogFileName() };

    static constexpr std::string_view getLevelPrefix(LogLevel level) {
        switch (level) {
        case LogLevel::Error:   return "[ERROR]   ";
        case LogLevel::Message: return "[MESSAGE] ";
        case LogLevel::Warning: return "[WARNING] ";
        }
        return "[UNKNOWN] ";
    }

    static constexpr std::wstring_view getWLevelPrefix(LogLevel level) {
        switch (level) {
        case LogLevel::Error:   return L"[ERROR]   ";
        case LogLevel::Message: return L"[MESSAGE] ";
        case LogLevel::Warning: return L"[WARNING] ";
        }
        return L"[UNKNOWN] ";
    }

    inline void checkDirectories() const {
        static bool directoriesChecked = false;
        if (!directoriesChecked) {
            if (!std::filesystem::exists("logs")) {
                std::filesystem::create_directories("logs");
            }
            cleanOldLogs();
            directoriesChecked = true;
        }
    }

    template<typename T, typename U>
    void writeLog(T&& logLevel, U&& message) {
        checkDirectories();

        if constexpr (std::is_same_v<std::remove_cvref_t<U>, std::string> ||
            std::is_same_v<std::remove_cvref_t<U>, std::string_view>) {
            std::ofstream logFile(nameFile, std::ios::app);
            if (logFile.is_open()) {
                logFile << std::forward<T>(logLevel) << std::forward<U>(message) << '\n';
            }
        }
        else {
            std::wofstream logFile(nameFile, std::ios::app);
            if (logFile.is_open()) {
                logFile.imbue(std::locale(""));
                logFile << std::forward<T>(logLevel) << std::forward<U>(message) << '\n';
            }
        }
    }

    template<typename T, typename U>
    void writeLogW(T&& logLevel, U&& message) {
        writeLog(std::forward<T>(logLevel), std::forward<U>(message));
    }

    std::string getCurrentLogFileName() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_s(&tm, &time);
        std::ostringstream oss;
        oss << "logs/font_loader_" << std::put_time(&tm, "%Y%m%d") << ".log";
        return oss.str();
    }

    void cleanOldLogs() const {
        const std::filesystem::path logsDir{ "logs" };
        std::string currentFileName = getCurrentLogFileName();

        for (const auto& entry : std::filesystem::directory_iterator(logsDir)) {
            if (entry.path().string() != currentFileName) {
                std::filesystem::remove(entry.path());
            }
        }
    }
};
