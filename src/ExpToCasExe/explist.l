%{

/* This LEX scanner produces list of items included in the EXPRESS schema  */
/* (TYPE, ENTITY, FUNCTION, RULE) */

/************************************/
/* Section 1                        */
/* definitions                      */
%}


%x CONST, TYP, TYP1, ENT, ENT1, LIST, COMM, SKP, FUN, FUN1, RUL

KEYWORD    [A-Z_]+
NAME       [a-z_][a-z0-9_]*
NUMBER     [0-9]+
SPACE      [ \t\n]
SPC        {SPACE}+
SP         {SPACE}*

%{

static int fun_level=0;

/************************************/
/* Section 2                        */
/* parsing rules                    */
%}

%%

"--".*               ; /* Eat line comments */
"(*"                 { printf ( "Starting multiline comment\n" ); BEGIN(COMM); }
<COMM>.|\n           ; /* Eat multiline comments */
<COMM>"*)"           { printf ( "End of multiline comment\n" ); BEGIN(0); }

SCHEMA" "{NAME};     printf ( "Starting %s\n", yytext ); 
END_SCHEMA;          printf ( "Schema finished\n" );

TYPE                 { BEGIN(TYP); }
<TYP>{NAME}          { printf ( "TYPE %s\n", yytext ); BEGIN(TYP1); }
<TYP1>.|\n           ; /* eat any unrecognized data */
<TYP,TYP1,SKP>END_TYPE  { printf ( "\n" ); BEGIN(0); }

ENTITY               { BEGIN(ENT); }
<ENT>{NAME}          { printf ( "ENTITY %s\n", yytext ); BEGIN(ENT1); }
<ENT1>.|\n           ; /* eat any unrecognized data */
<ENT,ENT1,SKP>END_ENTITY  BEGIN(0);

<ENT,TYP>DERIVE      |
<ENT,TYP>WHERE       BEGIN(SKP);
<SKP>.|\n            ; /* eat contents of WHERE and DERIVE subclauses */

<INITIAL,FUN1>^{SP}FUNCTION  { BEGIN(FUN); fun_level++; }
<FUN>{NAME}          { printf ( "FUNCTION %s\n", yytext ); BEGIN(FUN1); }
<FUN1>.|\n           ; /* eat contents of WHERE and DERIVE subclauses */
<FUN,FUN1>END_FUNCTION  { fun_level--; if ( ! fun_level ) BEGIN(0); }

RULE                 { BEGIN(RUL); }
<RUL>{NAME}          { printf ( "RULE %s\n", yytext ); BEGIN(SKP); }
<RUL,SKP>END_RULE        BEGIN(0);

.|\n                 ; /* eat any unrecognized data */

%%

/************************************/
/* Section 3                        */
/* calling and auxiliary procedures */

int main ( void )
{
  yylex();
}

int yywrap(void) { return 1; }
