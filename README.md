# Custom AI Agent for Claude CLI

A Python application that creates a custom AI agent interface using the Claude CLI. This allows you to create personalized AI assistants with specific behaviors, instructions, and conversation styles.

## Features

- 🤖 **Custom Agent Personality**: Define your agent's name, description, and behavior
- 💬 **Interactive Conversations**: Chat with your custom agent in real-time
- 📝 **Configurable Instructions**: Set detailed instructions for how your agent should behave
- 🚀 **Conversation Starters**: Pre-defined prompts to help users get started
- 💾 **Persistent Configuration**: Save and load agent configurations
- 📊 **Conversation History**: Track and review past conversations
- 🖥️ **Multiple Interfaces**: Command-line and GUI options
- ⚡ **Claude CLI Integration**: Leverages the official Claude CLI for responses

## Requirements

- Python 3.6 or higher
- Claude CLI installed and configured ([Installation Guide](https://docs.anthropic.com/en/docs/claude-cli))
- No additional Python dependencies required (uses only standard library)

## Installation

1. Ensure Claude CLI is installed and working:
   ```bash
   claude --version
   ```

2. Clone or download this repository
3. No additional installation required - uses only Python standard library

## Usage

### Command-Line Interface

#### Interactive Mode
```bash
python3 claude_agent.py
```

#### Single Query Mode
```bash
python3 claude_agent.py "What is Python?"
```

### GUI Interface

```bash
python3 claude_agent_gui.py
```

The GUI automatically falls back to an enhanced command-line interface if tkinter is not available.

## Configuration

The agent behavior is controlled by `agent_config.json`:

```json
{
  "name": "Custom AI Agent",
  "description": "A helpful AI assistant",
  "instructions": "Your detailed instructions here...",
  "conversation_starters": [
    "How can I help you today?",
    "What would you like to work on?"
  ],
  "max_tokens": 4000,
  "temperature": 0.7
}
```

### Configuration Options

- **name**: The display name of your agent
- **description**: A brief description shown to users
- **instructions**: Detailed behavioral instructions (up to 5000 characters)
- **conversation_starters**: Pre-defined prompts for users
- **max_tokens**: Maximum response length
- **temperature**: Response creativity (0.0-1.0)

## File Structure

```
custom-agent/
├── claude_agent.py           # Core agent functionality
├── claude_agent_gui.py       # GUI interface
├── agent_config.json         # Agent configuration
├── Claude.md                 # Template file
└── README.md                 # This file
```

## Features in Detail

### Interactive Commands

While chatting with your agent, you can use these commands:

- `quit` - Exit the application
- `config` - Open configuration menu
- `history` - View conversation history

### Configuration Menu

The configuration menu allows you to:

1. Edit agent name
2. Edit description
3. Edit instructions
4. Manage conversation starters
5. View current configuration
6. Save configuration
7. Reload configuration

### GUI Features

The GUI version includes:

- **Chat Interface**: Clean conversation display
- **Configuration Window**: Easy editing of agent settings
- **History Viewer**: Browse past conversations
- **Conversation Starters**: Click to use pre-defined prompts
- **Real-time Responses**: Threaded communication with Claude CLI

## Creating Custom Agents

### Example: Code Review Agent

```json
{
  "name": "Code Review Assistant",
  "description": "Expert code reviewer specializing in Python, JavaScript, and best practices",
  "instructions": "You are an expert code reviewer with years of experience in software development. When reviewing code:\n\n1. Look for bugs, security issues, and performance problems\n2. Check for code style and best practices\n3. Suggest improvements and optimizations\n4. Explain your reasoning clearly\n5. Be constructive and helpful in your feedback\n\nFocus on:\n- Code correctness and functionality\n- Security vulnerabilities\n- Performance optimizations\n- Code readability and maintainability\n- Best practices and design patterns",
  "conversation_starters": [
    "Please review this code for me",
    "Can you check this function for bugs?",
    "What improvements would you suggest?",
    "Is this code secure and efficient?"
  ]
}
```

### Example: Learning Assistant

```json
{
  "name": "Learning Buddy",
  "description": "Patient tutor that helps explain complex topics step by step",
  "instructions": "You are a patient and encouraging tutor. Your goal is to help users learn by:\n\n1. Breaking down complex topics into simple steps\n2. Using analogies and examples\n3. Asking questions to check understanding\n4. Providing encouragement and positive feedback\n5. Adapting explanations to the user's level\n\nAlways:\n- Be patient and supportive\n- Encourage questions\n- Provide clear, step-by-step explanations\n- Use real-world examples\n- Check for understanding before moving on",
  "conversation_starters": [
    "I need help understanding a concept",
    "Can you explain this topic to me?",
    "I'm struggling with this problem",
    "How does this work?"
  ]
}
```

## Troubleshooting

### Claude CLI Not Found

If you get "Claude CLI not found" error:

1. Install Claude CLI: `npm install -g @anthropic-ai/claude-cli`
2. Ensure it's in your PATH
3. Test with: `claude --version`

### Configuration Issues

- Check that `agent_config.json` is valid JSON
- Ensure file permissions allow reading/writing
- Use the built-in configuration menu to make changes

### GUI Not Working

The application automatically falls back to command-line if:
- tkinter is not available
- Display is not available (headless systems)

## Advanced Usage

### Custom System Prompts

You can create sophisticated agents by crafting detailed instructions:

```
You are a [ROLE] with expertise in [DOMAIN].

Your personality:
- [TRAIT 1]
- [TRAIT 2]
- [TRAIT 3]

Your responsibilities:
1. [RESPONSIBILITY 1]
2. [RESPONSIBILITY 2]
3. [RESPONSIBILITY 3]

When responding:
- [GUIDELINE 1]
- [GUIDELINE 2]
- [GUIDELINE 3]

Output format:
- [FORMAT REQUIREMENT 1]
- [FORMAT REQUIREMENT 2]
```

### Integration with Other Tools

The agent can be integrated into workflows:

```bash
# Use in scripts
python3 claude_agent.py "Analyze this log file"

# Pipe input
echo "Explain this error" | python3 claude_agent.py
```

## Contributing

This is a standalone tool designed for customization. Feel free to:

- Modify the agent behavior
- Add new features
- Create specialized agent configurations
- Integrate with your own tools

## License

This project is provided as-is for educational and personal use.

## Support

For Claude CLI issues, refer to the [official documentation](https://docs.anthropic.com/en/docs/claude-cli).

For this tool, check the troubleshooting section above or modify the code to suit your needs.