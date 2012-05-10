/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <cassert>
#include <cloud.h>
#include <iostream>

#include "AccountsWindow.h"
#include "AccountListWidget.h"
#include "AddAccountPanel.h"
#include "Account.h"
#include "AccountType.h"


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
		accountList = new AccountListWidget(this);
		/*add(*accountList);
		set_border_width(20);*/
		
		Gtk::ButtonBox *accountButtonBox = new Gtk::ButtonBox;
		this->accountButtonBox = Glib::RefPtr<Gtk::ButtonBox>(accountButtonBox);
		accountButtonBox->set_orientation(Gtk::ORIENTATION_VERTICAL);
		
		Gtk::Paned *paned = new Gtk::Paned;
		this->paned = Glib::RefPtr<Gtk::Paned>(paned);
		paned->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
		paned->add1(*accountButtonBox);
		add(*paned);
		
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
	Account *account = new Account(type);
	accounts[account->getUUID()] = account;
	
	std::cout << "created " << account->getType()->getName() << " with UUID " << account->getUUID() << std::endl;
	struct cld_object *obj = account->encode();
	cld_object_print(obj);
	cld_client_account_set(cloud, obj);
	cld_object_destroy(obj);
	
	reloadAccountButtons();
}

void AccountsWindow::reloadAccountButtons()
{
	std::vector<Gtk::Widget*> children = accountButtonBox->get_children();
	for (int i = 0; i < children.size(); i++)
		accountButtonBox->remove(*children[i]);
	accountButtons.clear();
	
	for (std::map<std::string, Account*>::iterator it = accounts.begin(); it != accounts.end(); it++) {
		Gtk::Button *button = new Gtk::Button;
		button->set_label(it->second->getType()->getName());
		accountButtonBox->add(*button);
		accountButtons.insert(Glib::RefPtr<Gtk::Button>(button));
	}
	accountButtonBox->show_all_children();
}