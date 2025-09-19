#pragma once

#include <gtkmm.h>
#include <memory>
#include "claude_agent.h"

class ConfigLibraryDialog : public Gtk::Dialog {
public:
    ConfigLibraryDialog(Gtk::Window& parent, ClaudeAgent& agent);
    ~ConfigLibraryDialog() = default;

    void showDialog();

protected:
    void setupUi();
    void setupBrowseTab();
    void setupTemplatesTab();
    void setupImportExportTab();
    void setupManagementTab();

    // Event handlers
    void onRefreshClicked();
    void onLoadSelectedClicked();
    void onPreviewClicked();
    void onDeleteClicked();
    void onCreateTemplateClicked(const std::string& template_name);
    void onExportCurrentClicked();
    void onExportAllClicked();
    void onImportConfigClicked();
    void onImportBundleClicked();
    void onDuplicateCurrentClicked();
    void onResetToDefaultClicked();

private:
    ClaudeAgent& agent_;

    // UI components
    Gtk::Notebook notebook_;

    // Browse tab
    Gtk::Box browse_box_;
    Gtk::ScrolledWindow browse_scroll_;
    Gtk::TreeView config_tree_;
    Glib::RefPtr<Gtk::ListStore> config_store_;
    Gtk::ButtonBox browse_buttons_;
    Gtk::Button refresh_button_;
    Gtk::Button load_button_;
    Gtk::Button preview_button_;
    Gtk::Button delete_button_;

    // Templates tab
    Gtk::Box templates_box_;
    std::vector<std::unique_ptr<Gtk::Button>> template_buttons_;

    // Import/Export tab
    Gtk::Box import_export_box_;
    Gtk::Frame export_frame_;
    Gtk::Box export_box_;
    Gtk::Frame import_frame_;
    Gtk::Box import_box_;

    // Management tab
    Gtk::Box management_box_;
    Gtk::Frame current_frame_;
    Gtk::Label current_label_;
    Gtk::Frame actions_frame_;
    Gtk::Box actions_box_;

    // Tree model columns
    class ConfigColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ConfigColumns() {
            add(name);
            add(description);
            add(filename);
        }

        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<Glib::ustring> description;
        Gtk::TreeModelColumn<Glib::ustring> filename;
    };

    ConfigColumns config_columns_;

    // Helper methods
    void refreshConfigList();
    void createFromTemplate(const std::string& template_name, const std::string& description);
    void showPreviewDialog(const std::string& filename);
    std::shared_ptr<json::Value> getTemplateConfig(const std::string& template_name);

    static constexpr int DIALOG_WIDTH = 800;
    static constexpr int DIALOG_HEIGHT = 600;
};