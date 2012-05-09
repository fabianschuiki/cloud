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
	
	resize(400, 400);
	set_title("Add Account");
	
	box.set_orientation(Gtk::ORIENTATION_VERTICAL);
	
	for (int i = 0; i < AccountType::numTypes(); i++) {
		AccountType *type = AccountType::getType(i);
		std::cout << "adding type " << type->getIdentifier() << std::endl;
		
		Gtk::Button *button = new Gtk::Button(type->getName(), true);
		button->set_image(*type->getIcon());
		box.add(*button);
	}
	
	add(box);
	
	show_all_children();
}

AddAccountPanel::~AddAccountPanel()
{
}