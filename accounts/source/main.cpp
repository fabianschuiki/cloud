#include <gtkmm.h>
#include <iostream>

#include "Accounts.h"


int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(
		argc, argv, "ch.fabianschuiki.cloud.accounts");
	
	//Load the Glade file.
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	try {
		builder->add_from_file("accounts.glade");
	}
	catch (const Glib::FileError &ex) {
		std::cerr << "File Error: " << ex.what() << std::endl;
		return 1;
	}
	catch (const Glib::MarkupError &ex) {
		std::cerr << "Markup Error: " << ex.what() << std::endl;
		return 1;
	}
	catch (const Gtk::BuilderError &ex) {
		std::cerr << "Builder Error: " << ex.what() << std::endl;
		return 1;
	}
	
	Accounts *window = NULL;
	builder->get_widget_derived("accounts", window);
	if (!window) {
		std::cerr << "unable to create accounts window" << std::endl;
		return 1;
	}
	
	return app->run(*window);
}
