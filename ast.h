#ifndef _AST_H_
#define _AST_H_
#include "types.h"
#include "tinyruby.h"

#define AST_TYPE_ID          200
#define AST_TYPE_COND        201
#define AST_TYPE_COND_OPTS   202
#define AST_TYPE_ELSIF       203
#define AST_TYPE_FUNCTION    204
#define AST_TYPE_WHILE       205
#define AST_TYPE_VARIABLE    206
#define AST_TYPE_STMT        207
#define AST_TYPE_STMTS       208
#define AST_TYPE_NUMERIC     209
#define AST_TYPE_EXPR        210
#define AST_TYPE_PUTS        211
#define AST_TYPE_INTERNALVAL 212
#define AST_TYPE_STRING      213
#define AST_TYPE_GETS        214
#define AST_TYPE_CASE        215
#define AST_TYPE_WHEN        216
#define AST_TYPE_WHEN_OPTS   217
#define AST_TYPE_DEF	     218
#define AST_TYPE_FARGS	     219

#define AST_TYPE_METHODCALL  220
#define AST_TYPE_METHODCALLDESC 221
#define AST_TYPE_EXPRLIST    222
#define AST_TYPE_METHODNAME  223
#define AST_TYPE_METHODNAMEE 224
#define AST_TYPE_METHODNAMEQ 225
#define AST_TYPE_RETURN      026
#define AST_TYPE_BCLASE      025
#define AST_TYPE_CLASE       024
#define AST_TYPE_NEW	     023
#define AST_TYPE_DOT         022
 	

#define AST_OPASSIGN_PLUS    150
#define AST_OPASSIGN_MINS    151
#define AST_OPASSIGN_MULT    152
#define AST_OPASSIGN_DIV     153
#define AST_OPASSIGN_POW     154
#define AST_OPASSIGN_BAND    155
#define AST_OPASSIGN_BOR     156
#define AST_OPASSIGN_FREEE   157
#define AST_OPASSIGN_BXOR    158
#define AST_OPASSIGN_BLSHIFT 159
#define AST_OPASSIGN_BRSHIFT 160
#define AST_OPASSIGN_AND     161
#define AST_OPASSIGN_OR      162

#define AST_OP_POW           163
#define AST_OP_CMP           164
#define AST_OP_GR_EQUAL      165  
#define AST_OP_LE_EQUAL      166
#define AST_OP_EQUALS        167
#define AST_OP_IDENTITY      168
#define AST_OP_DISTINCT      169
#define AST_OP_ODISTINCT     170
#define AST_OP_AND           171
#define AST_OP_OR            172
#define AST_OP_NOT           173


#define AST_ID_C 1
#define AST_ID_A 2
#define AST_ID_G 3
 


typedef struct ast_tree_node {
    char * id;
    char node_type;
    unsigned long obj_id;
    VALUE val;
    char * klass;
    int tipo;
    struct ast_tree_node *left, *right;
} NODE;

// crea un nodo vacio
NODE * ast_node_new();

// ast_make_id: crea un node del tipo id
NODE * ast_make_id(const char * id, int t);

// ast_make_numeric: return a node that holds a value
NODE * ast_make_numeric(struct RNumeric * ref);

// ast_make_string: return a node that holds a string
NODE * ast_make_string(struct RString * ref);

// ast_make_uniop: return a node that holds an unary operation
NODE * ast_make_uniop(char oper, NODE * expr);

// ast_make_binop: return a node that holds a binary operation
NODE * ast_make_binop(char oper, NODE * lexpr, NODE * rexpr);

// ast_make_expr: return a node that holds a expression
NODE * ast_make_expr(NODE * expr);

// ast_make_expr: return a node that holds a expression to be outputed to stdout
NODE * ast_make_puts(NODE * expr);

// ast_make_internal: return a node that holds an internal value
NODE * ast_make_internal(VALUE cte);

// ast_make_stmts: crea un node del tipo stmts
NODE * ast_make_stmts(NODE * stmt, NODE * stmts);

// ast_make_cond: returns a node that represent a condition
NODE * ast_make_cond(NODE * cond, NODE * iftrue, NODE * ifelse);

//ast_make_gets: returns a node that represent a gets call
NODE * ast_make_gets(void);

// ast_make_while: returns anode with a while statement
NODE * ast_make_while(NODE * cond, NODE * body);

// ast_make_case: returns a node that holds a case stmt
NODE * ast_make_case(NODE * when_body);

// ast_make_when: returns a node that holds a when cond
NODE * ast_make_when(NODE * cond, NODE * body, NODE * parent);

NODE * ast_make_def(char * id, NODE* args, NODE * stmts);

NODE * ast_make_fargs(char * idArg, NODE * args);

NODE * ast_make_exprlist(NODE * expr, NODE * parent);

NODE * ast_make_operation(const char * id, char optype);

NODE * ast_make_methodcall(NODE * operation, NODE * expr_list, NODE * primary);

NODE * ast_make_return(NODE * expr);

NODE *  ast_make_clase(char * id, NODE * body);

NODE *  ast_make_new(char * idClase, NODE * fargs);

NODE * ast_make_body_clase(NODE * def , NODE * body);

NODE * ast_make_dot(NODE* o, NODE * f);

char * ast_get_node_type(char node_type);

void ast_node_print(NODE * node, int level, char *direction);

#endif
