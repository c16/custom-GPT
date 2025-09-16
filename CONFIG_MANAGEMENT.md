# Configuration Management Guide

The Custom AI Agent now includes comprehensive configuration management features that allow you to save, load, organize, and share agent configurations easily.

## Features Overview

### 📁 **Configuration Storage**
- Configurations saved in `configs/` directory
- JSON format for easy editing and sharing
- Automatic validation of configuration files
- Backup and versioning support

### 💾 **Save & Load Operations**
- Save current configuration with new name
- Load any saved configuration
- Automatic configuration discovery
- Manual file path entry support

### 📚 **Configuration Library**
- Browse all available configurations
- Preview configurations before loading
- Search and filter capabilities
- Configuration metadata display

### 🎯 **Template System**
- Pre-built templates for common use cases
- Customizable template parameters
- Quick agent creation from templates
- Template categories and descriptions

### 📤 **Import & Export**
- Export individual configurations
- Bundle multiple configurations
- Import from external files
- Claude.md template generation

### 🔧 **Management Tools**
- Duplicate configurations
- Delete unwanted configurations
- Rename configuration files
- Reset to default settings

## CLI Interface

### Configuration Menu

Access the configuration menu by typing `config` during a conversation:

```
CONFIGURATION MENU
==========================================
1. Edit agent name
2. Edit description  
3. Edit instructions
4. View/edit conversation starters
5. View current configuration
6. Save configuration
7. Save configuration as...
8. Load configuration
9. Configuration library
10. Export/Import
0. Return to conversation
```

### Save Operations

**Option 6: Save configuration**
- Saves to default `agent_config.json`
- Overwrites current active configuration

**Option 7: Save configuration as...**
- Saves with custom filename
- Creates new file in `configs/` directory
- Preserves original configuration

### Load Operations

**Option 8: Load configuration**
- Shows all available configuration files
- Displays agent names and descriptions
- Supports manual path entry
- Validates configuration before loading

### Configuration Library

**Option 9: Configuration library**

```
CONFIGURATION LIBRARY
==========================================
1. Browse available configs
2. Create new config from template
3. Duplicate current config
4. Delete config
5. Rename config
0. Return to config menu
```

#### Browse Configurations
- Lists all configuration files
- Shows name, description, and starter count
- Provides detailed view of each configuration

#### Create from Template
Available templates:
- **Assistant**: General-purpose helper
- **Coder**: Programming and development
- **Tutor**: Educational assistant  
- **Writer**: Writing and editing helper

#### Management Operations
- **Duplicate**: Create copy of current configuration
- **Delete**: Remove configuration files (with confirmation)
- **Rename**: Change configuration filenames

### Import/Export Menu

**Option 10: Export/Import**

```
IMPORT/EXPORT
==========================================
1. Export current config to file
2. Export all configs as bundle
3. Import config from file
4. Import config bundle
5. Export as Claude.md template
0. Return to config menu
```

#### Export Options
1. **Current config**: Export active configuration
2. **All configs**: Bundle all configurations into single file
3. **Claude.md**: Generate template for Claude CLI

#### Import Options
1. **Config file**: Import single configuration
2. **Config bundle**: Import multiple configurations from bundle

## GUI Interface

### Main Interface Buttons

- **Config**: Opens configuration editor
- **Library**: Opens configuration library window
- **Clear**: Clears conversation history

### Configuration Library Window

Four main tabs:

#### 1. Browse Tab
- TreeView display of all configurations
- Columns: Name, Description, File
- Operations: Refresh, Load, Preview, Delete
- Double-click to load configuration

#### 2. Templates Tab
- Pre-built agent templates
- One-click template creation
- Customizable names and descriptions
- Instant activation option

#### 3. Import/Export Tab
- **Export Section**:
  - Export Current Config
  - Export All Configs 
  - Export as Claude.md
- **Import Section**:
  - Import Config File
  - Import Config Bundle

#### 4. Manage Tab
- Current configuration display
- Duplicate current configuration
- Reset to default settings

### Configuration Editor Enhancements

New buttons in configuration window:
- **Save**: Save to current file
- **Save As...**: Save with new name
- **Load...**: Load from file picker

## File Structure

```
custom-agent/
├── agent_config.json           # Active configuration
├── configs/                    # Configuration library
│   ├── code_review_config.json
│   ├── learning_tutor_config.json
│   ├── writing_coach_config.json
│   └── custom_agent_config.json
├── exported_configs/           # Export destination
│   ├── my_agents_bundle.json
│   └── claude_templates/
└── imports/                    # Import staging
```

## Configuration Format

```json
{
  "name": "Agent Name",
  "description": "Brief description of agent purpose",
  "instructions": "Detailed behavioral instructions...",
  "conversation_starters": [
    "How can I help you?",
    "What would you like to work on?"
  ],
  "system_prompt": "",
  "max_tokens": 4000,
  "temperature": 0.7
}
```

### Required Fields
- `name`: Agent display name
- `description`: Agent description
- `instructions`: Behavioral instructions
- `conversation_starters`: Array of starter prompts

### Optional Fields
- `system_prompt`: Additional system prompt
- `max_tokens`: Response length limit
- `temperature`: Response creativity

## Best Practices

### Naming Conventions
- Use descriptive names: `code_review_assistant`
- Include purpose: `python_tutor_config`
- Avoid spaces in filenames
- Use consistent suffixes: `_config.json`

### Organization
- Group related configurations
- Use prefixes for categories: `dev_`, `writing_`, `research_`
- Regular cleanup of unused configurations
- Backup important configurations

### Sharing Configurations
1. **Individual sharing**: Export single config as JSON
2. **Bulk sharing**: Create configuration bundles
3. **Claude.md format**: For use with Claude CLI directly
4. **Version control**: Store configs in git repositories

### Security
- Review imported configurations before use
- Validate configuration sources
- Don't include sensitive information in instructions
- Regular security audits of configuration library

## Command Examples

### CLI Usage
```bash
# Interactive mode with config menu
python3 claude_agent.py
# Type 'config' → '9' → '2' → '1' to create Assistant template

# Load specific configuration
python3 claude_agent.py "config" <<< "8\n1"
```

### Programmatic Usage
```python
from claude_agent import ClaudeAgent

# Create agent
agent = ClaudeAgent()

# Save current config
agent.save_config_to_file(agent.config, "my_custom_agent")

# Load different config
agent.load_specific_config("configs/code_review_config.json")

# Export bundle
agent.export_config_bundle()
```

## Troubleshooting

### Common Issues

**Configuration not loading**
- Check file path and permissions
- Verify JSON syntax
- Ensure required fields present

**Export/Import failures**
- Check disk space
- Verify file permissions
- Validate file formats

**GUI not displaying configs**
- Refresh configuration list
- Check configs directory exists
- Verify file accessibility

### Error Messages

- `Invalid configuration: missing 'name' field`: Required field missing
- `Configuration file not found`: Check file path
- `Error reading configuration`: File format or permission issue
- `Cannot delete active configuration`: Switch configs before deleting

## Migration Guide

### From Basic Version
1. Existing `agent_config.json` automatically recognized
2. Use "Save As..." to create library entries
3. No configuration changes required

### Backup Strategy
```bash
# Backup entire configuration library
cp -r configs/ configs_backup_$(date +%Y%m%d)/

# Export all configs as bundle
python3 claude_agent.py "config" <<< "10\n2\nbackup_bundle"
```

This comprehensive configuration management system provides professional-grade tools for managing AI agent configurations while maintaining ease of use for both CLI and GUI interfaces.