# Custom AI Agent for Claude CLI

A multi-platform application that creates custom AI agent interfaces using the Claude CLI. Available in both Python (tkinter) and C++ (Gtkmm) implementations. This allows you to create personalized AI assistants with specific behaviors, instructions, and conversation styles.

## Available Implementations

### 🐍 Python Implementation
- **Location**: [`python/`](python/) directory
- **GUI Framework**: tkinter (cross-platform)
- **Requirements**: Python 3.6+, tkinter
- **Usage**: `python python/claude_agent_gui.py`

### ⚡ C++ Implementation
- **Location**: [`cpp/`](cpp/) directory
- **GUI Framework**: Gtkmm 3.0 (Linux/Unix)
- **Requirements**: C++17, Gtkmm 3.0 dev libraries
- **Usage**: `./cpp/bin/ClaudeAgentGtk`

Both implementations provide the same core functionality with their respective platform optimizations.

## Features

- 🤖 **Custom Agent Personality**: Define your agent's name, description, and behavior
- 💬 **Interactive Conversations**: Chat with your custom agent in real-time
- 📝 **Configurable Instructions**: Set detailed instructions for how your agent should behave
- 🚀 **Conversation Starters**: Pre-defined prompts to help users get started
- 💾 **Persistent Configuration**: Save and load agent configurations
- 📊 **Conversation History**: Track and review past conversations
- 🖥️ **Multiple Interfaces**: Command-line and GUI options
- ⚡ **Claude CLI Integration**: Leverages the official Claude CLI for responses
- 🔄 **Multi-CLI Support**: Support for both Claude CLI and Gemini CLI (C++ version)

## Quick Start

### Python Version
```bash
cd python/
python claude_agent_gui.py
```

### C++ Version
```bash
cd cpp/
make
./bin/ClaudeAgentGtk
```

## Requirements

### Common Requirements
- Claude CLI installed and configured ([Installation Guide](https://docs.anthropic.com/en/docs/claude-cli))
- Git (for cloning the repository)

### Python-Specific
- Python 3.6 or higher
- tkinter (usually included with Python)

### C++-Specific
- C++17 compatible compiler (g++, clang++)
- Gtkmm 3.0 development libraries
- pkg-config
- CMake (optional, for CMake build)

#### Installing C++ Dependencies (Ubuntu/Debian)
```bash
sudo apt install build-essential cmake pkg-config libgtkmm-3.0-dev
```

## Installation

1. Ensure Claude CLI is installed and working:
   ```bash
   claude --version
   ```

2. Clone this repository:
   ```bash
   git clone <repository-url>
   cd custom-agent
   ```

3. Choose your implementation:
   - **Python**: Ready to use, no compilation needed
   - **C++**: Requires compilation (see [`cpp/README.md`](cpp/README.md))

## Configuration

Both implementations share the same configuration format in `agent_config.json`:

```json
{
  "name": "Custom AI Agent",
  "description": "A helpful AI assistant",
  "instructions": "Your detailed instructions here...",
  "conversation_starters": [
    "How can I help you today?",
    "What would you like to work on?"
  ],
  "conversation_memory": 10
}
```

### Configuration Options

- **name**: The display name of your agent
- **description**: A brief description shown to users
- **instructions**: Detailed behavioral instructions (up to 5000 characters)
- **conversation_starters**: Pre-defined prompts for users
- **conversation_memory**: Number of previous messages to include in context

## File Structure

```
custom-agent/
├── python/                   # Python implementation
│   ├── claude_agent.py           # Core agent functionality
│   ├── claude_agent_gui.py       # tkinter GUI interface
│   ├── custom_agent_creator.py   # Configuration creator tool
│   ├── demo.py                   # Demo script
│   └── README.md                 # Python-specific documentation
├── cpp/                      # C++ implementation
│   ├── include/                  # Header files
│   ├── src/                      # Source files
│   ├── CMakeLists.txt           # CMake build configuration
│   ├── Makefile                 # Make build configuration
│   └── README.md                # C++-specific documentation
├── configs/                  # Shared configuration files
├── agent_config.json         # Main agent configuration
├── CHANGELOG.md             # Version history
├── CONFIG_MANAGEMENT.md     # Configuration guide
├── CONTRIBUTING.md          # Contribution guidelines
└── README.md                # This file
```

## Implementation Comparison

| Feature | Python | C++ |
|---------|--------|-----|
| **Performance** | Interpreted | Compiled (faster) |
| **Memory Usage** | Higher | Lower |
| **Startup Time** | Fast | Very fast |
| **Platform Support** | Cross-platform | Linux/Unix (Gtkmm) |
| **Dependencies** | Python + tkinter | C++17 + Gtkmm 3.0 |
| **CLI Support** | Claude CLI only | Claude CLI + Gemini CLI |
| **Logging** | Basic | Comprehensive multi-level |
| **Threading** | Basic | Advanced with queue system |
| **Build Process** | None required | Compilation required |

## Advanced Features (C++ Version)

The C++ implementation includes additional features:

- **Comprehensive Logging**: Multi-level logging with file output and debug modes
- **Dual CLI Support**: Automatic detection and switching between Claude CLI and Gemini CLI
- **Advanced Threading**: Background message processing with response queues
- **Enhanced Error Handling**: Detailed error reporting and recovery
- **Configuration Management**: Advanced configuration loading and validation

## Creating Custom Agents

Both implementations support creating specialized agents. See the existing configuration files in the `configs/` directory for examples:

- **Code Review Agent**: `code_review_agent_config.json`
- **Learning Assistant**: `learning_agent_config.json`
- **Writing Coach**: `writing_agent_config.json`

## Troubleshooting

### Claude CLI Not Found
1. Install Claude CLI: `npm install -g @anthropic-ai/claude-cli`
2. Ensure it's in your PATH
3. Test with: `claude --version`

### Implementation-Specific Issues
- **Python**: See [`python/README.md`](python/README.md)
- **C++**: See [`cpp/README.md`](cpp/README.md)

## Contributing

Contributions are welcome! Please see [`CONTRIBUTING.md`](CONTRIBUTING.md) for guidelines.

## License

This project is provided as-is for educational and personal use. See [`LICENSE`](LICENSE) for details.

## Support

- For Claude CLI issues: [Official Claude CLI Documentation](https://docs.anthropic.com/en/docs/claude-cli)
- For implementation-specific issues: Check the respective README files
- For general issues: Check the troubleshooting sections or create an issue