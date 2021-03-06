/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	util.c
Usage:	Utility library for the command-line tools.
----------------------------------------------------------------------------- */

#include "phorward.h"

void version( char** argv, char* descr )
{
	printf( "%s v%s [built %s]\n", *argv, LIBPHORWARD_VERSION, __DATE__ );

	if( descr && *descr )
		printf( "%s. Part of the Phorward Toolkit.\n\n", descr );

	printf( "Copyright (C) 2006-2018 by Phorward Software Technologies, "
				"Jan Max Meyer\n"
			"All rights reserved. See LICENSE for more information.\n" );
}
