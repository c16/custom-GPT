#!/bin/bash
# C++ Test Runner for Claude Agent
# Builds and runs all C++ unit tests

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPP_DIR="$SCRIPT_DIR/cpp"

echo "C++ Claude Agent Test Runner"
echo "============================"
echo "Running from: $SCRIPT_DIR"
echo "C++ directory: $CPP_DIR"
echo ""

# Check if C++ directory exists
if [ ! -d "$CPP_DIR" ]; then
    echo "ERROR: C++ directory not found: $CPP_DIR"
    exit 1
fi

# Change to C++ directory
cd "$CPP_DIR" || {
    echo "ERROR: Cannot change to C++ directory: $CPP_DIR"
    exit 1
}

# Check if make is available
if ! command -v make &> /dev/null; then
    echo "ERROR: make not found"
    echo "Please install build tools (build-essential on Ubuntu/Debian)"
    exit 1
fi

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ not found"
    echo "Please install g++ compiler"
    exit 1
fi

echo "Compiler: $(g++ --version | head -n1)"
echo ""

# Set up environment variables for tests
export CLAUDE_AGENT_CONFIG_DIR="$SCRIPT_DIR/configs"
echo "Config directory set to: $CLAUDE_AGENT_CONFIG_DIR"
echo ""

echo "Building C++ tests..."
echo "--------------------"

# Build the unit test
echo "Building unit tests..."
if g++ -std=c++17 -Wall -Wextra -O2 -Iinclude \
    test_claude_agent_unit.cpp \
    src/claude_agent.cpp \
    src/logger.cpp \
    src/json_utils.cpp \
    -o bin/test_claude_agent_unit; then
    echo "✓ Unit tests built successfully"
else
    echo "✗ Unit tests build FAILED"
    TESTS_FAILED=1
fi

# Build the config library functionality test
echo "Building config library tests..."
if g++ -std=c++17 -Wall -Wextra -O2 -Iinclude \
    test_config_library_functionality.cpp \
    src/claude_agent.cpp \
    src/logger.cpp \
    src/json_utils.cpp \
    -o bin/test_config_library_functionality; then
    echo "✓ Config library tests built successfully"
else
    echo "✗ Config library tests build FAILED"
    TESTS_FAILED=1
fi

echo ""

if [ "${TESTS_FAILED:-0}" -eq 1 ]; then
    echo "❌ C++ test compilation FAILED"
    exit 1
fi

echo "Running C++ tests..."
echo "-------------------"

# Run unit tests
echo "1. Running unit tests..."
if ./bin/test_claude_agent_unit; then
    echo "✓ Unit tests PASSED"
else
    echo "✗ Unit tests FAILED"
    TESTS_FAILED=1
fi

echo ""

# Run config library tests
echo "2. Running config library functionality tests..."
if ./bin/test_config_library_functionality; then
    echo "✓ Config library tests PASSED"
else
    echo "✗ Config library tests FAILED"
    TESTS_FAILED=1
fi

echo ""

# Run existing manual tests for completeness
echo "3. Running existing manual tests..."

echo "  3a. Config scanning test..."
if ./test_config_scan; then
    echo "✓ Config scanning test PASSED"
else
    echo "✗ Config scanning test FAILED"
    TESTS_FAILED=1
fi

echo ""

echo "  3b. Logging test..."
if ./test_logging > /dev/null 2>&1; then
    echo "✓ Logging test PASSED"
else
    echo "✗ Logging test FAILED"
    TESTS_FAILED=1
fi

echo ""

echo "C++ test run complete."

if [ "${TESTS_FAILED:-0}" -eq 1 ]; then
    echo "❌ Some C++ tests FAILED"
    exit 1
else
    echo "✅ All C++ tests PASSED"
    exit 0
fi