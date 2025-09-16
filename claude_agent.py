#!/usr/bin/env python3
"""
Custom AI Agent that interfaces with Claude CLI
A Python application that acts as a custom AI agent by interfacing with the Claude CLI
"""

import subprocess
import json
import os
import sys
import time
from pathlib import Path

class ClaudeAgent:
    def __init__(self, config_file="agent_config.json"):
        self.config_file = config_file
        self.config = self.load_config()
        self.conversation_history = []
        self.claude_cli_path = self.find_claude_cli()
        
    def find_claude_cli(self):
        """Find the Claude CLI executable"""
        possible_paths = [
            "claude",
            "/usr/local/bin/claude",
            "/usr/bin/claude",
            os.path.expanduser("~/.local/bin/claude"),
            "npx claude",
            "yarn claude"
        ]
        
        for path in possible_paths:
            try:
                result = subprocess.run([path, "--version"], 
                                      capture_output=True, text=True, timeout=5)
                if result.returncode == 0:
                    print(f"Found Claude CLI at: {path}")
                    return path
            except (subprocess.TimeoutExpired, FileNotFoundError):
                continue
        
        print("Claude CLI not found. Please ensure it's installed and in your PATH.")
        return None
    
    def load_config(self):
        """Load agent configuration from file or create default"""
        default_config = {
            "name": "Custom AI Agent",
            "description": "A helpful AI assistant",
            "instructions": """You are a helpful AI assistant. Please provide clear, concise, and accurate responses.
            
Your primary capabilities include:
- Answering questions across various topics
- Helping with problem-solving
- Providing explanations and guidance
- Assisting with code and technical issues

Always be polite, professional, and helpful in your responses.""",
            "conversation_starters": [
                "How can I help you today?",
                "What would you like to work on?",
                "Tell me about your project and I'll assist you.",
                "What questions do you have for me?"
            ],
            "system_prompt": "",
            "max_tokens": 4000,
            "temperature": 0.7
        }
        
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    config = json.load(f)
                print(f"Loaded configuration from {self.config_file}")
                return {**default_config, **config}
            except Exception as e:
                print(f"Error loading config: {e}. Using default configuration.")
                return default_config
        else:
            print("No configuration file found. Using default configuration.")
            return default_config
    
    def save_config(self):
        """Save current configuration to file"""
        try:
            with open(self.config_file, 'w') as f:
                json.dump(self.config, f, indent=2)
            print(f"Configuration saved to {self.config_file}")
        except Exception as e:
            print(f"Error saving config: {e}")
    
    def get_system_prompt(self):
        """Generate system prompt from configuration"""
        system_prompt = f"""You are {self.config['name']}.

Description: {self.config['description']}

Instructions:
{self.config['instructions']}

Please follow these instructions carefully and embody the role described above."""
        
        return system_prompt
    
    def send_to_claude(self, message, use_system_prompt=True):
        """Send message to Claude CLI and get response"""
        if not self.claude_cli_path:
            return "Error: Claude CLI not available"
        
        try:
            cmd = [self.claude_cli_path, "--print"]
            
            # Add system prompt if enabled
            if use_system_prompt:
                system_prompt = self.get_system_prompt()
                cmd.extend(["--append-system-prompt", system_prompt])
            
            # Add the user message
            cmd.append(message)
            
            print(f"Sending to Claude: {message[:100]}{'...' if len(message) > 100 else ''}")
            
            # Execute Claude CLI command
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
            
            if result.returncode == 0:
                response = result.stdout.strip()
                # Store in conversation history
                self.conversation_history.append({
                    "user": message,
                    "assistant": response,
                    "timestamp": time.time()
                })
                return response
            else:
                error_msg = result.stderr.strip() if result.stderr else "Unknown error"
                return f"Error from Claude CLI: {error_msg}"
                
        except subprocess.TimeoutExpired:
            return "Error: Claude CLI request timed out"
        except Exception as e:
            return f"Error communicating with Claude CLI: {str(e)}"
    
    def start_conversation(self):
        """Start an interactive conversation"""
        print("=" * 60)
        print(f"          {self.config['name'].upper()}")
        print("=" * 60)
        print(f"Description: {self.config['description']}")
        print()
        
        # Show conversation starters
        if self.config['conversation_starters']:
            print("Conversation starters:")
            for i, starter in enumerate(self.config['conversation_starters'], 1):
                print(f"{i}. {starter}")
            print()
        
        print("Type 'quit' to exit, 'config' to modify settings, or 'history' to view conversation history")
        print("-" * 60)
        
        while True:
            try:
                user_input = input("\nYou: ").strip()
                
                if user_input.lower() == 'quit':
                    print("Goodbye!")
                    break
                elif user_input.lower() == 'config':
                    self.config_menu()
                    continue
                elif user_input.lower() == 'history':
                    self.show_history()
                    continue
                elif not user_input:
                    continue
                
                print(f"\n{self.config['name']}: ", end="", flush=True)
                response = self.send_to_claude(user_input)
                print(response)
                
            except KeyboardInterrupt:
                print("\n\nGoodbye!")
                break
            except Exception as e:
                print(f"\nError: {e}")
    
    def config_menu(self):
        """Configuration menu"""
        while True:
            print("\n" + "=" * 40)
            print("CONFIGURATION MENU")
            print("=" * 40)
            print("1. Edit agent name")
            print("2. Edit description")
            print("3. Edit instructions")
            print("4. View/edit conversation starters")
            print("5. View current configuration")
            print("6. Save configuration")
            print("7. Save configuration as...")
            print("8. Load configuration")
            print("9. Configuration library")
            print("10. Export/Import")
            print("0. Return to conversation")
            
            choice = input("\nChoose option: ").strip()
            
            if choice == "1":
                new_name = input(f"Current name: {self.config['name']}\nNew name: ")
                if new_name:
                    self.config['name'] = new_name
                    print("Name updated!")
            
            elif choice == "2":
                print(f"Current description: {self.config['description']}")
                print("Enter new description (press Enter on empty line to finish):")
                lines = []
                while True:
                    line = input()
                    if not line:
                        break
                    lines.append(line)
                if lines:
                    self.config['description'] = '\n'.join(lines)
                    print("Description updated!")
            
            elif choice == "3":
                print(f"Current instructions:\n{self.config['instructions']}")
                print("\nEnter new instructions (press Enter on empty line to finish):")
                lines = []
                while True:
                    line = input()
                    if not line:
                        break
                    lines.append(line)
                if lines:
                    self.config['instructions'] = '\n'.join(lines)
                    print("Instructions updated!")
            
            elif choice == "4":
                self.manage_conversation_starters()
            
            elif choice == "5":
                self.view_current_config()
            
            elif choice == "6":
                self.save_config()
            
            elif choice == "7":
                self.save_config_as()
            
            elif choice == "8":
                self.load_config_menu()
            
            elif choice == "9":
                self.config_library_menu()
            
            elif choice == "10":
                self.import_export_menu()
            
            elif choice == "0":
                break
            else:
                print("Invalid option!")
    
    def manage_conversation_starters(self):
        """Manage conversation starters"""
        while True:
            print("\nConversation Starters:")
            for i, starter in enumerate(self.config['conversation_starters'], 1):
                print(f"{i}. {starter}")
            
            print("\nOptions:")
            print("1. Add starter")
            print("2. Remove starter")
            print("0. Return")
            
            choice = input("Choose option: ").strip()
            
            if choice == "1":
                new_starter = input("Enter new conversation starter: ")
                if new_starter:
                    self.config['conversation_starters'].append(new_starter)
                    print("Added!")
            
            elif choice == "2":
                try:
                    index = int(input("Enter number to remove: ")) - 1
                    if 0 <= index < len(self.config['conversation_starters']):
                        removed = self.config['conversation_starters'].pop(index)
                        print(f"Removed: {removed}")
                    else:
                        print("Invalid number!")
                except ValueError:
                    print("Please enter a valid number!")
            
            elif choice == "0":
                break
    
    def show_history(self):
        """Show conversation history"""
        if not self.conversation_history:
            print("No conversation history yet.")
            return
        
        print("\nConversation History:")
        print("-" * 40)
        for i, entry in enumerate(self.conversation_history, 1):
            timestamp = time.strftime("%H:%M:%S", time.localtime(entry['timestamp']))
            print(f"{i}. [{timestamp}]")
            print(f"You: {entry['user']}")
            print(f"{self.config['name']}: {entry['assistant'][:100]}{'...' if len(entry['assistant']) > 100 else ''}")
            print("-" * 40)
    
    def view_current_config(self):
        """Display current configuration in detail"""
        print("\n" + "=" * 50)
        print("CURRENT CONFIGURATION")
        print("=" * 50)
        print(f"Name: {self.config['name']}")
        print(f"Description: {self.config['description']}")
        print(f"\nInstructions:\n{self.config['instructions']}")
        print(f"\nConversation Starters ({len(self.config['conversation_starters'])}):")
        for i, starter in enumerate(self.config['conversation_starters'], 1):
            print(f"  {i}. {starter}")
        print("=" * 50)
        input("Press Enter to continue...")
    
    def save_config_as(self):
        """Save configuration with a new name"""
        filename = input("Enter filename (without .json extension): ").strip()
        if not filename:
            print("No filename provided!")
            return
        
        if not filename.endswith('.json'):
            filename += '.json'
        
        # Create configs directory if it doesn't exist
        configs_dir = Path("configs")
        configs_dir.mkdir(exist_ok=True)
        
        file_path = configs_dir / filename
        
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(self.config, f, indent=2)
            print(f"Configuration saved as {file_path}")
        except Exception as e:
            print(f"Error saving configuration: {e}")
    
    def load_config_menu(self):
        """Menu for loading configurations"""
        configs_dir = Path("configs")
        
        # Look for config files in current directory and configs subdirectory
        config_files = []
        
        # Current directory
        for file in Path(".").glob("*_config.json"):
            config_files.append(file)
        
        # Configs subdirectory
        if configs_dir.exists():
            for file in configs_dir.glob("*.json"):
                config_files.append(file)
        
        if not config_files:
            print("No configuration files found!")
            manual_path = input("Enter path to config file (or press Enter to cancel): ").strip()
            if manual_path and os.path.exists(manual_path):
                self.load_specific_config(manual_path)
            return
        
        print("\nAvailable configurations:")
        for i, file in enumerate(config_files, 1):
            # Try to read the name from the config
            try:
                with open(file, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                name = config_data.get('name', 'Unknown')
                desc = config_data.get('description', '')[:50]
                print(f"{i}. {file.name} - {name}")
                if desc:
                    print(f"   {desc}{'...' if len(config_data.get('description', '')) > 50 else ''}")
            except:
                print(f"{i}. {file.name} - (Error reading file)")
        
        print("0. Manual path entry")
        
        try:
            choice = int(input("\nSelect configuration to load: "))
            if choice == 0:
                manual_path = input("Enter path to config file: ").strip()
                if manual_path and os.path.exists(manual_path):
                    self.load_specific_config(manual_path)
            elif 1 <= choice <= len(config_files):
                self.load_specific_config(str(config_files[choice - 1]))
            else:
                print("Invalid choice!")
        except ValueError:
            print("Please enter a valid number!")
    
    def load_specific_config(self, file_path):
        """Load a specific configuration file"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                new_config = json.load(f)
            
            # Validate configuration
            required_keys = ['name', 'description', 'instructions', 'conversation_starters']
            for key in required_keys:
                if key not in new_config:
                    print(f"Invalid configuration: missing '{key}' field")
                    return
            
            self.config = new_config
            print(f"Configuration loaded from {file_path}")
            print(f"Now using: {self.config['name']}")
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
    
    def config_library_menu(self):
        """Configuration library management"""
        while True:
            print("\n" + "=" * 40)
            print("CONFIGURATION LIBRARY")
            print("=" * 40)
            print("1. Browse available configs")
            print("2. Create new config from template")
            print("3. Duplicate current config")
            print("4. Delete config")
            print("5. Rename config")
            print("0. Return to config menu")
            
            choice = input("\nChoose option: ").strip()
            
            if choice == "1":
                self.browse_configs()
            elif choice == "2":
                self.create_from_template()
            elif choice == "3":
                self.duplicate_config()
            elif choice == "4":
                self.delete_config()
            elif choice == "5":
                self.rename_config()
            elif choice == "0":
                break
            else:
                print("Invalid option!")
    
    def browse_configs(self):
        """Browse and preview configurations"""
        configs_dir = Path("configs")
        config_files = []
        
        # Collect all config files
        for file in Path(".").glob("*_config.json"):
            config_files.append(file)
        
        if configs_dir.exists():
            for file in configs_dir.glob("*.json"):
                config_files.append(file)
        
        if not config_files:
            print("No configuration files found!")
            return
        
        print("\nConfiguration Library:")
        print("-" * 40)
        
        for file in config_files:
            try:
                with open(file, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                
                print(f"\nFile: {file.name}")
                print(f"Name: {config_data.get('name', 'Unknown')}")
                print(f"Description: {config_data.get('description', 'No description')}")
                print(f"Starters: {len(config_data.get('conversation_starters', []))}")
                print("-" * 40)
            except Exception as e:
                print(f"\nFile: {file.name}")
                print(f"Error: Could not read file - {e}")
                print("-" * 40)
        
        input("Press Enter to continue...")
    
    def create_from_template(self):
        """Create new config from a template"""
        templates = {
            "assistant": {
                "name": "General Assistant",
                "description": "A helpful general-purpose AI assistant",
                "instructions": "You are a helpful, accurate, and friendly AI assistant. Provide clear, concise answers and always be respectful.",
                "conversation_starters": ["How can I help you?", "What would you like to know?"]
            },
            "coder": {
                "name": "Code Assistant",
                "description": "Programming and development helper",
                "instructions": "You are an expert programmer. Help with code review, debugging, best practices, and programming questions. Always explain your reasoning.",
                "conversation_starters": ["What code can I help with?", "Need help debugging?", "Looking for code review?"]
            },
            "tutor": {
                "name": "Learning Tutor",
                "description": "Patient educational assistant",
                "instructions": "You are a patient tutor. Break down complex topics, use examples, and adapt to the learner's level. Always encourage questions.",
                "conversation_starters": ["What would you like to learn?", "Need help understanding something?", "Ready for your lesson?"]
            },
            "writer": {
                "name": "Writing Assistant",
                "description": "Writing and editing helper",
                "instructions": "You are a professional writing coach. Help improve clarity, style, grammar, and structure while maintaining the author's voice.",
                "conversation_starters": ["What writing can I help with?", "Need editing assistance?", "Want to improve your text?"]
            }
        }
        
        print("\nAvailable templates:")
        for i, (key, template) in enumerate(templates.items(), 1):
            print(f"{i}. {key.title()} - {template['description']}")
        
        try:
            choice = int(input("\nSelect template: "))
            template_keys = list(templates.keys())
            if 1 <= choice <= len(template_keys):
                template = templates[template_keys[choice - 1]]
                
                # Customize the template
                name = input(f"Agent name [{template['name']}]: ").strip() or template['name']
                desc = input(f"Description [{template['description']}]: ").strip() or template['description']
                
                new_config = template.copy()
                new_config['name'] = name
                new_config['description'] = desc
                
                filename = input("Save as (without .json): ").strip()
                if filename:
                    self.save_config_to_file(new_config, filename)
                    
                    use_now = input("Use this config now? (y/n): ").strip().lower()
                    if use_now == 'y':
                        self.config = new_config
                        print(f"Now using: {self.config['name']}")
            else:
                print("Invalid choice!")
        except ValueError:
            print("Please enter a valid number!")
    
    def duplicate_config(self):
        """Duplicate the current configuration"""
        new_name = input(f"New name for duplicate [{self.config['name']} Copy]: ").strip()
        if not new_name:
            new_name = f"{self.config['name']} Copy"
        
        filename = input("Save as (without .json): ").strip()
        if not filename:
            print("No filename provided!")
            return
        
        new_config = self.config.copy()
        new_config['name'] = new_name
        
        self.save_config_to_file(new_config, filename)
    
    def delete_config(self):
        """Delete a configuration file"""
        configs_dir = Path("configs")
        config_files = []
        
        for file in Path(".").glob("*_config.json"):
            if file.name != "agent_config.json":  # Don't delete the active config
                config_files.append(file)
        
        if configs_dir.exists():
            for file in configs_dir.glob("*.json"):
                config_files.append(file)
        
        if not config_files:
            print("No configuration files available for deletion!")
            return
        
        print("\nConfigurations available for deletion:")
        for i, file in enumerate(config_files, 1):
            try:
                with open(file, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                name = config_data.get('name', 'Unknown')
                print(f"{i}. {file.name} - {name}")
            except:
                print(f"{i}. {file.name} - (Error reading)")
        
        try:
            choice = int(input("\nSelect file to delete (0 to cancel): "))
            if choice == 0:
                return
            elif 1 <= choice <= len(config_files):
                file_to_delete = config_files[choice - 1]
                confirm = input(f"Really delete {file_to_delete.name}? (y/n): ").strip().lower()
                if confirm == 'y':
                    file_to_delete.unlink()
                    print(f"Deleted {file_to_delete.name}")
                else:
                    print("Deletion cancelled")
            else:
                print("Invalid choice!")
        except ValueError:
            print("Please enter a valid number!")
    
    def rename_config(self):
        """Rename a configuration file"""
        configs_dir = Path("configs")
        config_files = []
        
        for file in Path(".").glob("*_config.json"):
            config_files.append(file)
        
        if configs_dir.exists():
            for file in configs_dir.glob("*.json"):
                config_files.append(file)
        
        if not config_files:
            print("No configuration files found!")
            return
        
        print("\nConfigurations available for renaming:")
        for i, file in enumerate(config_files, 1):
            print(f"{i}. {file.name}")
        
        try:
            choice = int(input("\nSelect file to rename (0 to cancel): "))
            if choice == 0:
                return
            elif 1 <= choice <= len(config_files):
                old_file = config_files[choice - 1]
                new_name = input(f"New filename (without .json) [{old_file.stem}]: ").strip()
                if new_name:
                    new_file = old_file.parent / f"{new_name}.json"
                    old_file.rename(new_file)
                    print(f"Renamed {old_file.name} to {new_file.name}")
            else:
                print("Invalid choice!")
        except ValueError:
            print("Please enter a valid number!")
    
    def import_export_menu(self):
        """Import/Export menu"""
        while True:
            print("\n" + "=" * 40)
            print("IMPORT/EXPORT")
            print("=" * 40)
            print("1. Export current config to file")
            print("2. Export all configs as bundle")
            print("3. Import config from file")
            print("4. Import config bundle")
            print("5. Export as Claude.md template")
            print("0. Return to config menu")
            
            choice = input("\nChoose option: ").strip()
            
            if choice == "1":
                self.export_current_config()
            elif choice == "2":
                self.export_config_bundle()
            elif choice == "3":
                self.import_config_file()
            elif choice == "4":
                self.import_config_bundle()
            elif choice == "5":
                self.export_claude_md()
            elif choice == "0":
                break
            else:
                print("Invalid option!")
    
    def save_config_to_file(self, config, filename):
        """Helper method to save config to file"""
        if not filename.endswith('.json'):
            filename += '.json'
        
        configs_dir = Path("configs")
        configs_dir.mkdir(exist_ok=True)
        
        file_path = configs_dir / filename
        
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(config, f, indent=2)
            print(f"Configuration saved as {file_path}")
        except Exception as e:
            print(f"Error saving configuration: {e}")
    
    def export_current_config(self):
        """Export current configuration"""
        filename = input("Export filename (without .json): ").strip()
        if filename:
            self.save_config_to_file(self.config, filename)
    
    def export_config_bundle(self):
        """Export all configurations as a bundle"""
        configs_dir = Path("configs")
        config_files = []
        
        for file in Path(".").glob("*_config.json"):
            config_files.append(file)
        
        if configs_dir.exists():
            for file in configs_dir.glob("*.json"):
                config_files.append(file)
        
        if not config_files:
            print("No configurations to export!")
            return
        
        bundle = {}
        for file in config_files:
            try:
                with open(file, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                bundle[file.stem] = config_data
            except Exception as e:
                print(f"Error reading {file.name}: {e}")
        
        filename = input("Bundle filename (without .json): ").strip()
        if filename:
            if not filename.endswith('.json'):
                filename += '.json'
            
            try:
                with open(filename, 'w', encoding='utf-8') as f:
                    json.dump(bundle, f, indent=2)
                print(f"Config bundle exported as {filename}")
                print(f"Exported {len(bundle)} configurations")
            except Exception as e:
                print(f"Error exporting bundle: {e}")
    
    def import_config_file(self):
        """Import a single configuration file"""
        file_path = input("Path to config file: ").strip()
        if not file_path:
            return
        
        if not os.path.exists(file_path):
            print("File not found!")
            return
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                config_data = json.load(f)
            
            # Validate
            required_keys = ['name', 'description', 'instructions', 'conversation_starters']
            for key in required_keys:
                if key not in config_data:
                    print(f"Invalid configuration: missing '{key}' field")
                    return
            
            # Save to configs directory
            filename = input(f"Save as [{config_data['name'].lower().replace(' ', '_')}_config]: ").strip()
            if not filename:
                filename = f"{config_data['name'].lower().replace(' ', '_')}_config"
            
            self.save_config_to_file(config_data, filename)
            
            use_now = input("Use this config now? (y/n): ").strip().lower()
            if use_now == 'y':
                self.config = config_data
                print(f"Now using: {self.config['name']}")
                
        except Exception as e:
            print(f"Error importing configuration: {e}")
    
    def import_config_bundle(self):
        """Import a configuration bundle"""
        file_path = input("Path to bundle file: ").strip()
        if not file_path:
            return
        
        if not os.path.exists(file_path):
            print("File not found!")
            return
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                bundle = json.load(f)
            
            imported = 0
            for name, config_data in bundle.items():
                try:
                    # Validate
                    required_keys = ['name', 'description', 'instructions', 'conversation_starters']
                    if all(key in config_data for key in required_keys):
                        self.save_config_to_file(config_data, name)
                        imported += 1
                    else:
                        print(f"Skipped invalid config: {name}")
                except Exception as e:
                    print(f"Error importing {name}: {e}")
            
            print(f"Imported {imported} configurations")
            
        except Exception as e:
            print(f"Error importing bundle: {e}")
    
    def export_claude_md(self):
        """Export current config as Claude.md template"""
        content = f"""# {self.config['name']}

## Name
{self.config['name']}

## Description
{self.config['description']}

## Instructions
{self.config['instructions']}

## Conversation Starters
"""
        for starter in self.config['conversation_starters']:
            content += f'- "{starter}"\n'
        
        content += """
## Knowledge
Upload relevant files here that your AI agent should reference. These files will be available to your agent during conversations and can include:
- Documentation
- Reference materials
- Templates
- Examples
- Data files
- Guidelines

Files uploaded here can be referenced in your instructions and will help your agent provide more accurate and contextual responses.
"""
        
        filename = input("Export as Claude.md filename: ").strip()
        if not filename:
            filename = f"{self.config['name'].lower().replace(' ', '_')}_claude.md"
        
        if not filename.endswith('.md'):
            filename += '.md'
        
        try:
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Claude.md template exported as {filename}")
        except Exception as e:
            print(f"Error exporting Claude.md: {e}")
    
    def run_single_query(self, query):
        """Run a single query and return response"""
        return self.send_to_claude(query)

def main():
    if len(sys.argv) > 1:
        if sys.argv[1] in ["--help", "-h"]:
            print("Custom AI Agent for Claude CLI")
            print("Usage:")
            print("  python3 claude_agent.py                    # Interactive mode")
            print("  python3 claude_agent.py 'your question'    # Single query mode")
            print("  python3 claude_agent.py --help             # Show this help")
            return
        else:
            # Single query mode
            query = " ".join(sys.argv[1:])
            agent = ClaudeAgent()
            response = agent.run_single_query(query)
            print(response)
            return
    
    # Interactive mode
    agent = ClaudeAgent()
    agent.start_conversation()

if __name__ == "__main__":
    main()