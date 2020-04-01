%{

/* File:	exptocas.y                     */
/* Created:	Thu Oct 28 12:21:16 1999       */
/* Author:	Andrey BETENEV                 */
/* 		<abv@doomox.nnov.matra-dtv.fr> */
/* Copyright:	Matra Datavision 1999          */

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

#include <values.h>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <ExpToCas_HSequenceOfField.hxx>
#include <ExpToCas_HSequenceOfItem.hxx>
#include <ExpToCas_Field.hxx>
#include <ExpToCas_Item.hxx>
#include <ExpToCas_Enum.hxx>
#include <ExpToCas_Alias.hxx>
#include <ExpToCas_Select.hxx>
#include <ExpToCas_Entity.hxx>
#include <ExpToCas_Type.hxx>
#include <ExpToCas_NamedType.hxx>
#include <ExpToCas_PredefinedType.hxx>
#include <ExpToCas_Number.hxx>
#include <ExpToCas_Integer.hxx>
#include <ExpToCas_Boolean.hxx>
#include <ExpToCas_Logical.hxx>
#include <ExpToCas_Real.hxx>
#include <ExpToCas_String.hxx>
#include <ExpToCas_ComplexType.hxx>
#include <ExpToCas_Array.hxx>
#include <ExpToCas_List.hxx>
#include <ExpToCas_Set.hxx>
#include <ExpToCas_Bag.hxx>
#include <ExpToCas_Schema.hxx>
#include <ExpToCas_Reference.hxx>
#include <ExpToCas.hxx>

/************************************************/
/* ERROR MESSAGE FUNCTION                       */

/* external functions (from exptocas.l) */
int ec_error ( char *s, char *text );
int ec_curline ( void );

int yyerror ( char *s )
{
  printf ( "\nParse error at line %d: %s\n", ec_curline(), s );
  return 0;
}

/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static ExpToCas_Schema *mkschema ( char *name, ExpToCas_HSequenceOfItem *ilist );
static ExpToCas_HSequenceOfItem *mkilist ( ExpToCas_Item *item, ExpToCas_HSequenceOfItem *seq );
static ExpToCas_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static ExpToCas_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static ExpToCas_Item *mkalias ( char *name, ExpToCas_Type *type );
static ExpToCas_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
				 ExpToCas_HSequenceOfField *field, int isabstract );
static ExpToCas_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items);
static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, TColStd_HSequenceOfHAsciiString *tlist2 );
static ExpToCas_Type *mktstd ( int keyword );
static ExpToCas_Type *mktname ( char *name );
static ExpToCas_Type *mktset ( int keyword, int ilow, int ihigh, ExpToCas_Type *of );
static ExpToCas_Field *mkfield ( char *name, ExpToCas_Type *type, int optional );
static ExpToCas_HSequenceOfField *mkflist ( ExpToCas_Field *field, ExpToCas_HSequenceOfField *seq );

%}

  /* Definition of possible types of expressions */
%union {
  int num;
  char *str;
  TColStd_HSequenceOfHAsciiString *tlist;
  ExpToCas_HSequenceOfField *flist;
  ExpToCas_HSequenceOfItem *ilist;
  ExpToCas_Field *field;
  ExpToCas_Item *item;
  ExpToCas_Type *type;
  ExpToCas_Schema *schema;
  ExpToCas_Reference *ref;
}

  /* Definition of keywords */

%token <num> KSCHEM  /* SCHEMA keyword */
%token <num> KENDS   /* END_SCHEMA keyword */
%token <num> KTYP    /* TYPE keyword */
%token <num> KENDT   /* END_TYPE keyword */
%token <num> KENT    /* ENTITY keyword */
%token <num> KENDE   /* END_ENTITY keyword */
%token <num> KREF    /* REFERENCE keyword */
%token <num> KFROM   /* FROM keyword */

%token <num> KSEL    /* SELECT keyword */
%token <num> KENUM   /* ENUMERATION keyword */
%token <num> KLIST   /* LIST keyword */
%token <num> KARR    /* ARRAY keyword */
%token <num> KBAG    /* BAG keyword */
%token <num> KSET    /* SET keyword */
%token <num> KOF     /* OF keyword */

%token <num> KNUM    /* NUMBER keyword */
%token <num> KINT    /* INTEGER keyword */
%token <num> KDBL    /* REAL keyword */
%token <num> KSTR    /* STRING keyword */
%token <num> KLOG    /* LOGICAL keyword */
%token <num> KBOOL   /* BOOLEAN keyword */

%token <num> KOPT    /* OPTIONAL keyword */
%token <num> KUNIQ   /* UNIQUE keyword */
%token <num> KSELF   /* SELF keyword */

%token <num> KABSTR  /* ABSTRACT keyword */
%token <num> KSUBT   /* SUBTYPE keyword */
%token <num> KSPRT   /* SUPERTYPE keyword */
%left  <num> KANDOR  /* ANDOR keyword (%left is for eliminating shift/reduce conflict on SUPLST) */
%left  <num> K1OF    /* ONEOF keyword */
%token <num> KAND    /* AND keyword */

%token <num> NUMBER  /* integer value */
%token <str> NAME    /* name of type or entity */

%left ','            /* to eliminate shift/reduce conflict in SUPERTYPE */

  /* Definition of expressions and their types */

%type <num>    INDEX OPTNL OPTUNI SUPERT SUPLST REDEF SPCLST
%type <tlist>  TLIST TLIST1 UNIQIT UNIQLS UNIQUE SUBT SPECIF
%type <type>   TYPE TSTD TNAME TSET 
%type <item>   ENUM SELECT ALIAS ENTITY ITEM
%type <ilist>  ILIST
%type <field>  FIELD
%type <flist>  FLIST FLIST1
%type <schema> SCHEMA
%type <ref>    REFERENCE

%%

  /************************************************/
  /*  Definition of parsing rules (expressions)   */
  /************************************************/

SCHEMA: KSCHEM NAME ';' ILIST KENDS ';' { $$ = mkschema ( $2, $4 ); /* Root: EXPRESS schema */ }
      ;
ILIST : ITEM             { $$ = mkilist ( $1, 0 ); /* list of items for schema */ }
      | ITEM ILIST       { $$ = mkilist ( $1, $2 ); }
      ;

ITEM  : ENUM
      | SELECT
      | ALIAS
      | ENTITY           { $$ = $1; /* item of schema (type definition) */ }
ENUM  : KTYP NAME '=' KENUM KOF TLIST1 ';' KENDT ';' { $$ = mkenum ( $2, $6 ); /* TYPE ENUMERATION definition */ }
      ;
SELECT: KTYP NAME '=' KSEL TLIST1 ';' KENDT ';' { $$ = mkselect ( $2, $5 ); /* TYPE SELECT definition */ }
      ;
ALIAS : KTYP NAME '=' TYPE ';' KENDT ';' { $$ = mkalias ( $2, $4 ); /* TYPE '=' definition (alias) */ }
      ;
ENTITY: KENT NAME        SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';' { $$ = mkentity ( $2, $4, $6, 0 ); /* ENTITY definition */ }
      | KENT NAME KABSTR SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';' { $$ = mkentity ( $2, $5, $7, 1 ); /* ENTITY definition */ }
      ;

REFERENCE: KREF KFROM NAME TLIST1 ';' { $$ = mkrefs ( $3, $4 ); /* REFERENCE FROM definition */ }

TLIST : NAME             { $$ = mktlist ( $1, 0 ); /* list of (type) names */ }
      | NAME ',' TLIST   { $$ = mktlist ( $1, $3 ); }
      ;
TLIST1: '(' TLIST ')'    { $$ = $2; /* TLIST in brackets */ }
      ;

TYPE  : TSTD
      | TNAME
      | TSET             { $$ = $1; /* type, simple or complex */ }
      ;
TSTD  : KINT             { $$ = mktstd ( $1 ); /* predefined type: INTEGER */ }
      | KNUM             { $$ = mktstd ( $1 ); /* predefined type: NUMBER */ }
      | KDBL             { $$ = mktstd ( $1 ); /* predefined type: REAL */ }
      | KSTR             { $$ = mktstd ( $1 ); /* predefined type: STRING */ }
      | KLOG             { $$ = mktstd ( $1 ); /* predefined type: LOGICAL */ }
      | KBOOL            { $$ = mktstd ( $1 ); /* predefined type: BOOLEAN */ }
      ;                  
TNAME : NAME             { $$ = mktname ( $1 ); /* named type */ }
      ;
TSET  : KLIST '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: LIST */ }
      | KARR  '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: ARRAY */ }
      | KSET  '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: SET */ }
      | KBAG  '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: BAG */ }
      ;
INDEX : NUMBER           { $$ = $1; /* index for array, set, bag, list range */ }
      | '?'              { $$ = -1; /* undefined */ }
      | NAME '(' NAME ')' {$$ = -1; printf ( "Warning at line %d: index function %s(%s) ignored\n", ec_curline(), $1, $3 ); /* some function.. */ }
      ;
OPTUNI: /* empty */      { $$ = 0; /* UNIQUE keyword for complex type definition */ }
      | KUNIQ            { $$ = 1; }
      ;

SUBT  : /* empty */      { $$ = NULL; /* no subtype clause */ }
      | KSUBT KOF TLIST1 { $$ = $3;   /* subtype clause */ }
      ;
SUPERT: /* empty */      { $$ = 0;  /* no supertype clause */ }
      | KSPRT            { $$ = 0;  /* supertype clause (ignored) */ }
      | KSPRT KOF SUPLST { $$ = 0;  /* supertype clause (ignored) */ }
      ;
SUPLST: NAME             { $$ = 0; /* simple list of supertypes */ }
      | '(' SUPLST ')'   { $$ = 0; /* allow bracketing */ }
      | NAME ',' SUPLST  { $$ = 0; /* ... */ }
      | K1OF SUPLST      { $$ = 0; /* ONEOF construct */ }
      | SUPLST KANDOR SUPLST { $$ = 0; /* ANDOR construct */ }
      ;

FLIST : FIELD            { $$ = mkflist ( $1, 0 ); /* list of fields of ENTITY item */ }
      | FIELD FLIST      { $$ = mkflist ( $1, $2 ); }
      | REDEF            { $$ = 0;  /* redefinition of inherited field, just skip */ }
      | REDEF FLIST      { $$ = 0; }
      ;
FLIST1: /* empty */      { $$ = NULL; /* empty list of fields */ }
      | FLIST            { $$ = $1;   /* or not empty.. just to fix reduce/reduce conflict */ }
      ;
FIELD : NAME ':' OPTNL TYPE ';' { $$ = mkfield ( $1, $4, $3 ); }
      ;
REDEF : KSELF '\\' SPECIF ':' TYPE ';' { $$ = NULL; printf ( "Warning at line %d: field redefinition ignored\n", ec_curline() ); /* redefinition of inherited field */ }
      ;
SPECIF: NAME             { $$ = mktlist ( $1, 0 ); /* inherited field specification */ } 
      | NAME '.' SPECIF  { $$ = mktlist ( $1, $3 ); }
      ;

OPTNL : /* empty */      { $$ = 0; }
      | KOPT             { $$ = 1; }
      ;

UNIQIT: NAME ':' TLIST ';'  { $$ = $3;   /* UNIQUE statement */ }
      | NAME ':' SPCLST ';' { $$ = NULL; /* UNIQUE statement */ }
      ;
UNIQLS: UNIQIT           { $$ = NULL;    /* list of 1 UNIQUE statements */ }
      | UNIQIT UNIQLS    { $$ = mktlists ( $1, $2 );/* list of UNIQUE statements */ }
      ;
UNIQUE: /* empty */      { $$ = NULL;    /* no UNIQUE clause in ENTITY */ }
      | KUNIQ UNIQLS     { $$ = $2;      /* UNIQUE clause in ENTITY */ }
      ;
SPCLST: KSELF '\\' SPECIF      { $$ = 0; /* list of specifications */ }
      | KSELF '\\' SPECIF ',' SPCLST { $$ = 0; }
      | NAME ',' SPCLST        { $$ = 0; }
      ;

%%

/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static ExpToCas_Schema *mkschema ( char *name, ExpToCas_HSequenceOfItem *ilist )
{
  ExpToCas_Schema *sch = new ExpToCas_Schema ( name, ilist );
  ExpToCas::Schema() = sch;
  return sch;
}

static ExpToCas_HSequenceOfItem *mkilist ( ExpToCas_Item *item, ExpToCas_HSequenceOfItem *seq )
{
  if ( ! seq ) { 
    seq = new ExpToCas_HSequenceOfItem;
    seq->Append ( item );
  }
  else seq->Prepend ( item );
  return seq;
}

static ExpToCas_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new ExpToCas_Enum ( name, tlist );
}

static ExpToCas_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new ExpToCas_Select ( name, tlist );
}

static ExpToCas_Item *mkalias ( char *name, ExpToCas_Type *type )
{
  return new ExpToCas_Alias ( name, type );
}

static ExpToCas_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
				 ExpToCas_HSequenceOfField *field, int isabstract )
{
  ExpToCas_Entity *ent = new ExpToCas_Entity ( name, inherit, field );
  if ( isabstract ) ent->SetAbstractFlag ( Standard_True );
  return ent;
}

static ExpToCas_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items)
{
  return new ExpToCas_Reference ( name, items );
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

static ExpToCas_Type *mktstd ( int keyword )
{
  switch ( keyword ) {
  case KINT : return new ExpToCas_Integer;
  case KNUM : return new ExpToCas_Number;
  case KDBL : return new ExpToCas_Real;
  case KSTR : return new ExpToCas_String;
  case KBOOL: return new ExpToCas_Boolean;
  case KLOG : return new ExpToCas_Logical;
  default   : ec_error ( "Predefined type not treated!", "" );
              return NULL;
  }
}

static ExpToCas_Type *mktname ( char *name )
{
  return new ExpToCas_NamedType ( name );
}

static ExpToCas_Type *mktset ( int keyword, int ilow, int ihigh, ExpToCas_Type *of )
{
  switch ( keyword ) {
  case KLIST: return new ExpToCas_List  ( ilow, ihigh, of );
  case KARR : return new ExpToCas_Array ( ilow, ihigh, of );
  case KBAG : return new ExpToCas_Bag   ( ilow, ihigh, of );
  case KSET : return new ExpToCas_Set   ( ilow, ihigh, of );
  default   : ec_error ( "Complex type not treated!", "" );
              return NULL;
  }
}

static ExpToCas_Field *mkfield ( char *name, ExpToCas_Type *type, int optional )
{
  return new ExpToCas_Field ( name, type, optional );
}

static ExpToCas_HSequenceOfField *mkflist ( ExpToCas_Field *field, ExpToCas_HSequenceOfField *seq )
{
  if ( seq ) seq->Prepend ( field );
  else {
    seq = new ExpToCas_HSequenceOfField;
    seq->Append ( field );
  }
  return seq;
}

/*******************************************************************/
/* External interface to result of parsing */

Handle(ExpToCas_Schema) ec_parse ( FILE *fin )
{
  extern FILE *yyin;
  yyin = fin;
  yyparse();
  return ExpToCas::Schema();
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

/*  tlistfree ( yylval.tlist ); * /
  return num;
}
*/
