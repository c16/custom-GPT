# C++ Claude Agent

This directory contains the C++ implementation of the Claude Agent using Gtkmm 3.0 for the GUI.

## Files

### Source Code
- `include/` - Header files (.h)
- `src/` - Source files (.cpp)
- `CMakeLists.txt` - CMake build configuration
- `Makefile` - Alternative make-based build system

### Build Artifacts (Generated)
- `bin/` - Compiled executables
- `obj/` - Object files
- `test_*` - Test executables

### Test Files
- `test_complex_message.cpp` - Tests complex message handling
- `test_gui_simple.cpp` - Simple GUI functionality test
- `test_logging.cpp` - Logging system test

## Building

### Using Make
```bash
make
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

## Usage

Run the GUI application:
```bash
./bin/ClaudeAgentGtk
```

Run with debug logging:
```bash
./bin/ClaudeAgentGtk --debug
```

## Requirements

- C++17 compatible compiler (g++, clang++)
- Gtkmm 3.0 development libraries
- pkg-config
- Claude CLI or Gemini CLI installed and configured

### Installing Dependencies (Ubuntu/Debian)
```bash
sudo apt install build-essential cmake pkg-config libgtkmm-3.0-dev
```

## Features

- Native C++ performance with Gtkmm GUI
- Comprehensive logging system with multiple levels
- Support for both Claude CLI and Gemini CLI
- JSON configuration management
- Threading for background message processing
- Configuration dialogs and conversation starters
- Automatic CLI detection and provider switching