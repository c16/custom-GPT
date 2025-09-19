#pragma once

#include <gtkmm.h>
#include <memory>
#include "claude_agent.h"

class ConfigDialog : public Gtk::Dialog {
public:
    ConfigDialog(Gtk::Window& parent, ClaudeAgent& agent);
    ~ConfigDialog() = default;

    void showDialog();

protected:
    void setupUi();
    void loadCurrentConfig();
    void onSaveClicked();
    void onSaveAsClicked();
    void onLoadClicked();
    void onCancelClicked();

private:
    ClaudeAgent& agent_;

    // UI components
    Gtk::Notebook notebook_;

    // Basic settings tab
    Gtk::Box basic_box_;
    Gtk::Label name_label_;
    Gtk::Entry name_entry_;
    Gtk::Label desc_label_;
    Gtk::ScrolledWindow desc_scroll_;
    Gtk::TextView desc_text_;
    Glib::RefPtr<Gtk::TextBuffer> desc_buffer_;

    // Instructions tab
    Gtk::Box inst_box_;
    Gtk::ScrolledWindow inst_scroll_;
    Gtk::TextView inst_text_;
    Glib::RefPtr<Gtk::TextBuffer> inst_buffer_;

    // Button area
    Gtk::ButtonBox button_box_;
    Gtk::Button save_button_;
    Gtk::Button save_as_button_;
    Gtk::Button load_button_;
    Gtk::Button cancel_button_;

    // File dialog methods
    void saveConfigAs();
    void loadConfigFrom();

    static constexpr int DIALOG_WIDTH = 600;
    static constexpr int DIALOG_HEIGHT = 500;
};