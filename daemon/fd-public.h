/*
 * Copyright © 2012 Fabian Schuiki
 */

enum {
	CLD_FD_READ  = 1 << 0,
	CLD_FD_WRITE = 1 << 1
};

struct cld_fd {
	int fd;
	unsigned int mask;
};