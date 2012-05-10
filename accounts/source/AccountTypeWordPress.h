/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include "AccountType.h"


class AccountTypeWordPress : public AccountType
{
public:
	virtual std::string getIdentifier() const { return "wordpress"; }
};