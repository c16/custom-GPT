#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "json_utils.h"

struct ConversationEntry {
    std::string user;
    std::string assistant;
    std::chrono::system_clock::time_point timestamp;
};

enum class CliProvider {
    AUTO,
    CLAUDE,
    GEMINI
};

class ClaudeAgent {
public:
    ClaudeAgent(const std::string& config_file = "agent_config.json",
                CliProvider cli_provider = CliProvider::AUTO);
    ~ClaudeAgent() = default;

    // Configuration management
    bool loadConfig();
    bool saveConfig();
    bool loadConfigFromFile(const std::string& file_path);
    bool saveConfigToFile(std::shared_ptr<json::Value> config, const std::string& filename);
    bool loadSpecificConfig(const std::string& file_path);

    // Core functionality
    std::string sendToClaudeApi(const std::string& message, bool use_system_prompt = true);
    std::string sendToCli(const std::string& message, bool use_system_prompt = true);

    // CLI provider management
    bool initializeCli();
    bool switchCliProvider(CliProvider new_provider);
    std::string getActiveProviderName() const;

    // Configuration access
    std::shared_ptr<json::Value> getConfig() const { return config_; }
    void setConfig(std::shared_ptr<json::Value> config) { config_ = config; }

    // Conversation history
    const std::vector<ConversationEntry>& getConversationHistory() const { return conversation_history_; }
    void clearConversationHistory() { conversation_history_.clear(); }

    // Utility methods
    std::string getName() const;
    std::string getDescription() const;
    std::string getInstructions() const;
    std::vector<std::string> getConversationStarters() const;
    int getConversationMemory() const;

    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setInstructions(const std::string& instructions);
    void setConversationStarters(const std::vector<std::string>& starters);
    void setConversationMemory(int memory);

private:
    std::string config_file_;
    std::string last_config_file_;
    CliProvider cli_provider_;
    CliProvider active_provider_;
    std::string cli_path_;
    std::shared_ptr<json::Value> config_;
    std::vector<ConversationEntry> conversation_history_;

    // Helper methods
    std::string findClaudeCli();
    std::string findGeminiCli();
    std::pair<std::string, CliProvider> findAvailableCli();
    std::string getSystemPrompt();
    std::string buildConversationContext(const std::string& current_message, int max_history = -1);
    void saveLastConfigPath(const std::string& config_path);
    std::string loadLastConfigPath();
    std::shared_ptr<json::Value> createDefaultConfig();
    std::string executeCommand(const std::vector<std::string>& command, const std::string& stdin_input = "");
    std::string escapeShellArg(const std::string& arg);
    std::string providerToString(CliProvider provider) const;
    CliProvider stringToProvider(const std::string& provider) const;
};