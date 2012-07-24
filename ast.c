#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "error.h"
#include "types.h"
#include "vtable.h"


NODE * ast_node_new() {
    NODE * t = (NODE *) malloc( sizeof(NODE) );
    t->id = NULL;
    t->klass = NULL;
    t->node_type = 0;
    t->obj_id = 0;
    t->left = NULL;
    t->right = NULL;
    t->val = 0;
    t->tipo = AST_ID_C;
    return t;
}

NODE * ast_make_id(const char * id,int tipo) {
    NODE * nodeid = ast_node_new();
    nodeid->id = (char *) malloc(sizeof(char) * strlen(id));
    strcpy(nodeid->id, id);
    nodeid->node_type = AST_TYPE_ID;
    nodeid->tipo= tipo;	
    return nodeid;
}

NODE * ast_make_numeric(struct RNumeric * ref) {
    NODE * node = ast_node_new();
    node->node_type = AST_TYPE_NUMERIC;
    node->val = (VALUE) ref;
    return node;
}

NODE * ast_make_string(struct RString * ref) {
    NODE * node = ast_node_new();
    node->node_type = AST_TYPE_STRING;
    node->val = (VALUE) ref;
    return node;
}

NODE * ast_make_uniop(char oper, NODE * expr) {
    NODE * node = ast_node_new();
    node->node_type = oper;
    node->left = NULL;
    node->right = expr;
    return node;
}

NODE * ast_make_binop(char oper, NODE * lexpr, NODE * rexpr) {
    NODE * node = ast_node_new();
    node->node_type = oper;
    node->left = lexpr;
    node->right = rexpr;
    return node;
}

NODE * ast_make_expr(NODE * expr) {
    NODE * node = ast_node_new();
    node->node_type = AST_TYPE_EXPR;
    node->left = expr;
    node->right = NULL;
    return node;
}

NODE * ast_make_puts(NODE * expr) {
    NODE * node = ast_node_new();
    node->node_type = AST_TYPE_PUTS;
    node->left = expr;
    node->right = NULL;
    return node;
}

NODE * ast_make_internal(VALUE cte) {
    NODE * node = ast_node_new();
    node->node_type = AST_TYPE_INTERNALVAL;
    node->val = cte;
    return node;
}

/*
       stmts
        /  \
       /    \
    stmt   stmts
 */
NODE * ast_make_stmts(NODE * stmt, NODE * stmts) { 
    NODE * parent = ast_node_new();
    parent->node_type = AST_TYPE_STMTS;
    parent->left = stmt;
    parent->right = stmts;
    return parent;
}

NODE * ast_make_cond(NODE * ifcond, NODE * iftrue, NODE * ifelse) {
    NODE * cond = ast_node_new();
    cond->node_type = AST_TYPE_COND;
    cond->left = ifcond;

    cond->right = ast_node_new();
    cond->right->node_type = AST_TYPE_COND_OPTS;
    cond->right->left = iftrue;
    cond->right->right = ifelse;
    return cond;
}

NODE * ast_make_gets(void) {
    NODE * ngets = ast_node_new();
    ngets->node_type = AST_TYPE_GETS;
    return ngets;
}

NODE * ast_make_while(NODE * cond, NODE * body) {
    NODE * nwhile = ast_node_new();
    nwhile->node_type = AST_TYPE_WHILE;
    nwhile->left = cond;
    nwhile->right = body;
    return nwhile;
}


NODE * ast_make_case(NODE * when_body) {
    NODE * ncase = ast_node_new();
    ncase->node_type = AST_TYPE_CASE;
    ncase->right = when_body;
    return ncase;
}

NODE * ast_make_when(NODE * cond, NODE * body, NODE * parent) {
    NODE * nwhen = ast_node_new();
    nwhen->node_type = AST_TYPE_WHEN;
    nwhen->left = cond;

    nwhen->right = ast_node_new();
    nwhen->right->node_type = AST_TYPE_WHEN_OPTS;
    nwhen->right->left = body;
    nwhen->right->right = parent;
    return nwhen;
}

NODE * ast_make_fargs(char * id, NODE * args) {
    NODE * narg = ast_node_new();
    narg->node_type = AST_TYPE_FARGS;
    narg->id = strdup(id);
    narg->right = args;
    return narg;
}

NODE * ast_make_def(char * id, NODE * args, NODE * stmts) {
    NODE * func = ast_node_new();
    func->node_type = AST_TYPE_DEF;
    func->id = strdup(id);
    func->left = args;
    func->right = stmts;
    return func;
}

NODE * ast_make_exprlist(NODE * expr, NODE * parent) {
    NODE * exprlist = ast_node_new();
    exprlist->node_type = AST_TYPE_EXPRLIST;
    exprlist->left = expr;
    exprlist->right = parent;
    return exprlist;
}

NODE * ast_make_operation(const char * id, char optype) {
    NODE * operation = ast_node_new();
    operation->id = strdup(id);
    switch (optype) {
        case '!':
            operation->node_type = AST_TYPE_METHODNAMEE;
            break;
        case '?':
            operation->node_type = AST_TYPE_METHODNAMEQ;
            break;
        default:
            operation->node_type = AST_TYPE_METHODNAME;
            break;
    }
    return operation; 
}

NODE * ast_make_methodcall(NODE * operation, NODE * expr_list, NODE * primary) {
    NODE * mcall = ast_node_new();
    mcall->node_type = AST_TYPE_METHODCALL;
    mcall->left = operation;
    mcall->right = ast_node_new();
    mcall->right->node_type = AST_TYPE_METHODCALLDESC;
    mcall->right->left = expr_list;
    mcall->right->right = primary;

    return mcall;
}

NODE * ast_make_return(NODE * exp) {
    NODE * mcall = ast_node_new();
    mcall->node_type = AST_TYPE_RETURN;
    mcall->left = NULL;
    mcall->right = exp;
   
     return mcall;
}


NODE * ast_make_new(char * id, NODE * expr_list ){
    NODE * mcall = ast_node_new();
    mcall->node_type = AST_TYPE_NEW;
    mcall->id = strdup(id);	
    mcall->left = NULL;
    NODE * op= ast_make_operation("initializer",0);
    mcall->right = ast_make_methodcall(op,expr_list,NULL);
 
    return mcall;
}

NODE *  ast_make_dot(NODE * o, NODE * f){
	
    /*VAMOS A VER CASO NEW*/

    if(((unsigned char)f->node_type) == AST_TYPE_METHODCALL){
	/*Es una llamada a funcion ahora tengo que ver si es new*/
	if(strcmp(f->left->id,"new")==0){
		/*ES NEW*/
		if(((unsigned char)o->node_type)==AST_TYPE_ID){
			return ast_make_new(o->id,f->right->left);
		}else{
			fprintf(stderr,"Erro al parsear la gramatica ast_make_dot\n");
		}
	}else{

		/*Es una invocación a metodo cualquiera de un objeto*/
		/*A la izquierda tiene que estar el id del objeto*/
		
    		NODE * mcall = ast_node_new();
		mcall->node_type = AST_TYPE_DOT;
    		mcall->id = strdup(".");	
    		mcall->left = o;
    		mcall->right = f;

		return mcall;
 
	}
    }else{

	if(((unsigned char)f->node_type)== AST_TYPE_ID){
		if( strcmp(f->id,strdup("to_s"))==0 || strcmp(f->id, strdup("object_id"))==0 || strcmp(f->id, strdup("lenght"))==0 ){

	
			NODE * mcall = ast_node_new();
			mcall->node_type= AST_TYPE_DOT;
			mcall->id= strdup(".");
			mcall->left = o;
			mcall->right= f;
			
			return mcall;
		}else{

			fprintf(stderr,"Solo se soporta llamadas a funcion no concatenacion de ids con . ast_make_dot\n");
		}
	}
    }	
 
    return NULL;
}


NODE * ast_make_clase(char * id, NODE * body) {
    NODE * mcall = ast_node_new();
    mcall->node_type = AST_TYPE_CLASE;
    mcall->id = strdup(id);	
    mcall->left = NULL;
    mcall->right = body;

	
    /*Se crea el puntero a la clase*/
    tClase * tc= vt_make_clase(id,mcall,vclase);
    vclase = tc;  

 
     return mcall;
}

NODE * ast_make_body_clase(NODE * def, NODE * resto) {
    NODE * exprlist = ast_node_new();
    exprlist->node_type = AST_TYPE_BCLASE;
    exprlist->left = def;
    exprlist->right = resto;
    return exprlist;
}

char * ast_get_node_type(char node_type) {
    switch ((unsigned char)node_type) {
	case AST_TYPE_DOT:
	    return strdup("DOT");
	case AST_TYPE_CLASE:
	    return strdup("CLASE");
	case AST_TYPE_BCLASE:
	    return strdup("BODY CLASE");	
	case AST_TYPE_RETURN:
	     return strdup("RETURN");	
        case AST_TYPE_ID:
            return strdup("ID");
        case AST_TYPE_COND:
            return strdup("COND");
        case AST_TYPE_COND_OPTS:
            return strdup("COND_OPTS");
	    case AST_TYPE_DEF:
	        return strdup("DEF");
    	case AST_TYPE_FARGS:
	        return strdup("FARGS"); 
        case AST_TYPE_ELSIF:
            return strdup("ELSIF");
        case AST_TYPE_FUNCTION:
            return strdup("FUNC");
        case AST_TYPE_WHILE:
            return strdup("WHILE");
        case AST_TYPE_VARIABLE:
            return strdup("VAR");
        case AST_TYPE_STMT:
            return strdup("STMT");
        case AST_TYPE_STMTS:
            return strdup("STMTS");
        case AST_TYPE_NUMERIC:
            return strdup("NUMERIC");
        case AST_TYPE_EXPR:
            return strdup("EXPR");
        case AST_TYPE_PUTS:
            return strdup("PUTS");
        case AST_TYPE_GETS:
            return strdup("GETS");
        case AST_TYPE_INTERNALVAL:
            return strdup("INTERNAL_VALUE");
        case AST_TYPE_STRING:
            return strdup("STRING");
        case AST_TYPE_CASE:
            return strdup("CASE");
        case AST_TYPE_WHEN:
            return strdup("WHEN");
        case AST_TYPE_WHEN_OPTS:
            return strdup("WHEN_OPTS");
        case AST_OPASSIGN_PLUS:
            return strdup("+=");
        case AST_OPASSIGN_MINS:
            return strdup("-=");
        case AST_OPASSIGN_MULT:
            return strdup("*=");
        case AST_OPASSIGN_DIV:
            return strdup("/=");
        case AST_OPASSIGN_POW:
            return strdup("**=");
        case AST_OPASSIGN_BAND:
            return strdup("&=");
        case AST_OPASSIGN_BOR:
            return strdup("|=");
        case AST_OPASSIGN_BXOR:
            return strdup("^=");
        case AST_OPASSIGN_BLSHIFT:
            return strdup("<<=");
        case AST_OPASSIGN_BRSHIFT:
            return strdup(">>=");
        case AST_OPASSIGN_AND:
            return strdup("&&=");
        case AST_OPASSIGN_OR:
            return strdup("||=");
        case AST_OP_POW:
            return strdup("**");
        case AST_OP_CMP:
            return strdup("<=>");
        case AST_OP_GR_EQUAL:  
            return strdup(">=");
        case AST_OP_LE_EQUAL:
            return strdup("<=");
        case AST_OP_EQUALS:
            return strdup("==");
        case AST_OP_IDENTITY:
            return strdup("===");
        case AST_OP_DISTINCT:
            return strdup("!=");
        case AST_OP_ODISTINCT:
            return strdup("!~");
        case AST_OP_AND:
            return strdup("&&");
        case AST_OP_OR:
            return strdup("||");
        case AST_OP_NOT:
            return strdup("not");
        case AST_TYPE_METHODCALL:
            return strdup("METHODCALL");
        case AST_TYPE_METHODCALLDESC:
            return strdup("METHODCALL_DUMMYNODE");
        case AST_TYPE_EXPRLIST:
            return strdup("EXPR_LIST");
        case AST_TYPE_METHODNAME:
            return strdup("METHOD_NAME");
        case AST_TYPE_METHODNAMEE:
            return strdup("METHOD_NAME_!");
        case AST_TYPE_METHODNAMEQ:
            return strdup("METHOD_NAME_?");
        default: {
            char _s[2];
            _s[0] = node_type;
            _s[1] = '\0';
            return strdup(_s);
        }
    }
}

void ast_node_print(NODE * node, int level, char * direction) {
    if (node !=NULL) {
        printf("[%d, %s, ->%d] %s[%d](id='%s', value='%s')\n", 
            level, direction, level-1, ast_get_node_type(node->node_type), 
            (unsigned char)node->node_type, node->id, ruby_to_string(node->val));
        ast_node_print(node->left, level + 1, "L");
        ast_node_print(node->right, level + 1, "R");
    } 
}

