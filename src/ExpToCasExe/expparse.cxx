
# line 2 "Express.y"

// Created:	Thu Oct 28 12:21:16 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

/*****************************************************************************\

This YACC parser implements parsing algorithm for EXPRESS -> CASCADE/XSTEP
classes generator

Input in the form of tokens is obtained from lexical analyser. Then, data 
structure representing schema is created on the basis of grammar analysis.

NOTE: The grammar currently implemented is not full. 
FUNCTION, RULE and CONSTANT items, WHERE, INVERSE and DERIVE clauses 
of TYPE and ENTITY items are not considered (ignored on the level of lexical 
scanner). 
SUPERTYPE and UNIQUE clauses of ENTITY item are recognized but ignored.
Also, complex constructs such as using call to function in dimensions of 
complex time or redefinition of inherited field are ignored.

\*****************************************************************************/

#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_HSequenceOfItem.hxx>
#include <Express_Field.hxx>
#include <Express_Item.hxx>
#include <Express_Enum.hxx>
#include <Express_Alias.hxx>
#include <Express_Select.hxx>
#include <Express_Entity.hxx>
#include <Express_Type.hxx>
#include <Express_NamedType.hxx>
#include <Express_PredefinedType.hxx>
#include <Express_Number.hxx>
#include <Express_Integer.hxx>
#include <Express_Boolean.hxx>
#include <Express_Logical.hxx>
#include <Express_Real.hxx>
#include <Express_String.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Array.hxx>
#include <Express_List.hxx>
#include <Express_Set.hxx>
#include <Express_Bag.hxx>
#include <Express_Schema.hxx>
#include <Express_Reference.hxx>
#include <Express.hxx>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4274)
#pragma warning(disable:4800)
#endif
#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif

/************************************************/
/* ERROR MESSAGE FUNCTION                       */

/* external functions (from Express.l) */
int ec_error ( char *s, char *text );
int ec_curline ( void );

#define yyerror ec_error
void ec_error ( char *s )
{
  printf ( "\nParse error at line %d: %s\n", ec_curline(), s );
}

/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static Express_Schema *mkschema ( char *name, Express_HSequenceOfItem *ilist );
static Express_HSequenceOfItem *mkilist ( Express_Item *item, Express_HSequenceOfItem *seq );
static Express_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static Express_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static Express_Item *mkalias ( char *name, Express_Type *type );
static Express_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
				 Express_HSequenceOfField *field, int isabstract );
static Express_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items);
static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, TColStd_HSequenceOfHAsciiString *tlist2 );
static Express_Type *mktstd ( int keyword );
static Express_Type *mktname ( char *name );
static Express_Type *mktset ( int keyword, int ilow, int ihigh, Express_Type *of );
static Express_Field *mkfield ( char *name, Express_Type *type, int optional );
static Express_HSequenceOfField *mkflist ( Express_Field *field, Express_HSequenceOfField *seq );


# line 91 "Express.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
 {
  int num;
  char *str;
  TColStd_HSequenceOfHAsciiString *tlist;
  Express_HSequenceOfField *flist;
  Express_HSequenceOfItem *ilist;
  Express_Field *field;
  Express_Item *item;
  Express_Type *type;
  Express_Schema *schema;
  Express_Reference *ref;
} YYSTYPE;
# define KSCHEM 257
# define KENDS 258
# define KTYP 259
# define KENDT 260
# define KENT 261
# define KENDE 262
# define KREF 263
# define KFROM 264
# define KSEL 265
# define KENUM 266
# define KLIST 267
# define KARR 268
# define KBAG 269
# define KSET 270
# define KOF 271
# define KNUM 272
# define KINT 273
# define KDBL 274
# define KSTR 275
# define KLOG 276
# define KBOOL 277
# define KOPT 278
# define KUNIQ 279
# define KSELF 280
# define KABSTR 281
# define KSUBT 282
# define KSPRT 283
# define KANDOR 284
# define K1OF 285
# define KAND 286
# define NUMBER 287
# define NAME 288

#ifdef __STDC__
#include <stdlib.h>
#include <string.h>
#else
#include <malloc.h>
#include <memory.h>
#endif

#ifdef __cplusplus

#ifndef yyerror
	void yyerror(const char *);
#endif

#ifndef yylex
#ifdef __EXTERN_C__
	extern "C" { int yylex(void); }
#else
	int yylex(void);
#endif
#endif
	int yyparse(void);

#endif
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 266 "Express.y"


/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static Express_Schema *mkschema ( char *name, Express_HSequenceOfItem *ilist )
{
  Express_Schema *sch = new Express_Schema ( name, ilist );
  Express::Schema() = sch;
  return sch;
}

static Express_HSequenceOfItem *mkilist ( Express_Item *item, Express_HSequenceOfItem *seq )
{
  if ( ! seq ) { 
    seq = new Express_HSequenceOfItem;
    seq->Append ( item );
  }
  else seq->Prepend ( item );
  return seq;
}

static Express_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new Express_Enum ( name, tlist );
}

static Express_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new Express_Select ( name, tlist );
}

static Express_Item *mkalias ( char *name, Express_Type *type )
{
  return new Express_Alias ( name, type );
}

static Express_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
				 Express_HSequenceOfField *field, int isabstract )
{
  Express_Entity *ent = new Express_Entity ( name, inherit, field );
  if ( isabstract ) ent->SetAbstractFlag ( Standard_True );
  return ent;
}

static Express_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items)
{
  return new Express_Reference ( name, items );
}

static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString ( name );
  if ( tlist ) tlist->Prepend ( str );
  else {
    tlist = new TColStd_HSequenceOfHAsciiString;
    tlist->Append ( str );
  }
  return tlist;
}

static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, 
						   TColStd_HSequenceOfHAsciiString *tlist2 )
{
  if ( ! tlist1 ) return tlist2;
  if ( ! tlist2 ) return tlist1;
  for ( int i=1; i <= tlist2->Length(); i++ )
    tlist1->Append ( tlist2->Value(i) );
  return tlist1;
}

static Express_Type *mktstd ( int keyword )
{
  switch ( keyword ) {
  case KINT : return new Express_Integer;
  case KNUM : return new Express_Number;
  case KDBL : return new Express_Real;
  case KSTR : return new Express_String;
  case KBOOL: return new Express_Boolean;
  case KLOG : return new Express_Logical;
  default   : ec_error ( "Predefined type not treated!", "" );
              return NULL;
  }
}

static Express_Type *mktname ( char *name )
{
  return new Express_NamedType ( name );
}

static Express_Type *mktset ( int keyword, int ilow, int ihigh, Express_Type *of )
{
  switch ( keyword ) {
  case KLIST: return new Express_List  ( ilow, ihigh, of );
  case KARR : return new Express_Array ( ilow, ihigh, of );
  case KBAG : return new Express_Bag   ( ilow, ihigh, of );
  case KSET : return new Express_Set   ( ilow, ihigh, of );
  default   : ec_error ( "Complex type not treated!", "" );
              return NULL;
  }
}

static Express_Field *mkfield ( char *name, Express_Type *type, int optional )
{
  return new Express_Field ( name, type, optional );
}

static Express_HSequenceOfField *mkflist ( Express_Field *field, Express_HSequenceOfField *seq )
{
  if ( seq ) seq->Prepend ( field );
  else {
    seq = new Express_HSequenceOfField;
    seq->Append ( field );
  }
  return seq;
}

/*******************************************************************/
/* External interface to result of parsing */

Handle(Express_Schema) ec_parse ( FILE *fin )
{
  extern FILE *yyin;
  yyin = fin;
  yyparse();
  return Express::Schema();
}

/*******************************************************************/
/* MAIN & co */

/*
void tlistfree ( struct tlist *list )
{
  if ( ! list ) return;
  tlistfree ( list->next );
  list->next = 0;
  free ( list->str );
  list->str = 0;
}
* /

int printtlist ( struct ec_tlist *tl )
{
  int num=0;
  while ( tl ) {
    num++;
    printf ( "%s\n", tl->name );
    tl = tl->next;
  }
  return num;
}

int main ( void )
{
  int num = 0;
  yyparse();

  printf ( "\nFinished\n" );
  if ( schema ) {
    struct ec_item *it;
    it = schema->items;
    printf ( "\nSchema %s", schema->name );
    printf ( "\nItems:" );
    while ( it ) {
      num++;
      printf ( "\n%s", it->name );
      it = it->next;
    }
//    num = printtlist ( res );
    printf ( "\nTotal %d", num );
  }

//  tlistfree ( yylval.tlist );
  return num;
}
*/
static const yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 67
# define YYLAST 279
static const yytabelem yyact[]={

    23,    22,    35,    36,    38,    37,    65,    29,    28,    30,
    31,    32,    33,    56,   117,   113,   106,    61,    16,    35,
    36,    38,    37,    34,    29,    28,    30,    31,    32,    33,
   139,   139,    15,    75,     3,   101,   145,    21,   162,   138,
    34,    74,    78,    20,    93,    21,    40,   115,   135,   134,
   133,   132,    52,    43,    42,   131,   110,    11,   158,    12,
   102,    83,    62,    13,     2,    24,   144,   116,   104,   111,
    92,    63,   124,   123,   122,   120,   152,    97,    50,    49,
    48,    47,    70,    71,    18,    54,   153,   150,   149,   143,
   140,   125,   119,   103,    86,    82,    77,    59,    51,    46,
    17,     4,   129,   127,    96,    91,    90,    89,    87,   130,
   163,   160,   151,    85,    60,    79,    39,   121,    84,    88,
    67,    68,    69,    44,    45,    19,     5,    -1,     1,    72,
     6,    10,     9,    14,     8,     7,    27,    26,    25,   112,
    73,   114,    80,    81,     0,     0,    41,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    94,    95,    53,   105,
    98,   107,   108,   109,    99,   100,     0,    58,     0,   118,
     0,     0,     0,     0,     0,     0,    76,     0,     0,     0,
   128,     0,   126,     0,     0,     0,   137,     0,     0,     0,
     0,     0,     0,     0,     0,   141,   136,     0,   142,     0,
   146,   147,   148,     0,     0,     0,     0,     0,     0,     0,
   154,     0,   155,   156,   157,     0,     0,     0,     0,   161,
   159,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    64,    66,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    57,     0,
     0,    55,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    78 };
static const yytabelem yypact[]={

  -193,-10000000,  -254,    42,  -202,  -195,  -202,-10000000,-10000000,-10000000,
-10000000,  -256,  -270,    41,-10000000,    23,  -238,-10000000,  -265,  -236,
  -246,  -217,  -218,    84,    40,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,   -10,   -11,   -12,   -13,    39,
  -219,  -236,   -27,    84,    38,  -271,  -198,   -57,   -57,   -57,
   -57,  -247,    84,    37,  -242,    71,   -27,   -27,    36,  -199,
    77,    69,    35,    50,-10000000,-10000000,    79,    49,    48,    47,
  -235,-10000000,  -247,  -247,    46,   -15,-10000000,  -247,   -27,   -27,
    -6,-10000000,  -200,    34,-10000000,  -271,-10000000,   -57,  -272,   -57,
   -57,   -57,  -206,  -273,-10000000,-10000000,  -231,  -274,  -235,-10000000,
-10000000,-10000000,    33,-10000000,-10000000,   -18,    76,   -19,   -20,   -21,
    32,-10000000,  -273,    45,  -248,-10000000,    44,    63,  -207,-10000000,
  -220,-10000000,  -221,  -222,  -223,-10000000,-10000000,  -249,    31,  -248,
  -274,    30,  -243,  -243,  -243,  -243,    29,    28,    68,   -16,
-10000000,    27,-10000000,-10000000,  -248,-10000000,  -248,  -248,  -248,-10000000,
-10000000,  -249,  -274,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,    67,
  -250,-10000000,    66,  -250 };
static const yytabelem yypgo[]={

     0,    71,   141,    66,   125,    85,   140,    58,    68,   123,
   139,    69,    70,   116,    67,    65,   138,   137,   136,   135,
   134,   132,   131,   130,   126,   129,    83,    82,   128,   127 };
static const yytabelem yyr1[]={

     0,    28,    24,    24,    23,    23,    23,    23,    19,    20,
    21,    22,    22,    29,     8,     8,     9,    15,    15,    15,
    16,    16,    16,    16,    16,    16,    17,    18,    18,    18,
    18,     1,     1,     1,     3,     3,    13,    13,     4,     4,
     4,     5,     5,     5,     5,     5,    26,    26,    26,    26,
    27,    27,    25,     6,    14,    14,     2,     2,    10,    10,
    11,    11,    12,    12,     7,     7,     7 };
static const yytabelem yyr2[]={

     0,    13,     3,     5,     2,     2,     2,     3,    19,    17,
    15,    19,    21,    11,     3,     7,     7,     2,     2,     3,
     3,     3,     3,     3,     3,     3,     3,    19,    19,    19,
    19,     3,     3,     9,     1,     3,     1,     7,     1,     3,
     7,     3,     7,     7,     5,     7,     3,     5,     3,     5,
     1,     3,    11,    13,     3,     7,     1,     3,     9,     9,
     3,     5,     1,     5,     7,    11,     7 };
static const yytabelem yychk[]={

-10000000,   -28,   257,   288,    59,   -24,   -23,   -19,   -20,   -21,
   -22,   259,   261,   258,   -24,   288,   288,    59,    61,    -4,
   281,   283,   266,   265,   -15,   -16,   -17,   -18,   273,   272,
   274,   275,   276,   277,   288,   267,   268,   270,   269,   -13,
   282,    -4,   271,   271,    -9,    40,    59,    91,    91,    91,
    91,    59,   271,   -13,    -5,   288,    40,   285,    -9,    59,
    -8,   288,   260,    -1,   287,    63,   288,    -1,    -1,    -1,
   -27,   -26,   -25,    -6,   288,   280,    -9,    59,   284,    44,
    -5,    -5,    59,   260,    41,    44,    59,    58,    40,    58,
    58,    58,   -12,   279,   -26,   -26,    58,    92,   -27,    -5,
    -5,    41,   260,    59,    -8,    -1,   288,    -1,    -1,    -1,
   262,   -11,   -10,   288,    -2,   278,   -14,   288,   -12,    59,
    93,    41,    93,    93,    93,    59,   -11,    58,   -15,    58,
    46,   262,   271,   271,   271,   271,    -8,    -7,   288,   280,
    59,   -15,   -14,    59,    -3,   279,    -3,    -3,    -3,    59,
    59,    44,    92,    59,   -15,   -15,   -15,   -15,    -7,   -14,
    44,    -7,   288,    44 };
static const yytabelem yydef[]={

     0,    -2,     0,     0,     0,     0,     2,     4,     5,     6,
     7,     0,     0,     0,     3,     0,    38,     1,     0,    36,
    38,    39,     0,     0,     0,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,     0,     0,     0,     0,     0,
     0,    36,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    50,     0,     0,    40,    41,     0,     0,     0,     0,
     0,    14,     0,     0,    31,    32,     0,     0,     0,     0,
    62,    51,    46,    48,     0,     0,    37,    50,     0,     0,
     0,    44,     0,     0,    16,     0,    10,     0,     0,     0,
     0,     0,     0,     0,    47,    49,    56,     0,    62,    45,
    43,    42,     0,     9,    15,     0,     0,     0,     0,     0,
     0,    63,    60,     0,     0,    57,     0,    54,     0,     8,
     0,    33,     0,     0,     0,    11,    61,     0,     0,     0,
     0,     0,    34,    34,    34,    34,     0,     0,    14,     0,
    52,     0,    55,    12,     0,    35,     0,     0,     0,    58,
    59,     0,     0,    53,    27,    28,    29,    30,    66,    64,
     0,    65,     0,     0 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{ char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"KSCHEM",	257,
	"KENDS",	258,
	"KTYP",	259,
	"KENDT",	260,
	"KENT",	261,
	"KENDE",	262,
	"KREF",	263,
	"KFROM",	264,
	"KSEL",	265,
	"KENUM",	266,
	"KLIST",	267,
	"KARR",	268,
	"KBAG",	269,
	"KSET",	270,
	"KOF",	271,
	"KNUM",	272,
	"KINT",	273,
	"KDBL",	274,
	"KSTR",	275,
	"KLOG",	276,
	"KBOOL",	277,
	"KOPT",	278,
	"KUNIQ",	279,
	"KSELF",	280,
	"KABSTR",	281,
	"KSUBT",	282,
	"KSPRT",	283,
	"KANDOR",	284,
	"K1OF",	285,
	"KAND",	286,
	"NUMBER",	287,
	"NAME",	288,
	",",	44,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"SCHEMA : KSCHEM NAME ';' ILIST KENDS ';'",
	"ILIST : ITEM",
	"ILIST : ITEM ILIST",
	"ITEM : ENUM",
	"ITEM : SELECT",
	"ITEM : ALIAS",
	"ITEM : ENTITY",
	"ENUM : KTYP NAME '=' KENUM KOF TLIST1 ';' KENDT ';'",
	"SELECT : KTYP NAME '=' KSEL TLIST1 ';' KENDT ';'",
	"ALIAS : KTYP NAME '=' TYPE ';' KENDT ';'",
	"ENTITY : KENT NAME SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';'",
	"ENTITY : KENT NAME KABSTR SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';'",
	"REFERENCE : KREF KFROM NAME TLIST1 ';'",
	"TLIST : NAME",
	"TLIST : NAME ',' TLIST",
	"TLIST1 : '(' TLIST ')'",
	"TYPE : TSTD",
	"TYPE : TNAME",
	"TYPE : TSET",
	"TSTD : KINT",
	"TSTD : KNUM",
	"TSTD : KDBL",
	"TSTD : KSTR",
	"TSTD : KLOG",
	"TSTD : KBOOL",
	"TNAME : NAME",
	"TSET : KLIST '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE",
	"TSET : KARR '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE",
	"TSET : KSET '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE",
	"TSET : KBAG '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE",
	"INDEX : NUMBER",
	"INDEX : '?'",
	"INDEX : NAME '(' NAME ')'",
	"OPTUNI : /* empty */",
	"OPTUNI : KUNIQ",
	"SUBT : /* empty */",
	"SUBT : KSUBT KOF TLIST1",
	"SUPERT : /* empty */",
	"SUPERT : KSPRT",
	"SUPERT : KSPRT KOF SUPLST",
	"SUPLST : NAME",
	"SUPLST : '(' SUPLST ')'",
	"SUPLST : NAME ',' SUPLST",
	"SUPLST : K1OF SUPLST",
	"SUPLST : SUPLST KANDOR SUPLST",
	"FLIST : FIELD",
	"FLIST : FIELD FLIST",
	"FLIST : REDEF",
	"FLIST : REDEF FLIST",
	"FLIST1 : /* empty */",
	"FLIST1 : FLIST",
	"FIELD : NAME ':' OPTNL TYPE ';'",
	"REDEF : KSELF '\\' SPECIF ':' TYPE ';'",
	"SPECIF : NAME",
	"SPECIF : NAME '.' SPECIF",
	"OPTNL : /* empty */",
	"OPTNL : KOPT",
	"UNIQIT : NAME ':' TLIST ';'",
	"UNIQIT : NAME ':' SPCLST ';'",
	"UNIQLS : UNIQIT",
	"UNIQLS : UNIQIT UNIQLS",
	"UNIQUE : /* empty */",
	"UNIQUE : KUNIQ UNIQLS",
	"SPCLST : KSELF '\\' SPECIF",
	"SPCLST : KSELF '\\' SPECIF ',' SPCLST",
	"SPCLST : NAME ',' SPCLST",
};
#endif /* YYDEBUG */
# line	1 "/usr/ccs/bin/yaccpar"
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yymaxdepth * sizeof (type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



#ifdef YYNMBCHARS
#define YYLEX()		yycvtok(yylex())
/*
** yycvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int yycvtok(int i)
#else
int yycvtok(i) int i;
#endif
{
	int first = 0;
	int last = YYNMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( yymbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = yymbchars[mid].character;
			if( j==i ){/*Found*/ 
				return yymbchars[mid].tvalue;
			}else if( j<i ){
				first = mid + 1;
			}else{
				last = mid -1;
			}
		}
		/*No entry in the table.*/
		return i;/* Giving up.*/
	}else{/* i is already a token. */
		return i;
	}
}
#else/*!YYNMBCHARS*/
#define YYLEX()		yylex()
#endif/*!YYNMBCHARS*/

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int yyparse(void)
#else
int yyparse()
#endif
{
	register YYSTYPE *yypvt = 0;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside
	switch should never be executed
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto yyerrlab;
		case 2: goto yynewstate;
	}
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
			yyerror("yacc initialization error");
			YYABORT;
		}
	}
#endif

	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */
	goto yystack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			int yynewmax;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				char *newyys = (char *)YYNEW(int);
				char *newyyv = (char *)YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
				yyerror( "yacc stack overflow" );
				YYABORT;
			}
			yymaxdepth = yynewmax;

			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register const int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
			skip_init:
				yynerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 164 "Express.y"
{ yyval.schema = mkschema ( yypvt[-4].str, yypvt[-2].ilist ); /* Root: EXPRESS schema */ } break;
case 2:
# line 166 "Express.y"
{ yyval.ilist = mkilist ( yypvt[-0].item, 0 ); /* list of items for schema */ } break;
case 3:
# line 167 "Express.y"
{ yyval.ilist = mkilist ( yypvt[-1].item, yypvt[-0].ilist ); } break;
case 7:
# line 173 "Express.y"
{ yyval.item = yypvt[-0].item; /* item of schema (type definition) */ } break;
case 8:
# line 174 "Express.y"
{ yyval.item = mkenum ( yypvt[-7].str, yypvt[-3].tlist ); /* TYPE ENUMERATION definition */ } break;
case 9:
# line 176 "Express.y"
{ yyval.item = mkselect ( yypvt[-6].str, yypvt[-3].tlist ); /* TYPE SELECT definition */ } break;
case 10:
# line 178 "Express.y"
{ yyval.item = mkalias ( yypvt[-5].str, yypvt[-3].type ); /* TYPE '=' definition (alias) */ } break;
case 11:
# line 180 "Express.y"
{ yyval.item = mkentity ( yypvt[-7].str, yypvt[-5].tlist, yypvt[-3].flist, 0 ); /* ENTITY definition */ } break;
case 12:
# line 181 "Express.y"
{ yyval.item = mkentity ( yypvt[-8].str, yypvt[-5].tlist, yypvt[-3].flist, 1 ); /* ENTITY definition */ } break;
case 13:
# line 184 "Express.y"
{ yyval.ref = mkrefs ( yypvt[-2].str, yypvt[-1].tlist ); /* REFERENCE FROM definition */ } break;
case 14:
# line 186 "Express.y"
{ yyval.tlist = mktlist ( yypvt[-0].str, 0 ); /* list of (type) names */ } break;
case 15:
# line 187 "Express.y"
{ yyval.tlist = mktlist ( yypvt[-2].str, yypvt[-0].tlist ); } break;
case 16:
# line 189 "Express.y"
{ yyval.tlist = yypvt[-1].tlist; /* TLIST in brackets */ } break;
case 19:
# line 194 "Express.y"
{ yyval.type = yypvt[-0].type; /* type, simple or complex */ } break;
case 20:
# line 196 "Express.y"
{ yyval.type = mktstd ( yypvt[-0].num ); /* predefined type: INTEGER */ } break;
case 21:
# line 197 "Express.y"
{ yyval.type = mktstd ( yypvt[-0].num ); /* predefined type: NUMBER */ } break;
case 22:
# line 198 "Express.y"
{ yyval.type = mktstd ( yypvt[-0].num ); /* predefined type: REAL */ } break;
case 23:
# line 199 "Express.y"
{ yyval.type = mktstd ( yypvt[-0].num ); /* predefined type: STRING */ } break;
case 24:
# line 200 "Express.y"
{ yyval.type = mktstd ( yypvt[-0].num ); /* predefined type: LOGICAL */ } break;
case 25:
# line 201 "Express.y"
{ yyval.type = mktstd ( yypvt[-0].num ); /* predefined type: BOOLEAN */ } break;
case 26:
# line 203 "Express.y"
{ yyval.type = mktname ( yypvt[-0].str ); /* named type */ } break;
case 27:
# line 205 "Express.y"
{ yyval.type = mktset ( yypvt[-8].num, yypvt[-6].num, yypvt[-4].num, yypvt[-0].type ); /* complex type: LIST */ } break;
case 28:
# line 206 "Express.y"
{ yyval.type = mktset ( yypvt[-8].num, yypvt[-6].num, yypvt[-4].num, yypvt[-0].type ); /* complex type: ARRAY */ } break;
case 29:
# line 207 "Express.y"
{ yyval.type = mktset ( yypvt[-8].num, yypvt[-6].num, yypvt[-4].num, yypvt[-0].type ); /* complex type: SET */ } break;
case 30:
# line 208 "Express.y"
{ yyval.type = mktset ( yypvt[-8].num, yypvt[-6].num, yypvt[-4].num, yypvt[-0].type ); /* complex type: BAG */ } break;
case 31:
# line 210 "Express.y"
{ yyval.num = yypvt[-0].num; /* index for array, set, bag, list range */ } break;
case 32:
# line 211 "Express.y"
{ yyval.num = -1; /* undefined */ } break;
case 33:
# line 212 "Express.y"
{yyval.num = -1; printf ( "Warning at line %d: index function %s(%s) ignored\n", ec_curline(), yypvt[-3].str, yypvt[-1].str ); /* some function.. */ } break;
case 34:
# line 214 "Express.y"
{ yyval.num = 0; /* UNIQUE keyword for complex type definition */ } break;
case 35:
# line 215 "Express.y"
{ yyval.num = 1; } break;
case 36:
# line 218 "Express.y"
{ yyval.tlist = NULL; /* no subtype clause */ } break;
case 37:
# line 219 "Express.y"
{ yyval.tlist = yypvt[-0].tlist;   /* subtype clause */ } break;
case 38:
# line 221 "Express.y"
{ yyval.num = 0;  /* no supertype clause */ } break;
case 39:
# line 222 "Express.y"
{ yyval.num = 0;  /* supertype clause (ignored) */ } break;
case 40:
# line 223 "Express.y"
{ yyval.num = 0;  /* supertype clause (ignored) */ } break;
case 41:
# line 225 "Express.y"
{ yyval.num = 0; /* simple list of supertypes */ } break;
case 42:
# line 226 "Express.y"
{ yyval.num = 0; /* allow bracketing */ } break;
case 43:
# line 227 "Express.y"
{ yyval.num = 0; /* ... */ } break;
case 44:
# line 228 "Express.y"
{ yyval.num = 0; /* ONEOF construct */ } break;
case 45:
# line 229 "Express.y"
{ yyval.num = 0; /* ANDOR construct */ } break;
case 46:
# line 232 "Express.y"
{ yyval.flist = mkflist ( yypvt[-0].field, 0 ); /* list of fields of ENTITY item */ } break;
case 47:
# line 233 "Express.y"
{ yyval.flist = mkflist ( yypvt[-1].field, yypvt[-0].flist ); } break;
case 48:
# line 234 "Express.y"
{ yyval.flist = 0;  /* redefinition of inherited field, just skip */ } break;
case 49:
# line 235 "Express.y"
{ yyval.flist = yypvt[-0].flist; /* ignore redefinition of inherited field, take trailing list */ } break;
case 50:
# line 237 "Express.y"
{ yyval.flist = NULL; /* empty list of fields */ } break;
case 51:
# line 238 "Express.y"
{ yyval.flist = yypvt[-0].flist;   /* or not empty.. just to fix reduce/reduce conflict */ } break;
case 52:
# line 240 "Express.y"
{ yyval.field = mkfield ( yypvt[-4].str, yypvt[-1].type, yypvt[-2].num ); } break;
case 53:
# line 242 "Express.y"
{ yyval.num = NULL; printf ( "Warning at line %d: field redefinition ignored\n", ec_curline() ); /* redefinition of inherited field */ } break;
case 54:
# line 244 "Express.y"
{ yyval.tlist = mktlist ( yypvt[-0].str, 0 ); /* inherited field specification */ } break;
case 55:
# line 245 "Express.y"
{ yyval.tlist = mktlist ( yypvt[-2].str, yypvt[-0].tlist ); } break;
case 56:
# line 248 "Express.y"
{ yyval.num = 0; } break;
case 57:
# line 249 "Express.y"
{ yyval.num = 1; } break;
case 58:
# line 252 "Express.y"
{ yyval.tlist = yypvt[-1].tlist;   /* UNIQUE statement */ } break;
case 59:
# line 253 "Express.y"
{ yyval.tlist = NULL; /* UNIQUE statement */ } break;
case 60:
# line 255 "Express.y"
{ yyval.tlist = NULL;    /* list of 1 UNIQUE statements */ } break;
case 61:
# line 256 "Express.y"
{ yyval.tlist = mktlists ( yypvt[-1].tlist, yypvt[-0].tlist );/* list of UNIQUE statements */ } break;
case 62:
# line 258 "Express.y"
{ yyval.tlist = NULL;    /* no UNIQUE clause in ENTITY */ } break;
case 63:
# line 259 "Express.y"
{ yyval.tlist = yypvt[-0].tlist;      /* UNIQUE clause in ENTITY */ } break;
case 64:
# line 261 "Express.y"
{ yyval.num = 0; /* list of specifications */ } break;
case 65:
# line 262 "Express.y"
{ yyval.num = 0; } break;
case 66:
# line 263 "Express.y"
{ yyval.num = 0; } break;
# line	531 "/usr/ccs/bin/yaccpar"
	}
	goto yystack;		/* reset registers in driver code */
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
