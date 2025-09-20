# Custom AI Agent Development Guide

## Project Overview
This repository contains a dual-implementation AI agent application with both Python (Tkinter) and C++ (Gtkmm) versions. The application provides a GUI interface for interacting with Claude and Gemini AI models through their respective CLI tools.

## Architecture
```
custom-agent/
├── python/           # Python/Tkinter implementation
├── cpp/             # C++/Gtkmm implementation
├── configs/         # Configuration files for different AI agents
├── launch-python.sh # Python version launcher
├── launch-cpp.sh    # C++ version launcher
└── CLAUDE.md       # This development guide
```

## Quick Start
- **Python version**: `./launch-python.sh`
- **C++ version**: `./launch-cpp.sh`

## Development Best Practices

### Branch Management Strategy
**🚨 IMPORTANT: Always create feature branches for new development**

1. **Main Branch Protection**: The `main` branch should always contain stable, working code
2. **Feature Development**: Create a new branch for each feature or bug fix:
   ```bash
   git checkout -b feature/your-feature-name
   git checkout -b bugfix/issue-description
   git checkout -b refactor/component-name
   ```
3. **Branch Naming Convention**:
   - `feature/` - New functionality
   - `bugfix/` - Bug fixes
   - `refactor/` - Code improvements without functional changes
   - `docs/` - Documentation updates
4. **Development Workflow**:
   ```bash
   # Create feature branch
   git checkout -b feature/new-config-validator

   # Develop and commit changes
   git add .
   git commit -m "Add config validation for JSON files"

   # Push branch and create PR
   git push -u origin feature/new-config-validator

   # After PR approval, merge to main
   git checkout main
   git pull origin main
   git branch -d feature/new-config-validator
   ```

### C++ Development Best Practices

#### Build System
- Use the provided `Makefile` for building
- Always run `make clean && make` after significant changes
- Check for compilation warnings and address them

#### Code Standards
- **C++17 Standard**: Use modern C++ features appropriately
- **Include Management**:
  - Use forward declarations in headers when possible
  - Include guards or `#pragma once`
  - Order includes: system headers, then project headers
- **Memory Management**: Prefer smart pointers and RAII
- **Error Handling**: Use exceptions for error conditions
- **Logging**: Use the Logger class for debug/info/error messages

#### Testing
```bash
cd cpp
make clean && make           # Build
./test_config_scan          # Test config scanning
./bin/ClaudeAgentGtk --help # Test application help
```

#### Key Components
- `ClaudeAgent`: Core agent logic and CLI interface
- `ClaudeAgentGUI`: Main GTK+ interface
- `ConfigLibraryDialog`: Configuration management dialog
- `Logger`: Centralized logging system
- `json_utils`: JSON parsing utilities

### Python Development Best Practices

#### Environment Setup
- Use Python 3.7+
- Ensure Tkinter is available (usually bundled with Python)
- Install required dependencies if any

#### Code Standards
- **PEP 8 Compliance**: Follow Python style guidelines
- **Type Hints**: Use type annotations where helpful
- **Error Handling**: Use try/except blocks appropriately
- **Imports**: Group imports (standard library, third-party, local)

#### Testing
```bash
./launch-python.sh          # Test application launch
python3 python/claude_agent.py --help  # Test CLI mode
```

#### Key Components
- `claude_agent.py`: Core agent logic
- `claude_agent_gui.py`: Tkinter GUI interface
- `custom_agent_creator.py`: Configuration creation utility

### Configuration Management

#### Adding New Configurations
1. Create new JSON file in `configs/` directory
2. Follow the standard configuration schema:
   ```json
   {
     "name": "Agent Name",
     "description": "Brief description",
     "instructions": "Detailed agent instructions",
     "conversation_starters": [
       "Example starter 1",
       "Example starter 2"
     ]
   }
   ```
3. Test configuration loads correctly in both versions

#### Environment Variables
- `CLAUDE_AGENT_CONFIG_DIR`: Override default config directory path
- Set in launch scripts or shell environment

### Common Development Tasks

#### Adding a New Feature
1. **Create feature branch**: `git checkout -b feature/your-feature`
2. **Implement in both versions** (if UI-related):
   - Update Python implementation in `python/`
   - Update C++ implementation in `cpp/`
3. **Test thoroughly**:
   - Test Python version: `./launch-python.sh`
   - Test C++ version: `./launch-cpp.sh`
4. **Update documentation** if needed
5. **Commit and push**: Create PR for review

#### Debugging Issues
- **Python**: Use print statements or Python debugger
- **C++**: Use Logger class with DEBUG level, compile with debug symbols
- **Both**: Check working directory and file paths are correct

#### Path Resolution Guidelines
- **Python**: Runs from `python/` directory, configs at `../configs/`
- **C++**: Runs from `cpp/` directory, configs at `../configs/`
- **Environment**: Use `CLAUDE_AGENT_CONFIG_DIR` for custom paths

### Testing Checklist
Before merging any feature branch:
- [ ] Both Python and C++ versions launch successfully
- [ ] Configuration library loads and displays all configs
- [ ] Selected configurations load without errors
- [ ] New features work in both implementations
- [ ] No regression in existing functionality
- [ ] Code follows project style guidelines
- [ ] Commit messages are clear and descriptive

### Release Process
1. Ensure all features are merged to `main`
2. Test both versions thoroughly
3. Update version numbers if applicable
4. Tag release: `git tag -a v1.x.x -m "Release notes"`
5. Push tags: `git push origin --tags`

## Troubleshooting

### Common Issues
- **"Config not found"**: Check working directory and config paths
- **GUI won't start**: Verify GTK+/Tkinter dependencies installed
- **Build errors**: Ensure all required development packages installed

### Platform-Specific Notes
- **Linux**: Ensure `gtkmm-3.0` development packages installed
- **Development**: Use provided launch scripts for proper environment setup