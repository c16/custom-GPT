#!/usr/bin/env python3
"""
GUI version of Custom AI Agent for Claude CLI
Uses tkinter if available, falls back to enhanced command-line interface
"""

import sys
import os

# Try to import tkinter, fall back to command-line if not available
try:
    import tkinter as tk
    from tkinter import ttk, scrolledtext, messagebox
    TKINTER_AVAILABLE = True
except ImportError:
    TKINTER_AVAILABLE = False

from claude_agent import ClaudeAgent
import threading
import queue
import time

class ClaudeAgentGUI:
    def __init__(self):
        self.agent = ClaudeAgent()
        self.response_queue = queue.Queue()
        
        if TKINTER_AVAILABLE:
            self.setup_tkinter_gui()
        else:
            print("Tkinter not available. Using enhanced command-line interface.")
            self.run_enhanced_cli()
    
    def setup_tkinter_gui(self):
        """Setup the tkinter GUI"""
        self.root = tk.Tk()
        self.root.title(f"{self.agent.config['name']} - Claude CLI Agent")
        self.root.geometry("1600x1200")
        
        # Main frame
        # Configure colorful modern theme
        self.root.configure(bg='#2c3e50')

        # Create styled frame with background
        style = ttk.Style()
        style.configure('Custom.TFrame', background='#34495e')
        main_frame = ttk.Frame(self.root, padding="20", style='Custom.TFrame')
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(0, weight=1)
        main_frame.rowconfigure(1, weight=1)
        
        # Header
        header_frame = ttk.Frame(main_frame)
        header_frame.grid(row=0, column=0, sticky=(tk.W, tk.E), pady=(0, 10))
        header_frame.columnconfigure(1, weight=1)
        
        # Store references to header labels for dynamic updates
        self.name_label = ttk.Label(header_frame, text=self.agent.config['name'], font=("Arial", 16, "bold"))
        self.name_label.grid(row=0, column=0, sticky=tk.W)

        # CLI Provider selection
        ttk.Label(header_frame, text="CLI:").grid(row=0, column=2, padx=(10, 5))
        self.cli_var = tk.StringVar(value=self.agent.active_provider or "auto")
        cli_combo = ttk.Combobox(header_frame, textvariable=self.cli_var, values=["auto", "claude", "gemini"],
                                width=8, state="readonly")
        cli_combo.grid(row=0, column=3, padx=5)
        cli_combo.bind("<<ComboboxSelected>>", self.on_cli_change)

        ttk.Button(header_frame, text="Config", command=self.open_config_window).grid(row=0, column=4, padx=5)
        ttk.Button(header_frame, text="Library", command=self.open_config_library).grid(row=0, column=5, padx=5)
        ttk.Button(header_frame, text="Copy All", command=self.copy_chat_to_clipboard).grid(row=0, column=6, padx=5)
        ttk.Button(header_frame, text="Clear", command=self.clear_chat).grid(row=0, column=7, padx=5)

        self.description_label = ttk.Label(header_frame, text=self.agent.config['description'], wraplength=500)
        self.description_label.grid(row=1, column=0, columnspan=6, sticky=(tk.W, tk.E), pady=5)
        
        # Chat area
        chat_frame = ttk.Frame(main_frame)
        chat_frame.grid(row=1, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        chat_frame.columnconfigure(0, weight=1)
        chat_frame.rowconfigure(0, weight=1)
        
        # Create text widget with custom scrollbar to avoid ScrolledText issues
        text_scroll_frame = tk.Frame(chat_frame, bg='#34495e')
        text_scroll_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        text_scroll_frame.grid_rowconfigure(0, weight=1)
        text_scroll_frame.grid_columnconfigure(0, weight=1)

        self.chat_display = tk.Text(text_scroll_frame, wrap=tk.WORD, state=tk.DISABLED,
                                   relief=tk.FLAT, borderwidth=0, bg='#34495e', fg='#ecf0f1',
                                   cursor='arrow', insertbackground='#34495e', highlightthickness=0,
                                   exportselection=False, selectbackground='#34495e', selectforeground='#ecf0f1',
                                   font=('Arial', 12), spacing1=0, spacing2=0, spacing3=12,
                                   insertwidth=0, insertofftime=0, insertontime=0)

        scrollbar = tk.Scrollbar(text_scroll_frame, orient=tk.VERTICAL, command=self.chat_display.yview, bg='#2c3e50', troughcolor='#34495e', activebackground='#3498db')
        self.chat_display.configure(yscrollcommand=scrollbar.set)

        self.chat_display.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        scrollbar.grid(row=0, column=1, sticky=(tk.N, tk.S))

        # Prevent focus and cursor positioning completely
        self.chat_display.bind('<Button-1>', lambda e: 'break')
        self.chat_display.bind('<B1-Motion>', lambda e: 'break')
        self.chat_display.bind('<Double-Button-1>', lambda e: 'break')
        self.chat_display.bind('<Triple-Button-1>', lambda e: 'break')
        self.chat_display.bind('<Key>', lambda e: 'break')
        self.chat_display.bind('<FocusIn>', lambda e: 'break')
        
        # Input area
        input_frame = ttk.Frame(main_frame)
        input_frame.grid(row=2, column=0, sticky=(tk.W, tk.E))
        input_frame.columnconfigure(0, weight=1)
        
        self.input_text = tk.Text(input_frame, height=6, wrap=tk.WORD, bg='#ecf0f1', fg='#2c3e50', relief=tk.SUNKEN, borderwidth=2, highlightthickness=2, highlightcolor='#e74c3c', highlightbackground='#bdc3c7', insertbackground='#e74c3c')
        self.input_text.grid(row=0, column=0, sticky=(tk.W, tk.E), padx=(0, 5))
        
        button_frame = ttk.Frame(input_frame)
        button_frame.grid(row=0, column=1, sticky=(tk.N, tk.S))
        
        ttk.Button(button_frame, text="Send", command=self.send_message).pack(pady=2)
        ttk.Button(button_frame, text="History", command=self.show_history).pack(pady=2)
        
        # Conversation starters frame (store reference for dynamic updates)
        self.starters_frame = ttk.LabelFrame(main_frame, text="Conversation Starters", padding="5")
        self.starters_frame.grid(row=3, column=0, sticky=(tk.W, tk.E), pady=10)

        # Build initial conversation starters
        self.refresh_conversation_starters()
        
        # Bind Enter key
        self.input_text.bind('<Control-Return>', lambda e: self.send_message())

        # Configure text tags properly to prevent rendering issues
        self.chat_display.tag_configure('sel', background='#34495e', foreground='#ecf0f1')
        self.chat_display.tag_configure('normal', background='#34495e', foreground='#ecf0f1')
        
        # Welcome message
        self.add_message("System", "Welcome! How can I help you today?")
        
        # Start checking for responses
        self.root.after(100, self.check_response_queue)
    
    def fix_double_newlines(self, text):
        """Fix problematic double newlines - experiment with different approaches here"""
        # Option 1: Replace with single newline
        # return text.replace('\n\n', '\n')

        # Option 2: Replace with newline + space + newline (avoid \r)
        # return text.replace('\n\n', '\n \n')

        # Option 3: Replace with newline + dash + newline
        # return text.replace('\n\n', '\n---\n')

        # Option 4: Replace with double space on same line
        # return text.replace('\n\n', '  ')

        # Option 6: Use newline + invisible Unicode space + newline
        # return text.replace('\n\n', '\n\u00A0\n')  # Non-breaking space

        # Option 7: Use newline + dot + newline (minimal visual)
        # return text.replace('\n\n', '\n.\n')

        # Option 8: Use newline + multiple spaces + newline
        # return text.replace('\n\n', '\n    \n')

        # Option 9: Use newline + tab + newline
        # return text.replace('\n\n', '\n\t\n')

        # Option 10: Use Unicode thin space (less visible than regular space)
        # return text.replace('\n\n', '\n\u2009\n')  # Thin space

        # Option 11: Use zero-width joiner (invisible)
        # return text.replace('\n\n', '\n\u200D\n')  # Zero-width joiner

        # Option 12: Use hair space (ultra-thin)
        return text.replace('\n\n', '\n\u200A\n')  # Hair space

        # Option 13: Use figure space (same width as a digit)
        # return text.replace('\n\n', '\n\u2007\n')  # Figure space

    def add_message(self, sender, message):
        """Add a message to the chat display"""
        self.chat_display.config(state=tk.NORMAL)
        timestamp = time.strftime("%H:%M:%S")

        # Clean and fix problematic newlines
        clean_message = message.replace('\r\n', '\n').replace('\r', '\n').strip()
        #clean_message = self.fix_double_newlines(clean_message)

        self.chat_display.insert(tk.END, f"[{timestamp}] {sender}: {clean_message}\n")
        self.chat_display.config(state=tk.DISABLED)
        self.chat_display.see(tk.END)

    def update_header(self):
        """Update header labels with current config info"""
        self.name_label.config(text=self.agent.config['name'])
        self.description_label.config(text=self.agent.config['description'])
        provider_name = self.agent.active_provider or "CLI"
        self.root.title(f"{self.agent.config['name']} - {provider_name.title()} Agent")

    def on_cli_change(self, event=None):
        """Handle CLI provider change"""
        new_provider = self.cli_var.get()
        old_provider = self.agent.active_provider

        # Try to switch CLI provider
        success = self.agent.switch_cli_provider(new_provider)

        # Update header to reflect new provider
        self.update_header()

        # Show status message
        if success:
            provider_name = self.agent.active_provider.title()
            self.add_message("System", f"Switched to {provider_name} CLI")
        else:
            self.add_message("System", f"Warning: {new_provider.title()} CLI not found")
            # Revert to previous working provider
            self.cli_var.set(old_provider or "auto")

    def refresh_conversation_starters(self):
        """Refresh conversation starter buttons based on current config"""
        # Update header when config changes
        self.update_header()

        # Clear existing starter buttons
        for widget in self.starters_frame.winfo_children():
            widget.destroy()

        # Add new starter buttons if they exist in config
        if self.agent.config.get('conversation_starters'):
            for i, starter in enumerate(self.agent.config['conversation_starters']):
                ttk.Button(self.starters_frame, text=starter,
                          command=lambda s=starter: self.use_starter(s)).pack(fill=tk.X, pady=2)
        else:
            # Show message if no starters available
            ttk.Label(self.starters_frame, text="No conversation starters available",
                     foreground='gray').pack(pady=10)

    def send_message(self):
        """Send user message to Claude"""
        user_message = self.input_text.get("1.0", tk.END).strip()
        if not user_message:
            return
        
        # Clear input
        self.input_text.delete("1.0", tk.END)
        
        # Add user message to chat
        self.add_message("You", user_message)
        
        # Show thinking message
        self.add_message("System", "Thinking...")
        
        # Send to Claude in background thread
        thread = threading.Thread(target=self.get_claude_response, args=(user_message,))
        thread.daemon = True
        thread.start()
    
    def get_claude_response(self, message):
        """Get response from Claude CLI in background thread"""
        response = self.agent.send_to_claude(message)
        self.response_queue.put(response)
    
    def check_response_queue(self):
        """Check for responses from background thread"""
        try:
            while True:
                response = self.response_queue.get_nowait()
                # Remove "thinking" message
                self.chat_display.config(state=tk.NORMAL)
                content = self.chat_display.get("1.0", tk.END)
                if "System: Thinking..." in content:
                    lines = content.split('\n')
                    # Remove the last "Thinking..." message
                    for i in range(len(lines) - 1, -1, -1):
                        if "System: Thinking..." in lines[i]:
                            lines.pop(i)
                            if i < len(lines) and not lines[i].strip():
                                lines.pop(i)
                            break
                    self.chat_display.delete("1.0", tk.END)
                    self.chat_display.insert("1.0", '\n'.join(lines))
                self.chat_display.config(state=tk.DISABLED)
                
                # Add Claude's response
                self.add_message(self.agent.config['name'], response)
        except queue.Empty:
            pass
        
        # Schedule next check
        self.root.after(100, self.check_response_queue)
    
    def use_starter(self, starter):
        """Use a conversation starter"""
        self.input_text.delete("1.0", tk.END)
        self.input_text.insert("1.0", starter)
    
    def clear_chat(self):
        """Clear the chat display"""
        self.chat_display.config(state=tk.NORMAL)
        self.chat_display.delete("1.0", tk.END)
        self.chat_display.config(state=tk.DISABLED)
        self.add_message("System", "Chat cleared. How can I help you?")

    def copy_chat_to_clipboard(self):
        """Copy all chat content to clipboard"""
        try:
            # Get all text from the chat display
            chat_content = self.chat_display.get("1.0", tk.END)
            # Remove the trailing newline that tk.END includes
            chat_content = chat_content.rstrip('\n')

            # Copy to clipboard
            self.root.clipboard_clear()
            self.root.clipboard_append(chat_content)

            # Show confirmation
            messagebox.showinfo("Copied", "Chat content copied to clipboard!")

        except Exception as e:
            messagebox.showerror("Error", f"Error copying to clipboard: {e}")
    
    def show_history(self):
        """Show conversation history in a new window"""
        if not self.agent.conversation_history:
            messagebox.showinfo("History", "No conversation history yet.")
            return
        
        history_window = tk.Toplevel(self.root)
        history_window.title("Conversation History")
        history_window.geometry("600x400")
        
        history_text = scrolledtext.ScrolledText(history_window, wrap=tk.WORD, relief=tk.SUNKEN, borderwidth=2, bg='#ecf0f1', fg='#2c3e50', selectbackground='#ecf0f1', selectforeground='#2c3e50', highlightthickness=0)
        history_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        for entry in self.agent.conversation_history:
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(entry['timestamp']))
            history_text.insert(tk.END, f"[{timestamp}]\n")
            history_text.insert(tk.END, f"You: {entry['user']}\n")
            history_text.insert(tk.END, f"{self.agent.config['name']}: {entry['assistant']}\n")
            history_text.insert(tk.END, "-" * 50 + "\n\n")
        
        history_text.config(state=tk.DISABLED)
    
    def open_config_window(self):
        """Open configuration window"""
        config_window = tk.Toplevel(self.root)
        config_window.title("Agent Configuration")
        config_window.geometry("500x400")
        
        notebook = ttk.Notebook(config_window)
        notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Basic settings tab
        basic_frame = ttk.Frame(notebook, padding="10")
        notebook.add(basic_frame, text="Basic Settings")
        
        ttk.Label(basic_frame, text="Agent Name:").grid(row=0, column=0, sticky=tk.W, pady=5)
        name_var = tk.StringVar(value=self.agent.config['name'])
        ttk.Entry(basic_frame, textvariable=name_var, width=40).grid(row=0, column=1, sticky=(tk.W, tk.E), pady=5)
        
        ttk.Label(basic_frame, text="Description:").grid(row=1, column=0, sticky=(tk.W, tk.N), pady=5)
        desc_text = tk.Text(basic_frame, height=4, width=40)
        desc_text.grid(row=1, column=1, sticky=(tk.W, tk.E), pady=5)
        desc_text.insert("1.0", self.agent.config['description'])
        
        # Instructions tab
        inst_frame = ttk.Frame(notebook, padding="10")
        notebook.add(inst_frame, text="Instructions")
        
        inst_text = scrolledtext.ScrolledText(inst_frame, wrap=tk.WORD, relief=tk.SUNKEN, borderwidth=2, bg='#ecf0f1', fg='#2c3e50', selectbackground='#ecf0f1', selectforeground='#2c3e50', cursor='arrow', insertbackground='#e74c3c', highlightthickness=0)
        inst_text.pack(fill=tk.BOTH, expand=True)
        inst_text.insert("1.0", self.agent.config['instructions'])
        
        # Save function
        def save_config():
            self.agent.config['name'] = name_var.get()
            self.agent.config['description'] = desc_text.get("1.0", tk.END).strip()
            self.agent.config['instructions'] = inst_text.get("1.0", tk.END).strip()
            self.agent.save_config()
            self.root.title(f"{self.agent.config['name']} - Claude CLI Agent")
            config_window.destroy()
            messagebox.showinfo("Success", "Configuration saved!")
        
        button_frame = ttk.Frame(config_window)
        button_frame.pack(pady=10)
        
        ttk.Button(button_frame, text="Save", command=save_config).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Save As...", command=lambda: self.save_config_as_gui(config_window)).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Load...", command=lambda: self.load_config_gui(config_window)).pack(side=tk.LEFT, padx=5)
    
    def save_config_as_gui(self, parent):
        """Save configuration with new name (GUI)"""
        from tkinter import simpledialog
        filename = simpledialog.askstring("Save As", "Enter filename (without .json):", parent=parent)
        if filename:
            self.agent.save_config_to_file(self.agent.config, filename)
            messagebox.showinfo("Success", f"Configuration saved as configs/{filename}.json")
    
    def load_config_gui(self, parent):
        """Load configuration (GUI)"""
        from tkinter import filedialog
        file_path = filedialog.askopenfilename(
            title="Load Configuration",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")],
            parent=parent
        )
        
        if file_path:
            try:
                import json
                with open(file_path, 'r', encoding='utf-8') as f:
                    new_config = json.load(f)
                
                # Validate configuration
                required_keys = ['name', 'description', 'instructions', 'conversation_starters']
                for key in required_keys:
                    if key not in new_config:
                        messagebox.showerror("Error", f"Invalid configuration: missing '{key}' field")
                        return
                
                self.agent.config = new_config
                self.refresh_conversation_starters()  # Refresh starters and header for new config
                parent.destroy()
                messagebox.showinfo("Success", f"Configuration loaded: {self.agent.config['name']}")
                
            except Exception as e:
                messagebox.showerror("Error", f"Error loading configuration: {e}")
    
    def open_config_library(self):
        """Open configuration library window"""
        library_window = tk.Toplevel(self.root)
        library_window.title("Configuration Library")
        library_window.geometry("700x500")
        library_window.transient(self.root)
        
        # Main frame
        main_frame = ttk.Frame(library_window, padding="10")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Title
        ttk.Label(main_frame, text="Configuration Library", font=("Arial", 14, "bold")).pack(pady=(0, 10))
        
        # Notebook for tabs
        notebook = ttk.Notebook(main_frame)
        notebook.pack(fill=tk.BOTH, expand=True)
        
        # Browse tab
        browse_frame = ttk.Frame(notebook, padding="10")
        notebook.add(browse_frame, text="Browse")
        self.setup_browse_tab(browse_frame)
        
        # Templates tab
        templates_frame = ttk.Frame(notebook, padding="10")
        notebook.add(templates_frame, text="Templates")
        self.setup_templates_tab(templates_frame)
        
        # Import/Export tab
        import_export_frame = ttk.Frame(notebook, padding="10")
        notebook.add(import_export_frame, text="Import/Export")
        self.setup_import_export_tab(import_export_frame)
        
        # Management tab
        management_frame = ttk.Frame(notebook, padding="10")
        notebook.add(management_frame, text="Manage")
        self.setup_management_tab(management_frame)
    
    def setup_browse_tab(self, parent):
        """Setup the browse configurations tab"""
        # Config list
        list_frame = ttk.Frame(parent)
        list_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 10))
        
        # Treeview for config list
        columns = ("Name", "Description", "File")
        self.config_tree = ttk.Treeview(list_frame, columns=columns, show="headings", height=15)
        
        for col in columns:
            self.config_tree.heading(col, text=col)
            self.config_tree.column(col, width=200)
        
        # Scrollbar
        scrollbar = ttk.Scrollbar(list_frame, orient=tk.VERTICAL, command=self.config_tree.yview)
        self.config_tree.configure(yscrollcommand=scrollbar.set)
        
        self.config_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # Buttons
        button_frame = ttk.Frame(parent)
        button_frame.pack(fill=tk.X)
        
        ttk.Button(button_frame, text="Refresh", command=self.refresh_config_list).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Load Selected", command=self.load_selected_config).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Preview", command=self.preview_selected_config).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Delete", command=self.delete_selected_config).pack(side=tk.LEFT, padx=5)
        
        # Load initial list
        self.refresh_config_list()
    
    def setup_templates_tab(self, parent):
        """Setup the templates tab"""
        ttk.Label(parent, text="Create new configuration from template:", font=("Arial", 12, "bold")).pack(pady=(0, 10))
        
        templates = {
            "General Assistant": "A helpful general-purpose AI assistant",
            "Code Assistant": "Programming and development helper",
            "Learning Tutor": "Patient educational assistant",
            "Writing Assistant": "Writing and editing helper",
            "Research Assistant": "Research and analysis helper",
            "Creative Assistant": "Creative writing and brainstorming helper"
        }
        
        for name, desc in templates.items():
            frame = ttk.LabelFrame(parent, text=name, padding="10")
            frame.pack(fill=tk.X, pady=5)
            
            ttk.Label(frame, text=desc).pack(side=tk.LEFT)
            ttk.Button(frame, text="Create", command=lambda n=name: self.create_from_template_gui(n)).pack(side=tk.RIGHT)
    
    def setup_import_export_tab(self, parent):
        """Setup the import/export tab"""
        # Export section
        export_frame = ttk.LabelFrame(parent, text="Export", padding="10")
        export_frame.pack(fill=tk.X, pady=(0, 10))
        
        ttk.Button(export_frame, text="Export Current Config", command=self.export_current_gui).pack(side=tk.LEFT, padx=5)
        ttk.Button(export_frame, text="Export All Configs", command=self.export_all_gui).pack(side=tk.LEFT, padx=5)
        ttk.Button(export_frame, text="Export as Claude.md", command=self.export_claude_md_gui).pack(side=tk.LEFT, padx=5)
        
        # Import section
        import_frame = ttk.LabelFrame(parent, text="Import", padding="10")
        import_frame.pack(fill=tk.X)
        
        ttk.Button(import_frame, text="Import Config File", command=self.import_config_gui).pack(side=tk.LEFT, padx=5)
        ttk.Button(import_frame, text="Import Config Bundle", command=self.import_bundle_gui).pack(side=tk.LEFT, padx=5)
    
    def setup_management_tab(self, parent):
        """Setup the management tab"""
        ttk.Label(parent, text="Configuration Management", font=("Arial", 12, "bold")).pack(pady=(0, 10))
        
        # Current config info
        info_frame = ttk.LabelFrame(parent, text="Current Configuration", padding="10")
        info_frame.pack(fill=tk.X, pady=(0, 10))
        
        self.current_config_label = ttk.Label(info_frame, text=f"Name: {self.agent.config['name']}")
        self.current_config_label.pack(anchor=tk.W)
        
        # Management buttons
        mgmt_frame = ttk.LabelFrame(parent, text="Actions", padding="10")
        mgmt_frame.pack(fill=tk.X)
        
        ttk.Button(mgmt_frame, text="Duplicate Current", command=self.duplicate_current_gui).pack(side=tk.LEFT, padx=5)
        ttk.Button(mgmt_frame, text="Reset to Default", command=self.reset_to_default_gui).pack(side=tk.LEFT, padx=5)
    
    def refresh_config_list(self):
        """Refresh the configuration list"""
        # Clear existing items
        for item in self.config_tree.get_children():
            self.config_tree.delete(item)

        # Collect config files
        from pathlib import Path
        import json
        import os

        config_files = []

        # Get config directory from environment or use default
        config_dir = os.environ.get('CLAUDE_AGENT_CONFIG_DIR', '../configs')
        configs_dir = Path(config_dir)

        # Current directory (legacy support)
        for file in Path(".").glob("*_config.json"):
            config_files.append(file)

        # Configs directory (new location)
        if configs_dir.exists():
            for file in configs_dir.glob("*.json"):
                config_files.append(file)
        
        # Add to tree
        for file in config_files:
            try:
                with open(file, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                
                name = config_data.get('name', 'Unknown')
                desc = config_data.get('description', 'No description')[:50] + "..."
                
                self.config_tree.insert("", tk.END, values=(name, desc, file.name))
            except Exception as e:
                self.config_tree.insert("", tk.END, values=("Error", f"Could not read: {e}", file.name))
    
    def load_selected_config(self):
        """Load the selected configuration"""
        selection = self.config_tree.selection()
        if not selection:
            messagebox.showwarning("No Selection", "Please select a configuration to load.")
            return

        item = self.config_tree.item(selection[0])
        filename = item['values'][2]

        # Find the full path
        from pathlib import Path
        import os
        file_path = None

        # Get config directory from environment or use default
        config_dir = os.environ.get('CLAUDE_AGENT_CONFIG_DIR', '../configs')

        if Path(filename).exists():
            file_path = Path(filename)
        elif (Path(config_dir) / filename).exists():
            file_path = Path(config_dir) / filename
        elif (Path("configs") / filename).exists():  # Legacy fallback
            file_path = Path("configs") / filename
        
        if file_path:
            self.agent.load_specific_config(str(file_path))
            self.current_config_label.config(text=f"Name: {self.agent.config['name']}")
            self.refresh_conversation_starters()  # Refresh starters and header for new config
            messagebox.showinfo("Success", f"Loaded configuration: {self.agent.config['name']}")
        else:
            messagebox.showerror("Error", "Configuration file not found.")
    
    def preview_selected_config(self):
        """Preview the selected configuration"""
        selection = self.config_tree.selection()
        if not selection:
            messagebox.showwarning("No Selection", "Please select a configuration to preview.")
            return
        
        item = self.config_tree.item(selection[0])
        filename = item['values'][2]
        
        # Find and read the file
        from pathlib import Path
        import json
        
        file_path = None
        if Path(filename).exists():
            file_path = Path(filename)
        elif (Path("configs") / filename).exists():
            file_path = Path("configs") / filename
        
        if file_path:
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                
                # Create preview window
                preview_window = tk.Toplevel(self.root)
                preview_window.title(f"Preview: {config_data.get('name', 'Unknown')}")
                preview_window.geometry("600x400")
                
                text_widget = scrolledtext.ScrolledText(preview_window, wrap=tk.WORD, padx=10, pady=10, relief=tk.SUNKEN, borderwidth=2, bg='#ecf0f1', fg='#2c3e50', selectbackground='#ecf0f1', selectforeground='#2c3e50', highlightthickness=0)
                text_widget.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
                
                content = f"""Name: {config_data.get('name', 'Unknown')}

Description:
{config_data.get('description', 'No description')}

Instructions:
{config_data.get('instructions', 'No instructions')}

Conversation Starters:
"""
                for starter in config_data.get('conversation_starters', []):
                    content += f"- {starter}\n"
                
                text_widget.insert("1.0", content)
                text_widget.config(state=tk.DISABLED)
                
            except Exception as e:
                messagebox.showerror("Error", f"Error reading configuration: {e}")
        else:
            messagebox.showerror("Error", "Configuration file not found.")
    
    def delete_selected_config(self):
        """Delete the selected configuration"""
        selection = self.config_tree.selection()
        if not selection:
            messagebox.showwarning("No Selection", "Please select a configuration to delete.")
            return
        
        item = self.config_tree.item(selection[0])
        config_name = item['values'][0]
        filename = item['values'][2]
        
        if filename == "agent_config.json":
            messagebox.showwarning("Cannot Delete", "Cannot delete the active configuration file.")
            return
        
        if messagebox.askyesno("Confirm Delete", f"Really delete '{config_name}'?\nThis cannot be undone."):
            from pathlib import Path
            
            file_path = None
            if Path(filename).exists():
                file_path = Path(filename)
            elif (Path("configs") / filename).exists():
                file_path = Path("configs") / filename
            
            if file_path:
                try:
                    file_path.unlink()
                    self.refresh_config_list()
                    messagebox.showinfo("Success", f"Deleted '{config_name}'")
                except Exception as e:
                    messagebox.showerror("Error", f"Error deleting file: {e}")
            else:
                messagebox.showerror("Error", "Configuration file not found.")
    
    def create_from_template_gui(self, template_name):
        """Create configuration from template (GUI)"""
        templates = {
            "General Assistant": {
                "name": "General Assistant",
                "description": "A helpful general-purpose AI assistant",
                "instructions": "You are a helpful, accurate, and friendly AI assistant. Provide clear, concise answers and always be respectful.",
                "conversation_starters": ["How can I help you?", "What would you like to know?"]
            },
            "Code Assistant": {
                "name": "Code Assistant",
                "description": "Programming and development helper",
                "instructions": "You are an expert programmer. Help with code review, debugging, best practices, and programming questions. Always explain your reasoning.",
                "conversation_starters": ["What code can I help with?", "Need help debugging?", "Looking for code review?"]
            },
            "Learning Tutor": {
                "name": "Learning Tutor",
                "description": "Patient educational assistant",
                "instructions": "You are a patient tutor. Break down complex topics, use examples, and adapt to the learner's level. Always encourage questions.",
                "conversation_starters": ["What would you like to learn?", "Need help understanding something?", "Ready for your lesson?"]
            },
            "Writing Assistant": {
                "name": "Writing Assistant",
                "description": "Writing and editing helper",
                "instructions": "You are a professional writing coach. Help improve clarity, style, grammar, and structure while maintaining the author's voice.",
                "conversation_starters": ["What writing can I help with?", "Need editing assistance?", "Want to improve your text?"]
            },
            "Research Assistant": {
                "name": "Research Assistant",
                "description": "Research and analysis helper",
                "instructions": "You are a research assistant. Help analyze information, summarize sources, and provide insights. Always cite sources and be thorough.",
                "conversation_starters": ["What research can I help with?", "Need analysis of data?", "Looking for insights?"]
            },
            "Creative Assistant": {
                "name": "Creative Assistant",
                "description": "Creative writing and brainstorming helper",
                "instructions": "You are a creative assistant. Help with brainstorming, creative writing, storytelling, and imaginative projects. Be inspiring and encouraging.",
                "conversation_starters": ["What creative project are you working on?", "Need brainstorming help?", "Want to explore ideas?"]
            }
        }
        
        if template_name in templates:
            template = templates[template_name]
            
            # Create customization dialog
            dialog = tk.Toplevel(self.root)
            dialog.title(f"Create {template_name}")
            dialog.geometry("600x500")  # Made taller and wider for instructions field
            dialog.transient(self.root)
            dialog.grab_set()

            frame = ttk.Frame(dialog, padding="10")
            frame.pack(fill=tk.BOTH, expand=True)

            # Name
            ttk.Label(frame, text="Agent Name:").grid(row=0, column=0, sticky=tk.W, pady=5)
            name_var = tk.StringVar(value=template['name'])
            ttk.Entry(frame, textvariable=name_var, width=40).grid(row=0, column=1, sticky=(tk.W, tk.E), pady=5)

            # Description
            ttk.Label(frame, text="Description:").grid(row=1, column=0, sticky=(tk.W, tk.N), pady=5)
            desc_text = tk.Text(frame, height=3, width=40)
            desc_text.grid(row=1, column=1, sticky=(tk.W, tk.E), pady=5)
            desc_text.insert("1.0", template['description'])

            # Instructions (NEW)
            ttk.Label(frame, text="Instructions:").grid(row=2, column=0, sticky=(tk.W, tk.N), pady=5)
            inst_text = scrolledtext.ScrolledText(frame, height=8, width=40, wrap=tk.WORD)
            inst_text.grid(row=2, column=1, sticky=(tk.W, tk.E, tk.N, tk.S), pady=5)
            inst_text.insert("1.0", template['instructions'])

            # Filename
            ttk.Label(frame, text="Save as:").grid(row=3, column=0, sticky=tk.W, pady=5)
            filename_var = tk.StringVar(value=template['name'].lower().replace(' ', '_'))
            ttk.Entry(frame, textvariable=filename_var, width=40).grid(row=3, column=1, sticky=(tk.W, tk.E), pady=5)

            frame.columnconfigure(1, weight=1)
            frame.rowconfigure(2, weight=1)  # Make instructions row expandable
            
            def create_config():
                new_config = template.copy()
                new_config['name'] = name_var.get()
                new_config['description'] = desc_text.get("1.0", tk.END).strip()
                new_config['instructions'] = inst_text.get("1.0", tk.END).strip()  # Include instructions
                
                filename = filename_var.get()
                if filename:
                    self.agent.save_config_to_file(new_config, filename)
                    dialog.destroy()
                    
                    if messagebox.askyesno("Use Now", "Configuration created! Use it now?"):
                        self.agent.config = new_config
                        self.current_config_label.config(text=f"Name: {self.agent.config['name']}")
                        self.refresh_conversation_starters()  # Refresh starters and header for new config
                    
                    self.refresh_config_list()
            
            button_frame = ttk.Frame(frame)
            button_frame.grid(row=4, column=0, columnspan=2, pady=20)  # Updated row number
            
            ttk.Button(button_frame, text="Create", command=create_config).pack(side=tk.LEFT, padx=5)
            ttk.Button(button_frame, text="Cancel", command=dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    def export_current_gui(self):
        """Export current configuration (GUI)"""
        from tkinter import filedialog
        filename = filedialog.asksaveasfilename(
            title="Export Configuration",
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if filename:
            import json
            try:
                with open(filename, 'w', encoding='utf-8') as f:
                    json.dump(self.agent.config, f, indent=2)
                messagebox.showinfo("Success", f"Configuration exported to {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Error exporting configuration: {e}")
    
    def export_all_gui(self):
        """Export all configurations as bundle (GUI)"""
        from tkinter import filedialog
        from pathlib import Path
        import json
        
        filename = filedialog.asksaveasfilename(
            title="Export All Configurations",
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if filename:
            config_files = []
            
            for file in Path(".").glob("*_config.json"):
                config_files.append(file)
            
            configs_dir = Path("configs")
            if configs_dir.exists():
                for file in configs_dir.glob("*.json"):
                    config_files.append(file)
            
            bundle = {}
            for file in config_files:
                try:
                    with open(file, 'r', encoding='utf-8') as f:
                        config_data = json.load(f)
                    bundle[file.stem] = config_data
                except Exception as e:
                    print(f"Error reading {file.name}: {e}")
            
            try:
                with open(filename, 'w', encoding='utf-8') as f:
                    json.dump(bundle, f, indent=2)
                messagebox.showinfo("Success", f"Exported {len(bundle)} configurations to {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Error exporting bundle: {e}")
    
    def export_claude_md_gui(self):
        """Export as Claude.md template (GUI)"""
        from tkinter import filedialog
        filename = filedialog.asksaveasfilename(
            title="Export as Claude.md",
            defaultextension=".md",
            filetypes=[("Markdown files", "*.md"), ("All files", "*.*")]
        )
        
        if filename:
            self.agent.export_claude_md()
    
    def import_config_gui(self):
        """Import configuration file (GUI)"""
        from tkinter import filedialog
        file_path = filedialog.askopenfilename(
            title="Import Configuration",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if file_path:
            import json
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    config_data = json.load(f)
                
                # Validate
                required_keys = ['name', 'description', 'instructions', 'conversation_starters']
                for key in required_keys:
                    if key not in config_data:
                        messagebox.showerror("Error", f"Invalid configuration: missing '{key}' field")
                        return
                
                # Save to configs directory
                from tkinter import simpledialog
                filename = simpledialog.askstring(
                    "Save As", 
                    f"Save as:", 
                    initialvalue=f"{config_data['name'].lower().replace(' ', '_')}_config"
                )
                
                if filename:
                    self.agent.save_config_to_file(config_data, filename)
                    self.refresh_config_list()
                    
                    if messagebox.askyesno("Use Now", "Configuration imported! Use it now?"):
                        self.agent.config = config_data
                        self.current_config_label.config(text=f"Name: {self.agent.config['name']}")
                        self.refresh_conversation_starters()  # Refresh starters and header for new config
                        
            except Exception as e:
                messagebox.showerror("Error", f"Error importing configuration: {e}")
    
    def import_bundle_gui(self):
        """Import configuration bundle (GUI)"""
        from tkinter import filedialog
        file_path = filedialog.askopenfilename(
            title="Import Configuration Bundle",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if file_path:
            import json
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    bundle = json.load(f)
                
                imported = 0
                for name, config_data in bundle.items():
                    try:
                        # Validate
                        required_keys = ['name', 'description', 'instructions', 'conversation_starters']
                        if all(key in config_data for key in required_keys):
                            self.agent.save_config_to_file(config_data, name)
                            imported += 1
                    except Exception as e:
                        print(f"Error importing {name}: {e}")
                
                self.refresh_config_list()
                messagebox.showinfo("Success", f"Imported {imported} configurations")
                
            except Exception as e:
                messagebox.showerror("Error", f"Error importing bundle: {e}")
    
    def duplicate_current_gui(self):
        """Duplicate current configuration (GUI)"""
        from tkinter import simpledialog
        new_name = simpledialog.askstring(
            "Duplicate Config", 
            "New name for duplicate:", 
            initialvalue=f"{self.agent.config['name']} Copy"
        )
        
        if new_name:
            filename = simpledialog.askstring(
                "Save As", 
                "Save as:", 
                initialvalue=f"{new_name.lower().replace(' ', '_')}_config"
            )
            
            if filename:
                new_config = self.agent.config.copy()
                new_config['name'] = new_name
                
                self.agent.save_config_to_file(new_config, filename)
                self.refresh_config_list()
                messagebox.showinfo("Success", f"Configuration duplicated as {filename}")
    
    def reset_to_default_gui(self):
        """Reset to default configuration (GUI)"""
        if messagebox.askyesno("Reset Config", "Reset to default configuration? Current changes will be lost."):
            default_config = {
                "name": "Custom AI Agent",
                "description": "A helpful AI assistant that can be customized for specific tasks and domains.",
                "instructions": "You are a helpful AI assistant. Please provide clear, concise, and accurate responses.",
                "conversation_starters": [
                    "How can I help you today?",
                    "What would you like to work on?",
                    "Tell me about your project and I'll assist you.",
                    "What questions do you have for me?"
                ]
            }
            
            self.agent.config = default_config
            self.current_config_label.config(text=f"Name: {self.agent.config['name']}")
            self.refresh_conversation_starters()  # Refresh starters and header for new config
            messagebox.showinfo("Success", "Configuration reset to default")
    
    def run_enhanced_cli(self):
        """Enhanced command-line interface with better formatting"""
        print("\n" + "🤖 " + "=" * 58)
        print(f"   {self.agent.config['name'].upper()}")
        print("=" * 60)
        print(f"📝 {self.agent.config['description']}")
        print()
        
        if self.agent.config['conversation_starters']:
            print("💬 Conversation starters:")
            for i, starter in enumerate(self.agent.config['conversation_starters'], 1):
                print(f"   {i}. {starter}")
            print()
        
        print("Commands: 'quit' to exit, 'config' for settings, 'history' for chat history")
        print("Press Ctrl+C to exit anytime")
        print("-" * 60)
        
        while True:
            try:
                print("\n🧑 You:", end=" ")
                user_input = input().strip()
                
                if user_input.lower() == 'quit':
                    print("\n👋 Goodbye!")
                    break
                elif user_input.lower() == 'config':
                    self.agent.config_menu()
                    continue
                elif user_input.lower() == 'history':
                    self.agent.show_history()
                    continue
                elif not user_input:
                    continue
                
                print(f"\n🤖 {self.agent.config['name']}: ", end="")
                print("Thinking...", end="", flush=True)
                response = self.agent.send_to_claude(user_input)
                print(f"\r🤖 {self.agent.config['name']}: {response}")
                
            except KeyboardInterrupt:
                print("\n\n👋 Goodbye!")
                break
            except Exception as e:
                print(f"\n❌ Error: {e}")
    
    def run(self):
        """Run the GUI"""
        if TKINTER_AVAILABLE:
            self.root.mainloop()

def main():
    if len(sys.argv) > 1:
        if sys.argv[1] in ["--help", "-h"]:
            print("Custom AI Agent GUI for Claude CLI")
            print("Usage:")
            print("  python3 claude_agent_gui.py                # Start GUI")
            print("  python3 claude_agent_gui.py --help         # Show this help")
            return
    
    gui = ClaudeAgentGUI()
    gui.run()

if __name__ == "__main__":
    main()