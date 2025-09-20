#!/bin/bash

# Launch script for Python Claude Agent
# This script launches the Python tkinter implementation

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_DIR="$SCRIPT_DIR/python"

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is not installed or not in PATH"
    echo "Please install Python 3.6 or higher"
    exit 1
fi

# Check if Python directory exists
if [ ! -d "$PYTHON_DIR" ]; then
    echo "Error: Python directory not found at $PYTHON_DIR"
    exit 1
fi

# Check if the main script exists
if [ ! -f "$PYTHON_DIR/claude_agent_gui.py" ]; then
    echo "Error: claude_agent_gui.py not found in $PYTHON_DIR"
    exit 1
fi

# Check if Claude CLI is available
if ! command -v claude &> /dev/null; then
    echo "Warning: Claude CLI not found in PATH"
    echo "Please ensure Claude CLI is installed and configured"
    echo "Installation: npm install -g @anthropic-ai/claude-cli"
    echo ""
fi

echo "Launching Python Claude Agent..."
echo "Python implementation using tkinter GUI"
echo "Working directory: $PYTHON_DIR"
echo "Configuration directory: $SCRIPT_DIR/configs"
echo ""

# Change to Python directory and launch
cd "$PYTHON_DIR"
export CLAUDE_AGENT_CONFIG_DIR="$SCRIPT_DIR/configs"
python3 claude_agent_gui.py "$@"