#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <mutex>
#include <chrono>
#include <sstream>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

class Logger {
public:
    static Logger& getInstance();

    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filename);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);

    void log(LogLevel level, const std::string& message);
    void log(LogLevel level, const std::string& component, const std::string& message);

    // Convenience methods
    void debug(const std::string& message);
    void debug(const std::string& component, const std::string& message);
    void info(const std::string& message);
    void info(const std::string& component, const std::string& message);
    void warning(const std::string& message);
    void warning(const std::string& component, const std::string& message);
    void error(const std::string& message);
    void error(const std::string& component, const std::string& message);
    void critical(const std::string& message);
    void critical(const std::string& component, const std::string& message);

    // Special methods for debugging
    void logCommand(const std::vector<std::string>& command, const std::string& stdin_input = "");
    void logResponse(const std::string& response, int status_code = 0);
    void logConversationContext(const std::string& context);
    void logConfigChange(const std::string& config_name, const std::string& change_description);
    void logError(const std::string& component, const std::string& operation, const std::string& error_details);

public:
    ~Logger();

private:
    Logger() = default;

    std::string formatMessage(LogLevel level, const std::string& component, const std::string& message);
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();

    LogLevel current_level_ = LogLevel::INFO;
    bool console_output_ = true;
    bool file_output_ = false;
    std::string log_filename_;
    std::unique_ptr<std::ofstream> log_file_;
    std::mutex log_mutex_;

    // Static instance
    static std::unique_ptr<Logger> instance_;
    static std::mutex instance_mutex_;
};

// Convenience macros for easier logging
#define LOG_DEBUG(msg) Logger::getInstance().debug(__func__, msg)
#define LOG_INFO(msg) Logger::getInstance().info(__func__, msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(__func__, msg)
#define LOG_ERROR(msg) Logger::getInstance().error(__func__, msg)
#define LOG_CRITICAL(msg) Logger::getInstance().critical(__func__, msg)

#define LOG_DEBUG_COMP(comp, msg) Logger::getInstance().debug(comp, msg)
#define LOG_INFO_COMP(comp, msg) Logger::getInstance().info(comp, msg)
#define LOG_WARNING_COMP(comp, msg) Logger::getInstance().warning(comp, msg)
#define LOG_ERROR_COMP(comp, msg) Logger::getInstance().error(comp, msg)
#define LOG_CRITICAL_COMP(comp, msg) Logger::getInstance().critical(comp, msg)