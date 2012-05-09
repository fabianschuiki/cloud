/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once

#include "AccountType.h"


class AccountTypeTwitter : public AccountType
{
public:
	virtual std::string getIdentifier() const { return "twitter"; }
};