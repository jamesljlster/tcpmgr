#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcpmgr.h"
#include "tcpmgr_global.h"

#include "debug.h"

int main(int argc, char* argv[])
{
	int ret = 0;

	// Initial tcpmgr global variable
	memset(&tcpMgr, 0, sizeof(tcpmgr_t));

RET:
	LOG("exit");
	return ret;
}
