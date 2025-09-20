#include "logger.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>

int main() {
    // Initialize logger (just to avoid issues)
    // Logger::getInstance().initialize();

    std::cout << "Testing config directory scanning..." << std::endl;

    // Get config directory from environment or use default
    const char* config_dir_env = std::getenv("CLAUDE_AGENT_CONFIG_DIR");
    std::string config_dir = config_dir_env ? config_dir_env : "../configs";

    std::cout << "Config directory: " << config_dir << std::endl;

    if (std::filesystem::exists(config_dir)) {
        std::cout << "Config directory exists!" << std::endl;

        for (const auto& entry : std::filesystem::directory_iterator(config_dir)) {
            if (entry.path().extension() == ".json") {
                std::cout << "Found config: " << entry.path().string() << std::endl;
            }
        }
    } else {
        std::cout << "Config directory does NOT exist!" << std::endl;
    }

    // Also check current directory
    std::cout << "\nChecking current directory for legacy configs..." << std::endl;
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.path().extension() == ".json" &&
            entry.path().filename().string().find("_config") != std::string::npos) {
            std::cout << "Found legacy config: " << entry.path().string() << std::endl;
        }
    }

    return 0;
}