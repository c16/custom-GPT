# Testing Guide for Claude Agent

This document describes the comprehensive test suite for both Python and C++ implementations of the Claude Agent application.

## Overview

The test suite includes:
- **Python Tests**: Unit tests using Python's `unittest` framework
- **C++ Tests**: Custom unit test framework with comprehensive coverage
- **Integration Tests**: End-to-end functionality verification
- **Configuration Tests**: Config loading, scanning, and management
- **Manual Tests**: Interactive testing for GUI components

## Quick Start

### Run All Tests
```bash
./run-tests-all.sh
```

### Run Python Tests Only
```bash
./run-tests-python.sh
```

### Run C++ Tests Only
```bash
./run-tests-cpp.sh
```

## Python Tests

### Test Files
- `python/test_claude_agent.py` - Core agent functionality tests
- `python/test_claude_agent_gui.py` - GUI component tests

### Test Coverage
- Configuration loading and validation
- CLI detection (Claude/Gemini)
- Message handling and conversation history
- Environment variable support
- Error handling and fallback behavior
- GUI component initialization (mocked)
- Configuration browsing and selection

### Running Python Tests
```bash
cd python
python3 test_claude_agent.py        # Core tests
python3 test_claude_agent_gui.py    # GUI tests
```

Or use the runner script:
```bash
./run-tests-python.sh
```

### Python Test Environment
- Uses temporary directories for test configurations
- Mocks external dependencies (tkinter, subprocess)
- Tests environment variable behavior
- Validates JSON configuration parsing

## C++ Tests

### Test Files
- `cpp/test_claude_agent_unit.cpp` - Comprehensive unit tests
- `cpp/test_config_library_functionality.cpp` - Config library tests
- `cpp/test_logging.cpp` - Logging system tests (existing)
- `cpp/test_config_scan.cpp` - Config scanning tests (existing)

### Test Coverage
- Configuration loading from files and environment variables
- JSON parsing and validation
- Logger functionality and output
- CLI provider selection
- Configuration scanning and path resolution
- Error handling for invalid configurations
- Memory management and resource cleanup

### Building and Running C++ Tests

#### Using Makefile
```bash
cd cpp
make tests        # Build all tests
make run-tests    # Build and run all tests
```

#### Manual Build
```bash
cd cpp
g++ -std=c++17 -Iinclude test_claude_agent_unit.cpp \
    src/claude_agent.cpp src/logger.cpp src/json_utils.cpp \
    -o bin/test_claude_agent_unit

./bin/test_claude_agent_unit
```

#### Using Runner Script
```bash
./run-tests-cpp.sh
```

### C++ Test Framework
The C++ tests use a custom lightweight test framework that provides:
- Test case organization and execution
- Assertion methods (`assert_true`, `assert_equals`)
- Exception handling and error reporting
- Test result summary

## Test Categories

### 1. Configuration Tests
**Purpose**: Verify configuration loading, parsing, and management

**Python Tests**:
- `test_config_loading_with_environment_variable`
- `test_config_loading_default_path`
- `test_invalid_config_file`
- `test_malformed_json_config`

**C++ Tests**:
- `test_config_loading_valid_file`
- `test_config_loading_invalid_file`
- `test_config_loading_nonexistent_file`
- `test_config_directory_environment_variable`

### 2. CLI Detection Tests
**Purpose**: Test Claude and Gemini CLI detection and fallback

**Python Tests**:
- `test_claude_cli_detection`
- `test_gemini_cli_detection`
- `test_no_cli_detection`

**C++ Tests**:
- `test_cli_provider_setting`

### 3. Core Functionality Tests
**Purpose**: Test agent core features

**Python Tests**:
- `test_conversation_history_management`
- `test_config_saving`
- `test_last_config_persistence`
- `test_message_sending_mock`

**C++ Tests**:
- `test_conversation_history`

### 4. GUI Component Tests
**Purpose**: Test GUI functionality without display

**Python Tests**:
- `test_gui_initialization`
- `test_config_refresh_functionality`
- `test_conversation_starter_functionality`
- `test_message_handling`

**C++ Tests**:
- Config library functionality tests
- Path resolution tests

### 5. Error Handling Tests
**Purpose**: Verify graceful error handling

**All Test Suites**:
- Invalid JSON handling
- Missing file handling
- Network/CLI failure simulation
- Memory management verification

## Test Environment Setup

### Environment Variables
Tests use these environment variables:
- `CLAUDE_AGENT_CONFIG_DIR`: Points to test config directory
- Automatically set by test runner scripts

### Temporary Files
Tests create temporary files and directories:
- Python: Uses `tempfile.mkdtemp()`
- C++: Uses `/tmp/test_*` patterns
- All temporary resources are cleaned up after tests

### Mock Dependencies
Python tests mock external dependencies:
- `tkinter` - For headless GUI testing
- `subprocess.run` - For CLI command simulation
- File system operations where needed

## Expected Test Results

### Success Indicators
- All tests report "PASSED"
- No exceptions or assertion failures
- Clean resource cleanup
- Exit code 0 from test runners

### Common Failure Modes
1. **Missing Dependencies**: Build tools, Python packages
2. **Permission Issues**: Temp directory access
3. **Environment Issues**: Missing config directories
4. **Logic Errors**: Failed assertions

## Continuous Integration

### Test Automation
The test suite is designed for CI/CD integration:
- All tests are scriptable and non-interactive
- Clear exit codes (0 = success, 1 = failure)
- Comprehensive logging and error reporting
- No external network dependencies

### Required Dependencies
**Python Tests**:
- Python 3.7+
- Standard library only (unittest, tempfile, pathlib)

**C++ Tests**:
- g++ with C++17 support
- Standard library only
- No external libraries required for core tests

## Development Workflow

### Adding New Tests
1. Create feature branch: `git checkout -b feature/new-tests`
2. Add test cases to appropriate test files
3. Update this documentation if needed
4. Run full test suite: `./run-tests-all.sh`
5. Commit and create pull request

### Test-Driven Development
1. Write failing test for new feature
2. Implement feature to make test pass
3. Refactor while keeping tests green
4. Add integration tests if needed

### Before Merging
Always run the complete test suite:
```bash
./run-tests-all.sh
```

Ensure all tests pass before merging to main branch.

## Troubleshooting

### Python Test Issues
```bash
# Missing Python 3
sudo apt-get install python3

# Permission issues
chmod +x run-tests-python.sh
```

### C++ Test Issues
```bash
# Missing build tools
sudo apt-get install build-essential

# Compilation errors
cd cpp && make clean && make tests
```

### General Issues
```bash
# Check environment
echo $CLAUDE_AGENT_CONFIG_DIR

# Verify file permissions
ls -la run-tests-*.sh

# Check test file locations
find . -name "*test*" -type f
```

## Performance Considerations

### Test Execution Time
- Python tests: ~5-10 seconds
- C++ tests: ~10-15 seconds (including compilation)
- Full suite: ~20-30 seconds

### Resource Usage
- Minimal memory footprint
- Temporary files cleaned up automatically
- No persistent state between test runs

## Future Enhancements

### Planned Improvements
- Code coverage reporting
- Performance benchmarking
- Integration with GitHub Actions
- Cross-platform testing (Windows, macOS)
- Memory leak detection for C++

### Test Metrics
Consider tracking:
- Test execution time
- Code coverage percentage
- Test reliability over time
- Performance regression detection