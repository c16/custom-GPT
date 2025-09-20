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

    // Test agent creation and basic functionality (without CLI calls)
    std::cout << "\nTesting ClaudeAgent creation with logging..." << std::endl;

    try {
        ClaudeAgent agent("agent_config.json", CliProvider::AUTO);
        std::cout << "ClaudeAgent created successfully." << std::endl;

        // Test logging without making actual CLI calls
        std::cout << "Agent name: " << agent.getName() << std::endl;
        std::cout << "Agent description: " << agent.getDescription() << std::endl;

        // Log that we successfully tested agent creation
        LOG_INFO("ClaudeAgent instantiation test completed successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to create ClaudeAgent: " + std::string(e.what()));
        std::cout << "ClaudeAgent creation failed (expected in test environment): " << e.what() << std::endl;
    }

    // Test flush and cleanup
    std::cout << "\nFlushing logs and cleaning up..." << std::endl;

    // Give logger time to write everything
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Logging test complete. Check test_logging.log for detailed logs." << std::endl;

    return 0;
}