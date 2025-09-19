#!/usr/bin/env python3
"""
Custom AI Agent Creator for Claude CLI
A command-line tool to create and manage custom AI agents
"""

import json
import os
import sys
from pathlib import Path

class CustomAgentCreator:
    def __init__(self):
        self.agent_config = {
            "name": "Custom AI Agent",
            "description": "Enter a brief description of what your custom AI agent does and its primary purpose.",
            "instructions": """You are a helpful AI assistant. Please customize these instructions to define your specific role, capabilities, and behavior.

Your instructions can include:
- Your primary role and responsibilities
- How you should interact with users
- Specific knowledge domains you should focus on
- Output formats you should use
- Any constraints or guidelines you should follow
- Step-by-step processes you should follow
- Examples of how you should respond

You have up to 5000 characters to define your agent's behavior and capabilities.""",
            "conversation_starters": [
                "How can I help you today?",
                "What would you like to work on?",
                "Tell me about your project and I'll assist you.",
                "What questions do you have for me?"
            ],
            "knowledge_files": []
        }
    
    def clear_screen(self):
        os.system('cls' if os.name == 'nt' else 'clear')
    
    def display_header(self):
        print("=" * 60)
        print("          CUSTOM AI AGENT CREATOR FOR CLAUDE CLI")
        print("=" * 60)
        print()
    
    def display_menu(self):
        print("\nMAIN MENU:")
        print("1. Edit Agent Name")
        print("2. Edit Agent Description")
        print("3. Edit Agent Instructions")
        print("4. Manage Conversation Starters")
        print("5. Manage Knowledge Files")
        print("6. Preview Agent Configuration")
        print("7. Save Agent Configuration")
        print("8. Load Agent Configuration")
        print("9. Export Claude.md")
        print("0. Exit")
        print("-" * 40)
    
    def get_input(self, prompt, multiline=False):
        if multiline:
            print(f"{prompt}")
            print("(Enter 'END' on a new line to finish)")
            lines = []
            while True:
                line = input()
                if line.strip() == 'END':
                    break
                lines.append(line)
            return '\n'.join(lines)
        else:
            return input(f"{prompt}: ")
    
    def edit_name(self):
        print(f"\nCurrent name: {self.agent_config['name']}")
        new_name = self.get_input("Enter new name (or press Enter to keep current)")
        if new_name.strip():
            self.agent_config['name'] = new_name
            print("Name updated successfully!")
    
    def edit_description(self):
        print(f"\nCurrent description:\n{self.agent_config['description']}")
        print("\nEnter new description:")
        new_description = self.get_input("Description", multiline=True)
        if new_description.strip():
            self.agent_config['description'] = new_description
            print("Description updated successfully!")
    
    def edit_instructions(self):
        print(f"\nCurrent instructions:\n{self.agent_config['instructions']}")
        print("\nEnter new instructions:")
        new_instructions = self.get_input("Instructions", multiline=True)
        if new_instructions.strip():
            self.agent_config['instructions'] = new_instructions
            print("Instructions updated successfully!")
    
    def manage_conversation_starters(self):
        while True:
            print("\nCONVERSATION STARTERS:")
            for i, starter in enumerate(self.agent_config['conversation_starters'], 1):
                print(f"{i}. {starter}")
            
            print("\nOptions:")
            print("1. Add new starter")
            print("2. Edit existing starter")
            print("3. Remove starter")
            print("0. Return to main menu")
            
            choice = self.get_input("Choose option")
            
            if choice == "1":
                new_starter = self.get_input("Enter new conversation starter")
                if new_starter.strip():
                    self.agent_config['conversation_starters'].append(new_starter)
                    print("Conversation starter added!")
            
            elif choice == "2":
                try:
                    index = int(self.get_input("Enter starter number to edit")) - 1
                    if 0 <= index < len(self.agent_config['conversation_starters']):
                        current = self.agent_config['conversation_starters'][index]
                        print(f"Current: {current}")
                        new_starter = self.get_input("Enter new text")
                        if new_starter.strip():
                            self.agent_config['conversation_starters'][index] = new_starter
                            print("Conversation starter updated!")
                    else:
                        print("Invalid starter number!")
                except ValueError:
                    print("Please enter a valid number!")
            
            elif choice == "3":
                try:
                    index = int(self.get_input("Enter starter number to remove")) - 1
                    if 0 <= index < len(self.agent_config['conversation_starters']):
                        removed = self.agent_config['conversation_starters'].pop(index)
                        print(f"Removed: {removed}")
                    else:
                        print("Invalid starter number!")
                except ValueError:
                    print("Please enter a valid number!")
            
            elif choice == "0":
                break
            else:
                print("Invalid option!")
    
    def manage_knowledge_files(self):
        while True:
            print("\nKNOWLEDGE FILES:")
            for i, file_path in enumerate(self.agent_config['knowledge_files'], 1):
                print(f"{i}. {os.path.basename(file_path)} ({file_path})")
            
            print("\nOptions:")
            print("1. Add knowledge file")
            print("2. Remove knowledge file")
            print("3. View knowledge file")
            print("0. Return to main menu")
            
            choice = self.get_input("Choose option")
            
            if choice == "1":
                file_path = self.get_input("Enter file path")
                if os.path.exists(file_path):
                    self.agent_config['knowledge_files'].append(file_path)
                    print("Knowledge file added!")
                else:
                    print("File does not exist!")
            
            elif choice == "2":
                try:
                    index = int(self.get_input("Enter file number to remove")) - 1
                    if 0 <= index < len(self.agent_config['knowledge_files']):
                        removed = self.agent_config['knowledge_files'].pop(index)
                        print(f"Removed: {os.path.basename(removed)}")
                    else:
                        print("Invalid file number!")
                except ValueError:
                    print("Please enter a valid number!")
            
            elif choice == "3":
                try:
                    index = int(self.get_input("Enter file number to view")) - 1
                    if 0 <= index < len(self.agent_config['knowledge_files']):
                        file_path = self.agent_config['knowledge_files'][index]
                        try:
                            with open(file_path, 'r', encoding='utf-8') as f:
                                content = f.read()
                            print(f"\nContent of {os.path.basename(file_path)}:")
                            print("-" * 40)
                            print(content[:1000] + ("..." if len(content) > 1000 else ""))
                            print("-" * 40)
                            input("Press Enter to continue...")
                        except Exception as e:
                            print(f"Error reading file: {e}")
                    else:
                        print("Invalid file number!")
                except ValueError:
                    print("Please enter a valid number!")
            
            elif choice == "0":
                break
            else:
                print("Invalid option!")
    
    def preview_configuration(self):
        print("\nAGENT CONFIGURATION PREVIEW:")
        print("=" * 50)
        print(f"Name: {self.agent_config['name']}")
        print(f"Description: {self.agent_config['description']}")
        print(f"Instructions: {self.agent_config['instructions'][:200]}...")
        print(f"Conversation Starters ({len(self.agent_config['conversation_starters'])}):")
        for starter in self.agent_config['conversation_starters']:
            print(f"  - {starter}")
        print(f"Knowledge Files ({len(self.agent_config['knowledge_files'])}):")
        for file_path in self.agent_config['knowledge_files']:
            print(f"  - {os.path.basename(file_path)}")
        print("=" * 50)
        input("Press Enter to continue...")
    
    def save_configuration(self):
        file_path = self.get_input("Enter save file path (with .json extension)")
        if not file_path.endswith('.json'):
            file_path += '.json'
        
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(self.agent_config, f, indent=2)
            print(f"Configuration saved to {file_path}")
        except Exception as e:
            print(f"Error saving configuration: {e}")
    
    def load_configuration(self):
        file_path = self.get_input("Enter configuration file path")
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                self.agent_config = json.load(f)
            print(f"Configuration loaded from {file_path}")
        except Exception as e:
            print(f"Error loading configuration: {e}")
    
    def generate_claude_md(self):
        content = f"""# {self.agent_config['name']}

## Name
{self.agent_config['name']}

## Description
{self.agent_config['description']}

## Instructions
{self.agent_config['instructions']}

## Conversation Starters
"""
        for starter in self.agent_config['conversation_starters']:
            content += f'- "{starter}"\n'
        
        content += "\n## Knowledge\n"
        if self.agent_config['knowledge_files']:
            content += "Upload relevant files here that your AI agent should reference. These files will be available to your agent during conversations and can include:\n"
            content += "- Documentation\n- Reference materials\n- Templates\n- Examples\n- Data files\n- Guidelines\n\n"
            content += "Referenced files:\n"
            for file_path in self.agent_config['knowledge_files']:
                content += f"- {os.path.basename(file_path)}\n"
        else:
            content += """Upload relevant files here that your AI agent should reference. These files will be available to your agent during conversations and can include:
- Documentation
- Reference materials
- Templates
- Examples
- Data files
- Guidelines

Files uploaded here can be referenced in your instructions and will help your agent provide more accurate and contextual responses."""
        
        return content
    
    def export_claude_md(self):
        file_path = self.get_input("Enter export file path (with .md extension)")
        if not file_path.endswith('.md'):
            file_path += '.md'
        
        try:
            content = self.generate_claude_md()
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"Claude.md exported to {file_path}")
        except Exception as e:
            print(f"Error exporting Claude.md: {e}")
    
    def run(self):
        while True:
            self.clear_screen()
            self.display_header()
            self.display_menu()
            
            choice = self.get_input("Choose option")
            
            if choice == "1":
                self.edit_name()
            elif choice == "2":
                self.edit_description()
            elif choice == "3":
                self.edit_instructions()
            elif choice == "4":
                self.manage_conversation_starters()
            elif choice == "5":
                self.manage_knowledge_files()
            elif choice == "6":
                self.preview_configuration()
            elif choice == "7":
                self.save_configuration()
            elif choice == "8":
                self.load_configuration()
            elif choice == "9":
                self.export_claude_md()
            elif choice == "0":
                print("Goodbye!")
                break
            else:
                print("Invalid option! Please try again.")
                input("Press Enter to continue...")

def main():
    if len(sys.argv) > 1:
        if sys.argv[1] == "--help" or sys.argv[1] == "-h":
            print("Custom AI Agent Creator for Claude CLI")
            print("Usage: python3 custom_agent_creator.py")
            print("\nThis tool helps you create custom AI agents for use with Claude CLI.")
            print("You can configure agent name, description, instructions, conversation starters,")
            print("and knowledge files, then export the configuration as a Claude.md file.")
            return
    
    creator = CustomAgentCreator()
    creator.run()

if __name__ == "__main__":
    main()