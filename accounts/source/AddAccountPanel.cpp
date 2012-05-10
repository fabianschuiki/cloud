/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <iostream>
#include <gtkmm/button.h>

#include "AddAccountPanel.h"
#include "AccountsWindow.h"
#include "AccountType.h"


AddAccountPanel::AddAccountPanel(AccountsWindow *window)
:	Gtk::Window()
{
	this->window = window;
	
	set_border_width(20);
	resize(250, 100);
	set_title("Add Account");
	
	for (int i = 0; i < AccountType::numTypes(); i++) {
		AccountType *type = AccountType::getType(i);
		Gtk::Button *button = new Gtk::Button;
		button->set_label(type->getName());
		button->signal_clicked().connect(sigc::bind<AccountType*>(sigc::mem_fun(*this, &AddAccountPanel::on_add_clicked), type));
		buttons.insert(Glib::RefPtr<Gtk::Button>(button));
		box.add(*button);
	}
	
	box.set_orientation(Gtk::ORIENTATION_VERTICAL);
	add(box);
	show_all_children();
}

AddAccountPanel::~AddAccountPanel()
{
}

void AddAccountPanel::on_add_clicked(AccountType *type)
{
	std::cout << "adding " << type->getName() << " account" << std::endl;
	window->addAccount(type);
}