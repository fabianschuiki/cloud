#pragma once
#include <gtkmm/window.h>
#include <gtkmm/builder.h>

class Accounts : public Gtk::Window
{
public:
	Accounts(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &glade);
	virtual ~Accounts();
};
