/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	regex.h
Author:	Jan Max Meyer
Usage:	Header for the pregex object and functions.
----------------------------------------------------------------------------- */

/* Defines */
#define PREGEX_ALLOC_STEP		16
#define PREGEX_MAXREF			32

/* Regex flags */

/* ---> Part I: Compile-time Flags */
#define PREGEX_COMP_WCHAR		0x01	/* 	Regular expression is provided
											in wchar_t. */
#define PREGEX_COMP_NOANCHORS	0x02	/* 	Ignore anchor tokens, handle them
											as normal characters */
#define PREGEX_COMP_NOREF		0x04	/* 	Don't compile references. */
#define PREGEX_COMP_NONGREEDY	0x08	/* 	Compile pattern to be nongreedy */
#define PREGEX_COMP_NOERRORS	0x10	/*	Don't report errors, and try to
											compile as much as possible */
#define PREGEX_COMP_INSENSITIVE	0x20	/* 	Regular expression is case
											insensitive */
#define PREGEX_COMP_STATIC		0x40	/*	The regular expression passed
											to the compiler should be converted
											1:1 as it where a string-constant.
											Any regex-specific symbols will
											be ignored. */
#define PREGEX_COMP_PTN			0x80	/*	The regular expression passed
											already is a pattern, and shall be
											integrated. */

/* ---> Part II: Runtime flags */
#define PREGEX_RUN_WCHAR		0x100	/*	Run regular expression with
											wchar_t as input */
#define	PREGEX_RUN_UCHAR		0x200	/*  Handle input as unsigned char */
#define PREGEX_RUN_NOANCHORS	0x400	/* 	Ignore anchors while processing */
#define PREGEX_RUN_NOREF		0x800	/*	Don't create references */
#define PREGEX_RUN_NONGREEDY	0x1000	/*	Run regular expression nongreedy */
#define PREGEX_RUN_DEBUG		0x2000 	/*	Debug mode; output some debug to
											stderr */

/* Matching flags */
#define PREGEX_FLAG_NONE		0x00	/* No flags defined */
#define PREGEX_FLAG_BOL			0x01	/* Match at begin of line only */
#define PREGEX_FLAG_EOL			0x02	/* Match at end of line only */
#define PREGEX_FLAG_BOW			0x04	/* Match at begin of word only */
#define PREGEX_FLAG_EOW			0x08	/* Match at end of word only */
#define PREGEX_FLAG_NONGREEDY	0x10	/* Nongreedy match, overwrite mode. */

/* Regular Expression pattern types */
enum _regex_ptntype
{
	PREGEX_PTN_NULL,
	PREGEX_PTN_CHAR,
	PREGEX_PTN_SUB,
	PREGEX_PTN_REFSUB,
	PREGEX_PTN_ALT,
	PREGEX_PTN_KLE,
	PREGEX_PTN_POS,
	PREGEX_PTN_OPT
};

/* Typedefs */
typedef struct	_regex_nfa_st	pregex_nfa_st;
typedef struct	_regex_nfa		pregex_nfa;

typedef struct	_regex_dfa_tr	pregex_dfa_tr;
typedef struct	_regex_dfa_st	pregex_dfa_st;
typedef struct	_regex_dfa		pregex_dfa;

typedef enum 	_regex_ptntype	pregex_ptntype;
typedef struct	_regex_ptn		pregex_ptn;

typedef struct	_range			prange;
typedef struct	_regex			pregex;
typedef struct	_lex			plex;


/*
 * Internal Structures
 */

/* NFA state */
struct _regex_nfa_st
{
	pccl*			ccl;		/* Char-class; if ccl == (pccl*)NULL,
									then this is an epsilon edge */
	pregex_nfa_st*	next;		/* First following NFA-state */
	pregex_nfa_st*	next2;		/* Second following NFA-state */

	unsigned int	accept;		/* Accepting state */
	int				flags;		/* State flagging */
	int				refs;		/* References flags */
};

/* NFA state machine */
struct _regex_nfa
{
	plist*			states;		/* NFA states */
	int				modifiers;	/* Regex-modifiers */
};

/* DFA transition */
struct _regex_dfa_tr
{
	pccl*			ccl;		/* Matching character range */
	unsigned int	go_to;		/* Go-To state */
};

/* DFA state */
struct _regex_dfa_st
{
	plist*			trans;		/* Transition table row for this DFA state */
	pregex_dfa_tr*	def_trans;	/* If not NULL, this points to a default dfa
									transition, which covers the most characters
									of the entire character range */

	unsigned int	accept;		/* Accepting state */
	int				flags;		/* State flagging */
	int				refs;		/* References flags */
};

/* DFA state machine */
struct _regex_dfa
{
	plist*			states;		/* List of dfa-states */
};

/*
 * Patterns
 */

struct _regex_ptn
{
	pregex_ptntype	type;		/* Pattern state element type */
	pccl*			ccl;		/* Character-class for PREGEX_PTN_CHAR */

	pregex_ptn*		child[ 2 ];	/* Links to children pattern(s) */
	pregex_ptn*		next;		/* Next sequence element */

	unsigned int	accept;		/* Accepting state */
	int				flags;		/* State flagging */

	char*			str;		/* Holds the string representation generated by
									pregex_ptn_to_regex() */
};

/*
 * Userspace Objects
 */

/* A pattern matching range */
struct _range
{
	unsigned int	id;			/* Match ID */
	char*			start;		/* Begin pointer */
	char*			end;		/* End pointer */
};

/* The pregex object structure */
struct _regex
{
	int				flags;		/* Flags */
	pregex_ptn*		ptn;		/* Pattern */

	int				trans_cnt;	/* Counts of DFA states */
	wchar_t**		trans;		/* DFA transitions */

	prange			ref			[ PREGEX_MAXREF ];
};

/* The plex object structure */

struct _lex
{
	int				flags;		/* Flags */
	plist*			ptns;		/* Patterns */

	int				trans_cnt;	/* Counts of DFA states */
	wchar_t**		trans;		/* DFA transitions */

	prange			ref			[ PREGEX_MAXREF ];
};

