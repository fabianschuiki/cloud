/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <cassert>
#include <iostream>
#include <vector>
#include <glibmm/i18n.h>

#include <config.h>
#include "AccountType.h"
#include "AccountTypeTwitter.h"


static std::vector<AccountType *> types;


void AccountType::registerTypes()
{
	assert(types.empty() && "registerTypes() called more than once");
	
	types.push_back(new AccountTypeTwitter);
}

AccountType * AccountType::getType(std::string identifier)
{
	for (int i = 0; i < types.size(); i++)
		if (types[i]->getIdentifier() == identifier)
			return types[i];
	return NULL;
}

AccountType * AccountType::getType(int index)
{
	return types[index];
}

int AccountType::numTypes()
{
	return types.size();
}


AccountType::AccountType()
{
	icon = NULL;
}

AccountType::~AccountType()
{
	if (icon) {
		delete icon;
		icon = NULL;
	}
}

Glib::ustring AccountType::getName() const
{
	std::stringstream name;
	name << "account-" << getIdentifier();
	return gettext(name.str().c_str());
}

Gtk::Image* AccountType::getIcon()
{
	if (!icon) {
		Glib::ustring path(DATA_DIR);
		path += "/";
		path += getIdentifier();
		path += ".png";
		std::cout << "loading icon for " << getIdentifier() << " from " << path << std::endl;
		icon = new Gtk::Image(path);
	}
	return icon;
}
