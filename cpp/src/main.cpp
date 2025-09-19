#include <gtkmm.h>
#include <iostream>
#include "claude_agent_gui.h"
#include "logger.h"

void setupLogging(int argc, char* argv[]) {
    auto& logger = Logger::getInstance();

    // Check for debug flag
    bool debug_mode = false;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--debug" || std::string(argv[i]) == "-d") {
            debug_mode = true;
            break;
        }
    }

    if (debug_mode) {
        logger.setLogLevel(LogLevel::DEBUG);
        logger.enableConsoleOutput(true);
        std::cout << "Debug logging enabled" << std::endl;
    } else {
        logger.setLogLevel(LogLevel::INFO);
        logger.enableConsoleOutput(false);  // Only errors to console in normal mode
    }

    // Always log to file
    logger.enableFileOutput(true);
    logger.setLogFile("claude_agent.log");
}

void printUsage(const char* program_name) {
    std::cout << "Claude Agent Gtk - C++ GUI Application\n\n";
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help     Show this help message\n";
    std::cout << "  -d, --debug    Enable debug logging to console\n";
    std::cout << "  --log-level=LEVEL  Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)\n";
    std::cout << "  --log-file=FILE    Set log file path (default: claude_agent.log)\n\n";
    std::cout << "GTK Options are also available (use --help-gtk to see them)\n";
}

int main(int argc, char* argv[]) {
    // Check for help flag before creating GTK application
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }

    // Setup logging first
    setupLogging(argc, argv);

    LOG_INFO("Creating GTK application...");
    // Create GTK application
    auto app = Gtk::Application::create(argc, argv, "com.example.claude-agent");
    LOG_INFO("GTK application created successfully");

    try {
        LOG_INFO("Starting Claude Agent GTK application");
        LOG_INFO("Creating ClaudeAgentGUI window...");
        ClaudeAgentGUI window;
        LOG_INFO("ClaudeAgentGUI window created successfully");

        int result = app->run(window);
        LOG_INFO("Application exiting with code " + std::to_string(result));
        return result;
    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}