/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include <gtkmm/window.h>
#include <gtkmm/box.h>

class AccountsWindow;


class AddAccountPanel : public Gtk::Window
{
public:
	AddAccountPanel(AccountsWindow *window);
	virtual ~AddAccountPanel();
	
protected:
	AccountsWindow *window;
	Gtk::Box box;
};