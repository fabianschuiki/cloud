/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H


enum cloud_option_type {
	CLOUD_OPTION_INT,
	CLOUD_OPTION_UINT,
	CLOUD_OPTION_STRING,
	CLOUD_OPTION_BOOL
};

struct cloud_option {
	enum cloud_option_type type;
	const char *name;
	int short_name;
	void *data;
};

int cloud_options_parse(const struct cloud_option *options, int count, int argc, char *argv[]);


#endif