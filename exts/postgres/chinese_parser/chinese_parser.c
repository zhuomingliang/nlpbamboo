/*
 * Copyright (c) 2008, detrox@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY detrox@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL detrox@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*-------------------------------------------------------------------------
 *
 * test_parser.c
 *	  Simple example of a text search parser
 *
 * Copyright (c) 2007-2008, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  $PostgreSQL: pgsql/contrib/test_parser/test_parser.c,v 1.4 2008/01/01 20:31:21 tgl Exp $
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "commands/defrem.h"
#include "tsearch/ts_locale.h"
#include "tsearch/ts_public.h"
#include "tsearch/ts_type.h"
#include "tsearch/ts_utils.h"
#include "utils/builtins.h"

#include "bamboo.hxx"

/* Output token categories */

#define ASCIIWORD		1
#define WORD_T			2
#define NUMWORD			3
#define EMAIL			4
#define URL_T			5
#define HOST			6
#define SCIENTIFIC		7
#define VERSIONNUMBER	8
#define NUMPARTHWORD	9
#define PARTHWORD		10
#define ASCIIPARTHWORD	11
#define SPACE			12
#define TAG_T			13
#define PROTOCOL		14
#define NUMHWORD		15
#define ASCIIHWORD		16
#define HWORD			17
#define URLPATH			18
#define FILEPATH		19
#define DECIMAL			20
#define SIGNEDINT		21
#define UNSIGNEDINT		22
#define XMLENTITY		23

#define LASTNUM			23

/* Parser states */

typedef enum
{
	TPS_Base = 0,
	TPS_InNumWord,
	TPS_InAsciiWord,
	TPS_InWord,
	TPS_InUnsignedInt,
	TPS_InSignedIntFirst,
	TPS_InSignedInt,
	TPS_InSpace,
	TPS_InUDecimalFirst,
	TPS_InUDecimal,
	TPS_InDecimalFirst,
	TPS_InDecimal,
	TPS_InVerVersion,
	TPS_InSVerVersion,
	TPS_InVersionFirst,
	TPS_InVersion,
	TPS_InMantissaFirst,
	TPS_InMantissaSign,
	TPS_InMantissa,
	TPS_InXMLEntityFirst,
	TPS_InXMLEntity,
	TPS_InXMLEntityNumFirst,
	TPS_InXMLEntityNum,
	TPS_InXMLEntityHexNumFirst,
	TPS_InXMLEntityHexNum,
	TPS_InXMLEntityEnd,
	TPS_InTagFirst,
	TPS_InXMLBegin,
	TPS_InTagCloseFirst,
	TPS_InTagName,
	TPS_InTagBeginEnd,
	TPS_InTag,
	TPS_InTagEscapeK,
	TPS_InTagEscapeKK,
	TPS_InTagBackSleshed,
	TPS_InTagEnd,
	TPS_InCommentFirst,
	TPS_InCommentLast,
	TPS_InComment,
	TPS_InCloseCommentFirst,
	TPS_InCloseCommentLast,
	TPS_InCommentEnd,
	TPS_InHostFirstDomain,
	TPS_InHostDomainSecond,
	TPS_InHostDomain,
	TPS_InPortFirst,
	TPS_InPort,
	TPS_InHostFirstAN,
	TPS_InHost,
	TPS_InEmail,
	TPS_InFileFirst,
	TPS_InFileTwiddle,
	TPS_InPathFirst,
	TPS_InPathFirstFirst,
	TPS_InPathSecond,
	TPS_InFile,
	TPS_InFileNext,
	TPS_InURLPathFirst,
	TPS_InURLPathStart,
	TPS_InURLPath,
	TPS_InFURL,
	TPS_InProtocolFirst,
	TPS_InProtocolSecond,
	TPS_InProtocolEnd,
	TPS_InHyphenAsciiWordFirst,
	TPS_InHyphenAsciiWord,
	TPS_InHyphenWordFirst,
	TPS_InHyphenWord,
	TPS_InHyphenNumWordFirst,
	TPS_InHyphenNumWord,
	TPS_InHyphenDigitLookahead,
	TPS_InParseHyphen,
	TPS_InParseHyphenHyphen,
	TPS_InHyphenWordPart,
	TPS_InHyphenAsciiWordPart,
	TPS_InHyphenNumWordPart,
	TPS_InHyphenUnsignedInt,
	TPS_Null					/* last state (fake value) */
} TParserState;

/* forward declaration */
struct TParser;

typedef int (*TParserCharTest) (struct TParser *);		/* any p_is* functions
														 * except p_iseq */
typedef void (*TParserSpecial) (struct TParser *);		/* special handler for
														 * special cases... */

typedef struct
{
	TParserCharTest isclass;
	char		c;
	uint16		flags;
	TParserState tostate;
	int			type;
	TParserSpecial special;
} TParserStateActionItem;

/* Flag bits in TParserStateActionItem.flags */
#define A_NEXT		0x0000
#define A_BINGO		0x0001
#define A_POP		0x0002
#define A_PUSH		0x0004
#define A_RERUN		0x0008
#define A_CLEAR		0x0010
#define A_MERGE		0x0020
#define A_CLRALL	0x0040

typedef struct TParserPosition
{
	int			posbyte;		/* position of parser in bytes */
	int			poschar;		/* position of parser in characters */
	int			charlen;		/* length of current char */
	int			lenbytetoken;	/* length of token-so-far in bytes */
	int			lenchartoken;	/* and in chars */
	TParserState state;
	struct TParserPosition *prev;
	const TParserStateActionItem *pushedAtAction;
} TParserPosition;

typedef struct TParser
{
	/* string and position information */
	char	   *str;			/* multibyte string */
	int			lenstr;			/* length of mbstring */
#ifdef TS_USE_WIDE
	wchar_t    *wstr;			/* wide character string */
	int			lenwstr;		/* length of wsting */
#endif

	/* State of parse */
	int			charmaxlen;
	bool		usewide;
	TParserPosition *state;
	bool		ignore;
	bool		wanthost;

	/* silly char */
	char		c;

	/* out */
	char	   *token;
	int			lenbytetoken;
	int			lenchartoken;
	int			type;

	/* trimmed */
	char 		*trimmed;
	char 		*cur;
	int			len;
	int			pos;

	/* segmented */
	char 		*seg;
	char 		*sege;
} TParser;


/* forward decls here */
static bool TParserGet(TParser *prs);


static TParserPosition *
newTParserPosition(TParserPosition *prev)
{
	TParserPosition *res = (TParserPosition *) palloc(sizeof(TParserPosition));

	if (prev)
		memcpy(res, prev, sizeof(TParserPosition));
	else
		memset(res, 0, sizeof(TParserPosition));

	res->prev = prev;

	res->pushedAtAction = NULL;

	return res;
}

static TParser *
TParserInit(char *str, int len)
{
	TParser    *prs = (TParser *) palloc0(sizeof(TParser));

	prs->charmaxlen = pg_database_encoding_max_length();
	prs->str = str;
	prs->lenstr = len;
	prs->trimmed = palloc0(len + 1);
	prs->cur = prs->trimmed;
	prs->seg = NULL;

#ifdef TS_USE_WIDE

	/*
	 * Use wide char code only when max encoding length > 1.
	 */
	if (prs->charmaxlen > 1)
	{
		prs->usewide = true;
		prs->wstr = (wchar_t *) palloc(sizeof(wchar_t) * (prs->lenstr + 1));
		prs->lenwstr = char2wchar(prs->wstr, prs->lenstr + 1,
								  prs->str, prs->lenstr);
	}
	else
#endif
		prs->usewide = false;

	prs->state = newTParserPosition(NULL);
	prs->state->state = TPS_Base;

#ifdef WPARSER_TRACE
	fprintf(stderr, "parsing \"%.*s\"\n", len, str);
#endif

	return prs;
}

static void
TParserClose(TParser *prs)
{
	while (prs->state)
	{
		TParserPosition *ptr = prs->state->prev;

		pfree(prs->state);
		prs->state = ptr;
	}

#ifdef TS_USE_WIDE
	if (prs->wstr)
		pfree(prs->wstr);
#endif
	
	if (prs->trimmed) 
		pfree(prs->trimmed);

	if (prs->sege) 
		free(prs->sege);
	
	pfree(prs);
}

/*
 * Character-type support functions, equivalent to is* macros, but
 * working with any possible encodings and locales. Note,
 * that with multibyte encoding and C-locale isw* function may fail
 * or give wrong result. Note 2: multibyte encoding and C-locale
 * often are used for Asian languages
 */

#ifdef TS_USE_WIDE

#define p_iswhat(type)														\
static int																	\
p_is##type(TParser *prs) {													\
	Assert( prs->state );													\
	if ( prs->usewide )														\
	{																		\
		if ( lc_ctype_is_c() )												\
			return is##type( 0xff & *( prs->wstr + prs->state->poschar) );	\
																			\
		return isw##type( *(wint_t*)( prs->wstr + prs->state->poschar ) );	\
	}																		\
																			\
	return is##type( *(unsigned char*)( prs->str + prs->state->posbyte ) ); \
}	\
																			\
static int																	\
p_isnot##type(TParser *prs) {												\
	return !p_is##type(prs);												\
}

static int
p_isalnum(TParser *prs)
{
	Assert(prs->state);

	if (prs->usewide)
	{
		if (lc_ctype_is_c())
		{
			unsigned int c = *(prs->wstr + prs->state->poschar);

			/*
			 * any non-ascii symbol with multibyte encoding with C-locale is
			 * an alpha character
			 */
			if (c > 0x7f)
				return 1;

			return isalnum(0xff & c);
		}

		return iswalnum((wint_t) *(prs->wstr + prs->state->poschar));
	}

	return isalnum(*(unsigned char *) (prs->str + prs->state->posbyte));
}
static int
p_isnotalnum(TParser *prs)
{
	return !p_isalnum(prs);
}

static int
p_isalpha(TParser *prs)
{
	Assert(prs->state);

	if (prs->usewide)
	{
		if (lc_ctype_is_c())
		{
			unsigned int c = *(prs->wstr + prs->state->poschar);

			/*
			 * any non-ascii symbol with multibyte encoding with C-locale is
			 * an alpha character
			 */
			if (c > 0x7f)
				return 1;

			return isalpha(0xff & c);
		}

		return iswalpha((wint_t) *(prs->wstr + prs->state->poschar));
	}

	return isalpha(*(unsigned char *) (prs->str + prs->state->posbyte));
}

static int
p_isnotalpha(TParser *prs)
{
	return !p_isalpha(prs);
}

/* p_iseq should be used only for ascii symbols */

static int
p_iseq(TParser *prs, char c)
{
	Assert(prs->state);
	return ((prs->state->charlen == 1 && *(prs->str + prs->state->posbyte) == c)) ? 1 : 0;
}
#else							/* TS_USE_WIDE */

#define p_iswhat(type)														\
static int																	\
p_is##type(TParser *prs) {													\
	Assert( prs->state );													\
	return is##type( (unsigned char)*( prs->str + prs->state->posbyte ) );	\
}	\
																			\
static int																	\
p_isnot##type(TParser *prs) {												\
	return !p_is##type(prs);												\
}


static int
p_iseq(TParser *prs, char c)
{
	Assert(prs->state);
	return (*(prs->str + prs->state->posbyte) == c) ? 1 : 0;
}

p_iswhat(alnum)
p_iswhat(alpha)
#endif   /* TS_USE_WIDE */

p_iswhat(digit)
p_iswhat(lower)
p_iswhat(print)
p_iswhat(punct)
p_iswhat(space)
p_iswhat(upper)
p_iswhat(xdigit)

static int
p_isEOF(TParser *prs)
{
	Assert(prs->state);
	return (prs->state->posbyte == prs->lenstr || prs->state->charlen == 0) ? 1 : 0;
}

static int
p_iseqC(TParser *prs)
{
	return p_iseq(prs, prs->c);
}

static int
p_isneC(TParser *prs)
{
	return !p_iseq(prs, prs->c);
}

static int
p_isascii(TParser *prs)
{
	return (prs->state->charlen == 1 && isascii((unsigned char) *(prs->str + prs->state->posbyte))) ? 1 : 0;
}

static int
p_isasclet(TParser *prs)
{
	return (p_isascii(prs) && p_isalpha(prs)) ? 1 : 0;
}


/* deliberately suppress unused-function complaints for the above */
void		_make_compiler_happy(void);
void
_make_compiler_happy(void)
{
	p_isalnum(NULL);
	p_isnotalnum(NULL);
	p_isalpha(NULL);
	p_isnotalpha(NULL);
	p_isdigit(NULL);
	p_isnotdigit(NULL);
	p_islower(NULL);
	p_isnotlower(NULL);
	p_isprint(NULL);
	p_isnotprint(NULL);
	p_ispunct(NULL);
	p_isnotpunct(NULL);
	p_isspace(NULL);
	p_isnotspace(NULL);
	p_isupper(NULL);
	p_isnotupper(NULL);
	p_isxdigit(NULL);
	p_isnotxdigit(NULL);
	p_isEOF(NULL);
	p_iseqC(NULL);
	p_isneC(NULL);
}


static void
SpecialTags(TParser *prs)
{
	switch (prs->state->lenchartoken)
	{
		case 8:			/* </script */
			if (pg_strncasecmp(prs->token, "</script", 8) == 0)
				prs->ignore = false;
			break;
		case 7:			/* <script || </style */
			if (pg_strncasecmp(prs->token, "</style", 7) == 0)
				prs->ignore = false;
			else if (pg_strncasecmp(prs->token, "<script", 7) == 0)
				prs->ignore = true;
			break;
		case 6:			/* <style */
			if (pg_strncasecmp(prs->token, "<style", 6) == 0)
				prs->ignore = true;
			break;
		case 4:
			if (pg_strncasecmp(prs->token, "quot", 4) == 0)
				prs->ignore = true;
			break;
		default:
			break;
	}
}

static void
SpecialFURL(TParser *prs)
{
	prs->wanthost = true;
	prs->state->posbyte -= prs->state->lenbytetoken;
	prs->state->poschar -= prs->state->lenchartoken;
}

static void
SpecialHyphen(TParser *prs)
{
	prs->state->posbyte -= prs->state->lenbytetoken;
	prs->state->poschar -= prs->state->lenchartoken;
}

static void
SpecialVerVersion(TParser *prs)
{
	prs->state->posbyte -= prs->state->lenbytetoken;
	prs->state->poschar -= prs->state->lenchartoken;
	prs->state->lenbytetoken = 0;
	prs->state->lenchartoken = 0;
}

static int
p_isstophost(TParser *prs)
{
	if (prs->wanthost)
	{
		prs->wanthost = false;
		return 1;
	}
	return 0;
}

static int
p_isignore(TParser *prs)
{
	return (prs->ignore) ? 1 : 0;
}

static int
p_ishost(TParser *prs)
{
	TParser    *tmpprs = TParserInit(prs->str + prs->state->posbyte, prs->lenstr - prs->state->posbyte);
	int			res = 0;

	if (TParserGet(tmpprs) && tmpprs->type == HOST)
	{
		prs->state->posbyte += tmpprs->lenbytetoken;
		prs->state->poschar += tmpprs->lenchartoken;
		prs->state->lenbytetoken += tmpprs->lenbytetoken;
		prs->state->lenchartoken += tmpprs->lenchartoken;
		prs->state->charlen = tmpprs->state->charlen;
		res = 1;
	}
	TParserClose(tmpprs);

	return res;
}

static int
p_isURLPath(TParser *prs)
{
	TParser    *tmpprs = TParserInit(prs->str + prs->state->posbyte, prs->lenstr - prs->state->posbyte);
	int			res = 0;

	tmpprs->state = newTParserPosition(tmpprs->state);
	tmpprs->state->state = TPS_InFileFirst;

	if (TParserGet(tmpprs) && (tmpprs->type == URLPATH || tmpprs->type == FILEPATH))
	{
		prs->state->posbyte += tmpprs->lenbytetoken;
		prs->state->poschar += tmpprs->lenchartoken;
		prs->state->lenbytetoken += tmpprs->lenbytetoken;
		prs->state->lenchartoken += tmpprs->lenchartoken;
		prs->state->charlen = tmpprs->state->charlen;
		res = 1;
	}
	TParserClose(tmpprs);

	return res;
}

/*
 * Table of state/action of parser
 */

static const TParserStateActionItem actionTPS_Base[] = {
	{p_isEOF, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '<', A_PUSH, TPS_InTagFirst, 0, NULL},
	{p_isignore, 0, A_NEXT, TPS_InSpace, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InAsciiWord, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InWord, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InUnsignedInt, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InSignedIntFirst, 0, NULL},
	{p_iseqC, '+', A_PUSH, TPS_InSignedIntFirst, 0, NULL},
	{p_iseqC, '&', A_PUSH, TPS_InXMLEntityFirst, 0, NULL},
	{p_iseqC, '~', A_PUSH, TPS_InFileTwiddle, 0, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFileFirst, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InPathFirstFirst, 0, NULL},
	{NULL, 0, A_NEXT, TPS_InSpace, 0, NULL}
};


static const TParserStateActionItem actionTPS_InNumWord[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, NUMWORD, NULL},
	{p_isalnum, 0, A_NEXT, TPS_InNumWord, 0, NULL},
	{p_iseqC, '@', A_PUSH, TPS_InEmail, 0, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFileFirst, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InFileNext, 0, NULL},
/*	{p_iseqC, '-', A_PUSH, TPS_InHyphenNumWordFirst, 0, NULL}, */
	{NULL, 0, A_BINGO, TPS_Base, NUMWORD, NULL}
};

static const TParserStateActionItem actionTPS_InAsciiWord[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, ASCIIWORD, NULL},
	{p_isasclet, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InHostFirstDomain, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InFileNext, 0, NULL},
/*	{p_iseqC, '-', A_PUSH, TPS_InHostFirstAN, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHyphenAsciiWordFirst, 0, NULL}, */
	{p_iseqC, '@', A_PUSH, TPS_InEmail, 0, NULL},
	{p_iseqC, ':', A_PUSH, TPS_InProtocolFirst, 0, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFileFirst, 0, NULL},
	{p_isdigit, 0, A_PUSH, TPS_InHost, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InNumWord, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InWord, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, ASCIIWORD, NULL}
};

static const TParserStateActionItem actionTPS_InWord[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, WORD_T, NULL},
	{p_isalpha, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InNumWord, 0, NULL},
/*	{p_iseqC, '-', A_PUSH, TPS_InHyphenWordFirst, 0, NULL}, */
	{NULL, 0, A_BINGO, TPS_Base, WORD_T, NULL}
};

static const TParserStateActionItem actionTPS_InUnsignedInt[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, UNSIGNEDINT, NULL},
	{p_isdigit, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InHostFirstDomain, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InUDecimalFirst, 0, NULL},
	{p_iseqC, 'e', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{p_iseqC, 'E', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{p_isasclet, 0, A_PUSH, TPS_InHost, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InNumWord, 0, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFileFirst, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, UNSIGNEDINT, NULL}
};

static const TParserStateActionItem actionTPS_InSignedIntFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT | A_CLEAR, TPS_InSignedInt, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InSignedInt[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, SIGNEDINT, NULL},
	{p_isdigit, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InDecimalFirst, 0, NULL},
	{p_iseqC, 'e', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{p_iseqC, 'E', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, SIGNEDINT, NULL}
};

static const TParserStateActionItem actionTPS_InSpace[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, SPACE, NULL},
	{p_iseqC, '<', A_BINGO, TPS_Base, SPACE, NULL},
	{p_isignore, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_BINGO, TPS_Base, SPACE, NULL},
	{p_iseqC, '+', A_BINGO, TPS_Base, SPACE, NULL},
	{p_iseqC, '&', A_BINGO, TPS_Base, SPACE, NULL},
	{p_iseqC, '/', A_BINGO, TPS_Base, SPACE, NULL},
	{p_isnotalnum, 0, A_NEXT, TPS_InSpace, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, SPACE, NULL}
};

static const TParserStateActionItem actionTPS_InUDecimalFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_CLEAR, TPS_InUDecimal, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InUDecimal[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, DECIMAL, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InUDecimal, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InVersionFirst, 0, NULL},
	{p_iseqC, 'e', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{p_iseqC, 'E', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, DECIMAL, NULL}
};

static const TParserStateActionItem actionTPS_InDecimalFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_CLEAR, TPS_InDecimal, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InDecimal[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, DECIMAL, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InDecimal, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InVerVersion, 0, NULL},
	{p_iseqC, 'e', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{p_iseqC, 'E', A_PUSH, TPS_InMantissaFirst, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, DECIMAL, NULL}
};

static const TParserStateActionItem actionTPS_InVerVersion[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_RERUN, TPS_InSVerVersion, 0, SpecialVerVersion},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InSVerVersion[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_BINGO | A_CLRALL, TPS_InUnsignedInt, SPACE, NULL},
	{NULL, 0, A_NEXT, TPS_Null, 0, NULL}
};


static const TParserStateActionItem actionTPS_InVersionFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_CLEAR, TPS_InVersion, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InVersion[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, VERSIONNUMBER, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InVersion, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InVersionFirst, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, VERSIONNUMBER, NULL}
};

static const TParserStateActionItem actionTPS_InMantissaFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_CLEAR, TPS_InMantissa, 0, NULL},
	{p_iseqC, '+', A_NEXT, TPS_InMantissaSign, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InMantissaSign, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InMantissaSign[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_CLEAR, TPS_InMantissa, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InMantissa[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, SCIENTIFIC, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InMantissa, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, SCIENTIFIC, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntityFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '#', A_NEXT, TPS_InXMLEntityNumFirst, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, ':', A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_InXMLEntity, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntity[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isalnum, 0, A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, ':', A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, '.', A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InXMLEntity, 0, NULL},
	{p_iseqC, ';', A_NEXT, TPS_InXMLEntityEnd, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntityNumFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, 'x', A_NEXT, TPS_InXMLEntityHexNumFirst, 0, NULL},
	{p_iseqC, 'X', A_NEXT, TPS_InXMLEntityHexNumFirst, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InXMLEntityNum, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntityHexNumFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isxdigit, 0, A_NEXT, TPS_InXMLEntityHexNum, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntityNum[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InXMLEntityNum, 0, NULL},
	{p_iseqC, ';', A_NEXT, TPS_InXMLEntityEnd, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntityHexNum[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isxdigit, 0, A_NEXT, TPS_InXMLEntityHexNum, 0, NULL},
	{p_iseqC, ';', A_NEXT, TPS_InXMLEntityEnd, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLEntityEnd[] = {
	{NULL, 0, A_BINGO | A_CLEAR, TPS_Base, XMLENTITY, NULL}
};

static const TParserStateActionItem actionTPS_InTagFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InTagCloseFirst, 0, NULL},
	{p_iseqC, '!', A_PUSH, TPS_InCommentFirst, 0, NULL},
	{p_iseqC, '?', A_PUSH, TPS_InXMLBegin, 0, NULL},
	{p_isasclet, 0, A_PUSH, TPS_InTagName, 0, NULL},
	{p_iseqC, ':', A_PUSH, TPS_InTagName, 0, NULL},
	{p_iseqC, '_', A_PUSH, TPS_InTagName, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InXMLBegin[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	/* <?xml ... */
    /* XXX do we wants states for the m and l ?  Right now this accepts <?xZ */
	{p_iseqC, 'x', A_NEXT, TPS_InTag, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagCloseFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InTagName, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagName[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	/* <br/> case */
	{p_iseqC, '/', A_NEXT, TPS_InTagBeginEnd, 0, NULL},
	{p_iseqC, '>', A_NEXT, TPS_InTagEnd, 0, SpecialTags},
	{p_isspace, 0, A_NEXT, TPS_InTag, 0, SpecialTags},
	{p_isalnum, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, ':', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '.', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_Null, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagBeginEnd[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '>', A_NEXT, TPS_InTagEnd, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTag[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '>', A_NEXT, TPS_InTagEnd, 0, SpecialTags},
	{p_iseqC, '\'', A_NEXT, TPS_InTagEscapeK, 0, NULL},
	{p_iseqC, '"', A_NEXT, TPS_InTagEscapeKK, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '=', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '#', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '/', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, ':', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '.', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '&', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '?', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '%', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '~', A_NEXT, TPS_Null, 0, NULL},
	{p_isspace, 0, A_NEXT, TPS_Null, 0, SpecialTags},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagEscapeK[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '\\', A_PUSH, TPS_InTagBackSleshed, 0, NULL},
	{p_iseqC, '\'', A_NEXT, TPS_InTag, 0, NULL},
	{NULL, 0, A_NEXT, TPS_InTagEscapeK, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagEscapeKK[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '\\', A_PUSH, TPS_InTagBackSleshed, 0, NULL},
	{p_iseqC, '"', A_NEXT, TPS_InTag, 0, NULL},
	{NULL, 0, A_NEXT, TPS_InTagEscapeKK, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagBackSleshed[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{NULL, 0, A_MERGE, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InTagEnd[] = {
	{NULL, 0, A_BINGO | A_CLRALL, TPS_Base, TAG_T, NULL}
};

static const TParserStateActionItem actionTPS_InCommentFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InCommentLast, 0, NULL},
	/* <!DOCTYPE ...> */
	{p_iseqC, 'D', A_NEXT, TPS_InTag, 0, NULL},
	{p_iseqC, 'd', A_NEXT, TPS_InTag, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InCommentLast[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InComment, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InComment[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InCloseCommentFirst, 0, NULL},
	{NULL, 0, A_NEXT, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InCloseCommentFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InCloseCommentLast, 0, NULL},
	{NULL, 0, A_NEXT, TPS_InComment, 0, NULL}
};

static const TParserStateActionItem actionTPS_InCloseCommentLast[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_Null, 0, NULL},
	{p_iseqC, '>', A_NEXT, TPS_InCommentEnd, 0, NULL},
	{NULL, 0, A_NEXT, TPS_InComment, 0, NULL}
};

static const TParserStateActionItem actionTPS_InCommentEnd[] = {
	{NULL, 0, A_BINGO | A_CLRALL, TPS_Base, TAG_T, NULL}
};

static const TParserStateActionItem actionTPS_InHostFirstDomain[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHostDomainSecond, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHost, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHostDomainSecond[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHostDomain, 0, NULL},
	{p_isdigit, 0, A_PUSH, TPS_InHost, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHostFirstAN, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InHostFirstDomain, 0, NULL},
	{p_iseqC, '@', A_PUSH, TPS_InEmail, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHostDomain[] = {
	{p_isEOF, 0, A_BINGO | A_CLRALL, TPS_Base, HOST, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHostDomain, 0, NULL},
	{p_isdigit, 0, A_PUSH, TPS_InHost, 0, NULL},
	{p_iseqC, ':', A_PUSH, TPS_InPortFirst, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHostFirstAN, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InHostFirstDomain, 0, NULL},
	{p_iseqC, '@', A_PUSH, TPS_InEmail, 0, NULL},
	{p_isdigit, 0, A_POP, TPS_Null, 0, NULL},
	{p_isstophost, 0, A_BINGO | A_CLRALL, TPS_InURLPathStart, HOST, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFURL, 0, NULL},
	{NULL, 0, A_BINGO | A_CLRALL, TPS_Base, HOST, NULL}
};

static const TParserStateActionItem actionTPS_InPortFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InPort, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InPort[] = {
	{p_isEOF, 0, A_BINGO | A_CLRALL, TPS_Base, HOST, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InPort, 0, NULL},
	{p_isstophost, 0, A_BINGO | A_CLRALL, TPS_InURLPathStart, HOST, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFURL, 0, NULL},
	{NULL, 0, A_BINGO | A_CLRALL, TPS_Base, HOST, NULL}
};

static const TParserStateActionItem actionTPS_InHostFirstAN[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHost, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHost, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHost[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHost, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHost, 0, NULL},
	{p_iseqC, '@', A_PUSH, TPS_InEmail, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InHostFirstDomain, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHostFirstAN, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InEmail[] = {
	{p_ishost, 0, A_BINGO | A_CLRALL, TPS_Base, EMAIL, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InFileFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '.', A_NEXT, TPS_InPathFirst, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '?', A_PUSH, TPS_InURLPathFirst, 0, NULL},
	{p_iseqC, '~', A_PUSH, TPS_InFileTwiddle, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InFileTwiddle[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '/', A_NEXT, TPS_InFileFirst, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InPathFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '.', A_NEXT, TPS_InPathSecond, 0, NULL},
	{p_iseqC, '/', A_NEXT, TPS_InFileFirst, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InPathFirstFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '.', A_NEXT, TPS_InPathSecond, 0, NULL},
	{p_iseqC, '/', A_NEXT, TPS_InFileFirst, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InPathSecond[] = {
	{p_isEOF, 0, A_BINGO | A_CLEAR, TPS_Base, FILEPATH, NULL},
	{p_iseqC, '/', A_NEXT | A_PUSH, TPS_InFileFirst, 0, NULL},
	{p_iseqC, '/', A_BINGO | A_CLEAR, TPS_Base, FILEPATH, NULL},
	{p_isspace, 0, A_BINGO | A_CLEAR, TPS_Base, FILEPATH, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InFile[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, FILEPATH, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '.', A_PUSH, TPS_InFileNext, 0, NULL},
	{p_iseqC, '_', A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '-', A_NEXT, TPS_InFile, 0, NULL},
	{p_iseqC, '/', A_PUSH, TPS_InFileFirst, 0, NULL},
	{p_iseqC, '?', A_PUSH, TPS_InURLPathFirst, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, FILEPATH, NULL}
};

static const TParserStateActionItem actionTPS_InFileNext[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_CLEAR, TPS_InFile, 0, NULL},
	{p_isdigit, 0, A_CLEAR, TPS_InFile, 0, NULL},
	{p_iseqC, '_', A_CLEAR, TPS_InFile, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InURLPathFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '"', A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '\'', A_POP, TPS_Null, 0, NULL},
	{p_isnotspace, 0, A_CLEAR, TPS_InURLPath, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL},
};

static const TParserStateActionItem actionTPS_InURLPathStart[] = {
	{NULL, 0, A_NEXT, TPS_InURLPath, 0, NULL}
};

static const TParserStateActionItem actionTPS_InURLPath[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, URLPATH, NULL},
	{p_iseqC, '"', A_BINGO, TPS_Base, URLPATH, NULL},
	{p_iseqC, '\'', A_BINGO, TPS_Base, URLPATH, NULL},
	{p_isnotspace, 0, A_NEXT, TPS_InURLPath, 0, NULL},
	{NULL, 0, A_BINGO, TPS_Base, URLPATH, NULL}
};

static const TParserStateActionItem actionTPS_InFURL[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isURLPath, 0, A_BINGO | A_CLRALL, TPS_Base, URL_T, SpecialFURL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InProtocolFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '/', A_NEXT, TPS_InProtocolSecond, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InProtocolSecond[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_iseqC, '/', A_NEXT, TPS_InProtocolEnd, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InProtocolEnd[] = {
	{NULL, 0, A_BINGO | A_CLRALL, TPS_Base, PROTOCOL, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenAsciiWordFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHyphenAsciiWord, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWord, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenDigitLookahead, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenAsciiWord[] = {
	{p_isEOF, 0, A_BINGO | A_CLRALL, TPS_InParseHyphen, ASCIIHWORD, SpecialHyphen},
	{p_isasclet, 0, A_NEXT, TPS_InHyphenAsciiWord, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWord, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenNumWord, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHyphenAsciiWordFirst, 0, NULL},
	{NULL, 0, A_BINGO | A_CLRALL, TPS_InParseHyphen, ASCIIHWORD, SpecialHyphen}
};

static const TParserStateActionItem actionTPS_InHyphenWordFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWord, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenDigitLookahead, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenWord[] = {
	{p_isEOF, 0, A_BINGO | A_CLRALL, TPS_InParseHyphen, HWORD, SpecialHyphen},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWord, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenNumWord, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHyphenWordFirst, 0, NULL},
	{NULL, 0, A_BINGO | A_CLRALL, TPS_InParseHyphen, HWORD, SpecialHyphen}
};

static const TParserStateActionItem actionTPS_InHyphenNumWordFirst[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenNumWord, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenDigitLookahead, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenNumWord[] = {
	{p_isEOF, 0, A_BINGO | A_CLRALL, TPS_InParseHyphen, NUMHWORD, SpecialHyphen},
	{p_isalnum, 0, A_NEXT, TPS_InHyphenNumWord, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InHyphenNumWordFirst, 0, NULL},
	{NULL, 0, A_BINGO | A_CLRALL, TPS_InParseHyphen, NUMHWORD, SpecialHyphen}
};

static const TParserStateActionItem actionTPS_InHyphenDigitLookahead[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenDigitLookahead, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenNumWord, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InParseHyphen[] = {
	{p_isEOF, 0, A_RERUN, TPS_Base, 0, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHyphenAsciiWordPart, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWordPart, 0, NULL},
	{p_isdigit, 0, A_PUSH, TPS_InHyphenUnsignedInt, 0, NULL},
	{p_iseqC, '-', A_PUSH, TPS_InParseHyphenHyphen, 0, NULL},
	{NULL, 0, A_RERUN, TPS_Base, 0, NULL}
};

static const TParserStateActionItem actionTPS_InParseHyphenHyphen[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isalnum, 0, A_BINGO | A_CLEAR, TPS_InParseHyphen, SPACE, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenWordPart[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, PARTHWORD, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWordPart, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenNumWordPart, 0, NULL},
	{NULL, 0, A_BINGO, TPS_InParseHyphen, PARTHWORD, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenAsciiWordPart[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, ASCIIPARTHWORD, NULL},
	{p_isasclet, 0, A_NEXT, TPS_InHyphenAsciiWordPart, 0, NULL},
	{p_isalpha, 0, A_NEXT, TPS_InHyphenWordPart, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_InHyphenNumWordPart, 0, NULL},
	{NULL, 0, A_BINGO, TPS_InParseHyphen, ASCIIPARTHWORD, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenNumWordPart[] = {
	{p_isEOF, 0, A_BINGO, TPS_Base, NUMPARTHWORD, NULL},
	{p_isalnum, 0, A_NEXT, TPS_InHyphenNumWordPart, 0, NULL},
	{NULL, 0, A_BINGO, TPS_InParseHyphen, NUMPARTHWORD, NULL}
};

static const TParserStateActionItem actionTPS_InHyphenUnsignedInt[] = {
	{p_isEOF, 0, A_POP, TPS_Null, 0, NULL},
	{p_isdigit, 0, A_NEXT, TPS_Null, 0, NULL},
	{p_isalpha, 0, A_CLEAR, TPS_InHyphenNumWordPart, 0, NULL},
	{NULL, 0, A_POP, TPS_Null, 0, NULL}
};


/*
 * main table of per-state parser actions
 */
typedef struct
{
	const TParserStateActionItem *action;		/* the actual state info */
	TParserState state;			/* only for Assert crosscheck */
#ifdef WPARSER_TRACE
	const char *state_name;		/* only for debug printout */
#endif
} TParserStateAction;

#ifdef WPARSER_TRACE
#define TPARSERSTATEACTION(state) \
	{ CppConcat(action,state), state, CppAsString(state) }
#else
#define TPARSERSTATEACTION(state) \
	{ CppConcat(action,state), state }
#endif

/*
 * order must be the same as in typedef enum {} TParserState!!
 */

static const TParserStateAction Actions[] = {
	TPARSERSTATEACTION(TPS_Base),
	TPARSERSTATEACTION(TPS_InNumWord),
	TPARSERSTATEACTION(TPS_InAsciiWord),
	TPARSERSTATEACTION(TPS_InWord),
	TPARSERSTATEACTION(TPS_InUnsignedInt),
	TPARSERSTATEACTION(TPS_InSignedIntFirst),
	TPARSERSTATEACTION(TPS_InSignedInt),
	TPARSERSTATEACTION(TPS_InSpace),
	TPARSERSTATEACTION(TPS_InUDecimalFirst),
	TPARSERSTATEACTION(TPS_InUDecimal),
	TPARSERSTATEACTION(TPS_InDecimalFirst),
	TPARSERSTATEACTION(TPS_InDecimal),
	TPARSERSTATEACTION(TPS_InVerVersion),
	TPARSERSTATEACTION(TPS_InSVerVersion),
	TPARSERSTATEACTION(TPS_InVersionFirst),
	TPARSERSTATEACTION(TPS_InVersion),
	TPARSERSTATEACTION(TPS_InMantissaFirst),
	TPARSERSTATEACTION(TPS_InMantissaSign),
	TPARSERSTATEACTION(TPS_InMantissa),
	TPARSERSTATEACTION(TPS_InXMLEntityFirst),
	TPARSERSTATEACTION(TPS_InXMLEntity),
	TPARSERSTATEACTION(TPS_InXMLEntityNumFirst),
	TPARSERSTATEACTION(TPS_InXMLEntityNum),
	TPARSERSTATEACTION(TPS_InXMLEntityHexNumFirst),
	TPARSERSTATEACTION(TPS_InXMLEntityHexNum),
	TPARSERSTATEACTION(TPS_InXMLEntityEnd),
	TPARSERSTATEACTION(TPS_InTagFirst),
	TPARSERSTATEACTION(TPS_InXMLBegin),
	TPARSERSTATEACTION(TPS_InTagCloseFirst),
	TPARSERSTATEACTION(TPS_InTagName),
	TPARSERSTATEACTION(TPS_InTagBeginEnd),
	TPARSERSTATEACTION(TPS_InTag),
	TPARSERSTATEACTION(TPS_InTagEscapeK),
	TPARSERSTATEACTION(TPS_InTagEscapeKK),
	TPARSERSTATEACTION(TPS_InTagBackSleshed),
	TPARSERSTATEACTION(TPS_InTagEnd),
	TPARSERSTATEACTION(TPS_InCommentFirst),
	TPARSERSTATEACTION(TPS_InCommentLast),
	TPARSERSTATEACTION(TPS_InComment),
	TPARSERSTATEACTION(TPS_InCloseCommentFirst),
	TPARSERSTATEACTION(TPS_InCloseCommentLast),
	TPARSERSTATEACTION(TPS_InCommentEnd),
	TPARSERSTATEACTION(TPS_InHostFirstDomain),
	TPARSERSTATEACTION(TPS_InHostDomainSecond),
	TPARSERSTATEACTION(TPS_InHostDomain),
	TPARSERSTATEACTION(TPS_InPortFirst),
	TPARSERSTATEACTION(TPS_InPort),
	TPARSERSTATEACTION(TPS_InHostFirstAN),
	TPARSERSTATEACTION(TPS_InHost),
	TPARSERSTATEACTION(TPS_InEmail),
	TPARSERSTATEACTION(TPS_InFileFirst),
	TPARSERSTATEACTION(TPS_InFileTwiddle),
	TPARSERSTATEACTION(TPS_InPathFirst),
	TPARSERSTATEACTION(TPS_InPathFirstFirst),
	TPARSERSTATEACTION(TPS_InPathSecond),
	TPARSERSTATEACTION(TPS_InFile),
	TPARSERSTATEACTION(TPS_InFileNext),
	TPARSERSTATEACTION(TPS_InURLPathFirst),
	TPARSERSTATEACTION(TPS_InURLPathStart),
	TPARSERSTATEACTION(TPS_InURLPath),
	TPARSERSTATEACTION(TPS_InFURL),
	TPARSERSTATEACTION(TPS_InProtocolFirst),
	TPARSERSTATEACTION(TPS_InProtocolSecond),
	TPARSERSTATEACTION(TPS_InProtocolEnd),
	TPARSERSTATEACTION(TPS_InHyphenAsciiWordFirst),
	TPARSERSTATEACTION(TPS_InHyphenAsciiWord),
	TPARSERSTATEACTION(TPS_InHyphenWordFirst),
	TPARSERSTATEACTION(TPS_InHyphenWord),
	TPARSERSTATEACTION(TPS_InHyphenNumWordFirst),
	TPARSERSTATEACTION(TPS_InHyphenNumWord),
	TPARSERSTATEACTION(TPS_InHyphenDigitLookahead),
	TPARSERSTATEACTION(TPS_InParseHyphen),
	TPARSERSTATEACTION(TPS_InParseHyphenHyphen),
	TPARSERSTATEACTION(TPS_InHyphenWordPart),
	TPARSERSTATEACTION(TPS_InHyphenAsciiWordPart),
	TPARSERSTATEACTION(TPS_InHyphenNumWordPart),
	TPARSERSTATEACTION(TPS_InHyphenUnsignedInt)
};


static bool
TParserGet(TParser *prs)
{
	const TParserStateActionItem *item = NULL;

	Assert(prs->state);

	if (prs->state->posbyte >= prs->lenstr) {
		return false;
	}
	
	prs->token = prs->str + prs->state->posbyte;
	prs->state->pushedAtAction = NULL;

	/* look at string */
	while (prs->state->posbyte <= prs->lenstr)
	{
		if (prs->state->posbyte == prs->lenstr)
			prs->state->charlen = 0;
		else
			prs->state->charlen = (prs->charmaxlen == 1) ? prs->charmaxlen :
				pg_mblen(prs->str + prs->state->posbyte);

		Assert(prs->state->posbyte + prs->state->charlen <= prs->lenstr);
		Assert(prs->state->state >= TPS_Base && prs->state->state < TPS_Null);
		Assert(Actions[prs->state->state].state == prs->state->state);

		if (prs->state->pushedAtAction)
		{
			/* After a POP, pick up at the next test */
			item = prs->state->pushedAtAction + 1;
			prs->state->pushedAtAction = NULL;
		}
		else
		{
			item = Actions[prs->state->state].action;
			Assert(item != NULL);
		}

		/* find action by character class */
		while (item->isclass)
		{
			prs->c = item->c;
			if (item->isclass(prs) != 0)
				break;
			item++;
		}

#ifdef WPARSER_TRACE
		{
			TParserPosition *ptr;

			fprintf(stderr, "state ");
			/* indent according to stack depth */
			for (ptr = prs->state->prev; ptr; ptr = ptr->prev)
				fprintf(stderr, "  ");
			fprintf(stderr, "%s ", Actions[prs->state->state].state_name);
			if (prs->state->posbyte < prs->lenstr)
				fprintf(stderr, "at %c", *(prs->str + prs->state->posbyte));
			else
				fprintf(stderr, "at EOF");
			fprintf(stderr, " matched rule %d flags%s%s%s%s%s%s%s%s%s%s%s\n",
					(int) (item - Actions[prs->state->state].action),
					(item->flags & A_BINGO) ? " BINGO" : "",
					(item->flags & A_POP) ? " POP" : "",
					(item->flags & A_PUSH) ? " PUSH" : "",
					(item->flags & A_RERUN) ? " RERUN" : "",
					(item->flags & A_CLEAR) ? " CLEAR" : "",
					(item->flags & A_MERGE) ? " MERGE" : "",
					(item->flags & A_CLRALL) ? " CLRALL" : "",
					(item->tostate != TPS_Null) ? " tostate " : "",
					(item->tostate != TPS_Null) ? Actions[item->tostate].state_name : "",
					(item->type > 0) ? " type " : "",
					tok_alias[item->type]);
		}
#endif

		/* call special handler if exists */
		if (item->special)
			item->special(prs);

		/* BINGO, token is found */
		if (item->flags & A_BINGO)
		{
			Assert(item->type > 0);
			prs->lenbytetoken = prs->state->lenbytetoken;
			prs->lenchartoken = prs->state->lenchartoken;
			prs->state->lenbytetoken = prs->state->lenchartoken = 0;
			prs->type = item->type;
		}

		/* do various actions by flags */
		if (item->flags & A_POP)
		{						/* pop stored state in stack */
			TParserPosition *ptr = prs->state->prev;

			pfree(prs->state);
			prs->state = ptr;
			Assert(prs->state);
		}
		else if (item->flags & A_PUSH)
		{						/* push (store) state in stack */
			prs->state->pushedAtAction = item;	/* remember where we push */
			prs->state = newTParserPosition(prs->state);
		}
		else if (item->flags & A_CLEAR)
		{						/* clear previous pushed state */
			TParserPosition *ptr;

			Assert(prs->state->prev);
			ptr = prs->state->prev->prev;
			pfree(prs->state->prev);
			prs->state->prev = ptr;
		}
		else if (item->flags & A_CLRALL)
		{						/* clear all previous pushed state */
			TParserPosition *ptr;

			while (prs->state->prev)
			{
				ptr = prs->state->prev->prev;
				pfree(prs->state->prev);
				prs->state->prev = ptr;
			}
		}
		else if (item->flags & A_MERGE)
		{						/* merge posinfo with current and pushed state */
			TParserPosition *ptr = prs->state;

			Assert(prs->state->prev);
			prs->state = prs->state->prev;

			prs->state->posbyte = ptr->posbyte;
			prs->state->poschar = ptr->poschar;
			prs->state->charlen = ptr->charlen;
			prs->state->lenbytetoken = ptr->lenbytetoken;
			prs->state->lenchartoken = ptr->lenchartoken;
			pfree(ptr);
		}

		/* set new state if pointed */
		if (item->tostate != TPS_Null)
			prs->state->state = item->tostate;

		/* check for go away */
		if ((item->flags & A_BINGO) ||
			(prs->state->posbyte >= prs->lenstr &&
			 (item->flags & A_RERUN) == 0))
			break;

		/* go to beginning of loop if we should rerun or we just restore state */
		if (item->flags & (A_RERUN | A_POP))
			continue;

		/* move forward */
		if (prs->state->charlen)
		{
			prs->state->posbyte += prs->state->charlen;
			prs->state->lenbytetoken += prs->state->charlen;
			prs->state->poschar++;
			prs->state->lenchartoken++;
		}
	}
	if (prs->type != TAG_T && prs->type != XMLENTITY && !prs->ignore && prs->state->posbyte && (!prs->seg || prs->token >= prs->seg)) {
		memcpy(prs->cur, prs->token, prs->lenbytetoken);
		prs->cur += prs->lenbytetoken;
	}
	if (prs->seg <  prs->token + prs->lenbytetoken) {
		prs->seg = prs->token + prs->lenbytetoken;
	}
	return (item && (item->flags & A_BINGO)) ? true : false;
}

PG_MODULE_MAGIC;

/*
 * types
 */

/*
 * prototypes
 */
PG_FUNCTION_INFO_V1(chineseprs_start);
Datum		chineseprs_start(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(chineseprs_getlexeme);
Datum		chineseprs_getlexeme(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(chineseprs_getlexeme_ns);
Datum		chineseprs_getlexeme_ns(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(chineseprs_end);
Datum		chineseprs_end(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(chineseprs_lextype);
Datum		chineseprs_lextype(PG_FUNCTION_ARGS);


/*
 * functions
 */

static void *handle = NULL;

void _PG_init(void);
void _PG_fini(void);

void _PG_init(void)
{
	handle = bamboo_init("crf_seg", NULL);
	if (handle == NULL)
		elog(ERROR, "can not load bamboo");
}

void _PG_fini(void)
{
	if (handle)
		bamboo_clean(handle);
}

Datum
chineseprs_start(PG_FUNCTION_ARGS)
{
	const char *t = NULL;
	TParser *prs = TParserInit((char*)PG_GETARG_POINTER(0), PG_GETARG_INT32(1));

	if (handle == NULL)
		elog(ERROR, "can not load bamboo");
	while(TParserGet(prs))
		;
	bamboo_setopt(handle, BAMBOO_OPTION_TEXT, prs->trimmed);
	t = bamboo_parse(handle);
	prs->sege = strdup(t);
	if (prs->sege == NULL) {
		prs->sege = strdup(" ");
	}
	prs->len = strlen(prs->sege);
	prs->pos = 0;
	PG_RETURN_POINTER(prs);
}

/* We always return type = 3 , trim space */
Datum
chineseprs_getlexeme_ns(PG_FUNCTION_ARGS)
{
	TParser *prs = (TParser*) PG_GETARG_POINTER(0);
	char	  **t = (char **) PG_GETARG_POINTER(1);
	int		   *tlen = (int *) PG_GETARG_POINTER(2);
	int			type;


	if ((prs->sege)[prs->pos] == ' ')
	{
		while ((prs->sege)[prs->pos] == ' ' &&
			   prs->pos < prs->len)
			(prs->pos)++;
	}

	*tlen = prs->pos;
	*t = prs->sege + prs->pos;

	/* word type */
	type = 3;
	/* go to the next white-space character */
	while ((prs->sege)[prs->pos] != ' ' &&
			prs->pos < prs->len)
		(prs->pos)++;

	*tlen = prs->pos - *tlen;

	/* we are finished if (*tlen == 0) */
	if (*tlen == 0)
		type = 0;
	
	PG_RETURN_INT32(type);
 
}
Datum
chineseprs_getlexeme(PG_FUNCTION_ARGS)
{
	TParser *prs = (TParser*) PG_GETARG_POINTER(0);
	char	  **t = (char **) PG_GETARG_POINTER(1);
	int		   *tlen = (int *) PG_GETARG_POINTER(2);
	int			type;

	*tlen = prs->pos;
	*t = prs->sege + prs->pos;

	if ((prs->sege)[prs->pos] == ' ')
	{
		/* blank type */
		type = 12;
		/* go to the next non-white-space character */
		while ((prs->sege)[prs->pos] == ' ' &&
			   prs->pos < prs->len)
			(prs->pos)++;
	}
	else
	{
		/* word type */
		type = 3;
		/* go to the next white-space character */
		while ((prs->sege)[prs->pos] != ' ' &&
			   prs->pos < prs->len)
			(prs->pos)++;
	}

	*tlen = prs->pos - *tlen;

	/* we are finished if (*tlen == 0) */
	if (*tlen == 0)
		type = 0;
	
	PG_RETURN_INT32(type);
 
}

Datum
chineseprs_end(PG_FUNCTION_ARGS)
{
	TParser *p = (TParser*) PG_GETARG_POINTER(0);
	TParserClose(p);
	PG_RETURN_VOID();
}

Datum
chineseprs_lextype(PG_FUNCTION_ARGS)
{
	/*
	 * Remarks: - we have to return the blanks for headline reason - we use
	 * the same lexids like Teodor in the default word parser; in this way we
	 * can reuse the headline function of the default word parser.
	 */
	LexDescr   *descr = (LexDescr *) palloc(sizeof(LexDescr) * (2 + 1));

	/* there are only two types in this parser */
	descr[0].lexid = 3;
	descr[0].alias = pstrdup("word");
	descr[0].descr = pstrdup("Word");
	descr[1].lexid = 12;
	descr[1].alias = pstrdup("blank");
	descr[1].descr = pstrdup("Space symbols");
	descr[2].lexid = 0;

	PG_RETURN_POINTER(descr);
}
