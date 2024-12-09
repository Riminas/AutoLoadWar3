#pragma once
#include <filesystem>
#include <fstream>

class LogError
{
public:
	void logError(const std::string& message) {
        checkDirectories();

        std::ofstream logFile(nameFile.c_str(), std::ios::app);
        if (logFile.is_open()) {
            logFile << "[ERROR]   " << message << std::endl;
        }
	}

    void logMessage(const std::string& message) {
        checkDirectories();

        std::ofstream logFile(nameFile.c_str(), std::ios::app);
        if (logFile.is_open()) {
            logFile << "[MESSAGE] " << message << std::endl;
        }
    }
    void logWarning(const std::string& message) {
        checkDirectories();

        std::ofstream logFile(nameFile.c_str(), std::ios::app);
        if (logFile.is_open()) {
            logFile << "[WARNIG]  " << message << std::endl;
        }
    }
private:
    const std::string nameFile = { "logs\\font_loader.log" };

    inline void checkDirectories() {
        if (!std::filesystem::exists("logs"))
            std::filesystem::create_directories("logs");
    }
};