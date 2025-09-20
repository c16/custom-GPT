/**
 * Test for ConfigLibraryDialog functionality.
 *
 * Tests the configuration library features without requiring GUI display.
 * Focuses on the core logic of config scanning, loading, and management.
 */

#include "claude_agent.h"
#include "logger.h"
#include "json_utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <cstdlib>
#include <vector>

class ConfigLibraryTest {
private:
    std::string temp_dir;
    std::vector<std::string> created_files;

public:
    ConfigLibraryTest() {
        // Create temporary test directory
        temp_dir = "/tmp/test_config_lib_" + std::to_string(rand());
        std::filesystem::create_directory(temp_dir);
        std::cout << "Created test directory: " << temp_dir << std::endl;
    }

    ~ConfigLibraryTest() {
        // Cleanup
        cleanup();
    }

    void cleanup() {
        std::filesystem::remove_all(temp_dir);
        std::cout << "Cleaned up test directory: " << temp_dir << std::endl;
    }

    std::string create_test_config(const std::string& name, const std::string& description) {
        std::string config_content = R"({
            "name": ")" + name + R"(",
            "description": ")" + description + R"(",
            "instructions": "You are a test agent named )" + name + R"(.",
            "conversation_starters": ["Hello from )" + name + R"("]
        })";

        std::string filename = name + "_config.json";
        std::string filepath = temp_dir + "/" + filename;

        std::ofstream file(filepath);
        file << config_content;
        file.close();

        created_files.push_back(filepath);
        return filepath;
    }

    bool test_config_scanning() {
        std::cout << "Testing config scanning functionality..." << std::endl;

        // Create test configurations
        create_test_config("TestAgent1", "First test agent");
        create_test_config("TestAgent2", "Second test agent");
        create_test_config("TestAgent3", "Third test agent");

        // Simulate the config scanning logic from ConfigLibraryDialog::refreshConfigList()
        std::vector<std::filesystem::path> config_files;

        // Scan the test directory (simulating ../configs scan)
        if (std::filesystem::exists(temp_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(temp_dir)) {
                if (entry.path().extension() == ".json") {
                    config_files.push_back(entry.path());
                }
            }
        }

        std::cout << "Found " << config_files.size() << " config files" << std::endl;

        if (config_files.size() != 3) {
            std::cerr << "ERROR: Expected 3 config files, found " << config_files.size() << std::endl;
            return false;
        }

        // Test parsing each config file
        for (const auto& file : config_files) {
            try {
                auto config = json::parseFromFile(file.string());
                if (!config) {
                    std::cerr << "ERROR: Failed to parse config file: " << file << std::endl;
                    return false;
                }

                auto obj = config->asObject();
                if (obj.find("name") == obj.end()) {
                    std::cerr << "ERROR: Config file missing 'name' field: " << file << std::endl;
                    return false;
                }

                std::string name = obj.at("name")->asString();
                std::cout << "  Parsed config: " << name << std::endl;

            } catch (const std::exception& e) {
                std::cerr << "ERROR: Exception parsing config file " << file << ": " << e.what() << std::endl;
                return false;
            }
        }

        std::cout << "Config scanning test PASSED" << std::endl;
        return true;
    }

    bool test_config_loading() {
        std::cout << "Testing config loading functionality..." << std::endl;

        // Create a test config
        std::string config_file = create_test_config("LoadTest", "Test loading functionality");

        // Test loading with environment variable (simulating the fix we made)
        setenv("CLAUDE_AGENT_CONFIG_DIR", temp_dir.c_str(), 1);

        try {
            ClaudeAgent agent("LoadTest_config.json", CliProvider::AUTO);

            if (agent.getName() != "LoadTest") {
                std::cerr << "ERROR: Expected agent name 'LoadTest', got '" << agent.getName() << "'" << std::endl;
                unsetenv("CLAUDE_AGENT_CONFIG_DIR");
                return false;
            }

            std::cout << "Successfully loaded config with name: " << agent.getName() << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception loading config: " << e.what() << std::endl;
            unsetenv("CLAUDE_AGENT_CONFIG_DIR");
            return false;
        }

        unsetenv("CLAUDE_AGENT_CONFIG_DIR");

        std::cout << "Config loading test PASSED" << std::endl;
        return true;
    }

    bool test_config_path_resolution() {
        std::cout << "Testing config path resolution..." << std::endl;

        // Create a test config
        std::string config_file = create_test_config("PathTest", "Test path resolution");
        std::string filename = "PathTest_config.json";

        // Test the path resolution logic from onLoadSelectedClicked()
        std::string full_path;

        // Get config directory from environment or use default
        const char* config_dir_env = std::getenv("CLAUDE_AGENT_CONFIG_DIR");
        std::string config_dir = config_dir_env ? config_dir_env : temp_dir; // Use temp_dir instead of ../configs

        // Test path resolution logic
        if (std::filesystem::exists(filename)) {
            full_path = filename;
        } else if (std::filesystem::exists(config_dir + "/" + filename)) {
            full_path = config_dir + "/" + filename;
        } else if (std::filesystem::exists("configs/" + filename)) {  // Legacy fallback
            full_path = "configs/" + filename;
        }

        if (full_path.empty()) {
            std::cerr << "ERROR: Path resolution failed for filename: " << filename << std::endl;
            std::cerr << "  Checked paths:" << std::endl;
            std::cerr << "    " << filename << std::endl;
            std::cerr << "    " << config_dir + "/" + filename << std::endl;
            std::cerr << "    configs/" + filename << std::endl;
            return false;
        }

        std::cout << "Resolved path: " << full_path << std::endl;

        // Verify the resolved path actually works
        try {
            auto config = json::parseFromFile(full_path);
            if (!config) {
                std::cerr << "ERROR: Failed to parse resolved config file: " << full_path << std::endl;
                return false;
            }

            auto obj = config->asObject();
            std::string name = obj.at("name")->asString();
            if (name != "PathTest") {
                std::cerr << "ERROR: Wrong config loaded. Expected 'PathTest', got '" << name << "'" << std::endl;
                return false;
            }

        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception loading resolved config: " << e.what() << std::endl;
            return false;
        }

        std::cout << "Config path resolution test PASSED" << std::endl;
        return true;
    }

    bool test_invalid_config_handling() {
        std::cout << "Testing invalid config handling..." << std::endl;

        // Create an invalid config file
        std::string invalid_filepath = temp_dir + "/invalid_config.json";
        std::ofstream file(invalid_filepath);
        file << "{ invalid json content }";
        file.close();

        // Test scanning with invalid file mixed in
        std::vector<std::filesystem::path> config_files;
        std::vector<std::string> valid_configs;
        std::vector<std::string> invalid_configs;

        for (const auto& entry : std::filesystem::directory_iterator(temp_dir)) {
            if (entry.path().extension() == ".json") {
                config_files.push_back(entry.path());

                try {
                    auto config = json::parseFromFile(entry.path().string());
                    if (config) {
                        auto obj = config->asObject();
                        if (obj.find("name") != obj.end()) {
                            valid_configs.push_back(obj.at("name")->asString());
                        }
                    } else {
                        invalid_configs.push_back(entry.path().filename().string());
                    }
                } catch (const std::exception& e) {
                    invalid_configs.push_back(entry.path().filename().string());
                }
            }
        }

        std::cout << "Found " << valid_configs.size() << " valid configs and "
                  << invalid_configs.size() << " invalid configs" << std::endl;

        if (invalid_configs.size() != 1) {
            std::cerr << "ERROR: Expected 1 invalid config, found " << invalid_configs.size() << std::endl;
            return false;
        }

        if (valid_configs.size() < 3) {
            std::cerr << "ERROR: Expected at least 3 valid configs, found " << valid_configs.size() << std::endl;
            return false;
        }

        std::cout << "Invalid config handling test PASSED" << std::endl;
        return true;
    }

    bool run_all_tests() {
        std::cout << "Running ConfigLibrary functionality tests..." << std::endl;
        std::cout << "============================================" << std::endl;

        bool all_passed = true;

        all_passed &= test_config_scanning();
        std::cout << std::endl;

        all_passed &= test_config_loading();
        std::cout << std::endl;

        all_passed &= test_config_path_resolution();
        std::cout << std::endl;

        all_passed &= test_invalid_config_handling();
        std::cout << std::endl;

        if (all_passed) {
            std::cout << "All ConfigLibrary tests PASSED!" << std::endl;
        } else {
            std::cout << "Some ConfigLibrary tests FAILED!" << std::endl;
        }

        return all_passed;
    }
};

int main() {
    // Initialize random seed
    srand(time(nullptr));

    // Initialize logger with minimal output for testing
    Logger::getInstance().setLogLevel(LogLevel::WARNING);
    Logger::getInstance().enableConsoleOutput(false);

    ConfigLibraryTest test;
    bool success = test.run_all_tests();

    return success ? 0 : 1;
}