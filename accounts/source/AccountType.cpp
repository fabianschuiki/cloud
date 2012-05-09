/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <iostream>
#include <vector>
#include <glibmm/i18n.h>

#include "AccountType.h"
#include "AccountTypeTwitter.h"


static std::vector<AccountType *> types;

void AccountType::registerTypes()
{
	types.push_back(new AccountTypeTwitter);
	
	std::cout << "twitter type is called " << gettext("account-twitter") << std::endl;
}
