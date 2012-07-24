#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vtable.h"
#include "error.h"

int nfuncs = 0;


tClase * vt_get_clase(char * id ){
	tClase * aux = vclase;

	while(aux!=NULL && strcmp(aux->nombre, id)!=0){
		aux = (struct _nclase*)aux->next;
	}
	
	return aux;
}

tClase * vt_make_clase (char * id, NODE* ast, tClase * n){
	tClase * nc = (tClase *) malloc(sizeof(tClase));
	nc->nombre = strdup(id);
	nc->ast=ast;
	nc->init=0;
	nc->next = n;

	return nc;
} 

/*Retorna el f_id asignado*/	
int vt_add_function(const char * klass, const char * funcname, NODE * ast) {
	if (funcname == NULL) {
		fprintf(stderr, "Error vt_add_function: funcname es vacio\n");
        return INTERNAL_ERROR;
	}
	nfuncs++;
    ftable[nfuncs].klass = (klass != NULL) ? strdup(klass) : NULL;
	ftable[nfuncs].func = strdup(funcname);
	ftable[nfuncs].bloque = ast;
	ftable[nfuncs].count_args = 0;
	return nfuncs;
}

/* Agrega un parametro a una funcion */
void vt_add_argument(int func_id, const char * arg) { 
	if (nfuncs > func_id) {
		fprintf(stderr, "Error vt_add_argument: func_id=%d fuera del rango (max_func_id=%d).", func_id, nfuncs);
	} else {
	    ftable[func_id].args[ftable[func_id].count_args].id = strdup(arg);
    	ftable[func_id].count_args++;
    }
} 


/*Retorna 0 si njo existe y si existe retorna el f_id*/
vtable * 
vt_get_function(const char * klass, const char * funcname) {
	int i = 0;
    while (i < nfuncs) {
        i++;
        if (!strcmp(ftable[i].func, funcname)) {
            if (klass == NULL && ftable[i].klass == NULL)
                return &ftable[i];
            else if (klass != NULL && !strcmp(ftable[i].klass, klass)) 
                return &ftable[i];
            else
                continue;
        }
    }
    return NULL;
}


