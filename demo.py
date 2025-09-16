#!/usr/bin/env python3
"""
Demo script for Custom AI Agent
Shows how to use the agent programmatically
"""

from claude_agent import ClaudeAgent
import json

def create_demo_configs():
    """Create demo agent configurations"""
    
    # Code Review Agent
    code_review_config = {
        "name": "Code Review Assistant",
        "description": "Expert code reviewer specializing in Python, JavaScript, and best practices",
        "instructions": """You are an expert code reviewer with years of experience in software development. When reviewing code:

1. Look for bugs, security issues, and performance problems
2. Check for code style and best practices
3. Suggest improvements and optimizations
4. Explain your reasoning clearly
5. Be constructive and helpful in your feedback

Focus on:
- Code correctness and functionality
- Security vulnerabilities
- Performance optimizations
- Code readability and maintainability
- Best practices and design patterns

Always provide specific, actionable feedback.""",
        "conversation_starters": [
            "Please review this code for me",
            "Can you check this function for bugs?",
            "What improvements would you suggest?",
            "Is this code secure and efficient?"
        ]
    }
    
    # Learning Assistant
    learning_config = {
        "name": "Learning Buddy",
        "description": "Patient tutor that helps explain complex topics step by step",
        "instructions": """You are a patient and encouraging tutor. Your goal is to help users learn by:

1. Breaking down complex topics into simple steps
2. Using analogies and examples
3. Asking questions to check understanding
4. Providing encouragement and positive feedback
5. Adapting explanations to the user's level

Always:
- Be patient and supportive
- Encourage questions
- Provide clear, step-by-step explanations
- Use real-world examples
- Check for understanding before moving on

Make learning fun and accessible!""",
        "conversation_starters": [
            "I need help understanding a concept",
            "Can you explain this topic to me?",
            "I'm struggling with this problem",
            "How does this work?"
        ]
    }
    
    # Writing Assistant
    writing_config = {
        "name": "Writing Coach",
        "description": "Professional writing assistant for clear, effective communication",
        "instructions": """You are a professional writing coach with expertise in:

1. Clear and concise communication
2. Grammar and style improvement
3. Structure and organization
4. Audience-appropriate tone
5. Editing and proofreading

When helping with writing:
- Focus on clarity and readability
- Suggest specific improvements
- Explain the reasoning behind changes
- Maintain the author's voice
- Consider the target audience

Provide constructive feedback that helps improve writing skills.""",
        "conversation_starters": [
            "Can you help me improve this text?",
            "Please review my writing",
            "How can I make this clearer?",
            "What's the best way to structure this?"
        ]
    }
    
    return {
        "code_review": code_review_config,
        "learning": learning_config,
        "writing": writing_config
    }

def save_demo_configs():
    """Save demo configurations to files"""
    configs = create_demo_configs()
    
    for name, config in configs.items():
        filename = f"{name}_agent_config.json"
        with open(filename, 'w') as f:
            json.dump(config, f, indent=2)
        print(f"Saved {filename}")

def demo_agent_usage():
    """Demonstrate programmatic agent usage"""
    print("Demo: Programmatic Agent Usage")
    print("=" * 40)
    
    # Create agent with default config
    agent = ClaudeAgent()
    
    # Show current config
    print(f"Agent Name: {agent.config['name']}")
    print(f"Description: {agent.config['description']}")
    print()
    
    # Demo single query (this would normally call Claude CLI)
    print("Demo Query: 'What is Python?'")
    print("Note: This demo shows the structure without calling Claude CLI")
    print()
    
    # Show system prompt generation
    system_prompt = agent.get_system_prompt()
    print("Generated System Prompt:")
    print("-" * 25)
    print(system_prompt[:200] + "...")
    print()

def demo_config_switching():
    """Demonstrate switching between different agent configurations"""
    print("Demo: Agent Configuration Switching")
    print("=" * 40)
    
    configs = create_demo_configs()
    
    for name, config in configs.items():
        print(f"\n{name.upper()} AGENT:")
        print(f"Name: {config['name']}")
        print(f"Description: {config['description']}")
        print("Conversation Starters:")
        for starter in config['conversation_starters']:
            print(f"  - {starter}")
        print()

if __name__ == "__main__":
    print("Custom AI Agent Demo")
    print("=" * 50)
    
    # Create demo configurations
    print("1. Creating demo agent configurations...")
    save_demo_configs()
    print()
    
    # Demo basic usage
    print("2. Basic agent usage demo...")
    demo_agent_usage()
    print()
    
    # Demo different configurations
    print("3. Different agent configurations...")
    demo_config_switching()
    print()
    
    print("Demo complete!")
    print("\nTo try the agents:")
    print("1. Copy a config file to 'agent_config.json'")
    print("2. Run: python3 claude_agent.py")
    print("3. Or run: python3 claude_agent_gui.py")