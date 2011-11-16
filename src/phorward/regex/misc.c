/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	misc.c
Author:	Jan Max Meyer
Usage:	Utility and miscelleanous functions for additional usage that
		belong to the regex library.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */

/*
 * Functions
 */
 
/*NO_DOC*/

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_accept_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a structure of the type pregex_accept.
					The member accept will be set to REGEX_ACCEPT_NONE, which
					is -1, and greedyness defaults to TRUE.
					
	Parameters:		pregex_accept*	accept		Accepting information pointer
												to be initializes.
																	
	Returns:		pregex_accept*				Returns the input-pointer.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pregex_accept* pregex_accept_init( pregex_accept* accept )
{
	if( !accept )
	{
		WRONGPARAM;
		return (pregex_accept*)NULL;
	}

	memset( accept, 0, sizeof( pregex_accept ) );

	accept->accept = REGEX_ACCEPT_NONE;
	accept->anchors = REGEX_ANCHOR_NONE;
	accept->greedy = TRUE;
	
	return accept;	
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_check_anchors()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs an anchor checking within a string. It is used
					by the matching functions for NFA and DFA state machines.
					
	Parameters:		uchar*			all			The entire string. This can
												be equal to str, but is
												required to perform valid
												line-begin anchor checking.
												If 'all' is (uchar*)NULL,
												'str' is assumed as 'all'.
					uchar*			str			Pointer of the current match
												within entire.
					psize			len			Length of the matched string,
												in characters.
					int				anchors		Anchor configuration to be
												checked for the string.
					int				flags		Flags configuration, e. g.
												for wide-character enabled
												anchor checking.
																	
	Returns:		pboolean					TRUE, if all anchors match,
												FALSE else.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean pregex_check_anchors( uchar* all, uchar* str, psize len,
										int anchors, int flags )
{
	pchar	ch;
	int		charsize = sizeof( uchar );

	PROC( "pregex_check_anchors" );
	if( flags & REGEX_MOD_WCHAR )
	{
		PARMS( "all", "%ls", all );
		PARMS( "str", "%ls", str );
	}
	else
	{
		PARMS( "all", "%s", all );
		PARMS( "str", "%s", str );
	}
	PARMS( "anchors", "%d", anchors );
	PARMS( "flags", "%d", flags );

	/* Perform anchor checkings? */
	if( flags & REGEX_MOD_NO_ANCHORS || anchors == REGEX_ANCHOR_NONE )
	{
		MSG( "Anchor checking is disabled trough flags, or not required" );
		RETURN( TRUE );
	}

	/* Check parameter integrity */
	if( !( str || len ) )
	{
		MSG( "Not enough or wrong parameters!" );
		RETURN( FALSE );
	}

	if( !all )
		all = str;

	if( flags & REGEX_MOD_WCHAR )
		charsize = sizeof( pchar );

	/* Begin of line anchor */
	if( anchors & REGEX_ANCHOR_BOL )
	{
		MSG( "Begin of line anchor is set" );
		if( all < str )
		{
			if( flags & REGEX_MOD_WCHAR )
			{
				VARS( "str-1", "%ls", (pchar*)( str - 1 ) );
				ch = *( (pchar*)( str - 1 ) );
			}
			else
			{
				VARS( "str-1", "%s", str-1 );
				ch = *( str - 1 );
			}

			VARS( "ch", "%lc", ch );
			if( ch != '\n' && ch != '\r' )
				RETURN( FALSE );
		}
	}

	/* End of Line anchor */
	if( anchors & REGEX_ANCHOR_EOL )
	{
		MSG( "End of line anchor is set" );
		if( ( ch = *( str + ( len * charsize ) ) ) )
		{
			VARS( "ch", "%lc", ch );
			if( ch != '\n' && ch != '\r' )
				RETURN( FALSE );
		}
	}

	/* Begin of word anchor */
	if( anchors & REGEX_ANCHOR_BOW )
	{
		MSG( "Begin of word anchor is set" );
		if( all < str )
		{
			if( flags & REGEX_MOD_WCHAR )
			{
				VARS( "str-1", "%ls", (pchar*)( str - 1 ) );
				ch = *( (pchar*)( str - 1 ) );
			}
			else
			{
				VARS( "str-1", "%s", str-1 );
				ch = *( str - 1 );
			}

			VARS( "ch", "%lc", ch );
			if( Pisalnum( ch ) || ch == '_' )
				RETURN( FALSE );
		}
	}

	/* End of word anchor */
	if( anchors & REGEX_ANCHOR_EOW )
	{
		MSG( "End of word anchor is set" );
		if( ( ch = *( str + ( len * charsize ) ) ) )
		{
			VARS( "ch", "%lc", ch );
			if( Pisalnum( ch ) || ch == '_' )
				RETURN( FALSE );
		}
	}

	MSG( "All anchor tests where fine!" );
	RETURN( TRUE );
}

/*COD_ON*/

