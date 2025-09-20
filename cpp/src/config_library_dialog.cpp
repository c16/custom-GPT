#include "config_library_dialog.h"
#include "logger.h"
#include <iostream>
#include <filesystem>

ConfigLibraryDialog::ConfigLibraryDialog(Gtk::Window& parent, ClaudeAgent& agent)
    : Gtk::Dialog("Configuration Library", parent, true)
    , agent_(agent)
    , browse_box_(Gtk::ORIENTATION_VERTICAL)
    , templates_box_(Gtk::ORIENTATION_VERTICAL)
    , import_export_box_(Gtk::ORIENTATION_VERTICAL)
    , management_box_(Gtk::ORIENTATION_VERTICAL)
    , browse_buttons_(Gtk::ORIENTATION_HORIZONTAL)
    , export_box_(Gtk::ORIENTATION_HORIZONTAL)
    , import_box_(Gtk::ORIENTATION_HORIZONTAL)
    , actions_box_(Gtk::ORIENTATION_HORIZONTAL)
    , refresh_button_("Refresh")
    , load_button_("Load Selected")
    , preview_button_("Preview")
    , delete_button_("Delete")
    , export_frame_("Export")
    , import_frame_("Import")
    , current_frame_("Current Configuration")
    , actions_frame_("Actions") {

    setupUi();
}

void ConfigLibraryDialog::setupUi() {
    set_default_size(DIALOG_WIDTH, DIALOG_HEIGHT);

    auto content_area = get_content_area();
    content_area->pack_start(notebook_, Gtk::PACK_EXPAND_WIDGET, 10);

    setupBrowseTab();
    setupTemplatesTab();
    setupImportExportTab();
    setupManagementTab();

    show_all_children();
}

void ConfigLibraryDialog::setupBrowseTab() {
    browse_box_.set_spacing(10);
    browse_box_.set_margin_left(10);
    browse_box_.set_margin_right(10);
    browse_box_.set_margin_top(10);
    browse_box_.set_margin_bottom(10);

    // Create list store and tree view
    config_store_ = Gtk::ListStore::create(config_columns_);
    config_tree_.set_model(config_store_);

    config_tree_.append_column("Name", config_columns_.name);
    config_tree_.append_column("Description", config_columns_.description);
    config_tree_.append_column("File", config_columns_.filename);

    browse_scroll_.add(config_tree_);
    browse_scroll_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    browse_scroll_.set_min_content_height(300);

    browse_box_.pack_start(browse_scroll_, Gtk::PACK_EXPAND_WIDGET);

    // Buttons
    browse_buttons_.set_spacing(5);
    refresh_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onRefreshClicked));
    load_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onLoadSelectedClicked));
    preview_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onPreviewClicked));
    delete_button_.signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onDeleteClicked));

    browse_buttons_.pack_start(refresh_button_, Gtk::PACK_SHRINK);
    browse_buttons_.pack_start(load_button_, Gtk::PACK_SHRINK);
    browse_buttons_.pack_start(preview_button_, Gtk::PACK_SHRINK);
    browse_buttons_.pack_start(delete_button_, Gtk::PACK_SHRINK);

    browse_box_.pack_start(browse_buttons_, Gtk::PACK_SHRINK);

    notebook_.append_page(browse_box_, "Browse");

    // Load initial data
    refreshConfigList();
}

void ConfigLibraryDialog::setupTemplatesTab() {
    templates_box_.set_spacing(10);
    templates_box_.set_margin_left(10);
    templates_box_.set_margin_right(10);
    templates_box_.set_margin_top(10);
    templates_box_.set_margin_bottom(10);

    auto title = Gtk::manage(new Gtk::Label("Create new configuration from template:"));
    title->set_markup("<b>Create new configuration from template:</b>");
    templates_box_.pack_start(*title, Gtk::PACK_SHRINK);

    // Template definitions
    std::vector<std::pair<std::string, std::string>> templates = {
        {"General Assistant", "A helpful general-purpose AI assistant"},
        {"Code Assistant", "Programming and development helper"},
        {"Learning Tutor", "Patient educational assistant"},
        {"Writing Assistant", "Writing and editing helper"},
        {"Research Assistant", "Research and analysis helper"},
        {"Creative Assistant", "Creative writing and brainstorming helper"}
    };

    for (const auto& [name, desc] : templates) {
        auto frame = Gtk::manage(new Gtk::Frame(name));
        frame->set_margin_top(5);
        frame->set_margin_bottom(5);

        auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
        box->set_spacing(10);
        box->set_margin_left(10);
        box->set_margin_right(10);
        box->set_margin_top(5);
        box->set_margin_bottom(5);

        auto label = Gtk::manage(new Gtk::Label(desc));
        auto button = Gtk::manage(new Gtk::Button("Create"));
        button->signal_clicked().connect(
            sigc::bind(sigc::mem_fun(*this, &ConfigLibraryDialog::onCreateTemplateClicked), name));

        box->pack_start(*label, Gtk::PACK_EXPAND_WIDGET);
        box->pack_start(*button, Gtk::PACK_SHRINK);

        frame->add(*box);
        templates_box_.pack_start(*frame, Gtk::PACK_SHRINK);
    }

    notebook_.append_page(templates_box_, "Templates");
}

void ConfigLibraryDialog::setupImportExportTab() {
    import_export_box_.set_spacing(10);
    import_export_box_.set_margin_left(10);
    import_export_box_.set_margin_right(10);
    import_export_box_.set_margin_top(10);
    import_export_box_.set_margin_bottom(10);

    // Export section
    export_box_.set_spacing(5);

    auto export_current = Gtk::manage(new Gtk::Button("Export Current Config"));
    auto export_all = Gtk::manage(new Gtk::Button("Export All Configs"));

    export_current->signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onExportCurrentClicked));
    export_all->signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onExportAllClicked));

    export_box_.pack_start(*export_current, Gtk::PACK_SHRINK);
    export_box_.pack_start(*export_all, Gtk::PACK_SHRINK);

    export_frame_.add(export_box_);
    import_export_box_.pack_start(export_frame_, Gtk::PACK_SHRINK);

    // Import section
    import_box_.set_spacing(5);

    auto import_config = Gtk::manage(new Gtk::Button("Import Config File"));
    auto import_bundle = Gtk::manage(new Gtk::Button("Import Config Bundle"));

    import_config->signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onImportConfigClicked));
    import_bundle->signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onImportBundleClicked));

    import_box_.pack_start(*import_config, Gtk::PACK_SHRINK);
    import_box_.pack_start(*import_bundle, Gtk::PACK_SHRINK);

    import_frame_.add(import_box_);
    import_export_box_.pack_start(import_frame_, Gtk::PACK_SHRINK);

    notebook_.append_page(import_export_box_, "Import/Export");
}

void ConfigLibraryDialog::setupManagementTab() {
    management_box_.set_spacing(10);
    management_box_.set_margin_left(10);
    management_box_.set_margin_right(10);
    management_box_.set_margin_top(10);
    management_box_.set_margin_bottom(10);

    auto title = Gtk::manage(new Gtk::Label("Configuration Management"));
    title->set_markup("<b>Configuration Management</b>");
    management_box_.pack_start(*title, Gtk::PACK_SHRINK);

    // Current config info
    current_label_.set_text("Name: " + agent_.getName());
    current_frame_.add(current_label_);
    management_box_.pack_start(current_frame_, Gtk::PACK_SHRINK);

    // Management buttons
    actions_box_.set_spacing(5);

    auto duplicate_button = Gtk::manage(new Gtk::Button("Duplicate Current"));
    auto reset_button = Gtk::manage(new Gtk::Button("Reset to Default"));

    duplicate_button->signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onDuplicateCurrentClicked));
    reset_button->signal_clicked().connect(sigc::mem_fun(*this, &ConfigLibraryDialog::onResetToDefaultClicked));

    actions_box_.pack_start(*duplicate_button, Gtk::PACK_SHRINK);
    actions_box_.pack_start(*reset_button, Gtk::PACK_SHRINK);

    actions_frame_.add(actions_box_);
    management_box_.pack_start(actions_frame_, Gtk::PACK_SHRINK);

    notebook_.append_page(management_box_, "Manage");
}

void ConfigLibraryDialog::showDialog() {
    LOG_DEBUG("ConfigLibraryDialog::showDialog() called");
    refreshConfigList();
    current_label_.set_text("Name: " + agent_.getName());
    show();
}

void ConfigLibraryDialog::refreshConfigList() {
    config_store_->clear();

    // Collect config files
    std::vector<std::filesystem::path> config_files;

    // Get config directory from environment or use default
    const char* config_dir_env = std::getenv("CLAUDE_AGENT_CONFIG_DIR");
    std::string config_dir = config_dir_env ? config_dir_env : "../configs";

    LOG_DEBUG("ConfigLibraryDialog: Scanning for configs in directory: " + config_dir);

    // Current directory (legacy support)
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.path().extension() == ".json" &&
            entry.path().filename().string().find("_config") != std::string::npos) {
            config_files.push_back(entry.path());
            LOG_DEBUG("Found legacy config: " + entry.path().string());
        }
    }

    // Configs directory (new location)
    if (std::filesystem::exists(config_dir)) {
        LOG_DEBUG("Config directory exists: " + config_dir);
        for (const auto& entry : std::filesystem::directory_iterator(config_dir)) {
            if (entry.path().extension() == ".json") {
                config_files.push_back(entry.path());
                LOG_DEBUG("Found config: " + entry.path().string());
            }
        }
    } else {
        LOG_DEBUG("Config directory does not exist: " + config_dir);
    }

    // Legacy configs subdirectory fallback
    if (std::filesystem::exists("configs")) {
        LOG_DEBUG("Legacy configs directory exists");
        for (const auto& entry : std::filesystem::directory_iterator("configs")) {
            if (entry.path().extension() == ".json") {
                config_files.push_back(entry.path());
                LOG_DEBUG("Found legacy fallback config: " + entry.path().string());
            }
        }
    }

    LOG_DEBUG("Total config files found: " + std::to_string(config_files.size()));

    // Add to tree
    for (const auto& file : config_files) {
        try {
            auto config = json::parseFromFile(file.string());
            auto obj = config->asObject();

            std::string name = "Unknown";
            if (obj.find("name") != obj.end() && obj.at("name")) {
                name = obj.at("name")->asString();
            }

            std::string desc = "No description";
            if (obj.find("description") != obj.end() && obj.at("description")) {
                desc = obj.at("description")->asString();
                if (desc.length() > 50) {
                    desc = desc.substr(0, 50) + "...";
                }
            }

            auto row = *(config_store_->append());
            row[config_columns_.name] = name;
            row[config_columns_.description] = desc;
            row[config_columns_.filename] = file.filename().string();
            LOG_DEBUG("Added config to tree: " + name + " (" + file.filename().string() + ")");

        } catch (const std::exception& e) {
            LOG_DEBUG("Error parsing config file " + file.string() + ": " + e.what());
            auto row = *(config_store_->append());
            row[config_columns_.name] = "Error";
            row[config_columns_.description] = "Could not read: " + std::string(e.what());
            row[config_columns_.filename] = file.filename().string();
        }
    }
}

void ConfigLibraryDialog::onRefreshClicked() {
    refreshConfigList();
}

void ConfigLibraryDialog::onLoadSelectedClicked() {
    auto selection = config_tree_.get_selection();
    auto iter = selection->get_selected();

    if (iter) {
        Glib::ustring filename = (*iter)[config_columns_.filename];
        std::string full_path;

        std::string filename_str = filename;

        // Get config directory from environment or use default (same as refreshConfigList)
        const char* config_dir_env = std::getenv("CLAUDE_AGENT_CONFIG_DIR");
        std::string config_dir = config_dir_env ? config_dir_env : "../configs";

        if (std::filesystem::exists(filename_str)) {
            full_path = filename_str;
        } else if (std::filesystem::exists(config_dir + "/" + filename_str)) {
            full_path = config_dir + "/" + filename_str;
        } else if (std::filesystem::exists("configs/" + filename_str)) {  // Legacy fallback
            full_path = "configs/" + filename_str;
        }

        if (!full_path.empty() && agent_.loadSpecificConfig(full_path)) {
            current_label_.set_text("Name: " + agent_.getName());
            auto dialog = Gtk::MessageDialog(*this,
                "Loaded configuration: " + agent_.getName(),
                false, Gtk::MESSAGE_INFO);
            dialog.run();
            hide();
        } else {
            auto dialog = Gtk::MessageDialog(*this,
                "Error: Configuration file not found.",
                false, Gtk::MESSAGE_ERROR);
            dialog.run();
        }
    } else {
        auto dialog = Gtk::MessageDialog(*this,
            "Please select a configuration to load.",
            false, Gtk::MESSAGE_WARNING);
        dialog.run();
    }
}

void ConfigLibraryDialog::onPreviewClicked() {
    auto selection = config_tree_.get_selection();
    auto iter = selection->get_selected();

    if (iter) {
        Glib::ustring filename = (*iter)[config_columns_.filename];
        showPreviewDialog(filename);
    } else {
        auto dialog = Gtk::MessageDialog(*this,
            "Please select a configuration to preview.",
            false, Gtk::MESSAGE_WARNING);
        dialog.run();
    }
}

void ConfigLibraryDialog::onDeleteClicked() {
    auto selection = config_tree_.get_selection();
    auto iter = selection->get_selected();

    if (iter) {
        Glib::ustring config_name = (*iter)[config_columns_.name];
        Glib::ustring filename = (*iter)[config_columns_.filename];

        if (filename == "agent_config.json") {
            auto dialog = Gtk::MessageDialog(*this,
                "Cannot delete the active configuration file.",
                false, Gtk::MESSAGE_WARNING);
            dialog.run();
            return;
        }

        auto confirm_dialog = Gtk::MessageDialog(*this,
            "Really delete '" + std::string(config_name) + "'?\nThis cannot be undone.",
            false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);

        if (confirm_dialog.run() == Gtk::RESPONSE_YES) {
            std::string full_path;

            std::string filename_str = filename;
            if (std::filesystem::exists(filename_str)) {
                full_path = filename_str;
            } else if (std::filesystem::exists("configs/" + filename_str)) {
                full_path = "configs/" + filename_str;
            }

            if (!full_path.empty()) {
                try {
                    std::filesystem::remove(full_path);
                    refreshConfigList();
                    auto success_dialog = Gtk::MessageDialog(*this,
                        "Deleted '" + std::string(config_name) + "'",
                        false, Gtk::MESSAGE_INFO);
                    success_dialog.run();
                } catch (const std::exception& e) {
                    auto error_dialog = Gtk::MessageDialog(*this,
                        "Error deleting file: " + std::string(e.what()),
                        false, Gtk::MESSAGE_ERROR);
                    error_dialog.run();
                }
            } else {
                auto dialog = Gtk::MessageDialog(*this,
                    "Configuration file not found.",
                    false, Gtk::MESSAGE_ERROR);
                dialog.run();
            }
        }
    } else {
        auto dialog = Gtk::MessageDialog(*this,
            "Please select a configuration to delete.",
            false, Gtk::MESSAGE_WARNING);
        dialog.run();
    }
}

void ConfigLibraryDialog::onCreateTemplateClicked(const std::string& template_name) {
    createFromTemplate(template_name, "");
}

void ConfigLibraryDialog::onExportCurrentClicked() {
    // Implementation for export current
    auto dialog = Gtk::MessageDialog(*this,
        "Export functionality not yet implemented.",
        false, Gtk::MESSAGE_INFO);
    dialog.run();
}

void ConfigLibraryDialog::onExportAllClicked() {
    // Implementation for export all
    auto dialog = Gtk::MessageDialog(*this,
        "Export all functionality not yet implemented.",
        false, Gtk::MESSAGE_INFO);
    dialog.run();
}

void ConfigLibraryDialog::onImportConfigClicked() {
    // Implementation for import config
    auto dialog = Gtk::MessageDialog(*this,
        "Import functionality not yet implemented.",
        false, Gtk::MESSAGE_INFO);
    dialog.run();
}

void ConfigLibraryDialog::onImportBundleClicked() {
    // Implementation for import bundle
    auto dialog = Gtk::MessageDialog(*this,
        "Import bundle functionality not yet implemented.",
        false, Gtk::MESSAGE_INFO);
    dialog.run();
}

void ConfigLibraryDialog::onDuplicateCurrentClicked() {
    // Implementation for duplicate current
    auto dialog = Gtk::MessageDialog(*this,
        "Duplicate functionality not yet implemented.",
        false, Gtk::MESSAGE_INFO);
    dialog.run();
}

void ConfigLibraryDialog::onResetToDefaultClicked() {
    // Implementation for reset to default
    auto dialog = Gtk::MessageDialog(*this,
        "Reset functionality not yet implemented.",
        false, Gtk::MESSAGE_INFO);
    dialog.run();
}

void ConfigLibraryDialog::showPreviewDialog(const std::string& filename) {
    std::string full_path;

    if (std::filesystem::exists(filename)) {
        full_path = filename;
    } else if (std::filesystem::exists("configs/" + filename)) {
        full_path = "configs/" + filename;
    }

    if (full_path.empty()) {
        auto dialog = Gtk::MessageDialog(*this,
            "Configuration file not found.",
            false, Gtk::MESSAGE_ERROR);
        dialog.run();
        return;
    }

    try {
        auto config = json::parseFromFile(full_path);
        auto obj = config->asObject();

        auto preview_dialog = std::make_unique<Gtk::Dialog>("Preview: " + filename, *this);
        preview_dialog->set_default_size(600, 400);

        auto content_area = preview_dialog->get_content_area();

        auto scroll = Gtk::manage(new Gtk::ScrolledWindow());
        scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        auto text_view = Gtk::manage(new Gtk::TextView());
        text_view->set_editable(false);
        text_view->set_wrap_mode(Gtk::WRAP_WORD);

        auto buffer = text_view->get_buffer();

        std::string content = "Name: ";
        if (obj.find("name") != obj.end() && obj.at("name")) {
            content += obj.at("name")->asString();
        } else {
            content += "Unknown";
        }
        content += "\n\nDescription:\n";
        if (obj.find("description") != obj.end() && obj.at("description")) {
            content += obj.at("description")->asString();
        } else {
            content += "No description";
        }
        content += "\n\nInstructions:\n";
        if (obj.find("instructions") != obj.end() && obj.at("instructions")) {
            content += obj.at("instructions")->asString();
        } else {
            content += "No instructions";
        }
        content += "\n\nConversation Starters:\n";
        if (obj.find("conversation_starters") != obj.end() && obj.at("conversation_starters") &&
            obj.at("conversation_starters")->isArray()) {
            const auto& starters = obj.at("conversation_starters")->asArray();
            for (const auto& starter : starters) {
                if (starter && starter->isString()) {
                    content += "- " + starter->asString() + "\n";
                }
            }
        }

        buffer->set_text(content);

        scroll->add(*text_view);
        content_area->pack_start(*scroll, Gtk::PACK_EXPAND_WIDGET, 10);

        preview_dialog->add_button("Close", Gtk::RESPONSE_CLOSE);

        preview_dialog->show_all_children();
        preview_dialog->run();

    } catch (const std::exception& e) {
        auto dialog = Gtk::MessageDialog(*this,
            "Error reading configuration: " + std::string(e.what()),
            false, Gtk::MESSAGE_ERROR);
        dialog.run();
    }
}

void ConfigLibraryDialog::createFromTemplate(const std::string& template_name, const std::string& /* description */) {
    auto template_config = getTemplateConfig(template_name);
    if (!template_config) {
        auto dialog = Gtk::MessageDialog(*this,
            "Unknown template: " + template_name,
            false, Gtk::MESSAGE_ERROR);
        dialog.run();
        return;
    }

    // Create customization dialog
    auto create_dialog = std::make_unique<Gtk::Dialog>("Create " + template_name, *this);
    create_dialog->set_default_size(500, 400);

    auto content_area = create_dialog->get_content_area();

    auto main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    main_box->set_spacing(10);
    main_box->set_margin_left(10);
    main_box->set_margin_right(10);
    main_box->set_margin_top(10);
    main_box->set_margin_bottom(10);

    // Name entry
    auto name_label = Gtk::manage(new Gtk::Label("Agent Name:"));
    name_label->set_halign(Gtk::ALIGN_START);
    auto name_entry = Gtk::manage(new Gtk::Entry());
    auto obj = template_config->asObject();
    if (obj.find("name") != obj.end() && obj.at("name")) {
        name_entry->set_text(obj.at("name")->asString());
    }

    main_box->pack_start(*name_label, Gtk::PACK_SHRINK);
    main_box->pack_start(*name_entry, Gtk::PACK_SHRINK);

    // Filename entry
    auto filename_label = Gtk::manage(new Gtk::Label("Save as:"));
    filename_label->set_halign(Gtk::ALIGN_START);
    auto filename_entry = Gtk::manage(new Gtk::Entry());
    std::string default_filename = template_name;
    std::transform(default_filename.begin(), default_filename.end(), default_filename.begin(), ::tolower);
    std::replace(default_filename.begin(), default_filename.end(), ' ', '_');
    filename_entry->set_text(default_filename);

    main_box->pack_start(*filename_label, Gtk::PACK_SHRINK);
    main_box->pack_start(*filename_entry, Gtk::PACK_SHRINK);

    content_area->pack_start(*main_box, Gtk::PACK_EXPAND_WIDGET);

    create_dialog->add_button("Cancel", Gtk::RESPONSE_CANCEL);
    create_dialog->add_button("Create", Gtk::RESPONSE_OK);

    create_dialog->show_all_children();

    int result = create_dialog->run();

    if (result == Gtk::RESPONSE_OK) {
        std::string new_name = name_entry->get_text();
        std::string filename = filename_entry->get_text();

        if (!new_name.empty() && !filename.empty()) {
            // Update template with new name
            auto new_config = json::object();
            auto new_obj = std::static_pointer_cast<json::ObjectValue>(new_config);

            new_obj->set("name", json::string(new_name));
            if (obj.find("description") != obj.end() && obj.at("description")) {
                new_obj->set("description", obj.at("description"));
            }
            if (obj.find("instructions") != obj.end() && obj.at("instructions")) {
                new_obj->set("instructions", obj.at("instructions"));
            }
            if (obj.find("conversation_starters") != obj.end() && obj.at("conversation_starters")) {
                new_obj->set("conversation_starters", obj.at("conversation_starters"));
            }

            if (agent_.saveConfigToFile(new_config, filename)) {
                auto success_dialog = Gtk::MessageDialog(*this,
                    "Configuration created!",
                    false, Gtk::MESSAGE_INFO);
                success_dialog.run();
                refreshConfigList();
            } else {
                auto error_dialog = Gtk::MessageDialog(*this,
                    "Error creating configuration!",
                    false, Gtk::MESSAGE_ERROR);
                error_dialog.run();
            }
        }
    }
}

std::shared_ptr<json::Value> ConfigLibraryDialog::getTemplateConfig(const std::string& template_name) {
    auto config = json::object();
    auto obj = std::static_pointer_cast<json::ObjectValue>(config);

    if (template_name == "General Assistant") {
        obj->set("name", json::string("General Assistant"));
        obj->set("description", json::string("A helpful general-purpose AI assistant"));
        obj->set("instructions", json::string("You are a helpful, accurate, and friendly AI assistant. Provide clear, concise answers and always be respectful."));

        auto starters = json::array();
        auto starters_arr = std::static_pointer_cast<json::ArrayValue>(starters);
        starters_arr->push(json::string("How can I help you?"));
        starters_arr->push(json::string("What would you like to know?"));
        obj->set("conversation_starters", starters);

    } else if (template_name == "Code Assistant") {
        obj->set("name", json::string("Code Assistant"));
        obj->set("description", json::string("Programming and development helper"));
        obj->set("instructions", json::string("You are an expert programmer. Help with code review, debugging, best practices, and programming questions. Always explain your reasoning."));

        auto starters = json::array();
        auto starters_arr = std::static_pointer_cast<json::ArrayValue>(starters);
        starters_arr->push(json::string("What code can I help with?"));
        starters_arr->push(json::string("Need help debugging?"));
        starters_arr->push(json::string("Looking for code review?"));
        obj->set("conversation_starters", starters);

    } else {
        return nullptr; // Unknown template
    }

    return config;
}