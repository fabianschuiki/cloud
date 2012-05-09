/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include <glibmm/ustring.h>


class AccountType
{
public:
	static void registerTypes();
	
	static AccountType * getType(std::string identifier);
	static AccountType * getType(int index);
	static int numTypes();
	
	virtual std::string getIdentifier() const = 0;
};