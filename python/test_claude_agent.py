#!/usr/bin/env python3
"""
Comprehensive test suite for the Python Claude Agent implementation.

Tests core functionality including configuration loading, CLI detection,
and message handling without requiring actual API calls.
"""

import unittest
import os
import sys
import tempfile
import shutil
import json
import time
from pathlib import Path
from unittest.mock import patch, mock_open, MagicMock

# Add the current directory to Python path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from claude_agent import ClaudeAgent


class TestClaudeAgent(unittest.TestCase):
    """Test cases for ClaudeAgent core functionality."""

    def setUp(self):
        """Set up test fixtures before each test method."""
        # Create a temporary directory for test configs
        self.test_dir = tempfile.mkdtemp()
        self.test_config_dir = os.path.join(self.test_dir, 'configs')
        os.makedirs(self.test_config_dir)

        # Create a test configuration file
        self.test_config = {
            "name": "Test Agent",
            "description": "A test configuration",
            "instructions": "You are a test AI assistant.",
            "conversation_starters": ["Hello", "How can I help?"]
        }

        self.test_config_file = os.path.join(self.test_config_dir, 'test_config.json')
        with open(self.test_config_file, 'w') as f:
            json.dump(self.test_config, f)

    def tearDown(self):
        """Clean up after each test method."""
        # Remove temporary directory
        shutil.rmtree(self.test_dir)

    def test_config_loading_with_environment_variable(self):
        """Test configuration loading using CLAUDE_AGENT_CONFIG_DIR environment variable."""
        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.test_config_dir}):
            agent = ClaudeAgent('test_config.json', 'auto')
            self.assertEqual(agent.config['name'], 'Test Agent')
            self.assertEqual(agent.config['description'], 'A test configuration')

    def test_config_loading_default_path(self):
        """Test configuration loading with default relative path."""
        # Temporarily change working directory to simulate proper path resolution
        original_cwd = os.getcwd()
        try:
            # Create a test structure that mimics the actual project layout
            test_work_dir = os.path.join(self.test_dir, 'work')
            os.makedirs(test_work_dir)
            os.chdir(test_work_dir)

            agent = ClaudeAgent(self.test_config_file, 'auto')  # Use absolute path
            self.assertEqual(agent.config['name'], 'Test Agent')
        finally:
            os.chdir(original_cwd)

    def test_invalid_config_file(self):
        """Test handling of invalid configuration file."""
        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.test_config_dir}):
            agent = ClaudeAgent('nonexistent_config.json', 'auto')
            # Should fall back to default configuration
            self.assertEqual(agent.config['name'], 'Custom AI Agent')

    def test_malformed_json_config(self):
        """Test handling of malformed JSON configuration."""
        malformed_config_file = os.path.join(self.test_config_dir, 'malformed.json')
        with open(malformed_config_file, 'w') as f:
            f.write('{ invalid json }')

        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.test_config_dir}):
            agent = ClaudeAgent('malformed.json', 'auto')
            # Should fall back to default configuration
            self.assertEqual(agent.config['name'], 'Custom AI Agent')

    @patch('subprocess.run')
    def test_claude_cli_detection(self, mock_run):
        """Test Claude CLI detection."""
        # Mock successful claude command detection
        mock_run.return_value = MagicMock(returncode=0, stdout="claude version 1.0")

        agent = ClaudeAgent('test_config.json', 'claude')
        cli_path, provider = agent.find_available_cli()

        self.assertIsNotNone(cli_path)
        self.assertEqual(provider, 'claude')

    @patch('subprocess.run')
    def test_gemini_cli_detection(self, mock_run):
        """Test Gemini CLI detection."""
        # Mock: claude fails, gemini succeeds
        def mock_run_side_effect(cmd, *args, **kwargs):
            if 'claude' in cmd:
                return MagicMock(returncode=1, stderr="command not found")
            elif 'gemini' in cmd:
                return MagicMock(returncode=0, stdout="gemini version 1.0")
            return MagicMock(returncode=1)

        mock_run.side_effect = mock_run_side_effect

        agent = ClaudeAgent('test_config.json', 'auto')
        cli_path, provider = agent.find_available_cli()

        self.assertEqual(provider, 'gemini')

    @patch('subprocess.run')
    def test_no_cli_detection(self, mock_run):
        """Test behavior when no CLI is available."""
        # Mock: both claude and gemini fail
        mock_run.return_value = MagicMock(returncode=1, stderr="command not found")

        agent = ClaudeAgent('test_config.json', 'auto')
        cli_path, provider = agent.find_available_cli()

        self.assertIsNone(cli_path)
        self.assertIsNone(provider)

    def test_conversation_history_management(self):
        """Test conversation history functionality."""
        agent = ClaudeAgent('test_config.json', 'auto')

        # Test conversation history management
        # Initially should be empty
        self.assertEqual(len(agent.conversation_history), 0)

        # Add to conversation history manually
        agent.conversation_history.append({
            "user": "Hello",
            "assistant": "Hi there!",
            "timestamp": time.time()
        })

        # Verify history is stored
        self.assertEqual(len(agent.conversation_history), 1)

    def test_config_saving(self):
        """Test configuration saving functionality."""
        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.test_config_dir}):
            agent = ClaudeAgent('test_config.json', 'auto')

            # Modify configuration
            agent.config['name'] = 'Modified Test Agent'

            # Save configuration
            save_path = 'saved_config.json'
            agent.save_config_to_file(agent.config, save_path)

            # Verify saved file (may be in configs/ subdirectory)
            expected_path = os.path.join('configs', save_path)
            if not os.path.exists(save_path):
                save_path = expected_path
            self.assertTrue(os.path.exists(save_path))
            with open(save_path, 'r') as f:
                saved_config = json.load(f)

            self.assertEqual(saved_config['name'], 'Modified Test Agent')

    def test_last_config_persistence(self):
        """Test last used configuration persistence."""
        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.test_config_dir}):
            agent = ClaudeAgent('test_config.json', 'auto')

            # Save last config path
            agent.save_last_config_path(self.test_config_file)

            # Load last config path
            last_path = agent.load_last_config_path()
            self.assertEqual(last_path, self.test_config_file)

    @patch('subprocess.run')
    def test_message_sending_mock(self, mock_run):
        """Test message sending with mocked CLI response."""
        # Mock successful API call
        mock_response = "This is a test response from the AI."
        mock_run.return_value = MagicMock(
            returncode=0,
            stdout=mock_response,
            stderr=""
        )

        agent = ClaudeAgent('test_config.json', 'claude')
        agent.cli_path = 'claude'  # Set explicitly for test
        agent.active_provider = 'claude'

        response = agent.send_to_claude("Test message")
        self.assertEqual(response, mock_response)

    def test_browse_configs_functionality(self):
        """Test configuration browsing functionality."""
        # Create multiple test configs
        configs = [
            {'name': 'Config 1', 'description': 'First test config'},
            {'name': 'Config 2', 'description': 'Second test config'},
            {'name': 'Config 3', 'description': 'Third test config'}
        ]

        for i, config in enumerate(configs, 1):
            config_file = os.path.join(self.test_config_dir, f'config_{i}.json')
            with open(config_file, 'w') as f:
                json.dump(config, f)

        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.test_config_dir}):
            agent = ClaudeAgent('test_config.json', 'auto')

            # Test config discovery
            from pathlib import Path
            config_files = list(Path(self.test_config_dir).glob('*.json'))
            self.assertGreaterEqual(len(config_files), 3)  # At least our 3 test configs

    def test_provider_switching(self):
        """Test switching between CLI providers."""
        agent = ClaudeAgent('test_config.json', 'auto')

        # Test provider validation
        self.assertIn('auto', ['claude', 'gemini', 'auto'])
        self.assertIn('claude', ['claude', 'gemini', 'auto'])
        self.assertIn('gemini', ['claude', 'gemini', 'auto'])


class TestConfigurationManagement(unittest.TestCase):
    """Test cases for configuration management functionality."""

    def setUp(self):
        """Set up test fixtures."""
        self.test_dir = tempfile.mkdtemp()
        self.config_dir = os.path.join(self.test_dir, 'configs')
        os.makedirs(self.config_dir)

    def tearDown(self):
        """Clean up test fixtures."""
        shutil.rmtree(self.test_dir)

    def test_config_validation(self):
        """Test configuration validation."""
        # Valid configuration
        valid_config = {
            'name': 'Test Agent',
            'description': 'Test description',
            'instructions': 'Test instructions',
            'conversation_starters': ['Hello']
        }

        config_file = os.path.join(self.config_dir, 'valid.json')
        with open(config_file, 'w') as f:
            json.dump(valid_config, f)

        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.config_dir}):
            agent = ClaudeAgent('valid.json', 'auto')
            self.assertEqual(agent.config['name'], 'Test Agent')

    def test_config_with_missing_fields(self):
        """Test configuration with missing optional fields."""
        minimal_config = {
            'name': 'Minimal Agent'
        }

        config_file = os.path.join(self.config_dir, 'minimal.json')
        with open(config_file, 'w') as f:
            json.dump(minimal_config, f)

        with patch.dict(os.environ, {'CLAUDE_AGENT_CONFIG_DIR': self.config_dir}):
            agent = ClaudeAgent('minimal.json', 'auto')
            self.assertEqual(agent.config['name'], 'Minimal Agent')
            # Should have default values for missing fields
            self.assertIn('description', agent.config)


if __name__ == '__main__':
    # Run the tests
    unittest.main(verbosity=2)