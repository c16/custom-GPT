#pragma once

#include <gtkmm.h>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include "claude_agent.h"

class ConfigDialog;
class ConfigLibraryDialog;

class ClaudeAgentGUI : public Gtk::Window {
public:
    ClaudeAgentGUI();
    ~ClaudeAgentGUI();

protected:
    // UI setup
    void setupUi();
    void setupStyles();
    void setupHeaderArea();
    void setupChatArea();
    void setupInputArea();
    void setupConversationStarters();

    // Event handlers
    void onSendMessage();
    void onHistoryClicked();
    void onConfigClicked();
    void onLibraryClicked();
    void onCopyAllClicked();
    void onClearClicked();
    void onCliProviderChanged();
    void onStarterClicked(const std::string& starter);
    bool onKeyPressed(GdkEventKey* event);

    // Message handling
    void addMessage(const std::string& sender, const std::string& message);
    void showThinkingMessage();
    void removeThinkingMessage();

    // Threading
    void sendMessageBackground(const std::string& message);
    bool checkResponseQueue();

    // Configuration management
    void refreshConversationStarters();
    void updateHeader();
    void refreshInterface();

    // Dialog management
    void showHistoryDialog();

private:
    // Core components
    std::unique_ptr<ClaudeAgent> agent_;

    // UI components
    Gtk::Box main_box_;
    Gtk::Box header_box_;
    Gtk::Box chat_box_;
    Gtk::Box input_box_;
    Gtk::Box starter_box_;

    // Header widgets
    Gtk::Label name_label_;
    Gtk::Label description_label_;
    Gtk::Label cli_label_;
    Gtk::ComboBoxText cli_combo_;
    Gtk::Button config_button_;
    Gtk::Button library_button_;
    Gtk::Button copy_button_;
    Gtk::Button clear_button_;

    // Chat widgets
    Gtk::ScrolledWindow chat_scroll_;
    Gtk::TextView chat_display_;
    Glib::RefPtr<Gtk::TextBuffer> chat_buffer_;

    // Input widgets
    Gtk::ScrolledWindow input_scroll_;
    Gtk::TextView input_text_;
    Glib::RefPtr<Gtk::TextBuffer> input_buffer_;
    Gtk::Box button_box_;
    Gtk::Button send_button_;
    Gtk::Button history_button_;

    // Conversation starters
    Gtk::Frame starters_frame_;
    std::vector<std::unique_ptr<Gtk::Button>> starter_buttons_;

    // Threading components
    std::queue<std::string> response_queue_;
    std::mutex queue_mutex_;
    std::atomic<bool> processing_message_;
    sigc::connection timer_connection_;

    // Dialog management
    std::unique_ptr<ConfigDialog> config_dialog_;
    std::unique_ptr<ConfigLibraryDialog> library_dialog_;

    // Constants
    static constexpr int WINDOW_WIDTH = 1600;
    static constexpr int WINDOW_HEIGHT = 1200;
    static constexpr int INPUT_HEIGHT = 100;
    static constexpr int TIMER_INTERVAL = 100; // milliseconds
};