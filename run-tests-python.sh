#!/bin/bash
# Python Test Runner for Claude Agent
# Runs all Python unit tests with proper environment setup

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_DIR="$SCRIPT_DIR/python"

echo "Python Claude Agent Test Runner"
echo "==============================="
echo "Running from: $SCRIPT_DIR"
echo "Python directory: $PYTHON_DIR"
echo ""

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "ERROR: python3 not found"
    echo "Please install Python 3.7 or later"
    exit 1
fi

echo "Python version: $(python3 --version)"
echo ""

# Set up environment variables for tests
export CLAUDE_AGENT_CONFIG_DIR="$SCRIPT_DIR/configs"
echo "Config directory set to: $CLAUDE_AGENT_CONFIG_DIR"
echo ""

# Change to Python directory
cd "$PYTHON_DIR" || {
    echo "ERROR: Cannot change to Python directory: $PYTHON_DIR"
    exit 1
}

echo "Running Python unit tests..."
echo "----------------------------"

# Run core agent tests
echo "1. Running claude_agent core tests..."
if python3 test_claude_agent.py; then
    echo "✓ claude_agent tests PASSED"
else
    echo "✗ claude_agent tests FAILED"
    TESTS_FAILED=1
fi

echo ""

# Run GUI tests
echo "2. Running claude_agent_gui tests..."
if python3 test_claude_agent_gui.py; then
    echo "✓ claude_agent_gui tests PASSED"
else
    echo "✗ claude_agent_gui tests FAILED"
    TESTS_FAILED=1
fi

echo ""
echo "Python test run complete."

if [ "${TESTS_FAILED:-0}" -eq 1 ]; then
    echo "❌ Some Python tests FAILED"
    exit 1
else
    echo "✅ All Python tests PASSED"
    exit 0
fi