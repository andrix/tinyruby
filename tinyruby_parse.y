%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"
#include "types.h"
#include "tinyruby.h"
#include "eval.h"
#include "error.h"
#include "vtable.h"

extern FILE *yyin;

NODE * nodeprogram;

%}

%union {
    NODE * node;
    char * id;
    long vinteger;
    double vfloat; 
    char * vstring;
}
%start program
%error-verbose
%locations
%token <id> ID
%token <vinteger> NINTEGER
%token <vfloat> NFLOAT
%token <vstring> TSTRING
%token <node> DEF END RETURN PUTS DO IF ELSE ELSIF CASE WHEN THEN WHILE
%token <node> FALSE CLASS REQUIRE LOAD NIL B_AND B_OR B_NOT STMT_TERM GETS TRUE
%token <node> OP_POWER OP_EQUAL OP_LESS_EQUAL OP_GREAT_EQUAL OP_CMP
%token <node> OP_DISTINCT OP_IDENTITY OP_ASSIGNPLUS OP_ASSIGNMINS OP_ASSIGNMULT
%token <node> OP_ASSIGNDIV

%type <node> program stmts stmt dot expr lhs variable primary when_body fargs operation expr_list method_call body_class rdef

%nonassoc  IF WHILE
%left B_OR B_AND
%right B_NOT
%right '=' 
%right '?' ':'
%nonassoc  OP_CMP OP_EQUAL OP_IDENTITY OP_DISTINCT
%left  '>' OP_GREAT_EQUAL '<' OP_LESS_EQUAL
%left  '|' '^'
%left  '&'
/* %left  tLSHFT tRSHFT */
%left  '+' '-'
%left  '*' '/' '%'
/* %right tUMINUS_NUM tUMINUS */
%right OP_POWER
%right '!' '~' /* tUPLUS */
%left '.'

%% 
/* Grammar and rules */

program         : stmts { 
                        nodeprogram = $1;
                  } 
                ;

stmts           : stmt stmts {
                    $$ = ast_make_stmts($1, $2); 
                 } 
                |
                  /* empty */ { 
                    $$ = NULL; 
                  }
                ;

stmt            : expr STMT_TERM {
                    $$ = ast_make_expr($1);
                  }
                ;

expr            : lhs '=' expr                { $$ = ast_make_binop('=', $1, $3); }
                | lhs OP_ASSIGNPLUS expr      { $$ = ast_make_binop('=', $1, ast_make_binop('+', $1, $3)); }
                | lhs OP_ASSIGNMINS expr      { $$ = ast_make_binop('=', $1, ast_make_binop('-', $1, $3)); }
                | lhs OP_ASSIGNMULT expr      { $$ = ast_make_binop('=', $1, ast_make_binop('*', $1, $3)); }
                | lhs OP_ASSIGNDIV expr       { $$ = ast_make_binop('=', $1, ast_make_binop('/', $1, $3)); }
                | NINTEGER                    { $$ = ast_make_numeric(ruby_integer($1)); }
                | NFLOAT                      { $$ = ast_make_numeric(ruby_float($1)); }
                | TSTRING                     { $$ = ast_make_string(ruby_string($1)); }
                | NIL                         { $$ = ast_make_internal(Qnil); }
                | TRUE                        { $$ = ast_make_internal(Qtrue); }
                | FALSE                       { $$ = ast_make_internal(Qfalse); }
                | PUTS expr                   { $$ = ast_make_puts($2); }
                | '-' expr                    { $$ = ast_make_uniop('-', $2); }
                | '+' expr                    { $$ = ast_make_uniop('+', $2); }
                | '!' expr                    { $$ = ast_make_uniop('!', $2); }
                | '~' expr                    { $$ = ast_make_uniop('~', $2); }
                | B_NOT expr                  { $$ = ast_make_uniop(AST_OP_NOT, $2); }
                | expr '+' expr               { $$ = ast_make_binop('+', $1, $3); }
                | expr '-' expr               { $$ = ast_make_binop('-', $1, $3); }
                | expr '*' expr               { $$ = ast_make_binop('*', $1, $3); }
                | expr '/' expr               { $$ = ast_make_binop('/', $1, $3); }
                | expr '%' expr               { $$ = ast_make_binop('%', $1, $3); }
                | expr OP_POWER expr          { $$ = ast_make_binop(AST_OP_POW, $1, $3); }
                | expr B_AND expr             { $$ = ast_make_binop(AST_OP_AND, $1, $3); }
                | expr B_OR expr              { $$ = ast_make_binop(AST_OP_OR, $1, $3); }
                | expr '>' expr               { $$ = ast_make_binop('>', $1, $3); }
                | expr '<' expr               { $$ = ast_make_binop('<', $1, $3); }
                | expr OP_LESS_EQUAL expr     { $$ = ast_make_binop(AST_OP_LE_EQUAL, $1, $3); }
                | expr OP_GREAT_EQUAL expr    { $$ = ast_make_binop(AST_OP_GR_EQUAL, $1, $3); }
                | expr OP_EQUAL expr          { $$ = ast_make_binop(AST_OP_EQUALS, $1, $3); }
                | expr OP_CMP expr            { $$ = ast_make_binop(AST_OP_CMP, $1, $3); }
                | expr OP_DISTINCT expr       { $$ = ast_make_binop(AST_OP_DISTINCT, $1, $3); }
                | expr OP_IDENTITY expr       { $$ = ast_make_binop(AST_OP_IDENTITY, $1, $3); }
                | expr '?' expr ':' expr      { $$ = ast_make_cond($1, $3, $5); }
                | '(' expr ')'                { $$ = $2; }
                | GETS                        { $$ = ast_make_gets(); }
		        | dot			{$$=$1;}
                | primary           {$$=$1;}
		        | RETURN expr        { $$ = ast_make_return($2);}
		        ;

dot		        : expr '.' expr                 { $$ = ast_make_dot($1,$3);} 
		        ;

primary         : IF expr STMT_TERM stmts END                       { $$ = ast_make_cond($2, $4, NULL); }
                | IF expr STMT_TERM stmts ELSE STMT_TERM stmts END  { $$ = ast_make_cond($2, $4, $7); }
                | WHILE expr STMT_TERM stmts END                    { $$ = ast_make_while($2, $4); }
                | CASE STMT_TERM when_body END                      { $$ = ast_make_case($3); }
                | CLASS ID STMT_TERM body_class END  {$$ = ast_make_clase($2,$4); }
		        | rdef { $$=$1; }
		        | method_call
                | variable 
	            ;

rdef 		    : DEF ID '(' fargs ')' STMT_TERM stmts END          { $$ = ast_make_def($2, $4, $7); } 
                ;

body_class      : /**/ { $$=NULL;}
		        | rdef  STMT_TERM body_class                        {$$= ast_make_body_clase($1,$3);}  
                ; 

fargs		    : /*vacio*/                                         { $$ = NULL; }
                | ID                                                { $$ = ast_make_fargs($1, NULL); }
		        | ID ',' fargs                                      { $$ = ast_make_fargs($1, $3); }
                ;

expr_list       : /* empty */                                       { $$ = NULL; }
                | expr                                              { $$ = ast_make_exprlist($1, NULL); }
                | expr ',' expr_list                                { $$ = ast_make_exprlist($1, $3); }
                ;

variable        : ID { $$ = ast_make_id($1, AST_ID_C); }
		        | '@' ID {$$ = ast_make_id($2, AST_ID_A);}
		        | '$' ID {$$= ast_make_id($2, AST_ID_G);}
                ;

operation       : ID                                                { $$ = ast_make_operation($1, 0); }
                | ID '!'                                            { $$ = ast_make_operation($1, '!'); }
                | ID '?'                                            { $$ = ast_make_operation($1, '?'); }
                ;

method_call     : operation '(' expr_list ')'                       { $$ = ast_make_methodcall($1, $3, NULL); }
                ;

when_body       : WHEN expr THEN stmts                              { $$ = ast_make_when($2, $4, NULL); }
                | WHEN expr THEN stmts when_body                    { $$ = ast_make_when($2, $4, $5);   }
                ;

lhs             : variable { $$ = $1; }
                ;



%% 
/* C subroutines */

int main(int argc, char *argv[]) {
    if (argc > 1) 
        yyin = fopen(argv[1], "r");
    yyparse();
    if (DEBUG) {
        printf(" Abstract syntax tree : \n");
        printf(" ======================================== \n");
        ast_node_print(nodeprogram, 0, "ROOT");
        printf(" ======================================== \n");
    }
    VALUE init;
    int status;
    scope_init();
    scope_begin();
    E_STATUS_INFO eval_info;
    eval_info.o = 0;
    eval(nodeprogram, init, &eval_info, RB_MAIN_CLASS);
    scope_end();

    if(DEBUG) {	
        printf(" MORE DEBUG INFO\n");
        printf(" ======================================== \n");
    	scope_print();
        heap_print();
        printf(" ======================================== \n");
    }
    switch (eval_info.code) {
        case INTERNAL_ERROR:
            yyerror("internal error");
            break;
        case UNRECOGNIZED_NODE_TYPE:
            yyerror("unrecognized node type");
            break;
        case TYPE_MISMATCH:
            yyerror("type mismatch");
            break;
        case UNDEFINED_VARIABLE:
            yyerror("undefined variable '%s'", eval_info.node->id);
            break;
        case ARGUMENT_ERROR:
            yyerror("argument error");
            break;
        case NO_METHOD_ERROR:
            yyerror("no method error");
            break;
        default:
            return 0;
            break;
    }
    return 1;
}

 /* in code section at the end of the parser */
void
yyerror(char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  if(yylloc.first_line)
    fprintf(stderr, "%d.%d-%d.%d: error: ", yylloc.first_line, yylloc.first_column,
	    yylloc.last_line, yylloc.last_column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

void
lyyerror(YYLTYPE t, char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  if(t.first_line)
    fprintf(stderr, "%d.%d-%d.%d: error: ", t.first_line, t.first_column,
	    t.last_line, t.last_column);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

