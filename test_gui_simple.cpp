#include <gtkmm.h>
#include <iostream>
#include "logger.h"

class SimpleWindow : public Gtk::Window {
public:
    SimpleWindow() {
        set_title("Simple Test Window");
        set_default_size(400, 300);

        auto label = Gtk::manage(new Gtk::Label("Hello World!"));
        add(*label);

        show_all_children();

        LOG_INFO("Simple window created successfully");
    }
};

int main(int argc, char* argv[]) {
    LOG_INFO("Testing simple GTK window...");

    auto app = Gtk::Application::create(argc, argv, "com.example.simple-test");

    try {
        SimpleWindow window;
        LOG_INFO("Starting GTK main loop...");
        return app->run(window);
    } catch (const std::exception& e) {
        LOG_ERROR("Error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}