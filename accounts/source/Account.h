/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once

#include <string>

struct cld_object;
class AccountType;


class Account
{
public:
	//static Account *make(struct cld_object *cldobj);
	
	Account(AccountType *type);
	virtual ~Account();
	
	AccountType *getType() const;
	
	void decode(struct cld_object *obj);
	struct cld_object *encode() const;
	
	std::string getUUID() const;
	
protected:
	AccountType *type;
	std::string uuid;
};