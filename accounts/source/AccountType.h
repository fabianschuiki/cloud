/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include <glibmm/ustring.h>
#include <gtkmm/image.h>


class AccountType
{
public:
	static void registerTypes();
	
	static AccountType* getType(std::string identifier);
	static AccountType* getType(int index);
	static int numTypes();
	
	AccountType();
	virtual ~AccountType();
	
	virtual std::string getIdentifier() const = 0;
	
	Glib::ustring getName() const;
	Gtk::Image* getIcon();
	
protected:
	Gtk::Image* icon;
};