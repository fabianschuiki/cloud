/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <cassert>
#include <cloud.h>
#include <iostream>

#include "AccountsWindow.h"
#include "AccountList.h"
#include "AddAccountPanel.h"


AccountsWindow::AccountsWindow(
	BaseObjectType *cobject,
	const Glib::RefPtr<Gtk::Builder> &builder
):	Gtk::Window(cobject)
{
	//Connect to the cloud daemon.
	cloud = cld_client_create();
	
	//In case no connection was established, show an error message.
	if (cloud == NULL) {
		Gtk::Widget *no_daemon;
		builder->get_widget("no_daemon", no_daemon);
		assert(no_daemon);
		add(*no_daemon);
	}
	
	//Otherwise provide normal functionality.
	else {
		accountList = new AccountList;
		add(*accountList);
		set_border_width(20);
		
		addAccountPanel = new AddAccountPanel(this);
		addAccountPanel->show();
	}
	
	show_all_children();
}

AccountsWindow::~AccountsWindow()
{
	if (cloud) {
		cld_client_destroy(cloud);
		cloud = NULL;
	}
}

/** Creates a new account of the given type and alters the UI so the user may
 * input further information */
void AccountsWindow::addAccount(AccountType *type)
{
}