/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once

#include <map>
#include <string>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>


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
};
