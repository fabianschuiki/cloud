/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>


enum {
	CLD_OP_SERVICE_RECORD
};

struct cld_service_record {
	size_t name_len;
};


#endif