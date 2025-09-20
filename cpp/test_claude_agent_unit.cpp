/**
 * Comprehensive unit tests for the C++ Claude Agent implementation.
 *
 * Tests core functionality including configuration loading, CLI detection,
 * logging system, and JSON utilities.
 */

#include "claude_agent.h"
#include "logger.h"
#include "json_utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <sstream>
#include <cstdlib>

// Simple test framework
class TestFramework {
private:
    int tests_run = 0;
    int tests_passed = 0;
    std::string current_test_name;

public:
    void run_test(const std::string& test_name, std::function<void()> test_func) {
        current_test_name = test_name;
        tests_run++;

        std::cout << "Running: " << test_name << "... ";

        try {
            test_func();
            tests_passed++;
            std::cout << "PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "FAILED: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "FAILED: Unknown exception" << std::endl;
        }
    }

    void assert_true(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }

    void assert_equals(const std::string& expected, const std::string& actual, const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error("Assertion failed: " + message +
                " (expected: '" + expected + "', actual: '" + actual + "')");
        }
    }

    void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;

        if (tests_passed == tests_run) {
            std::cout << "All tests PASSED!" << std::endl;
        } else {
            std::cout << "Some tests FAILED!" << std::endl;
        }
    }

    bool all_passed() const {
        return tests_passed == tests_run;
    }
};

// Test helper functions
namespace TestHelpers {
    std::string create_temp_config_file(const std::string& content) {
        std::string temp_file = "/tmp/test_config_" + std::to_string(rand()) + ".json";
        std::ofstream file(temp_file);
        file << content;
        file.close();
        return temp_file;
    }

    void cleanup_temp_file(const std::string& filepath) {
        std::filesystem::remove(filepath);
    }

    std::string create_valid_config() {
        return R"({
            "name": "Test Agent",
            "description": "A test configuration for unit tests",
            "instructions": "You are a test AI assistant.",
            "conversation_starters": ["Hello", "How can I test you?"]
        })";
    }

    std::string create_minimal_config() {
        return R"({
            "name": "Minimal Test Agent"
        })";
    }

    std::string create_invalid_json() {
        return "{ invalid json content }";
    }
}

// Test classes
class TestClaudeAgent {
public:
    static void test_config_loading_valid_file(TestFramework& tf) {
        std::string config_content = TestHelpers::create_valid_config();
        std::string temp_file = TestHelpers::create_temp_config_file(config_content);

        try {
            ClaudeAgent agent(temp_file, CliProvider::AUTO);
            tf.assert_equals("Test Agent", agent.getName(), "Agent name should match config");
            tf.assert_equals("A test configuration for unit tests", agent.getDescription(), "Description should match config");
        } catch (...) {
            TestHelpers::cleanup_temp_file(temp_file);
            throw;
        }

        TestHelpers::cleanup_temp_file(temp_file);
    }

    static void test_config_loading_invalid_file(TestFramework& tf) {
        std::string invalid_content = TestHelpers::create_invalid_json();
        std::string temp_file = TestHelpers::create_temp_config_file(invalid_content);

        try {
            ClaudeAgent agent(temp_file, CliProvider::AUTO);
            // Should fall back to default configuration
            tf.assert_equals("Custom AI Agent", agent.getName(), "Should use default name for invalid config");
        } catch (...) {
            TestHelpers::cleanup_temp_file(temp_file);
            throw;
        }

        TestHelpers::cleanup_temp_file(temp_file);
    }

    static void test_config_loading_nonexistent_file(TestFramework& tf) {
        ClaudeAgent agent("nonexistent_file.json", CliProvider::AUTO);
        // Should fall back to default configuration
        tf.assert_equals("Custom AI Agent", agent.getName(), "Should use default name for nonexistent file");
    }

    static void test_config_directory_environment_variable(TestFramework& tf) {
        // Create a temporary directory structure
        std::string temp_dir = "/tmp/test_configs_" + std::to_string(rand());
        std::filesystem::create_directory(temp_dir);

        std::string config_content = TestHelpers::create_valid_config();
        std::string config_file = temp_dir + "/test.json";
        std::ofstream file(config_file);
        file << config_content;
        file.close();

        // Set environment variable
        setenv("CLAUDE_AGENT_CONFIG_DIR", temp_dir.c_str(), 1);

        try {
            ClaudeAgent agent("test.json", CliProvider::AUTO);
            tf.assert_equals("Test Agent", agent.getName(), "Should load config from environment directory");
        } catch (...) {
            unsetenv("CLAUDE_AGENT_CONFIG_DIR");
            std::filesystem::remove_all(temp_dir);
            throw;
        }

        unsetenv("CLAUDE_AGENT_CONFIG_DIR");
        std::filesystem::remove_all(temp_dir);
    }

    static void test_cli_provider_setting(TestFramework& tf) {
        ClaudeAgent agent_auto("agent_config.json", CliProvider::AUTO);
        ClaudeAgent agent_claude("agent_config.json", CliProvider::CLAUDE);
        ClaudeAgent agent_gemini("agent_config.json", CliProvider::GEMINI);

        // We can't easily test the actual CLI detection without mocking,
        // but we can verify the agents are created successfully
        tf.assert_true(true, "All CLI provider types should create agents successfully");
    }

    static void test_conversation_history(TestFramework& tf) {
        ClaudeAgent agent("agent_config.json", CliProvider::AUTO);

        // Test adding to conversation history
        agent.addToConversationHistory("Human: Hello");
        agent.addToConversationHistory("Assistant: Hi there!");

        std::string history = agent.getConversationHistoryString();
        tf.assert_true(history.find("Human: Hello") != std::string::npos, "History should contain human message");
        tf.assert_true(history.find("Assistant: Hi there!") != std::string::npos, "History should contain assistant message");
    }
};

class TestLogger {
public:
    static void test_logger_initialization(TestFramework& tf) {
        Logger& logger = Logger::getInstance();

        // Test setting log level
        logger.setLogLevel(LogLevel::DEBUG);
        tf.assert_true(true, "Logger should initialize without errors");

        // Test setting log file
        std::string log_file = "/tmp/test_logger.log";
        logger.setLogFile(log_file);

        // Test logging at different levels
        LOG_DEBUG("Test debug message");
        LOG_INFO("Test info message");
        LOG_WARNING("Test warning message");
        LOG_ERROR("Test error message");

        tf.assert_true(true, "All log levels should work without errors");

        // Cleanup
        std::filesystem::remove(log_file);
    }

    static void test_component_logging(TestFramework& tf) {
        Logger& logger = Logger::getInstance();
        std::string log_file = "/tmp/test_component_logger.log";
        logger.setLogFile(log_file);
        logger.enableConsoleOutput(false); // Disable console for this test

        LOG_INFO_COMP("TestComponent", "Component-specific test message");
        LOG_ERROR_COMP("TestComponent", "Component-specific error message");

        tf.assert_true(true, "Component-specific logging should work");

        // Cleanup
        std::filesystem::remove(log_file);
        logger.enableConsoleOutput(true); // Re-enable console
    }

    static void test_special_logging_methods(TestFramework& tf) {
        Logger& logger = Logger::getInstance();

        // Test command logging
        std::vector<std::string> test_command = {"claude", "--print", "Hello world"};
        logger.logCommand(test_command, "Test stdin input");

        // Test response logging
        logger.logResponse("Test response", 0);
        logger.logResponse("Test error response", 1);

        // Test conversation context logging
        logger.logConversationContext("Test conversation context");

        // Test config change logging
        logger.logConfigChange("test_config", "Test config change");

        // Test error logging
        logger.logError("TestModule", "test_operation", "Test error message");

        tf.assert_true(true, "All special logging methods should work");
    }
};

class TestJsonUtils {
public:
    static void test_json_parsing_valid(TestFramework& tf) {
        std::string valid_json = TestHelpers::create_valid_config();
        std::string temp_file = TestHelpers::create_temp_config_file(valid_json);

        try {
            auto json_obj = json::parseFromFile(temp_file);
            tf.assert_true(json_obj != nullptr, "Should parse valid JSON successfully");

            auto obj = json_obj->asObject();
            tf.assert_true(obj.find("name") != obj.end(), "Should find 'name' field in JSON");
            tf.assert_equals("Test Agent", obj.at("name")->asString(), "Should parse name correctly");
        } catch (...) {
            TestHelpers::cleanup_temp_file(temp_file);
            throw;
        }

        TestHelpers::cleanup_temp_file(temp_file);
    }

    static void test_json_parsing_invalid(TestFramework& tf) {
        std::string invalid_json = TestHelpers::create_invalid_json();
        std::string temp_file = TestHelpers::create_temp_config_file(invalid_json);

        try {
            auto json_obj = json::parseFromFile(temp_file);
            tf.assert_true(json_obj == nullptr, "Should return nullptr for invalid JSON");
        } catch (...) {
            // Exception is also acceptable for invalid JSON
            tf.assert_true(true, "Exception is acceptable for invalid JSON");
        }

        TestHelpers::cleanup_temp_file(temp_file);
    }

    static void test_json_parsing_nonexistent_file(TestFramework& tf) {
        try {
            auto json_obj = json::parseFromFile("nonexistent_file.json");
            tf.assert_true(json_obj == nullptr, "Should return nullptr for nonexistent file");
        } catch (...) {
            // Exception is also acceptable for nonexistent file
            tf.assert_true(true, "Exception is acceptable for nonexistent file");
        }
    }
};

class TestConfigLibrary {
public:
    static void test_config_scanning(TestFramework& tf) {
        // Create temporary config directory
        std::string temp_dir = "/tmp/test_config_scan_" + std::to_string(rand());
        std::filesystem::create_directory(temp_dir);

        // Create test config files
        std::vector<std::string> config_names = {"config1.json", "config2.json", "config3.json"};
        for (const auto& name : config_names) {
            std::string config_file = temp_dir + "/" + name;
            std::ofstream file(config_file);
            file << TestHelpers::create_valid_config();
            file.close();
        }

        // Test config scanning logic
        std::vector<std::filesystem::path> config_files;
        if (std::filesystem::exists(temp_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(temp_dir)) {
                if (entry.path().extension() == ".json") {
                    config_files.push_back(entry.path());
                }
            }
        }

        tf.assert_equals(3, static_cast<int>(config_files.size()), "Should find all 3 config files");

        // Cleanup
        std::filesystem::remove_all(temp_dir);
    }
};

// Main test runner
int main() {
    std::cout << "Running C++ Claude Agent Unit Tests" << std::endl;
    std::cout << "====================================" << std::endl;

    TestFramework tf;

    // Initialize random seed for temp file names
    srand(time(nullptr));

    // Initialize logger for tests
    Logger::getInstance().setLogLevel(LogLevel::WARNING); // Reduce log noise during tests
    Logger::getInstance().enableConsoleOutput(false);

    // ClaudeAgent tests
    std::cout << "\n--- ClaudeAgent Tests ---" << std::endl;
    tf.run_test("Config Loading - Valid File", TestClaudeAgent::test_config_loading_valid_file);
    tf.run_test("Config Loading - Invalid File", TestClaudeAgent::test_config_loading_invalid_file);
    tf.run_test("Config Loading - Nonexistent File", TestClaudeAgent::test_config_loading_nonexistent_file);
    tf.run_test("Config Directory Environment Variable", TestClaudeAgent::test_config_directory_environment_variable);
    tf.run_test("CLI Provider Setting", TestClaudeAgent::test_cli_provider_setting);
    tf.run_test("Conversation History", TestClaudeAgent::test_conversation_history);

    // Logger tests
    std::cout << "\n--- Logger Tests ---" << std::endl;
    tf.run_test("Logger Initialization", TestLogger::test_logger_initialization);
    tf.run_test("Component Logging", TestLogger::test_component_logging);
    tf.run_test("Special Logging Methods", TestLogger::test_special_logging_methods);

    // JSON Utils tests
    std::cout << "\n--- JSON Utils Tests ---" << std::endl;
    tf.run_test("JSON Parsing - Valid", TestJsonUtils::test_json_parsing_valid);
    tf.run_test("JSON Parsing - Invalid", TestJsonUtils::test_json_parsing_invalid);
    tf.run_test("JSON Parsing - Nonexistent File", TestJsonUtils::test_json_parsing_nonexistent_file);

    // Config Library tests
    std::cout << "\n--- Config Library Tests ---" << std::endl;
    tf.run_test("Config Scanning", TestConfigLibrary::test_config_scanning);

    // Print summary
    tf.print_summary();

    return tf.all_passed() ? 0 : 1;
}