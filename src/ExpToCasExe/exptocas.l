%{

/* File:	exptocas.l                     */
/* Created:	Thu Oct 28 12:21:16 1999       */
/* Author:	Andrey BETENEV                 */
/* 		<abv@doomox.nnov.matra-dtv.fr> */
/* Copyright:	Matra Datavision 1999          */

/*****************************************************************************\

This LEX scanner is performs lexical analysis of EXPRESS schema file   
for EXPRESS -> CASCADE/XSTEP classes generator                             

On the stage of lexical scanner comments (single- and multi-line),         
definitions of CONSTANT, FUNCTION, RULE items and clauses WHERE, INVERSE
and DERIVE of TYPE amd ENTITY items are dismissed (ignored)         

Then, keywords such as ENTITY, SUPERTYPE, SET etc., names of items         
and special symbols are identified and returned to parser (yacc) as tokens 

Also, error processing and current line number tracking functions are defined

\*****************************************************************************/

/************************************/
/* Section 1                        */
/* definitions                      */

#include <stdlib.h>
#include <string.h>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <ExpToCas_HSequenceOfField.hxx>
#include <ExpToCas_HSequenceOfItem.hxx>
#include <ExpToCas_Field.hxx>
#include <ExpToCas_Item.hxx>
#include <ExpToCas_Type.hxx>
#include <ExpToCas_Schema.hxx>
#include <ExpToCas_Reference.hxx>

#include "expparse.h"       /* define tokens */

/* Auxiliary functions */

static int fun_level=0;
static int ec_linenum=1;
static int ec_state = 0;

int yywrap(void) { return 1; }

int ec_curline ( void )
{
  return ec_linenum;
}

int ec_error ( char *s, char *text )
{
  printf ( "\nError at line %d: %s \"%s\"\n", ec_curline(), s, text );
  return 0;
}

%}

%s TYP ENT
%x COMM SKP RULE FUN

%a 4000
%o 6000

%{

/************************************/
/* Section 2                        */
/* parsing rules                    */
%}

%%

"--".*               { /* Eat line comments */ }
"(*"                 { ec_state = YYSTATE; BEGIN(COMM); }
<COMM>.              { /* Eat multiline comments */ }
<COMM>"*)"           { BEGIN(ec_state); }

"SCHEMA"             { return yylval.num = KSCHEM; }
"END_SCHEMA"         { return yylval.num = KENDS; }

"TYPE"               { BEGIN(TYP); return yylval.num = KTYP; }
<TYP,SKP>"END_TYPE"  { BEGIN(0);   return yylval.num = KENDT; }

"ENTITY"             { BEGIN(ENT); return yylval.num = KENT; }
<ENT,SKP>"END_ENTITY" { BEGIN(0);  return yylval.num = KENDE; }

<ENT>"INVERSE"       |
<ENT>"DERIVE"        |
<ENT,TYP>"WHERE"     { BEGIN(SKP); }
<SKP>.               { /* eat contents of WHERE and DERIVE subclauses of ENTITY and TYPE */ }

"SELECT"             { return yylval.num = KSEL; }
"ENUMERATION"        { return yylval.num = KENUM; }
"LIST"               { return yylval.num = KLIST; }
"ARRAY"              { return yylval.num = KARR; }
"SET"                { return yylval.num = KSET; }
"BAG"                { return yylval.num = KBAG; }
"OF"                 { return yylval.num = KOF; }

"NUMBER"             { return yylval.num = KNUM; }
"INTEGER"            { return yylval.num = KINT; }
"REAL"               { return yylval.num = KDBL; }
"STRING"             { return yylval.num = KSTR; }
"LOGICAL"            { return yylval.num = KLOG; }
"BOOLEAN"            { return yylval.num = KBOOL; }

"OPTIONAL"           { return yylval.num = KOPT; }
"UNIQUE"             { return yylval.num = KUNIQ; }
"SELF"               { return yylval.num = KSELF; }

"ABSTRACT"           { return yylval.num = KABSTR; }
"SUBTYPE"            { return yylval.num = KSUBT; }
"SUPERTYPE"          { return yylval.num = KSPRT; }
"ANDOR"              { return yylval.num = KANDOR; }
"ONEOF"              { return yylval.num = K1OF; }
"AND"                { return yylval.num = KAND; }

"UR"[0-9]+           { yylval.str = strdup ( yytext ); return NAME; }

[a-z_][a-z_0-9]*     { yylval.str = strdup ( yytext ); return NAME; }

[0-9]+               { yylval.num = atoi ( yytext ); return NUMBER; }
[,=();?:.\\]|"["|"]" { return yylval.num = yytext[0]; }

<INITIAL,FUN>"FUNCTION"  { BEGIN(FUN); fun_level++; }
<FUN>"(*"            { ec_state = YYSTATE; BEGIN(COMM); /* eat comments in functions */ }
<FUN>"--".*          { /* Eat line comments in functions */ }
<FUN>[A-Za-z_0-9]*   { /* eat FUNCTIONs - skip IDs explicitly */ }
<FUN>\'[^\']*\'      { /* eat FUNCTIONs - skip strings explicitly */ }
<FUN>.               { /* eat FUNCTIONs - skip all other symbols in functions */ }
<FUN>"END_FUNCTION;" { fun_level--; if ( ! fun_level ) BEGIN(0); }

"RULE"               { BEGIN(RULE); /* eat RULEs */ }
<RULE>.              { /* eat RULEs */ }
<RULE>"END_RULE;"    { BEGIN(0); }

"CONSTANT"[ \t\na-z_0-9:=;'()|-]+"END_CONSTANT;" { /* eat CONSTANTs */
                       char *s = yytext; /* but don't forget to count lines.. */
                       while ( *s ) if ( *(s++) == '\n' ) ec_linenum++;
                     }

[ \t]+               { /* eat spaces */ }
[A-Za-z0-9_]+        { ec_error ( "unknown keyword ", yytext ); /* put unrecognized keywords to cerr */ }
.                    { ec_error ( "unknown symbol ", yytext ); /* put unrecognized data to cerr */ }

<INITIAL,COMM,SKP,RULE,ENT,TYP,FUN>\n  { ec_linenum++; /* count lines */ }

%%

/************************************/
/* Section 3                        */
/* auxiliary procedures             */

/*
int main ( void )
{
  yylex();
}
*/
