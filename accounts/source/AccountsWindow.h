/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once

#include <map>
#include <set>
#include <string>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/paned.h>
#include <gtkmm/button.h>


struct cld_client;
class Account;
class AccountListWidget;
class AccountType;
class AddAccountPanel;


class AccountsWindow : public Gtk::Window
{
public:
	AccountsWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);
	virtual ~AccountsWindow();
	
	void addAccount(AccountType *type);
	
protected:
	struct cld_client *cloud;
	
	std::map<std::string, Account*> accounts;
	
	AccountListWidget *accountList;
	AddAccountPanel *addAccountPanel;
	
	Glib::RefPtr<Gtk::Paned> paned;
	Glib::RefPtr<Gtk::ButtonBox> accountButtonBox;
	std::set< Glib::RefPtr<Gtk::Button> > accountButtons;
	
	
	void reloadAccountButtons();
};
