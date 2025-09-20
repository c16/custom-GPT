#!/bin/bash

# Launch script for C++ Claude Agent
# This script builds (if needed) and launches the C++ Gtkmm implementation

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPP_DIR="$SCRIPT_DIR/cpp"

# Check if C++ directory exists
if [ ! -d "$CPP_DIR" ]; then
    echo "Error: C++ directory not found at $CPP_DIR"
    exit 1
fi

# Change to C++ directory
cd "$CPP_DIR"

# Check for required build tools
if ! command -v make &> /dev/null; then
    echo "Error: make is not installed"
    echo "Please install build-essential: sudo apt install build-essential"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler is not installed"
    echo "Please install build-essential: sudo apt install build-essential"
    exit 1
fi

# Check for Gtkmm development libraries
if ! pkg-config --exists gtkmm-3.0; then
    echo "Error: Gtkmm 3.0 development libraries not found"
    echo "Please install: sudo apt install libgtkmm-3.0-dev pkg-config"
    exit 1
fi

# Check if Claude CLI or Gemini CLI is available
CLAUDE_AVAILABLE=false
GEMINI_AVAILABLE=false

if command -v claude &> /dev/null; then
    CLAUDE_AVAILABLE=true
fi

if command -v gemini &> /dev/null; then
    GEMINI_AVAILABLE=true
fi

if [ "$CLAUDE_AVAILABLE" = false ] && [ "$GEMINI_AVAILABLE" = false ]; then
    echo "Warning: Neither Claude CLI nor Gemini CLI found in PATH"
    echo "Please install at least one:"
    echo "  Claude CLI: npm install -g @anthropic-ai/claude-cli"
    echo "  Gemini CLI: Follow Google's installation instructions"
    echo ""
fi

# Check if binary exists and is newer than source files
NEEDS_BUILD=false
BINARY="bin/ClaudeAgentGtk"

if [ ! -f "$BINARY" ]; then
    echo "Binary not found, building..."
    NEEDS_BUILD=true
else
    # Check if any source files are newer than the binary
    if find src include -name "*.cpp" -o -name "*.h" | xargs -r ls -t 2>/dev/null | head -1 | xargs -r stat -c %Y 2>/dev/null | head -1 | awk -v binary_time="$(stat -c %Y "$BINARY" 2>/dev/null || echo 0)" '{if($1 > binary_time) exit 1}'; then
        echo "Source files have been modified, rebuilding..."
        NEEDS_BUILD=true
    fi
fi

# Build if needed
if [ "$NEEDS_BUILD" = true ]; then
    echo "Building C++ Claude Agent..."
    echo "This may take a moment..."

    if ! make; then
        echo "Error: Build failed"
        echo "Please check that all dependencies are installed:"
        echo "  sudo apt install build-essential cmake pkg-config libgtkmm-3.0-dev"
        exit 1
    fi

    echo "Build completed successfully!"
fi

# Check if binary exists after build
if [ ! -f "$BINARY" ]; then
    echo "Error: Binary not found after build"
    exit 1
fi

# Make binary executable
chmod +x "$BINARY"

echo "Launching C++ Claude Agent..."
echo "C++ implementation using Gtkmm 3.0 GUI"
echo "Working directory: $CPP_DIR"
echo "Configuration directory: $SCRIPT_DIR/configs"
echo ""

if [ "$CLAUDE_AVAILABLE" = true ]; then
    echo "Claude CLI: Available"
fi
if [ "$GEMINI_AVAILABLE" = true ]; then
    echo "Gemini CLI: Available"
fi
echo ""

# Set environment variable for config directory and launch
export CLAUDE_AGENT_CONFIG_DIR="$SCRIPT_DIR/configs"
./"$BINARY" "$@"