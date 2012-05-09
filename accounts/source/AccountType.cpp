/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <cassert>
#include <iostream>
#include <vector>
#include <glibmm/i18n.h>

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
