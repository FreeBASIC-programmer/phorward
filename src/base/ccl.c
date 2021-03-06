/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	ccl.c
Author:	Jan Max Meyer
Usage:	Character classes
----------------------------------------------------------------------------- */

#define PREGEX_LOCAL
#include "phorward.h"

/* Sort-function required for quick sort */
static int ccl_SORTFUNC( plist* list, plistel* el, plistel* er )
{
	pcrange*	l	= (pcrange*)plist_access( el );
	pcrange*	r	= (pcrange*)plist_access( er );

	return r->begin - l->begin;
}

/** Constructor function to create a new character-class.


Returns a pointer to the newly created character-class. This pointer should be
released with p_ccl_free() when its existence is no longer required.
*/
pccl* p_ccl_create( int min, int max, char* ccldef )
{
	pccl*	ccl;

	PROC( "p_ccl_create" );
	PARMS( "min", "%d", min );
	PARMS( "max", "%d", max );
	PARMS( "ccldef", "%s", ccldef );

	if( min < 0 )
		min = PCCL_MIN;
	if( max < 0 )
		max = PCCL_MAX;

	ccl = (pccl*)pmalloc( sizeof( pccl ) );

	ccl->ranges = plist_create( sizeof( pcrange ), PLIST_MOD_RECYCLE );
	plist_set_sortfn( ccl->ranges, ccl_SORTFUNC );

	if( min > max )
	{
		ccl->min = max;
		ccl->max = min;
	}
	else
	{
		ccl->min = min;
		ccl->max = max;
	}

	if( ccldef )
		p_ccl_parse( ccl, ccldef, FALSE );


	RETURN( ccl );
}

/** Checks if the character-classes //l// and //r// are in the same
character universe and compatible for operations. */
pboolean p_ccl_compat( pccl* l, pccl* r )
{
	if( !( l && r ) )
		return FALSE;

	if( l->min != r->min && l->max != r->max )
		return FALSE;

	return TRUE;
}

/** Returns the number of range pairs within a character-class.

//ccl// is a pointer to the character-class to be processed.

To retrieve the number of characters in a character-class, use
p_ccl_count() instead.

Returns the number of pairs the charclass holds.
*/
int p_ccl_size( pccl* ccl )
{
	if( !ccl )
	{
		WRONGPARAM;
		return 0;
	}

	return plist_count( ccl->ranges );
}

/** Returns the number of characters within a character-class.

//ccl// is a pointer to the character-class to be processed.

Returns the total number of characters the class is holding.
*/
int p_ccl_count( pccl* ccl )
{
	plistel*	e;
	pcrange*	cr;
	int			cnt	= 0;

	if( !ccl )
	{
		WRONGPARAM;
		return 0;
	}

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		cr = (pcrange*)plist_access( e );
		cnt += ( cr->end - cr->begin ) + 1;
	}

	return cnt;
}

/** Duplicates a character-class into a new one.

//ccl// is the pointer to the character-class to be duplicated.

Returns a pointer to the duplicate of //ccl//, or (pcrange)NULL
in error case.
*/
pccl* p_ccl_dup( pccl* ccl )
{
	pccl* 		dup;
	plistel*	e;

	if( !ccl )
	{
		WRONGPARAM;
		return (pccl*)NULL;
	}

	/* Create new, empty ccl */
	dup = p_ccl_create( ccl->min, ccl->max, (char*)NULL );

	/* Copy elements */
	plist_for( ccl->ranges, e )
		plist_push( dup->ranges, plist_access( e ) );

	return dup;
}

/* Normalizes a pre-parsed or modified character-class.

Normalization means, that duplicate elements will be removed, the range pairs
are sorted and intersections are resolved. The result is a unique, normalized
character-class to be used for further operations.

//ccl// is the character-class to be normalized.

Returns the number of cycles used for normalization.
*/
static int p_ccl_normalize( pccl* ccl )
{
	plistel*	e;
	pcrange*	l;
	pcrange*	r;
	int			count		= 0;
	int			oldcount	= 0;
	int			cycles		= 0;

	PROC( "p_ccl_normalize" );
	PARMS( "ccl", "%p", ccl );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	do
	{
		oldcount = count;

		/* First sort the character ranges */
		plist_sort( ccl->ranges );

		/* Then, find intersections and... */
		for( e = plist_first( ccl->ranges ); e; )
		{
			l = (pcrange*)plist_access( e );

			if( ( r = (pcrange*)plist_access( plist_next( e ) ) ) )
			{
				if( r->begin <= l->end && r->end >= l->begin )
				{
					if( r->end > l->end )
						l->end = r->end;

					plist_remove( ccl->ranges, plist_next( e ) );
					continue;
				}
				else if( l->end + 1 == r->begin )
				{
					l->end = r->end;
					plist_remove( ccl->ranges, plist_next( e ) );
					continue;
				}
			}

			e = plist_next( e );
		}

		cycles++;
	}
	while( ( count = p_ccl_size( ccl ) ) != oldcount );

	RETURN( cycles );
}

/** Tests a character-class to match a character range.

//ccl// is a pointer to the character-class to be tested.
//begin// is the begin of character-range to be tested.
//end// is the end of character-range to be tested.

Returns TRUE if the entire character range matches the class, and FALSE if not.
*/
pboolean p_ccl_testrange( pccl* ccl, wchar_t begin, wchar_t end )
{
	pcrange*	cr;
	plistel*	e;

	if( !( ccl ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		cr = (pcrange*)plist_access( e );

		if( begin >= cr->begin && end <= cr->end )
			return TRUE;
	}

	return FALSE;
}

/** Tests a character-class if it cointains a character.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

The function is a shortcut for p_ccl_testrange().

It returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean p_ccl_test( pccl* ccl, wchar_t ch )
{
	return p_ccl_testrange( ccl, ch, ch );
}

/** Tests for a character in case-insensitive-mode if it matches
a character-class.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

The function is a shortcut for p_ccl_testrange().

It returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean p_ccl_instest( pccl* ccl, wchar_t ch )
{
	if( !ccl )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( p_ccl_test( ccl, ch ) )
		return TRUE;

#if UNICODE
	if( iswupper( ch ) )
		ch = towlower( ch );
	else
		ch = towupper( ch );
#else
	if( isupper( ch ) )
		ch = tolower( ch );
	else
		ch = toupper( ch );
#endif

	return p_ccl_test( ccl, ch );
}

/* Internal function without normalization */
static pboolean p_ccl_ADDRANGE( pccl* ccl, wchar_t begin, wchar_t end )
{
	pcrange		cr;

	PROC( "p_ccl_ADDRANGE" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( begin > end )
	{
		cr.begin = end;
		cr.end = begin;
	}
	else
	{
		cr.begin = begin;
		cr.end = end;
	}

	if( cr.begin < ccl->min )
		cr.begin = ccl->min;

	if( cr.end > ccl->max )
		cr.end = ccl->max;

	if( p_ccl_testrange( ccl, cr.begin, cr.end ) )
	{
		MSG( "Range already in character-class" );
		RETURN( TRUE );
	}

	if( cr.begin > ccl->max || cr.end < ccl->min )
	{
		MSG( "Character-range not in the universe of character-class" );
		RETURN( FALSE );
	}

	plist_push( ccl->ranges, (void*)&cr );

	RETURN( TRUE );
}

/** Integrates a character range into a character-class.

//ccl// is the pointer to the character-class to be affected. If //ccl// is
provided as (pccl*)NULL, it will be created by the function.

//begin// is the begin of character range to be integrated.
//end// is the end of character range to be integrated.

If //begin// is greater than //end//, the values will be swapped.
*/
pboolean p_ccl_addrange( pccl* ccl, wchar_t begin, wchar_t end )
{
	PROC( "p_ccl_addrange" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	if( !p_ccl_ADDRANGE( ccl, begin, end ) )
		RETURN( FALSE );

	p_ccl_normalize( ccl );
	RETURN( TRUE );
}


/** Integrates a single character into a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be integrated.

The function is a shortcut for p_ccl_addrange().
*/
pboolean p_ccl_add( pccl* ccl, wchar_t ch )
{
	return p_ccl_addrange( ccl, ch, ch );
}

/** Removes a character range from a character-class.

//ccl// is the pointer to the character-class to be affected.
//begin// is the begin of character range to be removed.
//end// is the end of character range to be removed.
*/
pboolean p_ccl_delrange( pccl* ccl, wchar_t begin, wchar_t end )
{
	plistel*	e;
	pcrange		d;
	pcrange*	r;

	PROC( "p_ccl_delrange" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( begin > end )
	{
		d.begin = end;
		d.end = begin;
	}
	else
	{
		d.begin = begin;
		d.end = end;
	}

	/* Which elements do match? */
	do
	{
		for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
		{
			r = (pcrange*)plist_access( e );

			if( d.begin <= r->end && end >= r->begin )
			{
				/* Slitting required? */
				if( d.begin > r->begin && d.end < r->end )
				{
					MSG( "Split current range" );
					end = r->end;
					r->end = d.begin - 1;

					if( !p_ccl_addrange( ccl, d.end + 1, end ) )
						RETURN( FALSE );

					break;
				}
				/* Move the end */
				else if( d.begin > r->begin )
				{
					MSG( "Move end of current range" );
					r->end = d.begin - 1;
				}
				/* Move the begin */
				else if( d.end < r->end )
				{
					MSG( "Change begin of current range" );
					r->begin = d.end + 1;
				}
				/* Remove entire range */
				else
				{
					MSG( "Remove entire range" );
					plist_remove( ccl->ranges, e );

					e = plist_first( ccl->ranges );
					break;
				}
			}
		}
	}
	while( e );

	p_ccl_normalize( ccl );
	RETURN( TRUE );
}

/** Removes a character from a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be removed from //ccl//.

The function is a shortcut for p_ccl_delrange().
*/
pboolean p_ccl_del( pccl* ccl, wchar_t ch )
{
	return p_ccl_delrange( ccl, ch, ch );
}

/** Negates all ranges in a character-class.

//ccl// is the pointer to the character-class to be negated.

Returns a pointer to //ccl//.
*/
pccl* p_ccl_negate( pccl* ccl )
{
	wchar_t		start;
	wchar_t		end;
	plistel*	e;
	plistel*	ne;
	pcrange*	r;

	PROC( "p_ccl_negate" );
	PARMS( "ccl", "%p", ccl );

	if( !ccl )
	{
		WRONGPARAM;
		RETURN( (pccl*)NULL );
	}

	start = end = ccl->min;

	for( e = plist_first( ccl->ranges ); e; e = ne )
	{
		r = (pcrange*)plist_access( e );
		ne = plist_next( e );

		if( end < r->begin )
		{
			start = r->begin;
			r->begin = end;

			end = r->end + 1;
			r->end = start - 1;
		}
		else
		{
			end = r->end + 1;
			plist_remove( ccl->ranges, e );
		}
	}

	if( end < ccl->max )
		p_ccl_addrange( ccl, end, ccl->max );

	p_ccl_normalize( ccl );

	RETURN( ccl );
}

/** Unions two character-classes into a new, normalized one.

//ccl// is the pointer to the character-class that will be extended to all
ranges contained in //add//.
//add// is character-class that will be unioned with //ccl//.

The function creates and returns a new character-class that is the union
if //ccl// and //add//.
*/
pccl* p_ccl_union( pccl* ccl, pccl* add )
{
	pccl*		un;
	plistel*	e;
	pcrange*	r;

	PROC( "p_ccl_union" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "add", "%p", add );

	if( !( ccl && add ) )
	{
		WRONGPARAM;
		RETURN( (pccl*)NULL );
	}

	if( !p_ccl_compat( ccl, add ) )
	{
		MSG( "Incompatible character-classes" );
		RETURN( (pccl*)NULL );
	}

	un = p_ccl_dup( ccl );

	for( e = plist_first( add->ranges ); e; e = plist_next( e ) )
	{
		r = (pcrange*)plist_access( e );

		if( !p_ccl_ADDRANGE( un, r->begin, r->end ) )
			RETURN( FALSE );
	}

	p_ccl_normalize( un );

	RETURN( un );
}


/** Returns the difference quantity of two character-classes.
All elements from //rem// will be removed from //ccl//, and put into a
new character-class.

//ccl// is the pointer to the first character-class.
//rem// is the pointer to the second character-class.

Returns a new pointer to a copy of //ccl//, without the ranges contained in
//rem//. Returns (pccl*)NULL in case of memory allocation or parameter
error.
*/
pccl* p_ccl_diff( pccl* ccl, pccl* rem )
{
	plistel*	e;
	pcrange*	r;
	pccl*		diff;

	PROC( "p_ccl_diff" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "rem", "%p", rem );

	if( !( ccl && rem ) )
	{
		WRONGPARAM;
		RETURN( (pccl*)NULL );
	}

	if( !p_ccl_compat( ccl, rem ) )
	{
		MSG( "Incompatible character-classes" );
		RETURN( (pccl*)NULL );
	}

	if( !( diff = p_ccl_dup( ccl ) ) )
		return (pccl*)NULL;

	for( e = plist_first( rem->ranges ); e; e = plist_next( e ) )
	{
		r = (pcrange*)plist_access( e );
		p_ccl_delrange( diff, r->begin, r->end );
	}

	return diff;
}

/** Checks for differences in two character-classes.

//left// is the pointer to the first character-class.
//right// is the pointer to the second character-class.

Returns a value < 0 if //left// is lower than //right//, 0 if //left// is
equal to //right// or a value > 0 if //left// is greater than //right//.
*/
int p_ccl_compare( pccl* left, pccl* right )
{
	plistel*	e;
	plistel*	f;
	pcrange*	l;
	pcrange*	r;
	int			ret		= 0;

	PROC( "p_ccl_compare" );
	PARMS( "left", "%p", left );
	PARMS( "right", "%p", right );

	if( !( left && right ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	if( !p_ccl_compat( left, right ) )
	{
		MSG( "Incompatible character-classes" );
		RETURN( left->max - right->max );
	}

	if( ( ret = p_ccl_size( left ) - p_ccl_size( right ) ) != 0 )
	{
		MSG( "Unequal number of range pairs" );
		RETURN( ret );
	}

	MSG( "Deep check all ranges" );
	for( e = plist_first( left->ranges ), f = plist_first( right->ranges );
			e && f; e = plist_next( e ), f = plist_next( f ) )
	{
		l = (pcrange*)plist_access( e );
		r = (pcrange*)plist_access( f );

		if( !( l->begin == r->begin && l->end == r->end ) )
		{
			MSG( "Ranges not equal" );
			RETURN( ( ( l->begin > r->begin ) ? 1 : -1 ) );
		}
	}

	MSG( "Character-classes are equal" );
	RETURN( ret );
}

/** Returns a new character-class with all characters that exist in both
provided character-classes.

//ccl// is the pointer to the first character-class.
//within// is the pointer to the second character-class.

Returns a new character-class containing the insersections from //ccl//
and //within//. If there is no intersection between both character-classes,
the function returns (pccl*)NULL.
*/
pccl* p_ccl_intersect( pccl* ccl, pccl* within )
{
	plistel*	e;
	plistel*	f;
	pcrange*	r;
	pcrange*	s;
	pccl*		in	= (pccl*)NULL;

	PROC( "p_ccl_intersect" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "within", "%p", within );

	if( !( ccl && within ) )
	{
		WRONGPARAM;
		RETURN( (pccl*)NULL );
	}

	if( !p_ccl_compat( ccl, within ) )
	{
		MSG( "Character-classes are not compatible" );
		RETURN( (pccl*)NULL );
	}

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		r = (pcrange*)plist_access( e );

		for( f = plist_first( within->ranges ); f; f = plist_next( f ) )
		{
			s = (pcrange*)plist_access( f );

			if( s->begin <= r->end && s->end >= r->begin )
			{
				if( !in )
					in = p_ccl_create( ccl->min, ccl->max, (char*)NULL );

				p_ccl_addrange( in,
					( r->begin > s->begin ) ? r->begin : s->begin,
					( r->end > s->end ) ? s->end : r->end );
			}
		}
	}

	if( in )
	{
		MSG( "Normalizing" );
		p_ccl_normalize( in );
	}

	RETURN( in );
}

/** Return a character or a character-range by its offset.

If the function is called only with pointer //from// provided, and //to// as
(wchar_t*)NULL, it writes the character in //offset//th position of the
character-class into from.

If the function is called both with pointer //from// and //to// provided,
it writes the //begin// and //end// character of the character-range in the
//offset//th position of the character-class into //from// and //to//.

If no character or range with the given offset was found, the function
returns FALSE, meaning that the end of the characters is reached.
On success, the function will always return TRUE. */
pboolean p_ccl_get( wchar_t* from, wchar_t* to, pccl* ccl, int offset )
{
	plistel*	e;
	pcrange*	cr;

	PROC( "p_ccl_get" );
	PARMS( "from", "%p", from );
	PARMS( "to", "%p", to );
	PARMS( "ccl", "%p", ccl );
	PARMS( "offset", "%d", offset );

	if( !( ccl && from && offset >= 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !to )
	{
		MSG( "Single-character retrival" );

		for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
		{
			cr = (pcrange*)plist_access( e );

			VARS( "offset", "%d", offset );
			VARS( "cr->begin", "%d", cr->begin );
			VARS( "cr->end", "%d", cr->end );
			VARS( "LEN cr->end - cr->begin + 1", "%d",
					cr->end - cr->begin + 1 );

			if( offset >= ( cr->end - cr->begin ) + 1 )
			{
				MSG( "Offset not in this range" );
				offset -= ( cr->end - cr->begin ) + 1;
			}
			else if( offset < ( cr->end - cr->begin ) + 1 )
			{
				MSG( "Offset is within this class" );

				*from = cr->begin + offset;

				VARS( "*from", "%d", *from );
				RETURN( TRUE );
			}
		}
	}
	else
	{
		MSG( "Range retrival" );

		if( ( cr = (pcrange*)plist_access(
						plist_get( ccl->ranges, offset ) ) ) )
		{
			*from = cr->begin;
			*to = cr->end;

			VARS( "*from", "%d", *from );
			VARS( "*to", "%d", *to );

			RETURN( TRUE );
		}
	}

	MSG( "Offset not available" );
	RETURN( FALSE );
}

/** Reads a character from a string. The character may exist of one single
character or it may be made up of an escape sequence or UTF-8 character.
The function returns the number of bytes read.

//retc// is the return pointer for the character code of the escaped string.
//str// is the begin pointer of the string at which character parsing begins.
If //escapeseq// is TRUE, the function regards escape sequences, else it ignores
them.

Returns the number of bytes that had been read for the character.
*/
size_t p_ccl_parsechar( wchar_t* retc, char *str, pboolean escapeseq )
{
	wchar_t	ch;
    char 	digs[9]		=	"\0\0\0\0\0\0\0\0";
    int		dno 		= 0;
	char*	p			= str;

	PROC( "p_ccl_parsechar" );
	PARMS( "retc", "%p", retc );
	PARMS( "str", "%s", str );
	PARMS( "escapeseq", "%s", BOOLEAN_STR( escapeseq ) );

	if( escapeseq && *p == '\\' )
	{
		p++;

		switch( *p )
		{
			case 'n':
				ch = '\n';
				p++;
				break;
			case 't':
				ch = '\t';
				p++;
				break;
			case 'r':
				ch = '\r';
				p++;
				break;
			case 'b':
				ch = '\b';
				p++;
				break;
			case 'f':
				ch = '\f';
				p++;
				break;
			case 'v':
				ch = '\v';
				p++;
				break;
			case 'a':
				ch = '\a';
				p++;
				break;

			default:
				if( octal_digit( *p ) )
				{
					do
						digs[dno++] = *( p++ );
					while( octal_digit( *p ) && dno < 3 );
					ch = strtol( digs, (char**)NULL, 8 );
				}
				else if( *p == 'x' )
				{
					p++;
					while( hex_digit( *p ) && dno < 2 )
						digs[ dno++ ] = *( p++ );

					if (dno > 0)
						ch = strtol( digs, (char**)NULL, 16 );
				}
#ifdef UTF8
				else if( *p == 'u' )
				{
					p++;
					while( hex_digit( *p ) && dno < 4 )
						digs[dno++] = *( p++ );

					if( dno > 0 )
						ch = strtol( digs, (char**)NULL, 16 );
				}
				else if( *p == 'U' )
				{
					p++;
					while( hex_digit( *p ) && dno < 8 )
						digs[dno++] = *( p++ );

					if( dno > 0 )
						ch = strtol( digs, (char**)NULL, 16 );
				}
#endif
				else
				{
#ifdef UTF8
					ch = u8_char( p );
					p += u8_seqlen( p );
#else
					ch = *( p++ );
#endif
				}
				break;
		}
	}
	else
	{
#ifdef UTF8
		ch = u8_char( p );
		p += u8_seqlen( p );
#else
		ch = *( p++ );
#endif
	}

	VARS( "ch", "%d", ch );
	VARS( "len", "%d", (int)( p - str ) );
	*retc = ch;

    RETURN( (size_t)( p - str ) );
}

/**  Tries to parse a shorthand sequence from a string. This matches the
shorthands \w, \W, \d, \D, \s and \S. If it matches, all characters are
added to //ccl//.

The function returns TRUE in case a shorthand has been parsed. If so,
the pointer //str// is moved the characters consumed.

If no shorthand sequence could be found, it returns FALSE, remaining //ccl//
untouched.
*/
pboolean p_ccl_parseshorthand( pccl* ccl, char **str )
{
	pccl*		sh;
	pboolean	neg	= FALSE;
	int			i;
	wchar_t		begin;
	wchar_t		end;

	PROC( "p_ccl_parseshorthand" );

	if( !( ccl && str && *str ) )
	{
		WRONGPARAM;
		RETURN( 0 );
	}

	PARMS( "ccl", "%p", ccl );
	PARMS( "str", "%s", *str );

	/* Check for shorthand */
	if( **str == '\\' )
	{
		VARS( "ch", "%c", *(*str + 1) );

		switch( *(*str + 1) )
		{
			/* This solution is ugly and does not support any Unicode features.
				So it would be nice to find out a cooler solution in future. */
			case 'D':
				neg = TRUE;
			case 'd':
				sh = p_ccl_create( ccl->min, ccl->max, "0-9" );
				break;

			case 'W':
				neg = TRUE;
			case 'w':
				sh = p_ccl_create( ccl->min, ccl->max, "a-zA-Z_0-9" );
				break;

			case 'S':
				neg = TRUE;
			case 's':
				sh = p_ccl_create( ccl->min, ccl->max, " \f\n\r\t\v" );
				break;

			default:
				MSG( "Not a shorthand" );
				RETURN( FALSE ); /* Not a shorthand! */
		}

		if( neg )
			p_ccl_negate( sh );

		for( i = 0; p_ccl_get( &begin, &end, sh, i ); i++ )
			p_ccl_ADDRANGE( ccl, begin, end );

		p_ccl_free( sh );
		p_ccl_normalize( ccl );

		*str += 2;
		RETURN( TRUE );
	}

	RETURN( FALSE );
}

/** Parses the character-class definition provided in //ccldef// and assigns
this definition to the character-class //ccl//. //ccldef// may contain
UTF-8 formatted input. Escape-sequences will be interpretered to their correct
character representations.

A typical character-class definition simply exists of single characters and
range definitions. For example, "$A-Z#0-9" defines a character-class that
consists of the characters "$#0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ".

The parameter //extend// specifies, if the provided character-class overwrites
(//extend// = FALSE) or extends (//extend// = TRUE) the provided
character-class. This means that definitions that already exist in the
character-class, should be erased first or not.

The function returns TRUE on success, and FALSE on an error.
*/
pboolean p_ccl_parse( pccl* ccl, char* ccldef, pboolean extend )
{
	char*		cclptr;
	wchar_t		begin;
	wchar_t		end;

	PROC( "p_ccl_parse" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "ccldef", "%s", ccldef );
	PARMS( "extend", "%s", BOOLEAN_STR( extend ) );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !extend )
		p_ccl_erase( ccl );

	for( cclptr = pstrget( ccldef ); *cclptr; )
	{
		VARS( "cclptr", "%s", cclptr );

		if( p_ccl_parseshorthand( ccl, &cclptr ) )
			continue;

		cclptr += p_ccl_parsechar( &begin, cclptr, TRUE );
		end = begin;

		VARS( "begin", "%d", begin );
		VARS( "end", "%d", end );

		/* Is this a range def? */
		if( *cclptr == '-' )
		{
			MSG( "This is a range!" );
			cclptr++;
			cclptr += p_ccl_parsechar( &end, cclptr, TRUE );
			VARS( "end", "%d", end );
		}

		VARS( "cclptr", "%s", cclptr );

		p_ccl_ADDRANGE( ccl, begin, end );
	}

	MSG( "Finally normalize the character-class" );
	p_ccl_normalize( ccl );

	RETURN( TRUE );
}

/** Erases a character-class //ccl//.

The function sets a character-class to zero, as it continas no character range
definitions. The object //ccl// will be still alive. To delete the entire
object, use p_ccl_free().
*/
pboolean p_ccl_erase( pccl* ccl )
{
	if( !ccl )
	{
		WRONGPARAM;
		return FALSE;
	}

	return plist_erase( ccl->ranges );
}

/** Frees a character-class //ccl// and all its used memory.

The function always returns (pccl*)NULL.
*/
pccl* p_ccl_free( pccl* ccl )
{
	if( !ccl )
		return (pccl*)NULL;

	plist_free( ccl->ranges );
	pfree( ccl->str );

	pfree( ccl );

	return (pccl*)NULL;
}

/** Converts a character-class back to a string representation of the
character-class definition, which in turn can be converted back into a
character-class using p_ccl_create().

//ccl// is the pointer to character-class to be converted.
//escape//, if TRUE, escapes "unprintable" characters in their hexadecimal
representation. If FALSE, it prints all characters, except the zero, which will
be returned as "\0"

Returns a pointer to the generated string that represents the charclass.
The returned pointer belongs to the //ccl// and is managed by the
character-class handling functions, so it should not be freed manually.
*/
char* p_ccl_to_str( pccl* ccl, pboolean escape )
{
	plistel*	e;
	pcrange*	r;
	char		from	[ 40 + 1 ];
	char		to		[ 20 + 1 ];

	PROC( "p_ccl_to_str" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "escape", "%s", BOOLEAN_STR( escape ) );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	ccl->str = pfree( ccl->str );

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		r = (pcrange*)plist_access( e );

		if( escape )
			u8_escape_wchar( from, sizeof( from ), r->begin );
		else
			u8_toutf8( from, sizeof( from ), &( r->begin ), 1 );

		if( r->begin != r->end )
		{
			if( escape )
				u8_escape_wchar( to, sizeof( to ), r->end );
			else
				u8_toutf8( to, sizeof( to ), &( r->end ), 1 );

			sprintf( from + strlen( from ), "-%s", to );
		}

		ccl->str = pstrcatstr( ccl->str, from, FALSE );
	}

	VARS( "ret", "%s", ccl->str );
	RETURN( ccl->str ? ccl->str : "" );
}

/** Print character-class to output stream.
This function is provided for debug-purposes only.

//stream// is the output stream to dump the character-class to; This can be
left (FILE*)NULL, so //stderr// will be used.
//ccl// is the pointer to character-class

//break_after// defines:
- if < 0 print with pointer info
- if 0 print all into one line
- if > 0 print linewise
-
*/
void p_ccl_print( FILE* stream, pccl* ccl, int break_after )
{
	plistel*	e;
	pcrange*	r;
	int			cnt;
	char		outstr[ 2 ] [ 10 + 1 ];

	if( !( ccl ) )
		return;

	if( !stream )
		stream = stderr;

	if( break_after < 0 )
		fprintf( stream, "*** begin of ccl %p ***\n", ccl );

	for( e = plist_first( ccl->ranges ), cnt = 0;
			e; e = plist_next( e ), cnt++ )
	{
		r = (pcrange*)plist_access( e );

		u8_toutf8( outstr[0], sizeof( outstr[0] ), &( r->begin ), 1 );

		if( r->begin != r->end )
		{
			u8_toutf8( outstr[1], sizeof( outstr[1] ), &( r->end ), 1 );
			fprintf( stream, "'%s' [%d] to '%s' [%d] ",
				outstr[0], (int)r->begin, outstr[1], (int)r->end );
		}
		else
			fprintf( stream, "'%s' [%d] ", outstr[0], (int)r->begin );

		if( break_after > 0 && cnt % break_after == 0 )
			fprintf( stream, "\n" );
	}

	if( break_after < 0 )
		fprintf( stream, "*** end of ccl %p ***\n", ccl );
}

#if 0
int main( int argc, char** argv )
{
	pccl*	c;
	pccl*	d;
	pccl*	e;

	c = p_ccl_create( 0, 255, "A-Za-z0-9" );
	d = p_ccl_create( 0, 255, "A-@." );

	/*p_ccl_addrange( c, 0, 10 );*/
	p_ccl_print( NULL, c, 1 );
	p_ccl_print( NULL, d, 1 );

	e = p_ccl_union( c, d );
	p_ccl_print( NULL, e, 1 );

	printf( "%s\n", p_ccl_to_str( e, TRUE ) );

	/*
	p_ccl_negate( c );
	p_ccl_delrange( c, 0, 255 );
	p_ccl_print( c );
	*/

	return 0;
}
#endif

