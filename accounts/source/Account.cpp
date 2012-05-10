/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <cloud.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>

#include "Account.h"
#include "AccountType.h"


Account::Account(AccountType *type)
{
	this->type = type;
	
	//Generate a new UUID for this account.
	FILE * uuidgen = popen("uuidgen", "r");
	char output[37];
	memset(output, 0, 37);
	if (fread(output, 36, 1, uuidgen) != 1) {
		std::cerr << __FUNCTION__ << ": unable to generate uuid" << std::endl;
		uuid = "<none generated>";
	} else {
		uuid = output;
	}
	pclose(uuidgen);
}

Account::~Account()
{
}

AccountType *Account::getType() const
{
	return type;
}

void Account::decode(struct cld_object *obj)
{
}

struct cld_object *Account::encode() const
{
	struct cld_object *obj = cld_object_create("account");
	if (!obj)
		return NULL;
	
	cld_object_set(obj, "type", cld_object_create_string(type->getIdentifier().c_str()));
	cld_object_set(obj, "uuid", cld_object_create_string(uuid.c_str()));
	
	return obj;
}

std::string Account::getUUID() const
{
	return uuid;
}