#include "claude_agent.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "Testing complex message handling..." << std::endl;

    ClaudeAgent agent("agent_config.json", CliProvider::AUTO);

    // Test with a complex message that contains quotes, newlines, and special characters
    std::string complex_message = R"(
This is a test message with:
- Single quotes: 'hello'
- Double quotes: "world"
- Newlines and formatting
- Special characters: $, !, @, #, %, ^, &, *
- Previous conversation:
  Human: What's your name?
  Assistant: I'm Claude.

Current message:
Human: Now provide a prompt for the custom GPT using the information you have gathered from your questions.
)";

    std::cout << "Sending complex message (length: " << complex_message.length() << " chars)" << std::endl;

    // This should use stdin instead of command line arguments
    std::string response = agent.sendToClaudeApi(complex_message);

    std::cout << "Response received (length: " << response.length() << " chars)" << std::endl;
    std::cout << "First 100 chars: " << response.substr(0, 100) << std::endl;

    if (response.find("Error") == 0) {
        std::cout << "ERROR: " << response << std::endl;
        return 1;
    } else {
        std::cout << "SUCCESS: Complex message handled correctly!" << std::endl;
        return 0;
    }
}