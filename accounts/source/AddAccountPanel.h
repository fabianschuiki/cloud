/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include <set>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

class AccountsWindow;
class AccountType;


class AddAccountPanel : public Gtk::Window
{
public:
	AddAccountPanel(AccountsWindow *window);
	virtual ~AddAccountPanel();
	
protected:
	AccountsWindow *window;
	Gtk::Box box;
	std::set< Glib::RefPtr<Gtk::Button> > buttons;
	
	void on_add_clicked(AccountType *type);
};