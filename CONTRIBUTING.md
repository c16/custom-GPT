# Contributing to Custom AI Agent for Claude CLI

Thank you for your interest in contributing! This project aims to provide a flexible and powerful interface for creating custom AI agents that work with Claude CLI.

## Getting Started

### Prerequisites
- Python 3.6 or higher
- Claude CLI installed and configured
- Git for version control

### Development Setup
1. Clone the repository
2. No additional dependencies required (uses Python standard library only)
3. Test the application:
   ```bash
   python3 claude_agent.py --help
   python3 claude_agent_gui.py --help
   ```

## Project Structure

```
custom-agent/
├── claude_agent.py           # Core CLI agent
├── claude_agent_gui.py       # GUI interface
├── demo.py                   # Example configurations
├── Claude.md                 # Template reference
├── README.md                 # Main documentation
├── CONFIG_MANAGEMENT.md      # Configuration guide
├── CONTRIBUTING.md           # This file
└── LICENSE                   # MIT License
```

## How to Contribute

### Types of Contributions
- **Bug fixes**: Fix issues in existing functionality
- **New features**: Add new capabilities or improvements
- **Documentation**: Improve docs, examples, or guides
- **Templates**: Create new agent templates
- **Testing**: Add test cases or improve reliability
- **UI/UX**: Enhance user interface and experience

### Contribution Process
1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/your-feature-name`
3. **Make** your changes
4. **Test** your changes thoroughly
5. **Commit** your changes: `git commit -m "Add your feature"`
6. **Push** to your fork: `git push origin feature/your-feature-name`
7. **Submit** a pull request

### Coding Standards

#### Python Style
- Follow PEP 8 guidelines
- Use descriptive variable and function names
- Add docstrings for all functions and classes
- Keep functions focused and concise
- Use type hints where appropriate

#### Code Organization
- Keep CLI and GUI functionality separated
- Use helper functions for common operations
- Maintain backward compatibility when possible
- Handle errors gracefully with user-friendly messages

#### Example Code Style
```python
def load_configuration(self, file_path: str) -> bool:
    """Load agent configuration from file.
    
    Args:
        file_path: Path to the configuration file
        
    Returns:
        True if successful, False otherwise
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            config_data = json.load(f)
        
        # Validate required fields
        required_keys = ['name', 'description', 'instructions']
        for key in required_keys:
            if key not in config_data:
                print(f"Invalid configuration: missing '{key}' field")
                return False
        
        self.config = config_data
        return True
        
    except Exception as e:
        print(f"Error loading configuration: {e}")
        return False
```

### Testing Guidelines

#### Manual Testing
- Test both CLI and GUI interfaces
- Test with different configuration files
- Test error conditions (missing files, invalid JSON)
- Test on different platforms if possible

#### Test Cases to Include
- Configuration save/load operations
- Template creation and usage
- Import/export functionality
- Error handling and validation
- Claude CLI integration

### Documentation

#### Documentation Standards
- Update README.md for new features
- Add examples for new functionality
- Update CONFIG_MANAGEMENT.md for config-related changes
- Include inline comments for complex logic

#### Example Documentation
```markdown
### New Feature: Batch Configuration Import

Import multiple configurations at once:

```bash
python3 claude_agent.py
# config → 10 → 4 → select bundle file
```

This feature allows you to import configuration bundles created with the export functionality.
```

## Feature Ideas

### Potential Enhancements
- **Configuration Validation**: Schema validation for configurations
- **Backup System**: Automatic configuration backups
- **Search and Filter**: Better configuration discovery
- **Categories**: Organize configurations by category
- **Versioning**: Track configuration versions
- **Sharing**: Direct sharing between users
- **Plugins**: Extensible plugin system
- **Templates**: More specialized templates
- **Performance**: Optimize for large configuration libraries

### Agent Templates Needed
- **Data Analysis Assistant**: For data science tasks
- **Technical Writer**: For documentation
- **Research Assistant**: For academic research
- **Language Tutor**: For language learning
- **Project Manager**: For task organization
- **Creative Writer**: For creative writing
- **Debug Assistant**: For troubleshooting
- **API Helper**: For API documentation and usage

## Bug Reports

### When Reporting Bugs
1. **Check** if the issue already exists
2. **Provide** a clear description
3. **Include** steps to reproduce
4. **Add** error messages or screenshots
5. **Specify** your environment (OS, Python version, Claude CLI version)

### Bug Report Template
```
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '....'
3. See error

**Expected behavior**
What you expected to happen.

**Environment:**
- OS: [e.g., macOS, Linux, Windows]
- Python version: [e.g., 3.9.0]
- Claude CLI version: [e.g., 1.0.0]

**Additional context**
Add any other context about the problem here.
```

## Code Review Process

### Pull Request Guidelines
- **Clear title** describing the change
- **Detailed description** of what was changed and why
- **Reference issues** if applicable
- **Test your changes** before submitting
- **Keep PRs focused** on a single feature or fix

### Review Criteria
- Code follows style guidelines
- Changes are well-tested
- Documentation is updated
- No breaking changes (unless necessary)
- Error handling is appropriate

## Community

### Communication
- Use GitHub Issues for bug reports and feature requests
- Be respectful and constructive in discussions
- Help others who are getting started
- Share your custom agent configurations

### Recognition
Contributors will be recognized in the project documentation and release notes.

## Development Philosophy

### Core Principles
- **Simplicity**: Keep the interface simple and intuitive
- **Flexibility**: Support diverse use cases and workflows
- **Reliability**: Ensure stable and predictable behavior
- **No Dependencies**: Maintain zero external dependencies
- **Cross-Platform**: Work on all major operating systems

### Design Goals
- Easy to use for beginners
- Powerful for advanced users
- Extensible for custom needs
- Compatible with Claude CLI
- Maintainable codebase

Thank you for contributing to making this project better for everyone!