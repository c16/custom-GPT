#include "claude_agent_gui.h"
#include "config_dialog.h"
#include "config_library_dialog.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <iomanip>
#include <sstream>

ClaudeAgentGUI::ClaudeAgentGUI()
    : main_box_(Gtk::ORIENTATION_VERTICAL)
    , header_box_(Gtk::ORIENTATION_HORIZONTAL)
    , chat_box_(Gtk::ORIENTATION_VERTICAL)
    , input_box_(Gtk::ORIENTATION_HORIZONTAL)
    , starter_box_(Gtk::ORIENTATION_VERTICAL)
    , cli_label_("CLI:")
    , config_button_("Config")
    , library_button_("Library")
    , copy_button_("Copy All")
    , clear_button_("Clear")
    , button_box_(Gtk::ORIENTATION_VERTICAL)
    , send_button_("Send")
    , history_button_("History")
    , starters_frame_("Conversation Starters")
    , processing_message_(false) {

    LOG_INFO("Initializing ClaudeAgentGUI");

    // Initialize UI first, then agent in background
    setupUi();
    setupStyles();

    LOG_INFO("GUI setup complete, initializing agent...");
    agent_ = std::make_unique<ClaudeAgent>();

    LOG_INFO("Agent created, now initializing CLI...");
    agent_->initializeCli();

    refreshInterface();

    LOG_INFO("ClaudeAgentGUI initialization complete");

    // Start response queue timer
    timer_connection_ = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &ClaudeAgentGUI::checkResponseQueue),
        TIMER_INTERVAL
    );
}

ClaudeAgentGUI::~ClaudeAgentGUI() {
    if (timer_connection_.connected()) {
        timer_connection_.disconnect();
    }
}

void ClaudeAgentGUI::setupUi() {
    std::string title = "Claude Agent";
    if (agent_) {
        title = agent_->getName() + " - Claude CLI Agent";
    }
    set_title(title);
    set_default_size(WINDOW_WIDTH, WINDOW_HEIGHT);

    add(main_box_);

    setupHeaderArea();
    setupChatArea();
    setupInputArea();
    setupConversationStarters();

    main_box_.pack_start(header_box_, Gtk::PACK_SHRINK, 10);
    main_box_.pack_start(chat_box_, Gtk::PACK_EXPAND_WIDGET, 10);
    main_box_.pack_start(input_box_, Gtk::PACK_SHRINK, 10);
    main_box_.pack_start(starters_frame_, Gtk::PACK_SHRINK, 10);

    show_all_children();

    // Add welcome message
    addMessage("System", "Welcome! How can I help you today?");
}

void ClaudeAgentGUI::setupStyles() {
    auto css = Gtk::CssProvider::create();
    css->load_from_data(R"(
        window {
            background-color: #2c3e50;
        }
        .chat-display {
            background-color: #34495e;
            color: #ecf0f1;
            font-family: Arial;
            font-size: 12px;
        }
        .input-text {
            background-color: #ecf0f1;
            color: #2c3e50;
            font-family: Arial;
            font-size: 12px;
        }
        .header-label {
            color: #ecf0f1;
            font-weight: bold;
        }
        .description-label {
            color: #bdc3c7;
        }
    )");

    auto screen = Gdk::Screen::get_default();
    auto context = get_style_context();
    context->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void ClaudeAgentGUI::setupHeaderArea() {
    // Name label
    std::string name = agent_ ? agent_->getName() : "Claude Agent";
    name_label_.set_text(name);
    name_label_.get_style_context()->add_class("header-label");
    header_box_.pack_start(name_label_, Gtk::PACK_SHRINK, 5);

    // Spacer
    auto spacer = Gtk::manage(new Gtk::Label(""));
    header_box_.pack_start(*spacer, Gtk::PACK_EXPAND_WIDGET, 5);

    // CLI provider combo
    header_box_.pack_start(cli_label_, Gtk::PACK_SHRINK, 5);
    cli_combo_.append("auto");
    cli_combo_.append("claude");
    cli_combo_.append("gemini");
    std::string provider = agent_ ? agent_->getActiveProviderName() : "auto";
    cli_combo_.set_active_text(provider);
    cli_combo_.signal_changed().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onCliProviderChanged));
    header_box_.pack_start(cli_combo_, Gtk::PACK_SHRINK, 5);

    // Buttons
    config_button_.signal_clicked().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onConfigClicked));
    library_button_.signal_clicked().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onLibraryClicked));
    copy_button_.signal_clicked().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onCopyAllClicked));
    clear_button_.signal_clicked().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onClearClicked));

    header_box_.pack_start(config_button_, Gtk::PACK_SHRINK, 5);
    header_box_.pack_start(library_button_, Gtk::PACK_SHRINK, 5);
    header_box_.pack_start(copy_button_, Gtk::PACK_SHRINK, 5);
    header_box_.pack_start(clear_button_, Gtk::PACK_SHRINK, 5);

    // Description label (on second row)
    std::string description = agent_ ? agent_->getDescription() : "A helpful AI assistant";
    description_label_.set_text(description);
    description_label_.set_line_wrap(true);
    description_label_.set_max_width_chars(80);
    description_label_.get_style_context()->add_class("description-label");
    main_box_.pack_start(description_label_, Gtk::PACK_SHRINK, 5);
}

void ClaudeAgentGUI::setupChatArea() {
    chat_buffer_ = Gtk::TextBuffer::create();
    chat_display_.set_buffer(chat_buffer_);
    chat_display_.set_editable(false);
    chat_display_.set_cursor_visible(false);
    chat_display_.set_wrap_mode(Gtk::WRAP_WORD);
    chat_display_.get_style_context()->add_class("chat-display");

    chat_scroll_.add(chat_display_);
    chat_scroll_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    chat_scroll_.set_min_content_height(400);

    chat_box_.pack_start(chat_scroll_, Gtk::PACK_EXPAND_WIDGET);
}

void ClaudeAgentGUI::setupInputArea() {
    input_buffer_ = Gtk::TextBuffer::create();
    input_text_.set_buffer(input_buffer_);
    input_text_.get_style_context()->add_class("input-text");
    input_text_.signal_key_press_event().connect(
        sigc::mem_fun(*this, &ClaudeAgentGUI::onKeyPressed), false);

    input_scroll_.add(input_text_);
    input_scroll_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    input_scroll_.set_min_content_height(INPUT_HEIGHT);

    // Buttons
    send_button_.signal_clicked().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onSendMessage));
    history_button_.signal_clicked().connect(sigc::mem_fun(*this, &ClaudeAgentGUI::onHistoryClicked));

    button_box_.pack_start(send_button_, Gtk::PACK_SHRINK, 2);
    button_box_.pack_start(history_button_, Gtk::PACK_SHRINK, 2);

    input_box_.pack_start(input_scroll_, Gtk::PACK_EXPAND_WIDGET, 5);
    input_box_.pack_start(button_box_, Gtk::PACK_SHRINK, 5);
}

void ClaudeAgentGUI::setupConversationStarters() {
    refreshConversationStarters();
}

void ClaudeAgentGUI::refreshConversationStarters() {
    // Clear existing buttons
    starter_buttons_.clear();
    auto children = starter_box_.get_children();
    for (auto child : children) {
        starter_box_.remove(*child);
    }

    if (!agent_) {
        auto label = Gtk::manage(new Gtk::Label("Agent not initialized"));
        starter_box_.pack_start(*label, Gtk::PACK_SHRINK, 2);
        starters_frame_.add(starter_box_);
        show_all_children();
        return;
    }

    // Add new starter buttons
    auto starters = agent_->getConversationStarters();
    if (!starters.empty()) {
        for (const auto& starter : starters) {
            auto button = std::make_unique<Gtk::Button>(starter);
            button->signal_clicked().connect(
                sigc::bind(sigc::mem_fun(*this, &ClaudeAgentGUI::onStarterClicked), starter));
            starter_box_.pack_start(*button, Gtk::PACK_SHRINK, 2);
            starter_buttons_.push_back(std::move(button));
        }
    } else {
        auto label = Gtk::manage(new Gtk::Label("No conversation starters available"));
        starter_box_.pack_start(*label, Gtk::PACK_SHRINK, 2);
    }

    starters_frame_.add(starter_box_);
    show_all_children();
}

void ClaudeAgentGUI::updateHeader() {
    if (!agent_) {
        name_label_.set_text("No Agent");
        description_label_.set_text("Agent not initialized");
        set_title("Claude Agent GUI - Testing");
        return;
    }
    name_label_.set_text(agent_->getName());
    description_label_.set_text(agent_->getDescription());
    std::string provider_name = agent_->getActiveProviderName();
    std::transform(provider_name.begin(), provider_name.end(), provider_name.begin(), ::toupper);
    set_title(agent_->getName() + " - " + provider_name + " Agent");
}

void ClaudeAgentGUI::refreshInterface() {
    updateHeader();
    refreshConversationStarters();
}

void ClaudeAgentGUI::onSendMessage() {
    if (processing_message_.load()) {
        LOG_DEBUG("Ignoring send request - already processing a message");
        return; // Already processing a message
    }

    auto start_iter = input_buffer_->begin();
    auto end_iter = input_buffer_->end();
    std::string user_message = input_buffer_->get_text(start_iter, end_iter);

    if (user_message.empty()) {
        LOG_DEBUG("Ignoring empty message");
        return;
    }

    LOG_INFO("User sending message (length: " + std::to_string(user_message.length()) + " chars)");
    LOG_DEBUG("User message preview: " + user_message.substr(0, 100) + (user_message.length() > 100 ? "..." : ""));

    // Clear input
    input_buffer_->set_text("");

    // Add user message to chat
    addMessage("You", user_message);

    // Show thinking message
    showThinkingMessage();

    // Send to Claude in background thread
    processing_message_.store(true);
    LOG_DEBUG("Starting background thread for message processing");
    std::thread worker(&ClaudeAgentGUI::sendMessageBackground, this, user_message);
    worker.detach();
}

void ClaudeAgentGUI::onHistoryClicked() {
    showHistoryDialog();
}

void ClaudeAgentGUI::onConfigClicked() {
    if (!config_dialog_) {
        config_dialog_ = std::make_unique<ConfigDialog>(*this, *agent_);
    }
    config_dialog_->showDialog();
    refreshInterface(); // Refresh in case config changed
}

void ClaudeAgentGUI::onLibraryClicked() {
    if (!library_dialog_) {
        library_dialog_ = std::make_unique<ConfigLibraryDialog>(*this, *agent_);
    }
    library_dialog_->showDialog();
    refreshInterface(); // Refresh in case config changed
}

void ClaudeAgentGUI::onCopyAllClicked() {
    auto start_iter = chat_buffer_->begin();
    auto end_iter = chat_buffer_->end();
    std::string chat_content = chat_buffer_->get_text(start_iter, end_iter);

    auto clipboard = Gtk::Clipboard::get();
    clipboard->set_text(chat_content);

    // Show brief notification (could be improved with a proper notification)
    addMessage("System", "Chat content copied to clipboard!");
}

void ClaudeAgentGUI::onClearClicked() {
    chat_buffer_->set_text("");
    agent_->clearConversationHistory();
    addMessage("System", "Chat cleared. How can I help you?");
}

void ClaudeAgentGUI::onCliProviderChanged() {
    std::string new_provider = cli_combo_.get_active_text();
    CliProvider provider = CliProvider::AUTO;

    if (new_provider == "claude") provider = CliProvider::CLAUDE;
    else if (new_provider == "gemini") provider = CliProvider::GEMINI;

    bool success = agent_->switchCliProvider(provider);
    updateHeader();

    if (success) {
        std::string provider_name = agent_->getActiveProviderName();
        std::transform(provider_name.begin(), provider_name.end(), provider_name.begin(), ::toupper);
        addMessage("System", "Switched to " + provider_name + " CLI");
    } else {
        addMessage("System", "Warning: " + new_provider + " CLI not found");
    }
}

void ClaudeAgentGUI::onStarterClicked(const std::string& starter) {
    input_buffer_->set_text(starter);
    input_text_.grab_focus();
}

bool ClaudeAgentGUI::onKeyPressed(GdkEventKey* event) {
    if (event->keyval == GDK_KEY_Return && (event->state & GDK_CONTROL_MASK)) {
        onSendMessage();
        return true;
    }
    return false;
}

void ClaudeAgentGUI::addMessage(const std::string& sender, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);

    std::ostringstream timestamp;
    timestamp << std::put_time(&tm, "%H:%M:%S");

    std::string full_message = "[" + timestamp.str() + "] " + sender + ": " + message + "\n";

    auto end_iter = chat_buffer_->end();
    chat_buffer_->insert(end_iter, full_message);

    // Scroll to bottom
    auto mark = chat_buffer_->get_insert();
    chat_display_.scroll_to(mark);
}

void ClaudeAgentGUI::showThinkingMessage() {
    addMessage("System", "Thinking...");
}

void ClaudeAgentGUI::removeThinkingMessage() {
    auto text = chat_buffer_->get_text();
    size_t pos = text.rfind("System: Thinking...");
    if (pos != std::string::npos) {
        // Find the start of the line
        size_t line_start = text.rfind('\n', pos);
        if (line_start == std::string::npos) line_start = 0;
        else line_start++;

        // Find the end of the line
        size_t line_end = text.find('\n', pos);
        if (line_end == std::string::npos) line_end = text.length();
        else line_end++;

        auto start_iter = chat_buffer_->get_iter_at_offset(line_start);
        auto end_iter = chat_buffer_->get_iter_at_offset(line_end);
        chat_buffer_->erase(start_iter, end_iter);
    }
}

void ClaudeAgentGUI::sendMessageBackground(const std::string& message) {
    LOG_DEBUG("Background thread started for message processing");

    try {
        std::string response = agent_->sendToClaudeApi(message);

        LOG_DEBUG("Background thread received response (length: " + std::to_string(response.length()) + " chars)");

        // Add response to queue
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            response_queue_.push(response);
            LOG_DEBUG("Response added to queue for UI thread");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in background thread: " + std::string(e.what()));

        // Add error response to queue
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            response_queue_.push("Error: " + std::string(e.what()));
        }
    }
}

bool ClaudeAgentGUI::checkResponseQueue() {
    std::lock_guard<std::mutex> lock(queue_mutex_);

    if (!response_queue_.empty()) {
        std::string response = response_queue_.front();
        response_queue_.pop();

        // Remove thinking message
        removeThinkingMessage();

        // Add Claude's response
        addMessage(agent_->getName(), response);

        processing_message_.store(false);
    }

    return true; // Continue timer
}

void ClaudeAgentGUI::showHistoryDialog() {
    const auto& history = agent_->getConversationHistory();

    if (history.empty()) {
        auto dialog = Gtk::MessageDialog(*this, "No conversation history yet.",
                                        false, Gtk::MESSAGE_INFO);
        dialog.run();
        return;
    }

    auto dialog = std::make_unique<Gtk::Dialog>("Conversation History", *this);
    dialog->set_default_size(600, 400);

    auto content_area = dialog->get_content_area();

    auto scroll = Gtk::manage(new Gtk::ScrolledWindow());
    scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    auto text_view = Gtk::manage(new Gtk::TextView());
    text_view->set_editable(false);
    text_view->set_wrap_mode(Gtk::WRAP_WORD);

    auto buffer = text_view->get_buffer();

    for (const auto& entry : history) {
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto tm = *std::localtime(&time_t);

        std::ostringstream timestamp;
        timestamp << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

        std::string history_text = "[" + timestamp.str() + "]\n";
        history_text += "You: " + entry.user + "\n";
        history_text += agent_->getName() + ": " + entry.assistant + "\n";
        history_text += std::string(50, '-') + "\n\n";

        auto end_iter = buffer->end();
        buffer->insert(end_iter, history_text);
    }

    scroll->add(*text_view);
    content_area->pack_start(*scroll, Gtk::PACK_EXPAND_WIDGET, 10);

    dialog->add_button("Close", Gtk::RESPONSE_CLOSE);

    dialog->show_all_children();
    dialog->run();
}