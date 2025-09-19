#include "logger.h"
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

std::unique_ptr<Logger> Logger::instance_ = nullptr;
std::mutex Logger::instance_mutex_;

Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<Logger>(new Logger());

        // Initialize with default log file
        std::string default_log = "claude_agent.log";
        instance_->setLogFile(default_log);
        instance_->enableFileOutput(true);

        // Log startup
        instance_->info("Logger", "Logging system initialized");
    }
    return *instance_;
}

Logger::~Logger() {
    if (log_file_ && log_file_->is_open()) {
        info("Logger", "Shutting down logging system");
        log_file_->close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    current_level_ = level;
    // Note: Cannot log here due to mutex deadlock
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex_);

    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }

    log_filename_ = filename;
    log_file_ = std::make_unique<std::ofstream>(filename, std::ios::app);

    if (log_file_->is_open()) {
        // Write session separator
        *log_file_ << "\n" << std::string(80, '=') << "\n";
        *log_file_ << "NEW SESSION STARTED: " << getCurrentTimestamp() << "\n";
        *log_file_ << std::string(80, '=') << "\n" << std::endl;
    }
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    console_output_ = enable;
}

void Logger::enableFileOutput(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    file_output_ = enable;
}

void Logger::log(LogLevel level, const std::string& message) {
    log(level, "General", message);
}

void Logger::log(LogLevel level, const std::string& component, const std::string& message) {
    if (level < current_level_) {
        return;
    }

    std::lock_guard<std::mutex> lock(log_mutex_);
    std::string formatted = formatMessage(level, component, message);

    if (console_output_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
    }

    if (file_output_ && log_file_ && log_file_->is_open()) {
        *log_file_ << formatted << std::endl;
        log_file_->flush();
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::debug(const std::string& component, const std::string& message) {
    log(LogLevel::DEBUG, component, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::info(const std::string& component, const std::string& message) {
    log(LogLevel::INFO, component, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::warning(const std::string& component, const std::string& message) {
    log(LogLevel::WARNING, component, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::error(const std::string& component, const std::string& message) {
    log(LogLevel::ERROR, component, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::critical(const std::string& component, const std::string& message) {
    log(LogLevel::CRITICAL, component, message);
}

void Logger::logCommand(const std::vector<std::string>& command, const std::string& stdin_input) {
    std::ostringstream oss;
    oss << "Executing command: ";
    for (size_t i = 0; i < command.size(); ++i) {
        if (i > 0) oss << " ";
        oss << "'" << command[i] << "'";
    }

    if (!stdin_input.empty()) {
        oss << " (with stdin input, " << stdin_input.length() << " chars)";
        debug("CommandExecutor", oss.str());

        // Log first 200 chars of stdin for debugging
        std::string preview = stdin_input.substr(0, 200);
        if (stdin_input.length() > 200) {
            preview += "...";
        }
        debug("CommandExecutor", "Stdin preview: " + preview);
    } else {
        debug("CommandExecutor", oss.str());
    }
}

void Logger::logResponse(const std::string& response, int status_code) {
    std::ostringstream oss;
    oss << "Command completed with status " << status_code << ", response length: " << response.length();

    if (status_code == 0) {
        debug("CommandExecutor", oss.str());

        // Log first 200 chars of response for debugging
        std::string preview = response.substr(0, 200);
        if (response.length() > 200) {
            preview += "...";
        }
        debug("CommandExecutor", "Response preview: " + preview);
    } else {
        error("CommandExecutor", oss.str());
        error("CommandExecutor", "Error response: " + response);
    }
}

void Logger::logConversationContext(const std::string& context) {
    debug("ConversationManager", "Built context with " + std::to_string(context.length()) + " characters");

    // Count newlines to estimate conversation entries
    size_t newlines = std::count(context.begin(), context.end(), '\n');
    debug("ConversationManager", "Context contains approximately " + std::to_string(newlines) + " lines");
}

void Logger::logConfigChange(const std::string& config_name, const std::string& change_description) {
    info("ConfigManager", "Configuration change in '" + config_name + "': " + change_description);
}

void Logger::logError(const std::string& component, const std::string& operation, const std::string& error_details) {
    error(component, "Failed to " + operation + ": " + error_details);
}

std::string Logger::formatMessage(LogLevel level, const std::string& component, const std::string& message) {
    std::ostringstream oss;

    oss << "[" << getCurrentTimestamp() << "] ";
    oss << "[" << std::setw(8) << std::left << levelToString(level) << "] ";
    oss << "[" << std::setw(15) << std::left << component << "] ";
    oss << message;

    return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}