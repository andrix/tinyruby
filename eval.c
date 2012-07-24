#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "error.h"
#include "tinyruby.h"
#include "types.h"
#include "ast.h"
#include "scope.h"
#include "heap.h"
#include "vtable.h"
#include "eval.h"

#define GFLAGS(X) ((struct RObject*)(X))->basic.flags
#define GINT(X) ((struct RNumeric*)(X))->vinteger
#define GFLOAT(X) ((struct RNumeric*)(X))->vfloat
#define GSTR(X) ((struct RString*)(X))

#define MOD(C, Q) ((C) - (Q) * (long)((C)/(Q)))

#define EVAL_INFO(X) set_eval_info(status, (X), ast, val, current_class);
#define EVAL_INFO_EX(X, Y, Z, T) set_eval_info(status, (X), (Y), (Z), (T));
#define EVAL_INFO_SIMPLE(X) set_eval_info(status, (X), NULL, 0, NULL);


enum CMP_OPERATORS {
    CMP_LESS,
    CMP_GREATER,
    CMP_LESS_OR_EQUAL,
    CMP_GREATER_OR_EQUAL
};


VALUE ruby_cmp(VALUE lval, VALUE rval, enum CMP_OPERATORS op, E_STATUS_INFO * status) {
    if (lval == Qnil || lval == Qtrue || lval == Qfalse) {
        //VAL_INFO_SIMPLE(NO_METHOD_ERROR);
        return Qnil;
    } 
    else if (rval == Qnil || rval == Qtrue || rval == Qfalse) {
        //EVAL_INFO_SIMPLE(TYPE_MISMATCH);
        return Qnil;
    }
    else if ((GFLAGS(lval) & T_STRING) && (GFLAGS(rval) & T_STRING)) {
        int vcmp = strcmp(GSTR(lval)->ptr, GSTR(rval)->ptr);
        switch (op) {
            case CMP_LESS:
                if (vcmp < 0) return Qtrue;
                break;
            case CMP_GREATER:
                if (vcmp > 0) return Qtrue;
                break;
            case CMP_LESS_OR_EQUAL:
                if (vcmp <= 0) return Qtrue;
                break;
            case CMP_GREATER_OR_EQUAL:
                if (vcmp >= 0) return Qtrue;
                break;
        }
    } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
        switch (op) {
            case CMP_LESS:
                if (GINT(lval) < GINT(rval)) return Qtrue;
                break;
            case CMP_GREATER:
                if (GINT(lval) > GINT(rval)) return Qtrue;
                break;
            case CMP_LESS_OR_EQUAL:
                if (GINT(lval) <= GINT(rval)) return Qtrue;
                break;
            case CMP_GREATER_OR_EQUAL:
                if (GINT(lval) >= GINT(rval)) return Qtrue;
                break;
        }
    } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
        switch (op) {
            case CMP_LESS:
                if (GFLOAT(lval) < GFLOAT(rval)) return Qtrue;
                break;
            case CMP_GREATER:
                if (GFLOAT(lval) > GFLOAT(rval)) return Qtrue;
                break;
            case CMP_LESS_OR_EQUAL:
                if (GFLOAT(lval) <= GFLOAT(rval)) return Qtrue;
                break;
            case CMP_GREATER_OR_EQUAL:
                if (GFLOAT(lval) >= GFLOAT(rval)) return Qtrue;
                break;
        }
    } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
        switch (op) {
            case CMP_LESS:
                if (GINT(lval) < GFLOAT(rval)) return Qtrue;
                break;
            case CMP_GREATER:
                if (GINT(lval) > GFLOAT(rval)) return Qtrue;
                break;
            case CMP_LESS_OR_EQUAL:
                if (GINT(lval) <= GFLOAT(rval)) return Qtrue;
                break;
            case CMP_GREATER_OR_EQUAL:
                if (GINT(lval) >= GFLOAT(rval)) return Qtrue;
                break;
        }
    } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
        switch (op) {
            case CMP_LESS:
                if (GFLOAT(lval) < GINT(rval)) return Qtrue;
                break;
            case CMP_GREATER:
                if (GFLOAT(lval) > GINT(rval)) return Qtrue;
                break;
            case CMP_LESS_OR_EQUAL:
                if (GFLOAT(lval) <= GINT(rval)) return Qtrue;
                break;
            case CMP_GREATER_OR_EQUAL:
                if (GFLOAT(lval) >= GINT(rval)) return Qtrue;
                break;
        }
    }
    else {
        //EVAL_INFO_SIMPLE(TYPE_MISMATCH);
        return Qnil;
    }
    return Qfalse;
}


VALUE ruby_equality(VALUE lval, VALUE rval) {
    if (lval == rval) {
        return Qtrue;
    } else {
        if (lval <= Qnil || rval <= Qnil || (GFLAGS(lval) != GFLAGS(rval))) {
            return Qfalse;
        } else {
            short result = 0;
            if ((GFLAGS(lval) & T_STRING) && (GFLAGS(rval) & T_STRING)) {
                result = !strcmp(GSTR(lval)->ptr, GSTR(rval)->ptr);
            } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                result = (GINT(lval) == GINT(rval));
            } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                result = (GFLOAT(lval) == GFLOAT(rval));
            } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                result = (GINT(lval) == GFLOAT(rval));
            } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                result = (GFLOAT(lval) == GINT(rval));
            }
            return result ? Qtrue : Qfalse;
        }
    }
}

int ruby_icmp(VALUE lval, VALUE rval, E_STATUS_INFO * status) {
    if (ruby_cmp(lval, rval, CMP_LESS, status) == Qtrue) {
        return -1;
    } else if (ruby_cmp(lval, rval, CMP_GREATER, status) == Qtrue) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * Returns a identifier of the form Klass.funcname.id
 */
char * symtab_create_id (const char * klass, const char * funcname, 
    const char * id) {
    int mlen = strlen(klass) + strlen(funcname) + strlen(id) + 1;
    char * newid = (char *) malloc(sizeof(char) * mlen);

    // copy first symbol
    strcpy(newid, klass);

    // concat the rest
    strcat(newid, ".");  	
    strcat(newid, funcname);
    strcat(newid, ".");
    strcat(newid, id);

    return newid;
}

void set_eval_info(E_STATUS_INFO * st, int code, NODE * n, VALUE v, char * lc) {
    st->code = code;
    st->node = n;
    st->last_val = v;
    st->o = 0;
    st->last_class = lc;
}

char * str_concat(const char * str1, const char * str2) {
    char * out = (char*) malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1));
    strcpy(out, str1);
    strcat(out, str2);
    return out;
}

VALUE eval(NODE * ast, VALUE val, E_STATUS_INFO * status, char * current_class) {
    if (ast == NULL)
        return val;

    unsigned char ntype = (unsigned char)ast->node_type;

    VALUE lval, rval;
    char buf[1000];
    char * newsym;

//   printf(" node_type = %s\n", ast_get_node_type(ntype));

    switch (ntype) {
        case AST_TYPE_INTERNALVAL:
        case AST_TYPE_NUMERIC:
        case AST_TYPE_STRING:
            return ast->val;
        case AST_TYPE_ID: {
		        char * newid;
                if (DEBUG)
                    printf(" ast->id = %s, ast->tipo=%d\n", ast->id, ast->tipo);
                switch (ast->tipo) {
                    case AST_ID_G:
                        newid = str_concat("$", ast->id);
                        break;
                    case AST_ID_A:
                        if (status->o != 0){
                            struct RObject * o = (struct RObject *) status->o;
                            struct RNumeric * i = ruby_integer((int) o->obj_id);
                            newid = str_concat(ruby_to_string((VALUE)i), ast->id);
                        }
                        break;
                    case AST_ID_C:
                        newid = ast->id;
                        break;
                }

		        Symbol * s = scope_lookup(newid);
                if (s == NULL)  {
                    EVAL_INFO(UNDEFINED_VARIABLE);
                    return Qnil;
                }
                if (DEBUG) {
                    printf("newid es %s\n", newid);
                    printf("simbolo = %s, %d, %d\n", s->simbolo, s->tipo, s->obj_id);
                }
                free(newid);
                VALUE v = heap_get_value(s->obj_id);
		        return v;
            }
            break;
        case '+':{
                if (ast->left == NULL) {
                    val = eval(ast->right, val, status, current_class);
                    if (GFLAGS(val) & T_STRING) {
                        EVAL_INFO(TYPE_MISMATCH);
                        return Qnil;
                    }
                    return val;
                }
                else {
                    lval = eval(ast->left, val, status, current_class);
                    rval = eval(ast->right, val, status, current_class);
                    if ((GFLAGS(lval) & T_STRING) && (GFLAGS(rval) & T_STRING)) {
                        struct RString * r = ruby_string_concat(GSTR(lval), GSTR(rval));
                        return (VALUE)r;
                        // concat string
                    } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                        struct RNumeric * r = ruby_integer(GINT(lval) + GINT(rval));
                        return (VALUE)r;
                    } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                        struct RNumeric * r = ruby_float(GFLOAT(lval) + GFLOAT(rval));
                        return (VALUE)r;
                    } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                        struct RNumeric * r = ruby_float(GINT(lval) + GFLOAT(rval));
                        return (VALUE)r;
                    } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                        struct RNumeric * r = ruby_float(GFLOAT(lval) + GINT(rval));
                        return (VALUE)r;
                    }
                    else {
                        EVAL_INFO(TYPE_MISMATCH);
                        return Qnil;
                    }
                }
            }
            break;
        case '*': {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);

                if ((GFLAGS(lval) & T_STRING) && (GFLAGS(rval) & T_STRING)) {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_integer(GINT(lval) * GINT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(GFLOAT(lval) * GFLOAT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(GINT(lval) * GFLOAT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_float(GFLOAT(lval) * GINT(rval));
                    return (VALUE)r;
                } else if (GFLAGS(lval) & T_STRING) {
                    long maxval;
                    if (GFLAGS(rval) & T_INTEGER) 
                        maxval = GINT(rval);
                    else if (GFLAGS(rval) & T_FLOAT)
                        maxval = (long)GFLOAT(rval);
                    else {
                        EVAL_INFO(TYPE_MISMATCH);
                        return Qnil;
                    }
                    if (maxval < 0) {
                        EVAL_INFO(ARGUMENT_ERROR);
                        return Qnil;
                    }

                    struct RString * r = ruby_string("");
                    while (maxval > 0) {
                        r = ruby_string_concat(r, GSTR(lval));
                        maxval--;
                    }
                    return (VALUE)r;
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
            }
            break;
        case '/': {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);

                if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_integer(GINT(lval) / GINT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(GFLOAT(lval) / GFLOAT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(GINT(lval) / GFLOAT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_float(GFLOAT(lval) / GINT(rval));
                    return (VALUE)r;
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
            }
            break;
        case '-':
            if (ast->left == NULL) {
                val = eval(ast->right, val, status, current_class);
                if (((struct RObject*)val)->basic.flags & T_STRING) {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
                struct RNumeric * ptr = (struct RNumeric*) val;
                if (ptr->basic.flags & T_INTEGER) {
                    ptr->vinteger = -ptr->vinteger;
                }
                else if (ptr->basic.flags & T_FLOAT) {
                    ptr->vfloat = -ptr->vfloat;
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
                return (VALUE)ptr;
            } else {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);

                if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_integer(GINT(lval) - GINT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(GFLOAT(lval) - GFLOAT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(GINT(lval) - GFLOAT(rval));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_float(GFLOAT(lval) - GINT(rval));
                    return (VALUE)r;
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
            }
            break;
        case '%': {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);

                if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_integer(MOD(GINT(lval), GINT(rval)));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(MOD(GFLOAT(lval), GFLOAT(rval)));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(MOD(GINT(lval), GFLOAT(rval)));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_float(MOD(GFLOAT(lval), GINT(rval)));
                    return (VALUE)r;
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
            }
            break;
        case AST_OP_POW: {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);

                if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_integer((long)pow(GINT(lval), GINT(rval)));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(pow(GFLOAT(lval), GFLOAT(rval)));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_INTEGER) && (GFLAGS(rval) & T_FLOAT)) {
                    struct RNumeric * r = ruby_float(pow(GINT(lval), GFLOAT(rval)));
                    return (VALUE)r;
                } else if ((GFLAGS(lval) & T_FLOAT) && (GFLAGS(rval) & T_INTEGER)) {
                    struct RNumeric * r = ruby_float(pow(GFLOAT(lval), GINT(rval)));
                    return (VALUE)r;
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
            }
            break;
        case AST_OP_NOT:
        case '!':
            if (ast->left == NULL) {
                val = eval(ast->right, val, status, current_class);
                switch (val) {
                    case Qnil:
                    case Qfalse:
                        return Qtrue;
                    default:
                        return Qfalse;
                }
            }
            break;
        case '~':
            if (ast->left == NULL) {
                val = eval(ast->right, val, status, current_class);
                if (GFLAGS(val) & T_INTEGER) {
                    return (VALUE)ruby_integer(~GINT(val));
                }
                else {
                    EVAL_INFO(TYPE_MISMATCH);
                    return Qnil;
                }
            }
            break;
        case AST_OP_AND:
            {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);
                if (lval == Qnil || rval == Qnil) {
                    return Qnil;
                } else if (lval == Qfalse || rval == Qfalse) {
                    return Qfalse;
                } else {
                    return rval;
                }
            }
            break;
        case AST_OP_OR:
            {
                lval = eval(ast->left, val, status, current_class);
                rval = eval(ast->right, val, status, current_class);
                if (lval == Qtrue) {
                    return Qtrue;
                } 
                else if (lval == Qnil || lval == Qfalse) {
                    return rval;
                } else {
                    return lval;
                }
            }
            break;

        /* Compare operators */
        case '>':
        case AST_OP_GR_EQUAL:
        case '<':
        case AST_OP_LE_EQUAL:
            lval = eval(ast->left, val, status, current_class);
            rval = eval(ast->right, val, status, current_class);

            enum CMP_OPERATORS cmp_op;
            switch (ntype) {
                case '>':
                    cmp_op = CMP_GREATER;
                    break;
                case '<':
                    cmp_op = CMP_LESS;
                    break;
                case AST_OP_LE_EQUAL:
                    cmp_op = CMP_LESS_OR_EQUAL;
                    break;
                case AST_OP_GR_EQUAL:
                    cmp_op = CMP_GREATER_OR_EQUAL;
                    break;
            }

             val= ruby_cmp(lval, rval, cmp_op, status);
	     return val;	
        case AST_OP_IDENTITY:
        case AST_OP_EQUALS:
            lval = eval(ast->left, val, status, current_class);
            rval = eval(ast->right, val, status, current_class);
            return ruby_equality(lval, rval);
        case AST_OP_CMP:
            lval = eval(ast->left, val, status, current_class);
            rval = eval(ast->right, val, status, current_class);
            return (VALUE)ruby_integer(ruby_icmp(lval, rval, status));
        case AST_OP_DISTINCT:
            lval = eval(ast->left, val, status, current_class);
            rval = eval(ast->right, val, status, current_class);
            return (ruby_equality(lval, rval) == Qtrue) ? Qfalse : Qtrue;
        case AST_TYPE_COND:
            /* evaluamos condicion */
            lval = eval(ast->left, val, status, current_class);
            if (lval == Qfalse || lval == Qnil) {
                /* ast->right->right: holds the ELSE body */
                return eval(ast->right->right, val, status, current_class); 
            } else {
                /* ast->right->left: holds de body when true */
                return eval(ast->right->left, val, status, current_class);
            }
        case AST_TYPE_EXPR: 
            val = eval(ast->left, val, status, current_class);
            if (DEBUG)
                printf("AST_TYPE_EXPR: eval to: %s\n", ruby_to_string(val));
            return val;
            break;
        case '=': {
		        //ast->let->id tiene el simbolo tal cual está en el codigo
                char * newid;
                if (DEBUG)
                    printf(" ast->left->id = %s, ast->left->tipo=%d\n", ast->left->id, ast->left->tipo);
                // si es un ID
                switch (ast->left->tipo) {
                    case AST_ID_G:
                        newid = str_concat("$", ast->left->id);
                        break;
                    case AST_ID_A:
                        if (status->o != 0){
                            struct RObject * o = (struct RObject *) status->o;
                            struct RNumeric * i = ruby_integer((int) o->obj_id);
                            newid = str_concat(ruby_to_string((VALUE)i), ast->left->id);
                        }
                        break;
                    case AST_ID_C:
                        newid = ast->left->id;
                        break;
                }

                Symbol * s = scope_lookup(newid);
                int tipo = ast->left->tipo;

                if (tipo == AST_ID_A || tipo == AST_ID_G) {
                    scope_insert_in_parent(newid, S_ATTR);
                } else { 
                    scope_insert(newid, S_VARIABLE);
                }		

                val = eval(ast->right, val, status, current_class);

                int obj_id;;
                if (val > Qnil && GFLAGS(val) & T_OBJECT) {
                    struct RObject * o = (struct RObject *) val;
                    if (o->obj_id == 0){
                        obj_id = heap_insert((VALUE)o);
                    } else {
                        obj_id = o->obj_id;
                    }
                }		
                else {
                    obj_id = heap_insert(val);
                }

                /*Si el objeto cambio bajo la referencia en el heap*/
                if ((s != NULL) && (s->obj_id > 0) && (obj_id == s->obj_id)) {
                    if (ast->left->tipo != AST_ID_G) {
                        heap_decref(s->obj_id);
                    }
                }
                scope_change_obj_id(newid, obj_id);
                if (DEBUG)
                    printf("NODE( = ):   value = %s\n", ruby_to_string(val));
		        return val;
            }
            break;
        case AST_TYPE_PUTS: {
                val = eval(ast->left, val, status, current_class);
                if (DEBUG)
                    printf("NODE( puts): value = %s\n", ruby_to_string(val));
                if (val > Qnil) {
                    if(GFLAGS(val) & T_OBJECT ) {
                        /*Si es un objeto en el obj_id esta el veradedo valor que se encuentra en el heap*/
                        struct RObject * o = (struct RObject *) val;
                        if(o->tipo == 0) {
                            val = heap_get_value(o->obj_id);
                        }	
                    }	
                }	
                printf("%s\n", ruby_to_string(val));
                return Qnil;
	        }
            break;
        case AST_TYPE_GETS:
            fgets(buf, 1000, stdin);
            return (VALUE)ruby_string(buf);
        case AST_TYPE_WHILE:
            lval = eval(ast->left, lval, status, current_class);
            while (lval == Qtrue) {
                rval = eval(ast->right, rval, status, current_class);
                lval = eval(ast->left, lval, status, current_class);
            }
            return rval;
        case AST_TYPE_CASE:
            return eval(ast->right, val, status, current_class);
        case AST_TYPE_WHEN:
            /* evaluamos la condicion del when */
            lval = eval(ast->left, lval, status, current_class);
            if (DEBUG)
                printf("AST_TYPE_WHEN: condicion evalua a: %s\n", ruby_to_string(lval));
            if (lval == Qnil || lval == Qfalse) {
                // Aqui evaluamos más clasulas when en caso de que la primera falle
                if (ast->right->right == NULL) 
                    return Qnil;
                return eval(ast->right->right, val, status, current_class);
            } else {
                // evaluamos el body
                return eval(ast->right->left, val, status, current_class);
            }
            break;
        case AST_TYPE_DEF: {
                // Register function 
                int func_id = vt_add_function(current_class, ast->id, ast->right);

                // Register function arguments
                NODE * aux = ast->left;
                while (aux != NULL) {
                     vt_add_argument(func_id, aux->id);
                     aux = aux->right;
                }
                return Qnil;
            }
            break;
        case AST_TYPE_METHODCALL: {
                NODE * methodname = ast->left;
                NODE * primary = ast->right->right;
                NODE * funcargs = ast->right->left;
		        NODE * funcargs2= ast->right->left;
                vtable * func_desc;
                int obj_id;
               
                if (primary == NULL) {
                    func_desc = vt_get_function(current_class, methodname->id);
		   	
		            if (func_desc==NULL) {
			            fprintf(stderr,"Error funcion no definida %s en la clase %s\n ",methodname->id, current_class);
			            return Qnil;
		            }

                    scope_begin();

                    // inserto atributos en el scope
                    int i;
                    for (i = 0; i < func_desc->count_args; i++) {
                        if (funcargs != NULL) {
			                scope_insert(func_desc->args[i].id, S_VARIABLE);
                            // funcargs->left : have the expression to evaluate
			                if (((unsigned char)funcargs->left->node_type) != AST_TYPE_ID) {
				                val = eval(funcargs->left, val, status, current_class);
				                obj_id = heap_insert(val);	
		   	                } else {
				                /* Pasaje por referencia */
			                    Symbol * s = scope_lookup_parent(funcargs->left->id);
				                if (s == NULL) {
					                EVAL_INFO(UNDEFINED_VARIABLE);
					                return Qnil;	
				                }
				                obj_id = s->obj_id;
				                heap_incref(obj_id);
			                }
                            scope_change_obj_id(func_desc->args[i].id, obj_id);
                            funcargs = funcargs->right;
                        }
                        else {
                            EVAL_INFO(WRONG_NUMBER_ARGUMENTS);
                            return Qnil;
                        }
                    }
                    if (funcargs != NULL) {
                        EVAL_INFO(WRONG_NUMBER_ARGUMENTS);
                        return Qnil;
                    }
		            /* Aca se llama a la fuincion propiamente dicho */
                    val = eval(func_desc->bloque, val, status, current_class);
                    scope_end();

        		    /*Descuento referencias a variables en el heap*/	
                    for (i=0; i < func_desc->count_args; i++) {
                        if (funcargs2!=NULL && ((unsigned char) funcargs2->left->node_type) == AST_TYPE_ID) {
                            Symbol * s = scope_lookup(funcargs2->left->id);
                            heap_decref(s->obj_id);
                            funcargs2 = funcargs2->right;
                        }
		            }
		            return val;
                }
		    return Qnil;
        }
	    break;
	case AST_TYPE_RETURN:
		val = eval(ast->right,val,status,current_class);
		/*Si el eval retorna un object lo retorno derecho no mas*/
		if(GFLAGS(val) & T_OBJECT){
			return val;
		}else{
			/*Si el eval retorna un valor basico lo transformo en objeto, como puntero a valor basico*/
			
			int obj_id= heap_insert(val);
		        struct RObject * o = ruby_object(obj_id,0,strdup(""));
			/*
	                  scope_insert_in_parent("return", S_VARIABLE);
			  scope_change_obj_id_parent("return",obj_id);
			*/
			return (VALUE)o;
		}		
		break;
	case AST_TYPE_NEW:{
		char * classNew = strdup(ast->id);
		tClase * c = vt_get_clase(classNew);
		if(c==NULL){
			fprintf(stderr,"No se encuentra la clase %s \n", classNew);
			return Qnil;	
		}


		if(c->init==0){
			/*Esto carga todas las funciones de la clase*/
			val = eval(c->ast,val,status,classNew);
			c->init=1;
		}

		/*Tengo que cargar al scope*/
		struct RObject * o = ruby_object(0,1,classNew);
		int obj_id = heap_insert((VALUE)o);
		o->obj_id= obj_id;
		
		/*A la derecha tiene que estar el call al inicializer*/
		/*TODO ver que pasa con los atributos de clase*/
		struct RObject * aux = (struct RObject*)status->o;
		status->o = (VALUE)o;
		val = eval(ast->right, val, status, classNew);
		status->o = (VALUE)aux;

                /*Queda en el HEAP un string con el nombre de la clase*/
		return (VALUE)o;
		}

		break;
        case AST_TYPE_CLASE:
		/*Se llama a agregar todas las funciones en esta clase*/
		val = eval(ast->right,val,status,ast->id);
		return val;
 	case AST_TYPE_BCLASE:
		val=eval(ast->left,val,status,current_class);
		if(ast->right!=NULL){
			val=eval(ast->right,val,status,current_class);
		} 
		return val;
		
		break;
	case AST_TYPE_DOT:{
		    /* Bueno en pricipio es solo id.id(arglist); */
		    val = eval(ast->left, val, status, current_class);
            if (DEBUG)
                printf("AST_TYPE_DOT: eval to : %s\n", ruby_to_string(val));
            if ((val > Qnil) && (GFLAGS(val) & T_OBJECT)) {
                /*Si es un object*/
                struct RObject * o= (struct RObject*)val;
                /*Le paso este objeto al method call*/
                struct RObject * className = (struct RObject *) heap_get_value(o->obj_id);
                int metNativo=0;
                if (ast->right!=NULL && ((unsigned char)ast->right->node_type) == AST_TYPE_ID) {
                    if(strcmp(ast->right->id,strdup("to_s"))==0) {
                        /*To STRING DE OBJECT*/
                        struct RString * s = ruby_string(ruby_to_string((VALUE)o));
                        val=(VALUE)s;
                        metNativo=1;
                    }
                    if(strcmp(ast->right->id , strdup("object_id"))==0) {
                        struct RNumeric * o2 = ruby_integer((unsigned long)o->obj_id);
                        val=(VALUE)o2;
                        metNativo=1;				
                    }
                }	
                if (!metNativo){
                    struct RObject * aux = (struct RObject*) status->o;
                    status->o = (VALUE) o;
                    val = eval(ast->right, val, status, className->clase);
                    if (DEBUG)
                        printf("AST_TYPE_DOT: nativo - eval to : %s\n", ruby_to_string(val));
                    status->o = (VALUE) aux;
                }

            } else {
                if(ast->right!=NULL && ((unsigned char)ast->right->node_type)==AST_TYPE_ID){
                    if (strcmp(ast->right->id, strdup("to_s")) ==0 )
                    {
                        struct RString * s = ruby_string(ruby_to_string((VALUE)val));
                        val=(VALUE)s;
                    }
                    if (strcmp(ast->right->id, strdup("object_id")) == 0 )
                    {
                        struct RNumeric * s = ruby_integer(0);
                        val=(VALUE)s;
                    }
                }
            }	
		    return val;
        }
        default:
 	   // printf("Llega a nodo dumy val=%s, type=%s\n",ruby_to_string(val),ast_get_node_type(ast->node_type));	
            val = eval(ast->left, val, status, current_class);
            return eval(ast->right, val, status, current_class);
    }
}
