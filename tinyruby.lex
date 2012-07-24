%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "y.tab.h"

/* handle locations */
int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
%}

%option yylineno

NINTEGER	            [0-9]+
NFLOAT	                [0-9]+\.[0-9]+
ID	                    [a-zA-Z_][a-zA-Z0-9_]*
TERMCHAR                [;\n]
S_COMMENT               ^#[^\n]*\n
M_COMMENT	            \=begin[.\n]*\=end
OP_POWER                \*\*
OP_ASSIGNPLUS           \+=
OP_ASSIGNMINS           \-=
OP_ASSIGNMULT           \*=
OP_ASSIGNDIV            \/=
OP_EQUAL                ==
OP_IDENTITY             ===
OP_LESS_EQUAL           <=
OP_GREAT_EQUAL          >=
OP_CMP                  <=>
OP_DISTINCT             \!=

%%
{NFLOAT}	            { yylval.vfloat = atof(yytext);
                          return (NFLOAT); }
{NINTEGER}	            { yylval.vinteger = atoi(yytext);
                          return (NINTEGER); }
\"(\\.|[^\\"])*\"        { yylval.vstring = strndup(&yytext[1], strlen(yytext) - 2);
                          return (TSTRING);}
\'(\\.|[^\\'])*\'        { yylval.vstring = strndup(&yytext[1], strlen(yytext) - 2);
                          return (TSTRING);}
def                     { return (DEF); }
end                     { return (END); }
return	                { return (RETURN); }
puts                    { return (PUTS); }
gets                    { return (GETS); }
do                      { return (DO); }
if                      { return (IF); }
else                    { return (ELSE); }
elsif                   { return (ELSIF); }
case	                { return (CASE); }
when	                { return (WHEN); }
then                    { return (THEN); }
while                   { return (WHILE); }
true                    { return (TRUE); }
false                   { return (FALSE); }
class                   { return (CLASS); }
require                 { return (REQUIRE); }
load	                { return (LOAD); }
nil	                    { return (NIL); }
and	                    { return (B_AND); }
\&\&                    { return (B_AND); }
or	                    { return (B_OR); }
\|\|                    { return (B_OR); }
not	                    { return (B_NOT); }
{ID}	                { yylval.id = (char *) strdup(yytext);
                          return (ID); }
^\n                     /* empty line - ignore */  { yycolumn = 1; }
{TERMCHAR}              { if (!strcmp(yytext, "\n"))
                            yycolumn = 1;
                          return (STMT_TERM); 
                        }
{S_COMMENT}	            /* single line comment */
{M_COMMENT}	            /* multiple line comment */ { yycolumn = 1; }
{OP_POWER}              { return (OP_POWER); }
{OP_CMP}                { return (OP_CMP); }
{OP_ASSIGNPLUS}         { return (OP_ASSIGNPLUS); }
{OP_ASSIGNMINS}         { return (OP_ASSIGNMINS); }
{OP_ASSIGNMULT}         { return (OP_ASSIGNMULT); }
{OP_ASSIGNDIV}          { return (OP_ASSIGNDIV); }
{OP_IDENTITY}           { return (OP_IDENTITY); }
{OP_EQUAL}              { return (OP_EQUAL); }
{OP_DISTINCT}           { return (OP_DISTINCT); }
{OP_LESS_EQUAL}         { return (OP_LESS_EQUAL); }
{OP_GREAT_EQUAL}        { return (OP_GREAT_EQUAL); }
[ \t]+                  /* white spaces */
.                       { return (yytext[0]); }
%%

