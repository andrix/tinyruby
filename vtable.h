#include "ast.h"

#define MAX_VTABLE 1000
#define MAX_VARGS  50


typedef struct _vargs{
	char * id;
} vargs;

typedef struct _vtable{
	char * klass;
	char * func;
	NODE * bloque;
	int count_args;
	vargs args[MAX_VARGS];	
} vtable;

vtable ftable[MAX_VTABLE];


/*Estructura auxiliar de punteros a clases*/
struct _nclase{
	char * nombre;	
	NODE * ast;
	int init;
	struct _ncalse * next;
};
typedef struct _nclase tClase;

tClase * vclase;

tClase * vt_make_clase(char * id,NODE * ast, tClase * n);

tClase * vt_get_clase(char * );

/* Retorna el f_id asignado */	
int 
vt_add_function(const char * klass, const char * funcname, NODE * ast);

/* Agrega un parametro a una funcion */
void 
vt_add_argument(int func_id, const char * arg); 

vtable * 
vt_get_function(const char * klass, const char * funcname);
