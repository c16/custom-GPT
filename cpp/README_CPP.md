# Claude Agent C++ (Gtkmm Version)

A C++ port of the Python tkinter Claude AI Agent using Gtkmm for the GUI. This application provides the same functionality as the Python version with a native C++ implementation.

## Features

- **Modern C++ GUI**: Built with Gtkmm 3.0 for cross-platform compatibility
- **CLI Integration**: Supports both Claude and Gemini CLI tools
- **Configuration Management**: JSON-based configuration system
- **Template System**: Pre-built agent templates
- **Conversation History**: Persistent conversation tracking
- **Import/Export**: Configuration sharing capabilities
- **Real-time Chat**: Threaded message processing

## Dependencies

- C++17 compatible compiler (GCC 7+ or Clang 5+)
- Gtkmm 3.0 development libraries
- pkg-config
- CMake 3.10+ (for CMake build) or Make

### Ubuntu/Debian Installation

```bash
sudo apt-get update
sudo apt-get install -y build-essential pkg-config libgtkmm-3.0-dev cmake
```

### Fedora/RHEL Installation

```bash
sudo dnf install gcc-c++ pkgconfig gtkmm30-devel cmake make
```

### Arch Linux Installation

```bash
sudo pacman -S base-devel gtkmm3 cmake
```

## Building

### Option 1: Using Make (Recommended)

```bash
# Check dependencies
make check-deps

# Build the application
make

# Run the application
make run
```

### Option 2: Using CMake

```bash
# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the application
./ClaudeAgentGtk
```

## Project Structure

```
├── include/              # Header files
│   ├── claude_agent.h           # Core agent functionality
│   ├── claude_agent_gui.h       # Main GUI window
│   ├── config_dialog.h          # Configuration dialog
│   ├── config_library_dialog.h  # Configuration library
│   └── json_utils.h             # JSON parsing utilities
├── src/                  # Source files
│   ├── main.cpp                 # Application entry point
│   ├── claude_agent.cpp         # Agent implementation
│   ├── claude_agent_gui.cpp     # GUI implementation
│   ├── config_dialog.cpp        # Config dialog implementation
│   ├── config_library_dialog.cpp # Library dialog implementation
│   └── json_utils.cpp           # JSON utilities implementation
├── CMakeLists.txt        # CMake configuration
├── Makefile             # Make configuration
└── README_CPP.md        # This file
```

## Usage

1. **First Run**: The application will create a default configuration
2. **CLI Setup**: Ensure you have either Claude CLI or Gemini CLI installed
3. **Configuration**: Use the Config button to customize your agent
4. **Templates**: Use the Library dialog to create agents from templates
5. **Chat**: Type messages and press Ctrl+Enter to send

## Key Features Comparison with Python Version

| Feature | Python (tkinter) | C++ (Gtkmm) | Status |
|---------|------------------|-------------|---------|
| Main Chat Interface | ✅ | ✅ | Complete |
| Configuration Dialog | ✅ | ✅ | Complete |
| Configuration Library | ✅ | ✅ | Complete |
| CLI Provider Selection | ✅ | ✅ | Complete |
| Conversation Starters | ✅ | ✅ | Complete |
| History Viewer | ✅ | ✅ | Complete |
| JSON Configuration | ✅ | ✅ | Complete |
| Template System | ✅ | ✅ | Complete |
| Import/Export | ✅ | 🚧 | Partial |
| Threading | ✅ | ✅ | Complete |
| Styling/Themes | ✅ | ✅ | Complete |

## Architecture

### Core Components

1. **ClaudeAgent**: Handles CLI communication and configuration management
2. **ClaudeAgentGUI**: Main window with chat interface
3. **ConfigDialog**: Agent configuration editor
4. **ConfigLibraryDialog**: Template and configuration management
5. **JSON Utils**: Custom JSON parser and serializer

### Threading Model

- Main UI thread handles all GUI operations
- Background worker threads handle CLI communication
- Message queue system for thread-safe communication
- Timer-based response checking (100ms intervals)

## Configuration Format

The application uses JSON configuration files with the following structure:

```json
{
  "name": "My Agent",
  "description": "Agent description",
  "instructions": "Detailed instructions for the agent",
  "conversation_starters": [
    "How can I help you?",
    "What would you like to work on?"
  ],
  "conversation_memory": 5,
  "system_prompt": "",
  "max_tokens": 4000,
  "temperature": 0.7
}
```

## CLI Integration

The application supports multiple CLI providers:

- **Claude CLI**: Primary integration with `claude` command
- **Gemini CLI**: Alternative integration with `gemini` command
- **Auto Detection**: Automatically finds available CLI tools

## Building from Source

### Debug Build

```bash
make debug
```

### Clean Build

```bash
make clean
make
```

### Installing Dependencies

```bash
make install-deps  # Ubuntu/Debian only
```

## Troubleshooting

### Common Issues

1. **Gtkmm not found**: Install development packages for your distribution
2. **Compiler errors**: Ensure C++17 support (GCC 7+ or Clang 5+)
3. **CLI not found**: Install Claude CLI or Gemini CLI tools
4. **Permission errors**: Check file permissions in configs directory

### Getting Help

1. Check the build output for specific error messages
2. Verify all dependencies are installed with `make check-deps`
3. Ensure the CLI tools are in your PATH
4. Try running with debug build: `make debug && make run`

## Contributing

This C++ version maintains feature parity with the Python version while providing:

- Better performance through native compilation
- Lower memory usage
- Improved startup time
- Native desktop integration

## License

Same license as the original Python version.