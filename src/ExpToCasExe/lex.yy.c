#include <stdio.h>
#include <stdlib.h>
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# ifndef YYLMAX 
# define YYLMAX BUFSIZ
# endif 
#ifndef __cplusplus
# define output(c) (void)putc(c,yyout)
#else
# define lex_output(c) (void)putc(c,yyout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
	int yylex(void);
#ifdef YYLEX_E
	void yywoutput(wchar_t);
	wchar_t yywinput(void);
#endif
#ifndef yyless
	int yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef LEXDEBUG
	void allprint(char);
	void sprint(char *);
#endif
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void exit(int);
#ifdef __cplusplus
}
#endif

#endif
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
#ifndef __cplusplus
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#else
# define lex_input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
#define YYISARRAY
char yytext[YYLMAX];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;


# line 4 "explist.l"
/* This LEX scanner produces list of items included in the EXPRESS schema  */

# line 5 "explist.l"
/* (TYPE, ENTITY, FUNCTION, RULE) */


# line 7 "explist.l"
/************************************/

# line 8 "explist.l"
/* Section 1                        */

# line 9 "explist.l"
/* definitions                      */
# define CONST 2
# define TYP 4
# define TYP1 6
# define ENT 8
# define ENT1 10
# define LIST 12
# define COMM 14
# define SKP 16
# define FUN 18
# define FUN1 20
# define RUL 22

static int fun_level=0;


# line 26 "explist.l"
/************************************/

# line 27 "explist.l"
/* Section 2                        */

# line 28 "explist.l"
/* parsing rules                    */
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
#ifdef __cplusplus
/* to avoid CC and lint complaining yyfussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto yyfussy;
#endif
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 32 "explist.l"
              ;
break;
case 2:

# line 33 "explist.l"
                { printf ( "Starting multiline comment\n" ); BEGIN(COMM); }
break;
case 3:

# line 34 "explist.l"
          ;
break;
case 4:

# line 35 "explist.l"
          { printf ( "End of multiline comment\n" ); BEGIN(0); }
break;
case 5:

# line 37 "explist.l"
    printf ( "Starting %s\n", yytext );
break;
case 6:

# line 38 "explist.l"
         printf ( "Schema finished\n" );
break;
case 7:

# line 40 "explist.l"
                { BEGIN(TYP); }
break;
case 8:

# line 41 "explist.l"
         { printf ( "TYPE %s\n", yytext ); BEGIN(TYP1); }
break;
case 9:

# line 42 "explist.l"
          ;
break;
case 10:

# line 43 "explist.l"
 { printf ( "\n" ); BEGIN(0); }
break;
case 11:

# line 45 "explist.l"
              { BEGIN(ENT); }
break;
case 12:

# line 46 "explist.l"
         { printf ( "ENTITY %s\n", yytext ); BEGIN(ENT1); }
break;
case 13:

# line 47 "explist.l"
          ;
break;
case 14:

# line 48 "explist.l"
 BEGIN(0);
break;
case 15:

# line 50 "explist.l"
     case 16:

# line 51 "explist.l"
      BEGIN(SKP);
break;
case 17:

# line 52 "explist.l"
           ;
break;
case 18:

# line 54 "explist.l"
 { BEGIN(FUN); fun_level++; }
break;
case 19:

# line 55 "explist.l"
         { printf ( "FUNCTION %s\n", yytext ); BEGIN(FUN1); }
break;
case 20:

# line 56 "explist.l"
          ;
break;
case 21:

# line 57 "explist.l"
 { fun_level--; if ( ! fun_level ) BEGIN(0); }
break;
case 22:

# line 59 "explist.l"
                { BEGIN(RUL); }
break;
case 23:

# line 60 "explist.l"
         { printf ( "RULE %s\n", yytext ); BEGIN(SKP); }
break;
case 24:

# line 61 "explist.l"
       BEGIN(0);
break;
case 25:

# line 63 "explist.l"
                ;
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

# line 66 "explist.l"

/************************************/
/* Section 3                        */
/* calling and auxiliary procedures */

int main ( void )
{
  yylex();
}

int yywrap(void) { return 1; }
int yyvstop[] = {
0,

25,
0, 

25,
0, 

25,
0, 

25,
0, 

25,
0, 

25,
0, 

25,
0, 

25,
0, 

25,
0, 

8,
0, 

9,
0, 

9,
0, 

12,
0, 

13,
0, 

13,
0, 

3,
0, 

3,
0, 

17,
0, 

17,
0, 

19,
0, 

20,
0, 

20,
0, 

20,
0, 

20,
0, 

23,
0, 

2,
0, 

1,
0, 

4,
0, 

22,
0, 

7,
0, 

16,
0, 

11,
0, 

15,
0, 

18,
0, 

10,
0, 

24,
0, 

5,
0, 

14,
0, 

6,
0, 

21,
0, 
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,25,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,25,	1,25,	
0,0,	0,0,	2,32,	2,32,	
0,0,	0,0,	22,52,	22,52,	
32,62,	32,62,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,26,	111,119,	32,62,	
0,0,	2,26,	1,27,	0,0,	
45,69,	1,25,	2,27,	16,45,	
15,44,	26,56,	27,57,	0,0,	
0,0,	0,0,	0,0,	0,0,	
15,44,	15,44,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,28,	5,34,	
5,35,	8,39,	2,28,	2,33,	
12,43,	18,47,	22,51,	22,53,	
30,60,	32,63,	34,65,	1,29,	
1,30,	1,31,	7,38,	2,29,	
2,30,	2,31,	5,36,	28,58,	
29,59,	15,45,	7,38,	7,38,	
1,25,	31,61,	5,37,	15,44,	
5,37,	5,37,	5,37,	5,37,	
5,37,	5,37,	5,37,	5,37,	
5,37,	5,37,	5,37,	5,37,	
5,37,	5,37,	5,37,	5,37,	
5,37,	5,37,	5,37,	5,37,	
5,37,	5,37,	5,37,	5,37,	
5,37,	5,37,	33,64,	35,66,	
36,67,	40,68,	47,70,	48,71,	
54,72,	7,38,	58,73,	59,75,	
60,76,	61,77,	64,78,	65,79,	
66,80,	67,81,	68,82,	70,83,	
71,84,	72,85,	15,44,	73,86,	
74,87,	75,88,	58,74,	76,89,	
77,90,	9,40,	7,39,	78,91,	
79,92,	80,93,	81,94,	82,95,	
83,96,	84,97,	85,98,	86,99,	
87,100,	89,101,	91,102,	92,103,	
57,57,	93,104,	94,105,	95,106,	
97,108,	98,107,	99,109,	100,110,	
57,57,	57,0,	101,111,	9,41,	
7,38,	9,41,	9,41,	9,41,	
9,41,	9,41,	9,41,	9,41,	
9,41,	9,41,	9,41,	9,41,	
9,41,	9,41,	9,41,	9,41,	
9,41,	9,41,	9,41,	9,41,	
9,41,	9,41,	9,41,	9,41,	
9,41,	9,41,	9,41,	10,40,	
102,112,	103,113,	104,114,	106,115,	
107,116,	96,106,	11,42,	57,57,	
17,46,	108,117,	109,118,	112,120,	
114,121,	115,122,	11,42,	11,42,	
17,46,	17,46,	96,107,	116,123,	
96,104,	117,124,	118,125,	120,127,	
121,128,	10,41,	122,129,	10,41,	
10,41,	10,41,	10,41,	10,41,	
10,41,	10,41,	10,41,	10,41,	
10,41,	10,41,	10,41,	10,41,	
10,41,	10,41,	10,41,	10,41,	
10,41,	10,41,	10,41,	10,41,	
10,41,	10,41,	10,41,	10,41,	
10,41,	11,42,	57,57,	17,46,	
123,130,	124,131,	125,132,	126,126,	
126,126,	126,126,	126,126,	126,126,	
126,126,	126,126,	126,126,	126,126,	
126,126,	129,134,	126,133,	131,135,	
132,136,	134,137,	11,43,	19,48,	
17,47,	135,138,	136,139,	138,140,	
140,141,	0,0,	21,50,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	21,50,	21,50,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
11,42,	19,49,	17,46,	19,49,	
19,49,	19,49,	19,49,	19,49,	
19,49,	19,49,	19,49,	19,49,	
19,49,	19,49,	19,49,	19,49,	
19,49,	19,49,	19,49,	19,49,	
19,49,	19,49,	19,49,	19,49,	
19,49,	19,49,	19,49,	19,49,	
19,49,	21,50,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	23,54,	21,51,	0,0,	
0,0,	0,0,	0,0,	0,0,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	23,55,	
21,50,	23,55,	23,55,	23,55,	
23,55,	23,55,	23,55,	23,55,	
23,55,	23,55,	23,55,	23,55,	
23,55,	23,55,	23,55,	23,55,	
23,55,	23,55,	23,55,	23,55,	
23,55,	23,55,	23,55,	23,55,	
23,55,	23,55,	23,55,	37,37,	
0,0,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	37,37,	
37,37,	37,37,	37,37,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	41,41,	0,0,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	41,41,	41,41,	
41,41,	41,41,	49,49,	0,0,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	49,49,	49,49,	
49,49,	49,49,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
119,126,	0,0,	119,126,	119,126,	
119,126,	119,126,	119,126,	119,126,	
119,126,	119,126,	119,126,	119,126,	
119,126,	119,126,	119,126,	119,126,	
119,126,	119,126,	119,126,	119,126,	
119,126,	119,126,	119,126,	119,126,	
119,126,	119,126,	119,126,	119,126,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	55,55,	0,0,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	55,55,	55,55,	55,55,	
55,55,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-5,	yysvec+1,	0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+3,	0,		0,	
yycrank+0,	yysvec+5,	0,	
yycrank+-85,	0,		0,	
yycrank+-4,	yysvec+7,	0,	
yycrank+84,	yysvec+5,	0,	
yycrank+138,	yysvec+5,	0,	
yycrank+-213,	0,		0,	
yycrank+-7,	yysvec+11,	0,	
yycrank+0,	0,		0,	
yycrank+0,	0,		0,	
yycrank+-51,	0,		0,	
yycrank+-9,	yysvec+15,	0,	
yycrank+-215,	0,		0,	
yycrank+-8,	yysvec+17,	0,	
yycrank+214,	0,		0,	
yycrank+0,	yysvec+19,	0,	
yycrank+-289,	0,		0,	
yycrank+-9,	yysvec+21,	0,	
yycrank+288,	0,		0,	
yycrank+0,	yysvec+23,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+11,	0,		yyvstop+3,
yycrank+9,	0,		yyvstop+5,
yycrank+13,	0,		yyvstop+7,
yycrank+7,	0,		yyvstop+9,
yycrank+13,	0,		yyvstop+11,
yycrank+8,	0,		yyvstop+13,
yycrank+11,	0,		yyvstop+15,
yycrank+41,	0,		yyvstop+17,
yycrank+13,	0,		0,	
yycrank+49,	0,		0,	
yycrank+56,	0,		0,	
yycrank+316,	0,		yyvstop+19,
yycrank+0,	0,		yyvstop+21,
yycrank+0,	yysvec+35,	yyvstop+23,
yycrank+51,	0,		0,	
yycrank+391,	0,		yyvstop+25,
yycrank+0,	0,		yyvstop+27,
yycrank+0,	yysvec+40,	yyvstop+29,
yycrank+0,	0,		yyvstop+31,
yycrank+7,	0,		yyvstop+33,
yycrank+0,	0,		yyvstop+35,
yycrank+52,	0,		yyvstop+37,
yycrank+53,	0,		0,	
yycrank+419,	0,		yyvstop+39,
yycrank+0,	0,		yyvstop+41,
yycrank+0,	yysvec+48,	yyvstop+43,
yycrank+0,	yysvec+32,	yyvstop+45,
yycrank+0,	yysvec+33,	yyvstop+47,
yycrank+54,	0,		0,	
yycrank+494,	0,		yyvstop+49,
yycrank+0,	0,		yyvstop+51,
yycrank+-167,	0,		yyvstop+53,
yycrank+66,	0,		0,	
yycrank+59,	0,		0,	
yycrank+64,	0,		0,	
yycrank+57,	0,		0,	
yycrank+0,	yysvec+32,	0,	
yycrank+0,	yysvec+33,	0,	
yycrank+60,	0,		0,	
yycrank+57,	0,		0,	
yycrank+72,	0,		0,	
yycrank+72,	0,		0,	
yycrank+74,	0,		0,	
yycrank+0,	0,		yyvstop+55,
yycrank+75,	0,		0,	
yycrank+76,	0,		0,	
yycrank+77,	0,		0,	
yycrank+52,	0,		0,	
yycrank+75,	0,		0,	
yycrank+80,	0,		0,	
yycrank+82,	0,		0,	
yycrank+83,	0,		0,	
yycrank+88,	0,		0,	
yycrank+83,	0,		0,	
yycrank+62,	0,		0,	
yycrank+76,	0,		0,	
yycrank+64,	0,		0,	
yycrank+65,	0,		0,	
yycrank+66,	0,		0,	
yycrank+67,	0,		0,	
yycrank+80,	0,		0,	
yycrank+80,	0,		0,	
yycrank+0,	0,		yyvstop+57,
yycrank+88,	0,		0,	
yycrank+0,	0,		yyvstop+59,
yycrank+82,	0,		0,	
yycrank+81,	0,		0,	
yycrank+85,	0,		0,	
yycrank+101,	0,		0,	
yycrank+102,	0,		0,	
yycrank+144,	0,		0,	
yycrank+102,	0,		0,	
yycrank+91,	0,		0,	
yycrank+107,	0,		0,	
yycrank+86,	0,		0,	
yycrank+113,	0,		0,	
yycrank+135,	0,		0,	
yycrank+140,	0,		0,	
yycrank+121,	0,		0,	
yycrank+0,	0,		yyvstop+61,
yycrank+133,	0,		0,	
yycrank+127,	0,		0,	
yycrank+132,	0,		0,	
yycrank+146,	0,		0,	
yycrank+0,	0,		yyvstop+63,
yycrank+10,	0,		0,	
yycrank+140,	0,		0,	
yycrank+0,	0,		yyvstop+65,
yycrank+140,	0,		0,	
yycrank+137,	0,		0,	
yycrank+151,	0,		0,	
yycrank+151,	0,		0,	
yycrank+161,	0,		0,	
yycrank+457,	0,		0,	
yycrank+153,	0,		0,	
yycrank+163,	0,		0,	
yycrank+161,	0,		0,	
yycrank+195,	0,		0,	
yycrank+198,	0,		0,	
yycrank+189,	0,		0,	
yycrank+219,	yysvec+119,	0,	
yycrank+0,	0,		yyvstop+67,
yycrank+0,	0,		yyvstop+69,
yycrank+193,	0,		0,	
yycrank+0,	0,		yyvstop+71,
yycrank+195,	0,		0,	
yycrank+215,	0,		0,	
yycrank+0,	0,		yyvstop+73,
yycrank+192,	0,		0,	
yycrank+212,	0,		0,	
yycrank+227,	0,		0,	
yycrank+0,	0,		yyvstop+75,
yycrank+208,	0,		0,	
yycrank+0,	0,		yyvstop+77,
yycrank+210,	0,		0,	
yycrank+0,	0,		yyvstop+79,
0,	0,	0};
struct yywork *yytop = yycrank+616;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,  95, 
  1,  95,  95,  95,  95,  95,  95,  95, 
 95,  95,  95,  95,  95,  95,  95,  95, 
 95,  95,  95,  95,  95,  95,  95,  95, 
 95,  95,  95,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)ncform	6.11	97/01/06 SMI"

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
#ifndef __cplusplus
			*yylastch++ = yych = input();
#else
			*yylastch++ = yych = lex_input();
#endif
#ifdef YYISARRAY
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
#else
			if (yylastch >= &yytext[ yytextsz ]) {
				int	x = yylastch - yytext;

				yytextsz += YYTEXTSZINC;
				if (yytext == yy_tbuf) {
				    yytext = (char *) malloc(yytextsz);
				    memcpy(yytext, yy_tbuf, sizeof (yy_tbuf));
				}
				else
				    yytext = (char *) realloc(yytext, yytextsz);
				if (!yytext) {
				    fprintf(yyout,
					"Cannot realloc yytext\n");
				    exit(1);
				}
				yylastch = yytext + x;
			}
#endif
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
#ifndef __cplusplus
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#else
		yyprevious = yytext[0] = lex_input();
		if (yyprevious>0)
			lex_output(yyprevious);
#endif
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
#ifndef __cplusplus
	return(input());
#else
	return(lex_input());
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
#ifndef __cplusplus
	output(c);
#else
	lex_output(c);
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
