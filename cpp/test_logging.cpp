#include "claude_agent.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Testing logging functionality..." << std::endl;

    // Initialize logging
    auto& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::DEBUG);
    logger.enableConsoleOutput(true);
    logger.setLogFile("test_logging.log");

    // Test different log levels
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARNING("This is a warning message");
    LOG_ERROR("This is an error message");
    LOG_CRITICAL("This is a critical message");

    // Test component-specific logging
    LOG_INFO_COMP("TestComponent", "Component-specific info message");
    LOG_ERROR_COMP("TestComponent", "Component-specific error message");

    // Test special logging methods
    std::vector<std::string> test_command = {"claude", "--print", "Hello world"};
    logger.logCommand(test_command, "Test stdin input with special chars: 'quotes' and \"double quotes\" and\nnewlines");

    logger.logResponse("This is a test response from CLI", 0);
    logger.logResponse("This is an error response", 1);

    logger.logConversationContext("Previous conversation:\nHuman: Hello\nAssistant: Hi there!\n\nCurrent message:\nHuman: How are you?");

    logger.logConfigChange("test_config", "Changed from template A to template B");

    logger.logError("TestModule", "perform critical operation", "Network connection failed");

    // Test agent functionality with logging
    std::cout << "\nTesting ClaudeAgent with logging..." << std::endl;

    ClaudeAgent agent("agent_config.json", CliProvider::AUTO);

    // Simulate a complex message that would trigger our logging
    std::string complex_message = R"(This is a test message with:
- Single quotes: 'hello'
- Double quotes: "world"
- Multiple lines
- Special characters: $@#%

Previous conversation:
Human: What's the weather like?
Assistant: I don't have access to current weather data.

Current message:
Human: Now provide a summary of our conversation.)";

    std::cout << "Sending complex message to test logging..." << std::endl;
    std::string response = agent.sendToClaudeApi(complex_message);

    if (response.find("Error") == 0) {
        std::cout << "Expected error (no CLI available in test environment): " << response << std::endl;
    } else {
        std::cout << "Unexpected success: " << response.substr(0, 100) << std::endl;
    }

    std::cout << "\nLogging test complete. Check test_logging.log for detailed logs." << std::endl;

    return 0;
}