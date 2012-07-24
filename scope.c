#include "scope.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _ScopeNodeSymbol{
	Symbol s;
	struct _ScopeNodeSymbol * next;	
} ScopeNodeSymbol;

typedef  struct _Scope{
		ScopeNodeSymbol * t;  /*SIMBOLOS*/
		ScopeNodeSymbol * u; /*Ultimo en la tabla de simbolos para inserción rápida*/
		int size;
		struct _Scope * p; /*PADRE*/
		struct _Scope * br; /*HERMANO SIGUIENTE*/
		struct _Scope * bl; /*HERMANO ANTERIOR*/
		struct _Scope * c; /*HIJOS*/
		struct _Scope * lc; /*ULTIMO HIJO*/
		int lch; /*CONTADOR DE HIJOS VISITADOS*/
} Scope;


Scope * scope; /*PRIMER SCOPE CREADO*/
Scope * iter;  /* SCOPE ACTUAL*/
Scope * ultimo; /*ULTIMO SCOPE HERMANO DEL PRIMERO*/
Scope * scopeSiguiente; /*Se utiliza para iterar y saber cual es el siguiente scope*/

Scope *  scope_vacio(){
	Scope * scope = (Scope *)malloc(sizeof(Scope));
	scope->size=0;
	scope->p=NULL;
	scope->br=NULL;
	scope->bl=NULL;
	scope->c=NULL;
	scope->t=NULL;
	scope->u=NULL;
	scope->lch=0;

	return scope;
}

void scope_init() {
	scope = NULL;
	scope_init_iter();
}

Scope * scope_add_children(){
	
	/*CASO NO TIENE HIJOS*/
	if(iter->c== NULL){
		iter->c = scope_vacio(); /* CREO EL HIJO*/
		iter->c->p = iter;
		iter->lc = iter->c;
		/* LE ASIGNO PADRE AL HIJO*/
		return iter->c;
	}

	/*CASO TIENE HIJOS*/
	Scope  * iterAux = iter->lc;
	iterAux->br = scope_vacio(); /* CREO EL HIJO ASIGNADO AL HERMANO SIGUIENTE*/
	iterAux->br->p = iter;		/* LE ASIGNO PADRE AL HIJO*/
	iterAux->br->bl=iterAux; /*HERMANO ANTERIOR*/
	iter->lc=iterAux->br;
	
	return iterAux->br;
}

void _scope_print(char p, int s, Scope * ps){
	while (ps!=NULL){
		ScopeNodeSymbol* t = ps->t;
		while (t!=NULL){
			fprintf(stderr,"Scope(%c,%d,%s,%d)\n",p,s,t->s.simbolo,t->s.obj_id);
			t=t->next;
		}
		_scope_print('H',s,ps->c);
		s++;
		ps=ps->br;
	}
}

void scope_print(){

	fprintf(stderr,"-------------SCOPE--------------------\n");
	_scope_print('P',1,scope);
	fprintf(stderr,"--__________________________________--\n");
}


Scope * scope_add_brother(){

	if(iter==NULL){
		fprintf(stderr,"Error scope_add_brother Iter en NULL\n");
	}
	iter->br= scope_vacio();
	iter->br->p = iter->p; 
	return iter->br;
}


void scope_begin(){
	
	if(scope==NULL)
	{
		scope = scope_vacio();
		iter=scope;
		ultimo=scope;

	}else
	{
		if(iter!= NULL){
			/*Se agrega un hijo porque iter está en un scope*/
			iter= scope_add_children();
		}else{
			/*Iter en NULL o sea que no está en ningun scope y existen scope, entonces tengo que iterar hasta el último scope hermano*/
			iter = ultimo;
			iter = scope_add_brother();
			ultimo = iter;
		}
	
	}

}

void scope_end(){
	
	if(iter==NULL)
	{
		fprintf(stderr,"Error end_scope Iter en NULL\n");
	}

	/*Si tengo un hermano anterior voy a el*/
	if(iter->bl!=NULL)
	{
		iter= iter->bl;
		
	}else
	{ /*Si no tengo hermano anterior voy al padre*/
		if(iter->p!=NULL)
		{
			iter= iter->p;
		}
		else if(iter == scope){
			/*Es el ultimo end y está bien que no tenga padre*/
			iter=NULL;
		}else
		{
			/*No es el último end y no tiene padre es un error*/
			fprintf(stderr,"Error end_scope Iter->p en NULL\n");
		}
	}

}

/* Retorna el último con ese simbolo por si está mas de una vez, por ejemplo */
Symbol * scope_buscar_en_tabla(char * simbolo,Scope *parent){

	ScopeNodeSymbol* tabla= parent->t;
	
	while(tabla!=NULL){
		if( strcmp(tabla->s.simbolo,simbolo)==0 ){
			return &(tabla->s);
		}
		tabla= tabla->next;
	}

	return NULL;

}

void scope_insert(char * simbolo, int tipo){

	Symbol * sim= scope_buscar_en_tabla(simbolo,iter);
	if(sim!=NULL){
		/*Ya existe se actualiza el tipo*/
		sim->tipo= tipo;
	}else	
	if (iter==NULL) {
		fprintf(stderr,"Error insert Iter en NULL\n");
	} else {
		if (iter->t==NULL){
			/*Se crea tabla de simbolos del scope*/
			iter->t=(ScopeNodeSymbol *) malloc(sizeof(ScopeNodeSymbol));
			iter->u= iter->t;
			iter->u->next=NULL;
		}
		else
		{
			/*SI YA EXISTE NO IMPORTA SE TOMA COMO UNO NUEVO Y SE LE ASIGNA UN NUEVO SYS ID*/
			
			/*Se agrega nuevo al final*/
			iter->u->next=(ScopeNodeSymbol *) malloc(sizeof(ScopeNodeSymbol));
			iter->u= iter->u->next;
			iter->u->next=NULL;
		}
		
		/*Asigno tipo*/
		iter->u->s.tipo=tipo;
		
		/*Cargo simbolo*/
		iter->u->s.simbolo = (char *) malloc(sizeof(char)* strlen(simbolo) );
		strcpy(iter->u->s.simbolo,simbolo);
	}
}




Symbol * lookup_parent(char * simbolo, Scope * parent ){

	/*Busca en el scope actual, si no lo encuentra lo busca en el scope padre y así hasta encontrarlo o llegar a padre NULL*/
	
	if(parent==NULL)
	{
		fprintf(stderr,"Error lookup_parent parent en NULL\n");
	}

	Symbol * s= scope_buscar_en_tabla(simbolo,parent); /*Tabla de simbolos del nodo actual*/
	
	if(s==NULL)
	{
		if(parent->p!=NULL)
		{
			return lookup_parent(simbolo,parent->p);
		}else{
			return NULL;/* NO SE ENCONTRÓ EN lA TABLA DE SIMBOLOS*/
		}
	}

	return s;


}

Symbol * scope_lookup(char * simbolo){
	/*Busca en el scope actual, si no lo encuentra lo busca en el scope padre y así hasta encontrarlo o llegar a padre NULL*/
	if (iter==NULL) 
        return NULL;
	return lookup_parent(simbolo,iter);
}

Symbol * scope_lookup_parent(char* simbolo){
	if(iter==NULL || iter->p==NULL){
		fprintf(stderr,"Scope scope_lookup_parent: Error no existe Padre\n");
	}

	return lookup_parent(simbolo,iter->p);
}


/*Retorna 0 si existe y cambió el valor, 1 si no existe, 2 si tiene distinto sysid */
int scope_change_obj_id(char * id, int obj_id){
	Symbol* sSys= lookup_parent(id, iter);
	if (sSys == NULL) {
		fprintf(stderr,"Error change_obj_id el simbolo %s no existe.", id);
		return 1;
	}
	sSys->obj_id = obj_id;
	return 0;
}

int scope_change_obj_id_parent(char * id, int obj_id){
	Symbol* sSys= lookup_parent(id, scope);
	if (sSys == NULL) {
		fprintf(stderr,"Error change_obj_id el simbolo %s no existe.", id);
		return 1;
	}
	sSys->obj_id = obj_id;
	return 0;
}


void scope_insert_in_parent(char * simbolo, int tipo){
	Scope * iterAux=iter;
	iter = scope;
	scope_insert(simbolo,tipo);
	iter=iterAux;
}

void scope_init_iter(){
	iter=NULL;
	scopeSiguiente= scope;
}


void scope_begin_iter(){

	if(iter==NULL){
		iter=scopeSiguiente;
	}else
	{
		if(iter->c!=NULL)
		{
			iter->lch=iter->lch+1;
			int j=0;
			int lch=iter->lch;
			while(j < lch)
			{
				iter= iter->c;
				if(iter==NULL)
				{
					fprintf(stderr,"Error begin_iter se pide ir al hijo siguiente y no existe lch %d",lch);
				}
				j=j+1;
			}

		}else
		{
			fprintf(stderr,"Error begin_iter iter->c NULL\n");
		}
	}
}

void scope_end_iter(){
	if(iter==NULL){
		fprintf(stderr,"Error end_iter iter es NULL están desbalanceados los begin_iter y los end_iter , son mas los end_iter que los begin_iter\n");
	}
	else if(iter->p!=NULL)
	{
		/*Si tengo padre*/
		iter=iter->p;
	}else
	{
		/*pero tengo hermano siguiente*/
		if(iter->br!=NULL)
		{
			scopeSiguiente= iter->br;
			iter=NULL;
		}else
		{
			iter=NULL; /*Tiene que ser el último end*/
		}
	}
}
