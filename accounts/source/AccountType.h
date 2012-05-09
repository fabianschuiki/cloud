/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include <glibmm/ustring.h>


class AccountType
{
public:
	static void registerTypes();
	
	virtual std::string getIdentifier() const = 0;
};