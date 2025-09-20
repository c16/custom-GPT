#include "claude_agent.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <unistd.h>

ClaudeAgent::ClaudeAgent(const std::string& config_file, CliProvider cli_provider)
    : cli_provider_(cli_provider)
    , active_provider_(CliProvider::AUTO) {

    // Check for config directory environment variable or use default
    const char* config_dir_env = std::getenv("CLAUDE_AGENT_CONFIG_DIR");
    std::string config_dir = config_dir_env ? config_dir_env : "../configs";

    // If config_file is just a filename, prepend the config directory
    if (config_file.find('/') == std::string::npos && config_file[0] != '/') {
        config_file_ = config_dir + "/" + config_file;
    } else {
        config_file_ = config_file;
    }

    last_config_file_ = config_dir + "/.last_config";

    LOG_INFO("ClaudeAgent constructor called with config_file=" + config_file);
    LOG_DEBUG("CLI provider: " + providerToString(cli_provider));

    LOG_DEBUG("Starting config load...");
    loadConfig();
    LOG_DEBUG("Config load completed");

    // Defer CLI detection to initializeCli() method
    LOG_INFO("ClaudeAgent constructor completed - CLI detection deferred");
}

bool ClaudeAgent::initializeCli() {
    LOG_DEBUG("Starting CLI initialization...");
    auto [path, provider] = findAvailableCli();
    LOG_DEBUG("CLI detection completed");

    cli_path_ = path;
    active_provider_ = provider;

    if (!cli_path_.empty()) {
        LOG_INFO("Successfully initialized with CLI: " + cli_path_ + " (provider: " + providerToString(active_provider_) + ")");
        return true;
    } else {
        LOG_WARNING("No CLI found during initialization");
        return false;
    }
}

bool ClaudeAgent::loadConfig() {
    LOG_DEBUG("Starting configuration load process");

    // Try to load last used config first
    std::string last_config_path = loadLastConfigPath();
    if (!last_config_path.empty() && std::filesystem::exists(last_config_path)) {
        LOG_DEBUG("Found last used config path: " + last_config_path);
        if (loadConfigFromFile(last_config_path)) {
            LOG_INFO("Loaded last used configuration from " + last_config_path);
            Logger::getInstance().logConfigChange("last_used", "Loaded from " + last_config_path);
            return true;
        }
        LOG_WARNING("Failed to load last used config, falling back to default");
    }

    // Fall back to default config file
    if (std::filesystem::exists(config_file_)) {
        LOG_DEBUG("Attempting to load default config file: " + config_file_);
        try {
            config_ = json::parseFromFile(config_file_);
            LOG_INFO("Loaded configuration from " + config_file_);
            Logger::getInstance().logConfigChange("default", "Loaded from " + config_file_);
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("Error loading config: " + std::string(e.what()) + ". Using default configuration.");
        }
    }

    // Create default config
    LOG_INFO("No configuration file found, creating default configuration");
    config_ = createDefaultConfig();
    Logger::getInstance().logConfigChange("default", "Created new default configuration");
    return true;
}

bool ClaudeAgent::saveConfig() {
    LOG_DEBUG("Attempting to save configuration to " + config_file_);
    try {
        bool result = json::saveToFile(config_file_, config_);
        if (result) {
            LOG_INFO("Configuration saved successfully to " + config_file_);
            Logger::getInstance().logConfigChange("save", "Saved to " + config_file_);
        } else {
            LOG_ERROR("Failed to save configuration to " + config_file_);
        }
        return result;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("ConfigManager", "save configuration", e.what());
        return false;
    }
}

bool ClaudeAgent::loadConfigFromFile(const std::string& file_path) {
    try {
        auto config = json::parseFromFile(file_path);

        // Validate configuration
        std::vector<std::string> required_keys = {"name", "description", "instructions", "conversation_starters"};
        for (const auto& key : required_keys) {
            if (!config->asObject().find(key)->second) {
                std::cerr << "Invalid configuration: missing '" << key << "' field" << std::endl;
                return false;
            }
        }

        config_ = config;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from " << file_path << ": " << e.what() << std::endl;
        return false;
    }
}

bool ClaudeAgent::saveConfigToFile(std::shared_ptr<json::Value> config, const std::string& filename) {
    std::string full_filename = filename;
    if (full_filename.find(".json") == std::string::npos) {
        full_filename += ".json";
    }

    // Create configs directory if it doesn't exist
    std::filesystem::create_directories("configs");

    std::string file_path = "configs/" + full_filename;

    try {
        return json::saveToFile(file_path, config);
    } catch (const std::exception& e) {
        std::cerr << "Error saving configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ClaudeAgent::loadSpecificConfig(const std::string& file_path) {
    if (loadConfigFromFile(file_path)) {
        saveLastConfigPath(file_path);
        std::cout << "Configuration loaded from " << file_path << std::endl;
        std::cout << "Now using: " << getName() << std::endl;
        return true;
    }
    return false;
}

std::string ClaudeAgent::sendToClaudeApi(const std::string& message, bool use_system_prompt) {
    return sendToCli(message, use_system_prompt);
}

std::string ClaudeAgent::sendToCli(const std::string& message, bool use_system_prompt) {
    LOG_INFO("Sending message to CLI (length: " + std::to_string(message.length()) + " chars)");
    LOG_DEBUG("Message preview: " + message.substr(0, 100) + (message.length() > 100 ? "..." : ""));

    if (cli_path_.empty()) {
        std::string error = "Error: " + getActiveProviderName() + " CLI not available";
        LOG_ERROR(error);
        return error;
    }

    try {
        std::vector<std::string> cmd;

        // Build conversation context
        std::string full_message = buildConversationContext(message);
        Logger::getInstance().logConversationContext(full_message);

        if (active_provider_ == CliProvider::CLAUDE) {
            cmd = {cli_path_, "--print"};

            if (use_system_prompt) {
                std::string system_prompt = getSystemPrompt();
                cmd.insert(cmd.end(), {"--append-system-prompt", system_prompt});
                LOG_DEBUG("Added system prompt (length: " + std::to_string(system_prompt.length()) + " chars)");
            }

            // For Claude CLI, use stdin instead of command line argument for complex messages
            bool use_stdin = full_message.length() > 100 || full_message.find('\n') != std::string::npos ||
                full_message.find('\'') != std::string::npos || full_message.find('"') != std::string::npos;

            if (use_stdin) {
                cmd.push_back("-"); // This tells claude CLI to read from stdin
                LOG_DEBUG("Using stdin for complex message");
            } else {
                cmd.push_back(full_message);
                LOG_DEBUG("Using command line argument for simple message");
            }
        } else if (active_provider_ == CliProvider::GEMINI) {
            cmd = {cli_path_, "--prompt"};

            if (use_system_prompt) {
                std::string system_prompt = getSystemPrompt();
                full_message = system_prompt + "\n\nUser: " + full_message;
                LOG_DEBUG("Added system prompt for Gemini (total length: " + std::to_string(full_message.length()) + " chars)");
            }

            // For Gemini CLI, also use stdin for complex messages
            bool use_stdin = full_message.length() > 100 || full_message.find('\n') != std::string::npos ||
                full_message.find('\'') != std::string::npos || full_message.find('"') != std::string::npos;

            if (use_stdin) {
                cmd.push_back("-"); // This tells gemini CLI to read from stdin
                LOG_DEBUG("Using stdin for complex message");
            } else {
                cmd.push_back(full_message);
                LOG_DEBUG("Using command line argument for simple message");
            }
        } else {
            std::string error = "Error: Unknown CLI provider";
            LOG_ERROR(error);
            return error;
        }

        std::cout << "Sending to " << getActiveProviderName() << ": "
                  << message.substr(0, 100) << (message.length() > 100 ? "..." : "") << std::endl;

        std::string response = executeCommand(cmd, full_message);

        if (!response.empty() && response.find("Error") != 0) {
            // Store in conversation history
            ConversationEntry entry;
            entry.user = message;
            entry.assistant = response;
            entry.timestamp = std::chrono::system_clock::now();
            conversation_history_.push_back(entry);

            LOG_INFO("Message sent successfully, response received (length: " + std::to_string(response.length()) + " chars)");
            LOG_DEBUG("Response preview: " + response.substr(0, 100) + (response.length() > 100 ? "..." : ""));
        } else {
            LOG_WARNING("Received error response: " + response);
        }

        return response;
    } catch (const std::exception& e) {
        std::string error = "Error communicating with " + getActiveProviderName() + " CLI: " + e.what();
        Logger::getInstance().logError("CLICommunicator", "send message", e.what());
        return error;
    }
}

bool ClaudeAgent::switchCliProvider(CliProvider new_provider) {
    cli_provider_ = new_provider;
    auto [path, provider] = findAvailableCli();
    cli_path_ = path;
    active_provider_ = provider;
    return !cli_path_.empty();
}

std::string ClaudeAgent::getActiveProviderName() const {
    return providerToString(active_provider_);
}

// Utility methods
std::string ClaudeAgent::getName() const {
    auto name_value = config_->asObject().find("name");
    return (name_value != config_->asObject().end() && name_value->second)
           ? name_value->second->asString() : "Custom AI Agent";
}

std::string ClaudeAgent::getDescription() const {
    auto desc_value = config_->asObject().find("description");
    return (desc_value != config_->asObject().end() && desc_value->second)
           ? desc_value->second->asString() : "A helpful AI assistant";
}

std::string ClaudeAgent::getInstructions() const {
    auto inst_value = config_->asObject().find("instructions");
    return (inst_value != config_->asObject().end() && inst_value->second)
           ? inst_value->second->asString() : "You are a helpful AI assistant.";
}

std::vector<std::string> ClaudeAgent::getConversationStarters() const {
    std::vector<std::string> starters;
    auto starters_value = config_->asObject().find("conversation_starters");

    if (starters_value != config_->asObject().end() && starters_value->second &&
        starters_value->second->isArray()) {
        const auto& array = starters_value->second->asArray();
        for (const auto& item : array) {
            if (item && item->isString()) {
                starters.push_back(item->asString());
            }
        }
    }

    return starters;
}

int ClaudeAgent::getConversationMemory() const {
    auto memory_value = config_->asObject().find("conversation_memory");
    return (memory_value != config_->asObject().end() && memory_value->second && memory_value->second->isNumber())
           ? static_cast<int>(memory_value->second->asNumber()) : 5;
}

void ClaudeAgent::setName(const std::string& name) {
    auto obj = std::static_pointer_cast<json::ObjectValue>(config_);
    obj->set("name", json::string(name));
}

void ClaudeAgent::setDescription(const std::string& description) {
    auto obj = std::static_pointer_cast<json::ObjectValue>(config_);
    obj->set("description", json::string(description));
}

void ClaudeAgent::setInstructions(const std::string& instructions) {
    auto obj = std::static_pointer_cast<json::ObjectValue>(config_);
    obj->set("instructions", json::string(instructions));
}

void ClaudeAgent::setConversationStarters(const std::vector<std::string>& starters) {
    auto arr = json::array();
    for (const auto& starter : starters) {
        std::static_pointer_cast<json::ArrayValue>(arr)->push(json::string(starter));
    }
    auto obj = std::static_pointer_cast<json::ObjectValue>(config_);
    obj->set("conversation_starters", arr);
}

void ClaudeAgent::setConversationMemory(int memory) {
    auto obj = std::static_pointer_cast<json::ObjectValue>(config_);
    obj->set("conversation_memory", json::number(memory));
}

// Private helper methods
std::string ClaudeAgent::findClaudeCli() {
    LOG_DEBUG("Starting Claude CLI detection");

    std::vector<std::string> possible_paths = {
        "claude",
        "/usr/local/bin/claude",
        "/usr/bin/claude"
    };

    for (const auto& path : possible_paths) {
        try {
            // Use which command first - much faster
            std::string which_cmd = "which " + path + " >/dev/null 2>&1";
            if (system(which_cmd.c_str()) == 0) {
                LOG_DEBUG("Found Claude CLI at: " + path);
                return path;
            }
        } catch (...) {
            continue;
        }
    }
    LOG_DEBUG("Claude CLI not found");
    return "";
}

std::string ClaudeAgent::findGeminiCli() {
    LOG_DEBUG("Starting Gemini CLI detection");

    std::vector<std::string> possible_paths = {
        "gemini",
        "/usr/local/bin/gemini",
        "/usr/bin/gemini"
    };

    for (const auto& path : possible_paths) {
        try {
            // Use which command first - much faster
            std::string which_cmd = "which " + path + " >/dev/null 2>&1";
            if (system(which_cmd.c_str()) == 0) {
                LOG_DEBUG("Found Gemini CLI at: " + path);
                return path;
            }
        } catch (...) {
            continue;
        }
    }
    LOG_DEBUG("Gemini CLI not found");
    return "";
}

std::pair<std::string, CliProvider> ClaudeAgent::findAvailableCli() {
    std::string claude_path = findClaudeCli();
    std::string gemini_path = findGeminiCli();

    if (cli_provider_ == CliProvider::CLAUDE) {
        if (!claude_path.empty()) {
            std::cout << "Found Claude CLI at: " << claude_path << std::endl;
            return {claude_path, CliProvider::CLAUDE};
        } else {
            std::cout << "Claude CLI not found but was specifically requested." << std::endl;
            return {"", CliProvider::AUTO};
        }
    } else if (cli_provider_ == CliProvider::GEMINI) {
        if (!gemini_path.empty()) {
            std::cout << "Found Gemini CLI at: " << gemini_path << std::endl;
            return {gemini_path, CliProvider::GEMINI};
        } else {
            std::cout << "Gemini CLI not found but was specifically requested." << std::endl;
            return {"", CliProvider::AUTO};
        }
    } else { // AUTO mode
        if (!claude_path.empty()) {
            std::cout << "Found Claude CLI at: " << claude_path << std::endl;
            return {claude_path, CliProvider::CLAUDE};
        } else if (!gemini_path.empty()) {
            std::cout << "Found Gemini CLI at: " << gemini_path << std::endl;
            return {gemini_path, CliProvider::GEMINI};
        } else {
            std::cout << "Neither Claude nor Gemini CLI found. Please ensure one is installed." << std::endl;
            return {"", CliProvider::AUTO};
        }
    }
}

std::string ClaudeAgent::getSystemPrompt() {
    std::ostringstream oss;
    oss << "You are " << getName() << ".\n\n";
    oss << "Description: " << getDescription() << "\n\n";
    oss << "Instructions:\n" << getInstructions() << "\n\n";
    oss << "Please follow these instructions carefully and embody the role described above.";
    return oss.str();
}

std::string ClaudeAgent::buildConversationContext(const std::string& current_message, int max_history) {
    if (conversation_history_.empty()) {
        return current_message;
    }

    if (max_history == -1) {
        max_history = getConversationMemory();
    }

    std::ostringstream oss;
    oss << "Previous conversation:\n";

    // Get recent conversation history
    size_t start = conversation_history_.size() > static_cast<size_t>(max_history)
                   ? conversation_history_.size() - max_history : 0;

    for (size_t i = start; i < conversation_history_.size(); ++i) {
        const auto& entry = conversation_history_[i];
        oss << "Human: " << entry.user << "\n";
        oss << "Assistant: " << entry.assistant << "\n";
    }

    oss << "\nCurrent message:\n";
    oss << "Human: " << current_message;

    return oss.str();
}

void ClaudeAgent::saveLastConfigPath(const std::string& config_path) {
    try {
        std::ofstream file(last_config_file_);
        if (file.is_open()) {
            file << config_path;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving last config path: " << e.what() << std::endl;
    }
}

std::string ClaudeAgent::loadLastConfigPath() {
    try {
        if (std::filesystem::exists(last_config_file_)) {
            std::ifstream file(last_config_file_);
            if (file.is_open()) {
                std::string path;
                std::getline(file, path);
                return path;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading last config path: " << e.what() << std::endl;
    }
    return "";
}

std::shared_ptr<json::Value> ClaudeAgent::createDefaultConfig() {
    auto config = json::object();
    auto obj = std::static_pointer_cast<json::ObjectValue>(config);

    obj->set("name", json::string("Custom AI Agent"));
    obj->set("description", json::string("A helpful AI assistant"));
    obj->set("instructions", json::string(
        "You are a helpful AI assistant. Please provide clear, concise, and accurate responses.\n\n"
        "Your primary capabilities include:\n"
        "- Answering questions across various topics\n"
        "- Helping with problem-solving\n"
        "- Providing explanations and guidance\n"
        "- Assisting with code and technical issues\n\n"
        "Always be polite, professional, and helpful in your responses."
    ));

    auto starters = json::array();
    auto starters_arr = std::static_pointer_cast<json::ArrayValue>(starters);
    starters_arr->push(json::string("How can I help you today?"));
    starters_arr->push(json::string("What would you like to work on?"));
    starters_arr->push(json::string("Tell me about your project and I'll assist you."));
    starters_arr->push(json::string("What questions do you have for me?"));
    obj->set("conversation_starters", starters);

    obj->set("system_prompt", json::string(""));
    obj->set("max_tokens", json::number(4000));
    obj->set("temperature", json::number(0.7));
    obj->set("conversation_memory", json::number(5));

    return config;
}

std::string ClaudeAgent::executeCommand(const std::vector<std::string>& command, const std::string& stdin_input) {
    Logger::getInstance().logCommand(command, stdin_input);

    if (command.empty()) {
        std::string error = "Error: Empty command";
        LOG_ERROR(error);
        return error;
    }

    try {
        // Check if we need to use stdin (last argument is "-")
        bool use_stdin = !command.empty() && command.back() == "-";

        if (use_stdin && stdin_input.empty()) {
            std::string error = "Error: Stdin input required but not provided";
            LOG_ERROR(error);
            return error;
        }

        // Build command string with proper escaping
        std::string cmd = escapeShellArg(command[0]);
        size_t end_idx = use_stdin ? command.size() - 1 : command.size();

        for (size_t i = 1; i < end_idx; ++i) {
            cmd += " " + escapeShellArg(command[i]);
        }

        if (use_stdin) {
            // Add the "-" argument for stdin
            cmd += " -";
        }

        LOG_DEBUG("Built command string: " + cmd);

        // Use popen with "w" mode if we need to write to stdin
        FILE* pipe;
        if (use_stdin) {
            pipe = popen(cmd.c_str(), "w");
            if (!pipe) {
                return "Error: Failed to execute command";
            }

            // Write stdin input and close the pipe
            fwrite(stdin_input.c_str(), 1, stdin_input.length(), pipe);
            fflush(pipe);

            int status = pclose(pipe);
            if (status != 0) {
                return "Error: Command failed with status " + std::to_string(status);
            }

            // For stdin mode, we need to capture output differently
            // Let's use a different approach with temporary file
            std::string temp_file = "/tmp/claude_agent_" + std::to_string(getpid()) + ".txt";
            std::ofstream temp(temp_file);
            if (!temp.is_open()) {
                return "Error: Could not create temporary file";
            }
            temp << stdin_input;
            temp.close();

            // Execute command with file redirection
            std::string cmd_with_redirect = cmd + " < \"" + temp_file + "\"";
            pipe = popen(cmd_with_redirect.c_str(), "r");
            if (!pipe) {
                std::filesystem::remove(temp_file);
                return "Error: Failed to execute command";
            }

            std::string result;
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }

            status = pclose(pipe);
            std::filesystem::remove(temp_file);

            if (status != 0) {
                return "Error: Command failed with status " + std::to_string(status);
            }

            // Remove trailing newline
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }

            return result;
        } else {
            // Regular command execution
            LOG_DEBUG("Executing regular command (no stdin)");
            pipe = popen(cmd.c_str(), "r");
            if (!pipe) {
                std::string error = "Error: Failed to execute command";
                LOG_ERROR(error);
                return error;
            }

            std::string result;
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }

            int status = pclose(pipe);
            Logger::getInstance().logResponse(result, status);

            if (status != 0) {
                std::string error = "Error: Command failed with status " + std::to_string(status);
                LOG_ERROR(error);
                return error;
            }

            // Remove trailing newline
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }

            return result;
        }
    } catch (const std::exception& e) {
        Logger::getInstance().logError("CommandExecutor", "execute command", e.what());
        return "Error: " + std::string(e.what());
    }
}

std::string ClaudeAgent::escapeShellArg(const std::string& arg) {
    std::string escaped = "'";
    for (char c : arg) {
        if (c == '\'') {
            escaped += "'\"'\"'";
        } else {
            escaped += c;
        }
    }
    escaped += "'";
    return escaped;
}

std::string ClaudeAgent::providerToString(CliProvider provider) const {
    switch (provider) {
        case CliProvider::CLAUDE: return "claude";
        case CliProvider::GEMINI: return "gemini";
        case CliProvider::AUTO: return "auto";
        default: return "unknown";
    }
}

CliProvider ClaudeAgent::stringToProvider(const std::string& provider) const {
    if (provider == "claude") return CliProvider::CLAUDE;
    if (provider == "gemini") return CliProvider::GEMINI;
    return CliProvider::AUTO;
}