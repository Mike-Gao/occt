%{

/* This LEX scanner is intended for formatting EXPRESS definitions */
/* in order to allow comparison with other: */
/* - remove comments */
/* - remove all spaces */
/* - insert a CR (\n) after each ';' */
/* - insert a space after each word (identifier or number) */

/************************************/
/* Section 1                        */
/* definitions                      */
%}


%x COMM

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
"(*"                 BEGIN(COMM);
<COMM>.|\n           ; /* Eat multiline comments */
<COMM>"*)"           BEGIN(0);

;                    printf ( ";\n" );
[A-Za-z_0-9+-]+      printf ( "%s ", yytext );
[ \t\n]+             ; /* condense spaces */

%%

/************************************/
/* Section 3                        */
/* calling and auxiliary procedures */

int main ( void )
{
  yylex();
}

int yywrap(void) { return 1; }
