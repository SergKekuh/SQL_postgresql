#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
private:
    std::string logFilePath;

    static std::string getCurrentTimestamp();

public:
    explicit Logger(const std::string& logDir = "logs/");
    void log(const std::string& message);
    static std::string format(const std::string& fmt, ...);

    // Вспомогательные методы
    static bool createDirectoryIfNotExists(const std::string& dirPath);
    static std::string generateLogFilename(const std::string& dirPath);
};

// Макрос для удобного логгирования
#define LOG(message) Logger::format("%s | %s", "[INFO]", (std::string(message)).c_str())

#endif // LOGGER_H