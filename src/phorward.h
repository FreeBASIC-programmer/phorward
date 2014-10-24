/*
	This file was automatically generated by pinclude.
	Source files: phorward.tpl.h
*/



#ifndef _PHORWARD_H
#define _PHORWARD_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <stdarg.h>

#ifndef va_copy
#define va_copy( a, b )	(a) = (b)
#endif

#include <limits.h>
#include <time.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef UNICODE
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif



typedef char 					pboolean;	
#ifndef BOOLEAN
#define BOOLEAN					pboolean
#endif

#ifndef boolean
#define boolean 				pboolean
#endif

#ifndef TRUE
#define TRUE					1
#endif

#ifndef FALSE
#define FALSE					0
#endif

#define BOOLEAN_STR( b ) 		( ( b ) ? "TRUE" : "FALSE" )
#define TRUEBOOLEAN( b ) 		( ( b ) ? TRUE : FALSE )


#define ERR_OK					0		
#define ERR_FAILURE				-1		
#define ERR_PARMS				-2		
#define ERR_MEM					-3		
#define ERR_SYSTEM				-4		
#define ERR_UNIMPL				-5		
#define ERR_OTHER				-6		


#define OUTOFMEM				fprintf( stderr, \
									"%s, %d: Ran out of memory\n", \
										__FILE__, __LINE__ ), exit( 1 )

#define WRONGPARAM				fprintf( stderr, \
									"%s, %d: Function called with wrong or " \
									"incomplete parameters, fix your call!\n", \
										__FILE__, __LINE__ )

#define MISSINGCASE				fprintf( stderr, \
									"%s, %d: Missing case enganged, " \
									"please check for correctness.\n", \
									__FILE__, __LINE__ )

#define TODO					fprintf( stderr, \
									"%s, %d: TODO alert! The program ran into "\
									"a module that is not finished yet!\n", \
											__FILE__, __LINE__ )


#ifndef _WIN32
#define PDIRSEP					':'
#define PPATHSEP				'/'
#else
#define PDIRSEP					';'
#define PPATHSEP				'\\'
#endif






#ifndef _DBG_H
#define _DBG_H


#ifdef DEBUG
	#define PROC( name ) \
		static char*	_dbg_proc_name	= name; \
		_dbg_trace( __FILE__, __LINE__, "ENTRY", "%s", _dbg_proc_name )
#else
	#define PROC( name )
#endif


#ifdef DEBUG
	#define RETURN( val ) \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", "%s", _dbg_proc_name ); \
			return val; \
		} \
		while( 0 )
#else
	#define RETURN( val ) return val
#endif


#ifdef DEBUG
	#define VOIDRET \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", "%s", _dbg_proc_name ); \
			return; \
		} \
		while( 0 )
#else
	#define VOIDRET return
#endif


#ifdef DEBUG
	#define MSG( text ) \
		_dbg_trace( __FILE__, __LINE__, "MSG", "%s", text )
#else
	#define MSG( text )
#endif


#ifdef DEBUG
	#define VARS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "VARS", "%s = >" format "<", name, val )
#else
	#define VARS( name, format, val )
#endif


#ifdef DEBUG
	#define PARMS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "PARMS", \
			"%s = >" format "<", name, val )
#else
	#define PARMS( name, format, val )
#endif

#endif



#ifndef PARRAY_H
#define PARRAY_H

typedef struct
{
	void*	array;
	size_t	first;
	size_t	last;
	size_t	count;

	size_t	size;
	size_t	step;
} parray;

#endif



#ifndef PLIST_H
#define PLIST_H


typedef struct Plistel		plistel;
typedef struct Plist		plist;


struct Plistel
{
	char*					key;
	plist*					list;

	plistel*				prev;
	plistel*				next;

	plistel*				hashnext;
	plistel*				hashprev;
};


struct Plist
{
	int						flags;
#define PLIST_MOD_NONE		0	
#define PLIST_MOD_PTR		1	
#define PLIST_MOD_RECYCLE	2	
#define PLIST_MOD_AUTOSORT	4	
#define PLIST_MOD_EXTKEYS	8	
#define PLIST_MOD_PTRKEYS	16	
#define PLIST_MOD_UNIQUE	32	
#define PLIST_MOD_WCHAR		64	

	int						size;
	int						count;
	int						hashsize;
#define PLIST_DFT_HASHSIZE	64

	int						(*comparefn)( plist*, plistel*, plistel* );
	int						(*sortfn)( plist*, plistel*, plistel* );
	void					(*printfn)( plist* );

	plistel*				unused;

	plistel*				first;
	plistel*				last;
	plistel**				hash;
};





#define plist_for( l, e )	\
	for( (e) = plist_first( l ); (e); (e) = plist_next( e ) )

#endif 





#ifndef _XML_H
#define _XML_H

#define XML_BUFSIZE 1024	
#define XML_NAMEM	0x80	
#define XML_TXTM	0x40	
#define XML_DUP		0x20	

typedef struct xml*		XML_T;
struct xml
{
	char*		name;		
	char **	 attr;		
	char*		txt;		
	size_t		off;		
	int			line;		
	XML_T		next;		
	XML_T		sibling;	
	XML_T		ordered;	
	XML_T		child;		
	XML_T		parent;		
	short		flags;		
};

#define xml_next( xml )		\
	( ( xml ) ? xml->next : NULL )

#define xml_next_inorder( xml ) \
	((xml) ? xml->ordered : NULL)

#define xml_name( xml )	\
	( ( xml ) ? xml->name : NULL )

#define xml_txt( xml ) \
	( ( xml ) ? xml->txt : "" )
	
#define xml_line(xml) \
	( ( xml ) ? (xml)->line : 0 )

#define xml_new_d( name ) \
	xml_set_flag( xml_new( pstrdup(name) ),XML_NAMEM )

#define xml_add_child_d( xml, name, off ) \
	xml_set_flag \
	( \
		xml_add_child( xml, pstrdup(name), off ), \
		XML_NAMEM \
	)

#define xml_set_txt_d( xml, txt ) \
	xml_set_flag \
	( \
		xml_set_txt( xml, pstrdup(txt) ), \
		XML_TXTM \
	)

#define xml_set_attr_d( xml, name, value ) \
	xml_set_attr \
	( \
		xml_set_flag( xml, XML_DUP ), \
		pstrdup( name ), \
		pstrdup( value ) \
	)
	
#define xml_add_child_f( xml, name, off ) \
	xml_set_flag \
	( \
		xml_add_child( xml, name, off ), \
		XML_NAMEM \
	)
	
#define xml_set_txt_f( xml, txt ) \
	xml_set_flag \
	( \
		xml_set_txt( xml, txt ), \
		XML_TXTM \
	)

#define xml_set_attr_f( xml, name, value ) \
	xml_set_attr \
	( \
		xml_set_flag( xml, XML_DUP ), \
		pstrdup( name ), \
		(value) \
	)

#define xml_move( xml, dest, off )	\
	xml_insert( xml_cut( xml ),dest,off )

#define xml_remove( xml ) \
	xml_free( xml_cut( xml ) )

#endif 





#define PCCL_MIN			0x0
#if UNICODE
#define PCCL_MAX			0xFFFF
#else
#define PCCL_MAX			0xFF
#endif

typedef struct
{
	wchar_t			begin;
	wchar_t			end;
} pcrange;

typedef struct
{
	int				min;
	int				max;

	plist*			ranges;

	char*			str;
} pccl;





#define PREGEX_ACCEPT_NONE		-1

#define PREGEX_ALLOC_STEP		16


#define PREGEX_STAT_NONE		0
#define PREGEX_STAT_NFA			1
#define PREGEX_STAT_DFA			2


#define PREGEX_MOD_NONE			0		
#define PREGEX_MOD_WCHAR		1		
#define PREGEX_MOD_INSENSITIVE	2		
#define PREGEX_MOD_GLOBAL		4		
#define PREGEX_MOD_STATIC		8		
#define PREGEX_MOD_NO_REF		16		
#define PREGEX_MOD_NO_ERRORS	32		
#define PREGEX_MOD_NO_ANCHORS	64		
#define PREGEX_MOD_GREEDY		128		
#define PREGEX_MOD_NONGREEDY	256		
#define PREGEX_MOD_DEBUG		1024 	


#define PREGEX_ANCHOR_NONE		0	
#define PREGEX_ANCHOR_BOL		1	
#define PREGEX_ANCHOR_EOL		2	
#define PREGEX_ANCHOR_BOW		4	
#define PREGEX_ANCHOR_EOW		8	


enum _regex_ptntype
{
	PREGEX_PTN_NULL,
	PREGEX_PTN_CHAR,
	PREGEX_PTN_SUB,
	PREGEX_PTN_ALT,
	PREGEX_PTN_KLE,
	PREGEX_PTN_POS,
	PREGEX_PTN_OPT
};


typedef struct	_regex_accept	pregex_accept;

typedef struct	_regex_nfa_st	pregex_nfa_st;
typedef struct	_regex_nfa		pregex_nfa;

typedef struct	_regex_dfa_tr	pregex_dfa_tr;
typedef struct	_regex_dfa_st	pregex_dfa_st;
typedef struct	_regex_dfa		pregex_dfa;

typedef enum 	_regex_ptntype	pregex_ptntype;
typedef struct	_regex_ptn		pregex_ptn;

typedef struct	_regex			pregex;
typedef	struct	_regex_range	pregex_range;

typedef struct 	_regex_in		pregex_in;


typedef	int 					(*pregex_fn)( pregex*, pregex_range* );
#define PREGEX_FN_NULL			( (pregex_fn)NULL )


struct _regex_accept
{
	int				accept;		
	pboolean		greedy;		
	int				anchors;	
};


struct _regex_nfa_st
{
	pccl*			ccl;		
	pregex_nfa_st*	next;		
	pregex_nfa_st*	next2;		

	int				ref;		

	pregex_accept	accept;		
};


struct _regex_nfa
{
	plist*			states;		
	int				modifiers;	

	int				ref_count;	
	int				ref_cur;	
};


struct _regex_dfa_tr
{
	pccl*			ccl;		
	unsigned int	go_to;		
};


struct _regex_dfa_st
{
	plist*			trans;		
	pregex_dfa_tr*	def_trans;	
	int*			ref;		
	int				ref_cnt;	

	pregex_accept	accept;		

	pboolean		done;		
	plist*			nfa_set;	
};


struct _regex_dfa
{
	plist*			states;		
	int				ref_count;	
};



struct _regex_ptn
{
	pregex_ptntype	type;		
	pccl*			ccl;		

	pregex_ptn*		child[ 2 ];	
	pregex_ptn*		next;		

	pregex_accept*	accept;		
};

struct _regex_range
{
	char*			begin;		
	wchar_t*		pbegin;		
	char*			end;		
	wchar_t*		pend;		
	size_t			pos;		
	size_t			len;		
	int				accept;		
	void*			user;		
};


struct _regex
{
	

	short			stat;		
	int				flags;		

	plist*			patterns;	

	union
	{
		pregex_nfa*	nfa;		
		pregex_dfa*	dfa;		
	} machine;

	

	int				age;		

	pregex_fn		match_fn;	

	
	int				last_err;	
	int				match_count;
	int				last_age;	

	char*			last_str;	
	char*			last_pos;	

	pregex_range	range;		
	pregex_range	split;		

	pregex_range*	refs;		
	int				refs_cnt;	

	char*			tmp_str;	
};





#ifndef PGVALUETYPE_H
#define PGVALUETYPE_H


typedef enum
{
	PGVALUETYPE_NULL,

	
	PGVALUETYPE_CHAR,
	PGVALUETYPE_INT,
	PGVALUETYPE_LONG,
	PGVALUETYPE_ULONG,
	PGVALUETYPE_FLOAT,
	PGVALUETYPE_DOUBLE,

	
	PGVALUETYPE_STRING,
	PGVALUETYPE_WSTRING,

	
	PGVALUETYPE_PTR
} pgvaluetype;


#define PGVALUEFLAG_CONSTANT	16	
#define PGVALUEFLAG_AUTOCONVERT	32	


typedef struct
{
	pgvaluetype	type;				
	short		flags;				

	union
	{
		char	c;
		

		int	i;
		

		long	l;
		

		ulong	ul;
		

		float	f;
		

		double	d;
		

		char*	s;
		
		

		wchar_t*	ws;
		
		

		void*	ptr;
		
	} val;
} pgvalue;


#define pg_value_set_string_d( val, str ) \
			pg_value_set_string( val, pstrdup( str ) )
#define pg_value_set_wstring_d( val, str ) \
			pg_value_set_wstring( val, pwcsdup( str ) )

#endif






typedef enum
{
	PPSYMTYPE_NONTERM,
	PPSYMTYPE_CCL,
	PPSYMTYPE_STRING,
	PPSYMTYPE_REGEX,
	PPSYMTYPE_SPECIAL
} ppsymtype;

typedef struct _ppsym		ppsym;
typedef struct _ppprod		ppprod;
typedef struct _ppgram		ppgram;

#define PPFLAG_CALLED		1
#define PPFLAG_DEFINED		2
#define PPFLAG_NULLABLE		4
#define PPFLAG_LEFTREC		8
#define PPFLAG_EMIT			16
#define PPFLAG_LEXEM		32
#define PPFLAG_WHITESPACE	64
#define PPFLAG_PREVENTLREC	128

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

struct _ppprod
{
	int						id;
	ppsym*					lhs;
	plist*					rhs;

	int						flags;

	
	char*					strval;
};

struct _ppsym
{
	int						id;
	ppsymtype				type;

	char*					name;
	int						flags;

	
	plist*					first;
	plist*					prods;

	
	pccl*					ccl;
	char*					str;
	pregex_ptn*				ptn;
	pregex_nfa*				nfa;

	
	int						emit_id;

	
	char*					strval;
};

struct _ppgram
{
	plist*					symbols;
	plist*					prods;

	plist*					ws;
	ppsym*					goal;
	ppsym*					eof;

	int						flags;
};

typedef struct
{
	#define PPMATCH_BEGIN	1
	#define PPMATCH_END		2
	int						type;

	int						emit_id;
	ppsym*					sym;
	ppprod*					prod;

	char*					start;
	char*					end;

	int						line;
	int						col;
} ppmatch;




#ifdef __cplusplus
extern "C"
{
#endif



pboolean parray_init( parray* array, size_t size, size_t step );
parray* parray_create( size_t size, size_t step );
pboolean parray_erase( parray* array );
parray* parray_free( parray* array );
void* parray_push( parray* array, void* item );
void* parray_malloc( parray* array );
void* parray_rmalloc( parray* array );
void* parray_insert( parray* array, size_t offset, void* item );
void* parray_remove( parray* array, size_t offset, void** item );
void* parray_pop( parray* array );
void* parray_unshift( parray* array, void* item );
void* parray_shift( parray* array );
void* parray_get( parray* array, size_t offset );
void* parray_put( parray* array, size_t offset, void* item );
void* parray_rget( parray* array, size_t offset );
void* parray_rput( parray* array, size_t offset, void* item );
void* parray_last( parray* array );
void* parray_first( parray* array );
void* parray_swap( parray* array, size_t pos1, size_t pos2 );
size_t parray_count( parray* array );
pboolean parray_partof( parray* array, void* ptr );
size_t parray_offset( parray* array, void* ptr );


void _dbg_trace( char* file, int line, char* type, char* format, ... );


pboolean plist_init( plist* list, size_t size, int flags );
plist* plist_create( size_t size, int flags );
plist* plist_dup( plist* list );
pboolean plist_erase( plist* list );
pboolean plist_clear( plist* list );
plist* plist_free( plist* list );
plistel* plist_insert( plist* list, plistel* pos, char* key, void* src );
plistel* plist_push( plist* list, void* src );
void* plist_malloc( plist* list );
#if 0
pboolean plist_preallocate( plist* list, int n, int chunk );
#endif
pboolean plist_remove( plist* list, plistel* e );
pboolean plist_pop( plist* list, void* dest );
pboolean plist_take( plist* list, void* dest );
plistel* plist_get( plist* list, int n );
plistel* plist_get_by_key( plist* list, char* key );
plistel* plist_get_by_ptr( plist* list, void* ptr );
int plist_union( plist* all, plist* from );
int plist_diff( plist* left, plist* right );
pboolean plist_subsort( plist* list, plistel* from, plistel* to );
pboolean plist_sort( plist* list );
pboolean plist_set_comparefn( plist* list, int (*comparefn)( plist*, plistel*, plistel* ) );
pboolean plist_set_sortfn( plist* list, int (*sortfn)( plist*, plistel*, plistel* ) );
pboolean plist_set_printfn( plist* list, void (*printfn)( plist* ) );
void* plist_access( plistel* e );
char* plist_key( plistel* e );
plistel* plist_next( plistel* u );
plistel* plist_prev( plistel* u );
plistel* plist_hashnext( plistel* u );
plistel* plist_hashprev( plistel* u );
int plist_offset( plistel* u );
pboolean plist_swap( plistel* a, plistel* b );
plistel* plist_first( plist* l );
plistel* plist_last( plist* l );
int plist_size( plist* l );
int plist_count( plist* l );


void* pmalloc( size_t size );
void* prealloc( void* oldptr, size_t size );
void* pfree( void* ptr );
void* pmemdup( void* ptr, size_t size );


pccl* p_ccl_create( int min, int max, char* ccldef );
pboolean p_ccl_compat( pccl* l, pccl* r );
int p_ccl_size( pccl* ccl );
int p_ccl_count( pccl* ccl );
pccl* p_ccl_dup( pccl* ccl );
pboolean p_ccl_testrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_test( pccl* ccl, wchar_t ch );
pboolean p_ccl_instest( pccl* ccl, wchar_t ch );
pboolean p_ccl_addrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_add( pccl* ccl, wchar_t ch );
pboolean p_ccl_delrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_del( pccl* ccl, wchar_t ch );
pboolean p_ccl_negate( pccl* ccl );
pccl* p_ccl_union( pccl* ccl, pccl* add );
pccl* p_ccl_diff( pccl* ccl, pccl* rem );
int p_ccl_compare( pccl* left, pccl* right );
pccl* p_ccl_intersect( pccl* ccl, pccl* within );
pboolean p_ccl_get( wchar_t* from, wchar_t* to, pccl* ccl, int offset );
pboolean p_ccl_parse( pccl* ccl, char* ccldef, pboolean extend );
pboolean p_ccl_erase( pccl* ccl );
pccl* p_ccl_free( pccl* ccl );
char* p_ccl_to_str( pccl* ccl, pboolean escape );
void p_ccl_print( FILE* stream, pccl* ccl, int break_after );


void pregex_dfa_print( pregex_dfa* dfa );
pregex_dfa* pregex_dfa_create( void );
pboolean pregex_dfa_reset( pregex_dfa* dfa );
pregex_dfa* pregex_dfa_free( pregex_dfa* dfa );
int pregex_dfa_from_nfa( pregex_dfa* dfa, pregex_nfa* nfa );
int pregex_dfa_minimize( pregex_dfa* dfa );
int pregex_dfa_match( pregex_dfa* dfa, char* str, size_t* len, int* anchors, pregex_range** ref, int* ref_count, int flags );
int pregex_dfa_to_matrix( wchar_t*** matrix, pregex_dfa* dfa );


int pregex_qmatch( char* regex, char* str, int flags, pregex_range** results );
int pregex_qsplit( char* regex, char* str, int flags, pregex_range** results );
char* pregex_qreplace( char* regex, char* str, char* replace, int flags );


pregex_accept* pregex_accept_init( pregex_accept* accept );
pboolean pregex_check_anchors( char* all, char* str, size_t len, int anchors, int flags );


pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, char* chardef, int flags );
void pregex_nfa_print( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_create( void );
pboolean pregex_nfa_reset( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_free( pregex_nfa* nfa );
int pregex_nfa_move( pregex_nfa* nfa, plist* hits, wchar_t from, wchar_t to );
int pregex_nfa_epsilon_closure( pregex_nfa* nfa, plist* closure, pregex_accept* accept );
int pregex_nfa_match( pregex_nfa* nfa, char* str, size_t* len, int* anchors, pregex_range** ref, int* ref_count, int flags );
int pregex_nfa_from_string( pregex_nfa* nfa, char* str, int flags, int acc );


pregex_ptn* pregex_ptn_create_char( pccl* ccl );
pregex_ptn* pregex_ptn_create_string( char* str, int flags );
pregex_ptn* pregex_ptn_create_sub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_alt( pregex_ptn* left, ... );
pregex_ptn* pregex_ptn_create_kle( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_pos( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_opt( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_seq( pregex_ptn* first, ... );
pregex_ptn* pregex_ptn_free( pregex_ptn* ptn );
void pregex_ptn_print( pregex_ptn* ptn, int rec );
int pregex_ptn_to_regex( char** regex, pregex_ptn* ptn );
int pregex_ptn_to_nfa( pregex_nfa* nfa, pregex_ptn* pattern );
int pregex_ptn_parse( pregex_ptn** ptn, char* str, int flags );


pregex* pregex_create( void );
pregex* pregex_free( pregex* regex );
pregex* pregex_reset( pregex* regex );
int pregex_compile( pregex* regex, char* pattern, int accept );
int pregex_finalize( pregex* regex );
pregex_range* pregex_match_next( pregex* regex, char* str );
int pregex_match( pregex* regex, char* str, pregex_range** results );
pregex_range* pregex_split_next( pregex* regex, char* str );
int pregex_split( pregex* regex, char* str, pregex_range** results );
char* pregex_replace( pregex* regex, char* str, char* replacement );
pregex_range* pregex_get_range( pregex* regex );
pregex_range* pregex_get_split( pregex* regex );
pregex_range* pregex_get_ref( pregex* regex, int offset );
int pregex_get_match_count( pregex* regex );
int pregex_get_flags( pregex* regex );
pboolean pregex_set_flags( pregex* regex, int flags );
pregex_fn pregex_get_match_fn( pregex* regex );
pboolean pregex_set_match_fn( pregex* regex, pregex_fn match_fn );


char* pregex_range_to_string( pregex_range* range );


int pregex_ref_init( pregex_range** ref, int* ref_count, int ref_all, int flags );
void pregex_ref_update( pregex_range* ref, char* strp, size_t off );


char* pwcs_to_str( wchar_t* str, pboolean freestr );
wchar_t* pstr_to_wcs( char* str, pboolean freestr );
char* pdbl_to_str( double d );
wchar_t* pdbl_to_wcs( double d );


char* pstrcatchar( char* str, wchar_t chr );
char* pstrcatstr( char* dest, char* src, boolean freesrc );
char* pstrncatstr( char* str, char* append, size_t n );
char* pstrreplace( char* str, char* find, char* replace );
char* pstrdup( char* str );
char* pstrndup( char* str, size_t len );
size_t pstrlen( char* str );
int psprintf( char* res, char* fmt, ... );
int pvasprintf( char** str, char* fmt, va_list ap );
char* pasprintf( char* fmt, ... );
char* psetstr( char** str, char* val );
char* pgetstr( char* str );
char* pstrrender( char* tpl, ... );
char* pstrltrim( char* s );
char* pstrrtrim( char* s );
char* pstrtrim( char* s );
int pstrsplit( char*** tokens, char* str, char* sep, int limit );
char* pstrupr( char* s );
char* pstrlwr( char* s );
int pstrcasecmp( char* s1, char* s2 );
int pstrncasecmp( char* s1, char* s2, int n );
int pstrparsechar( wchar_t* retc, char *str, pboolean escapeseq );
#ifdef UNICODE
wchar_t* pwcsdup( wchar_t* str );
int pwcsprintf( wchar_t* res, wchar_t* fmt, ... );
int pvawcsprintf( wchar_t** str, wchar_t* fmt, va_list ap );
wchar_t* pawcsprintf( wchar_t* fmt, ... );
wchar_t* pwcscatchar( wchar_t* str, wchar_t chr );
wchar_t* pwcscatstr( wchar_t* dest, wchar_t* src, boolean freesrc );
wchar_t* pwcsncatstr( wchar_t* str, wchar_t* append, size_t num );
size_t pwcslen( wchar_t* str );
wchar_t* pwcsndup( wchar_t* str, size_t len );
#endif


pboolean u8_isutf( unsigned char c );
int u8_seqlen(char *s);
wchar_t u8_char( char* str );
char* u8_move( char* str, int count );
wchar_t u8_parse_char( char** ch );
int u8_toucs(wchar_t *dest, int sz, char *src, int srcsz);
int u8_toutf8(char *dest, int sz, wchar_t *src, int srcsz);
int u8_wc_toutf8(char *dest, wchar_t ch);
int u8_offset(char *str, int charnum);
int u8_charnum(char *s, int offset);
int u8_strlen(char *s);
wchar_t u8_nextchar(char *s, int *i);
void u8_inc(char *s, int *i);
void u8_dec(char *s, int *i);
int octal_digit(char c);
int hex_digit(char c);
int u8_read_escape_sequence(char *str, wchar_t *dest);
int u8_unescape(char *buf, int sz, char *src);
int u8_escape_wchar(char *buf, int sz, wchar_t ch);
int u8_escape(char *buf, int sz, char *src, int escape_quotes);
char *u8_strchr(char *s, wchar_t ch, int *charn);
char *u8_memchr(char *s, wchar_t ch, size_t sz, int *charn);
int u8_is_locale_utf8(char *locale);


char* pwhich( char* filename, char* directories );
char* pbasename( char* path );
pboolean pfileexists( char* filename );
int map_file( char** cont, char* filename );
int pgetopt( char* opt, char** param, int* next, int argc, char** argv, char* optstr, char* loptstr, int idx );


XML_T xml_child( XML_T xml, char* name );
XML_T xml_idx( XML_T xml, int idx );
char* xml_attr( XML_T xml, char* attr );
long xml_int_attr( XML_T xml, char* attr );
double xml_float_attr( XML_T xml, char* attr );
XML_T xml_vget( XML_T xml, va_list ap );
XML_T xml_get( XML_T xml, ... );
char ** xml_pi( XML_T xml, char* target );
char* xml_decode( char* s, char ** ent, char t );
char* xml_str2utf8( char ** s, size_t* len );
void xml_free_attr( char ** attr );
XML_T xml_parse_str( char* s, size_t len );
XML_T xml_parse_fp( FILE* fp );
XML_T xml_parse_file( char* file );
char* xml_ampencode( char* s, size_t len, char ** dst, size_t* dlen, size_t* max, short a );
char* xml_toxml( XML_T xml );
void xml_free( XML_T xml );
char* xml_error( XML_T xml );
XML_T xml_new( char* name );
XML_T xml_insert( XML_T xml, XML_T dest, size_t off );
XML_T xml_add_child( XML_T xml, char* name, size_t off );
XML_T xml_set_txt( XML_T xml, char* txt );
XML_T xml_set_attr( XML_T xml, char* name, char* value );
XML_T xml_set_int_attr( XML_T xml, char* name, long value );
XML_T xml_set_float_attr( XML_T xml, char* name, double value );
XML_T xml_set_flag( XML_T xml, short flag );
int xml_count( XML_T xml );
int xml_count_all( XML_T xml );
XML_T xml_cut( XML_T xml );


void pp_ast_print( parray* ast );
void pp_ast_tree2svg( parray* ast );


ppsym* pp_sym_create( ppgram* g, ppsymtype type, char* name, char* def );
char* pp_sym_to_str( ppsym* sym );
ppprod* pp_prod_create( ppgram* g, ppsym* lhs, ... );
pboolean pp_prod_append( ppprod* p, ppsym* sym );
ppsym* pp_prod_getfromrhs( ppprod* p, int off );
char* pp_prod_to_str( ppprod* p );
ppgram* pp_gram_create( char* def );
void pp_gram_print( ppgram* g );
ppgram* pp_gram_free( ppgram* g );


pboolean pp_ll_parse( parray* ast, ppgram* grm, char* start, char** end );


plist* pp_parser_lr_closure( ppgram* gram, pboolean optimize );
pboolean pp_lr_parse( parray* ast, ppgram* grm, char* start, char** end );


pboolean pp_sym_in_input( ppsym* sym, char* start, char** end );
pboolean pp_white_in_input( ppgram* grm, char* start, char** end );


pboolean pg_value_init( pgvalue* val );
pgvalue* pg_value_create( void );
pboolean pg_value_reset( pgvalue* val );
pgvalue* pg_value_free( pgvalue* val );
pboolean pg_value_set_constant( pgvalue* val, pboolean constant );
pboolean pg_value_get_constant( pgvalue* val );
pboolean pg_value_set_autoconvert( pgvalue* val, pboolean autoconvert );
pboolean pg_value_get_autoconvert( pgvalue* val );


char pg_value_to_char( pgvalue* val );
int pg_value_to_int( pgvalue* val );
long pg_value_to_long( pgvalue* val );
ulong pg_value_to_ulong( pgvalue* val );
float pg_value_to_float( pgvalue* val );
double pg_value_to_double( pgvalue* val );
char* pg_value_to_string( pgvalue* val );
wchar_t* pg_value_to_wstring( pgvalue* val );
void* pg_value_to_ptr( pgvalue* val );
pboolean pg_value_convert( pgvalue* val, pgvaluetype type );


char pg_value_get_char( pgvalue* val );
int pg_value_get_int( pgvalue* val );
long pg_value_get_long( pgvalue* val );
ulong pg_value_get_ulong( pgvalue* val );
float pg_value_get_float( pgvalue* val );
double pg_value_get_double( pgvalue* val );
char* pg_value_get_cstring( pgvalue* val );
char* pg_value_get_string( pgvalue* val );
wchar_t* pg_value_get_wcstring( pgvalue* val );
wchar_t* pg_value_get_wstring( pgvalue* val );
void* pg_value_get_ptr( pgvalue* val );


char pg_value_set_char( pgvalue* val, char c );
int pg_value_set_int( pgvalue* val, int i );
long pg_value_set_long( pgvalue* val, long l );
ulong pg_value_set_ulong( pgvalue* val, ulong ul );
float pg_value_set_float( pgvalue* val, float f );
double pg_value_set_double( pgvalue* val, double d );
char* pg_value_set_cstring( pgvalue* val, char* s );
char* pg_value_set_string( pgvalue* val, char* s );
wchar_t* pg_value_set_wcstring( pgvalue* val, wchar_t* ws );
wchar_t* pg_value_set_wstring( pgvalue* val, wchar_t* ws );
void* pg_value_set_ptr( pgvalue* val, void* ptr );


#ifdef __cplusplus
}
#endif

#endif 

