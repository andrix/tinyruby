#ifndef _EVAL_H_
#define _EVAL_H_

#include "ast.h"
#include "tinyruby.h"

typedef struct status_info {
    int code;
    NODE * node;
    VALUE last_val;
    VALUE o; /*Utilizado para saber sobre que objeto se esta aplicando una operacion*/	
    char * last_class;
} E_STATUS_INFO;

void set_eval_info(E_STATUS_INFO * st, int code, NODE * n, VALUE v, char * lc);

VALUE eval(NODE * ast, VALUE val, E_STATUS_INFO * status, char * current_class);

#endif
