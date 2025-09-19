#include "config_dialog.h"
#include <iostream>

ConfigDialog::ConfigDialog(Gtk::Window& parent, ClaudeAgent& agent)
    : Gtk::Dialog("Agent Configuration", parent, true)
    , agent_(agent)
    , basic_box_(Gtk::ORIENTATION_VERTICAL)
    , inst_box_(Gtk::ORIENTATION_VERTICAL)
    , name_label_("Agent Name:")
    , desc_label_("Description:")
    , button_box_(Gtk::ORIENTATION_HORIZONTAL)
    , save_button_("Save")
    , save_as_button_("Save As...")
    , load_button_("Load...")
    , cancel_button_("Cancel") {

    setupUi();
}

void ConfigDialog::setupUi() {
    set_default_size(DIALOG_WIDTH, DIALOG_HEIGHT);

    auto content_area = get_content_area();

    // Create notebook
    content_area->pack_start(notebook_, Gtk::PACK_EXPAND_WIDGET, 10);

    // Basic settings tab
    basic_box_.set_spacing(10);
    basic_box_.set_margin_left(10);
    basic_box_.set_margin_right(10);
    basic_box_.set_margin_top(10);
    basic_box_.set_margin_bottom(10);

    basic_box_.pack_start(name_label_, Gtk::PACK_SHRINK);
    basic_box_.pack_start(name_entry_, Gtk::PACK_SHRINK);

    basic_box_.pack_start(desc_label_, Gtk::PACK_SHRINK);

    desc_buffer_ = Gtk::TextBuffer::create();
    desc_text_.set_buffer(desc_buffer_);
    desc_text_.set_wrap_mode(Gtk::WRAP_WORD);

    desc_scroll_.add(desc_text_);
    desc_scroll_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    desc_scroll_.set_min_content_height(100);

    basic_box_.pack_start(desc_scroll_, Gtk::PACK_EXPAND_WIDGET);

    notebook_.append_page(basic_box_, "Basic Settings");

    // Instructions tab
    inst_box_.set_spacing(10);
    inst_box_.set_margin_left(10);
    inst_box_.set_margin_right(10);
    inst_box_.set_margin_top(10);
    inst_box_.set_margin_bottom(10);

    inst_buffer_ = Gtk::TextBuffer::create();
    inst_text_.set_buffer(inst_buffer_);
    inst_text_.set_wrap_mode(Gtk::WRAP_WORD);

    inst_scroll_.add(inst_text_);
    inst_scroll_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    inst_box_.pack_start(inst_scroll_, Gtk::PACK_EXPAND_WIDGET);

    notebook_.append_page(inst_box_, "Instructions");

    // Button area
    button_box_.set_spacing(10);
    button_box_.set_margin_left(10);
    button_box_.set_margin_right(10);
    button_box_.set_margin_top(10);
    button_box_.set_margin_bottom(10);

    save_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigDialog::onSaveClicked));
    save_as_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigDialog::onSaveAsClicked));
    load_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigDialog::onLoadClicked));
    cancel_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigDialog::onCancelClicked));

    button_box_.pack_start(save_button_, Gtk::PACK_SHRINK);
    button_box_.pack_start(save_as_button_, Gtk::PACK_SHRINK);
    button_box_.pack_start(load_button_, Gtk::PACK_SHRINK);
    button_box_.pack_start(cancel_button_, Gtk::PACK_SHRINK);

    content_area->pack_start(button_box_, Gtk::PACK_SHRINK);

    show_all_children();
}

void ConfigDialog::showDialog() {
    loadCurrentConfig();
    show();
}

void ConfigDialog::loadCurrentConfig() {
    name_entry_.set_text(agent_.getName());
    desc_buffer_->set_text(agent_.getDescription());
    inst_buffer_->set_text(agent_.getInstructions());
}

void ConfigDialog::onSaveClicked() {
    // Update agent configuration
    agent_.setName(name_entry_.get_text());

    auto desc_start = desc_buffer_->begin();
    auto desc_end = desc_buffer_->end();
    agent_.setDescription(desc_buffer_->get_text(desc_start, desc_end));

    auto inst_start = inst_buffer_->begin();
    auto inst_end = inst_buffer_->end();
    agent_.setInstructions(inst_buffer_->get_text(inst_start, inst_end));

    // Save to file
    if (agent_.saveConfig()) {
        auto dialog = Gtk::MessageDialog(*this, "Configuration saved!",
                                        false, Gtk::MESSAGE_INFO);
        dialog.run();
        hide();
    } else {
        auto dialog = Gtk::MessageDialog(*this, "Error saving configuration!",
                                        false, Gtk::MESSAGE_ERROR);
        dialog.run();
    }
}

void ConfigDialog::onSaveAsClicked() {
    saveConfigAs();
}

void ConfigDialog::onLoadClicked() {
    loadConfigFrom();
}

void ConfigDialog::onCancelClicked() {
    hide();
}

void ConfigDialog::saveConfigAs() {
    auto dialog = Gtk::FileChooserDialog("Save Configuration As",
                                        Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Save", Gtk::RESPONSE_OK);

    auto filter_json = Gtk::FileFilter::create();
    filter_json->set_name("JSON files");
    filter_json->add_pattern("*.json");
    dialog.add_filter(filter_json);

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK) {
        std::string filename = dialog.get_filename();

        // Update current config with dialog values
        agent_.setName(name_entry_.get_text());

        auto desc_start = desc_buffer_->begin();
        auto desc_end = desc_buffer_->end();
        agent_.setDescription(desc_buffer_->get_text(desc_start, desc_end));

        auto inst_start = inst_buffer_->begin();
        auto inst_end = inst_buffer_->end();
        agent_.setInstructions(inst_buffer_->get_text(inst_start, inst_end));

        // Save to specified file
        if (agent_.saveConfigToFile(agent_.getConfig(), filename)) {
            auto success_dialog = Gtk::MessageDialog(*this,
                "Configuration saved as " + filename,
                false, Gtk::MESSAGE_INFO);
            success_dialog.run();
        } else {
            auto error_dialog = Gtk::MessageDialog(*this,
                "Error saving configuration!",
                false, Gtk::MESSAGE_ERROR);
            error_dialog.run();
        }
    }
}

void ConfigDialog::loadConfigFrom() {
    auto dialog = Gtk::FileChooserDialog("Load Configuration",
                                        Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    auto filter_json = Gtk::FileFilter::create();
    filter_json->set_name("JSON files");
    filter_json->add_pattern("*.json");
    dialog.add_filter(filter_json);

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK) {
        std::string filename = dialog.get_filename();

        if (agent_.loadSpecificConfig(filename)) {
            auto success_dialog = Gtk::MessageDialog(*this,
                "Configuration loaded: " + agent_.getName(),
                false, Gtk::MESSAGE_INFO);
            success_dialog.run();

            // Refresh dialog with new config
            loadCurrentConfig();
            hide();
        } else {
            auto error_dialog = Gtk::MessageDialog(*this,
                "Error loading configuration!",
                false, Gtk::MESSAGE_ERROR);
            error_dialog.run();
        }
    }
}