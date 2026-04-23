#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace utsk {

/**
 * @brief Класс для работы с конфигурацией приложения
 * * Загружает настройки из JSON-файла (db_config.json)
 */
class Config {
public:
    struct DatabaseInfo {
        std::string host = "localhost";
        int port = 5432;
        std::string dbname = "bd_intelligent_sales";
        std::string user = "postgres";
        std::string password;
    };

    Config() = default;
    ~Config() = default;

    /**
     * @brief Загрузить конфигурацию из файла
     * @param filepath Путь к JSON-файлу
     * @return true если загрузка успешна
     */
    bool load(const std::string& filepath);

    /**
     * @brief Сохранить конфигурацию в файл
     * @param filepath Путь к JSON-файлу
     * @return true если сохранение успешно
     */
    bool save(const std::string& filepath);

    // Геттеры и сеттеры
    const DatabaseInfo& getDatabaseInfo() const { return m_dbInfo; }
    void setDatabaseInfo(const DatabaseInfo& info) { m_dbInfo = info; }

    const std::string& getLogLevel() const { return m_logLevel; }
    void setLogLevel(const std::string& level) { m_logLevel = level; }

    const std::string& getLogFile() const { return m_logFile; }
    void setLogFile(const std::string& file) { m_logFile = file; }

private:
    DatabaseInfo m_dbInfo;
    std::string m_logLevel = "info";
    std::string m_logFile = "utsk.log";
};

} // namespace utsk

#endif // CONFIG_HPP
