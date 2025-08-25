#include "Logger.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include <stdarg.h>
#include <sys/stat.h>  // Для mkdir

std::string Logger::getCurrentTimestamp() {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(buffer);
}

bool Logger::createDirectoryIfNotExists(const std::string& dirPath) {
    struct stat info;
    if (stat(dirPath.c_str(), &info) != 0) {
#ifdef _WIN32
        if (_mkdir(dirPath.c_str()) != 0) {
#else
        if (mkdir(dirPath.c_str(), 0777) != 0) {
#endif
            std::cerr << "Failed to create directory: " << dirPath << std::endl;
            return false;
        }
    }
    return true;
}

std::string Logger::generateLogFilename(const std::string& dirPath) {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "log_%Y-%m-%d.txt", localTime);
    return dirPath + std::string(buffer);
}

Logger::Logger(const std::string& logDir) {
    if (!createDirectoryIfNotExists(logDir)) {
        std::cerr << "Error creating logs directory." << std::endl;
        return;
    }

    logFilePath = generateLogFilename(logDir);
}

void Logger::log(const std::string& message) {
    std::ofstream file(logFilePath, std::ios_base::app); // append mode
    if (file.is_open()) {
        file << getCurrentTimestamp() << " " << message << std::endl;
        file.close();
    } else {
        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
    }
}

std::string Logger::format(const std::string& fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    va_end(args);
    return std::string(buffer);
}