#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "types.h"

typedef struct _libre{
	int index;
	struct _libre *  next;
} libre;

int obj_id_count; /*Indice del heap*/
heap_object * heap[H_OBJECT_MAX]; /*Heap*/
libre* libres; /*Lugares libres, huecos en el heap para ser utilizados al altanzar el tope*/


int inc_obj(){
    if (H_OBJECT_MAX <= obj_id_count) {
		if (libres == NULL) {
			fprintf(stderr,"HEAP: Error OVERFLOW\n");
		}
		int i = libres->index;
		libre* l = libres;
		libres = libres->next;
		free(l);
		return i;
	} else {
		obj_id_count++;
		return obj_id_count; 
	}
}

/*Retorna el obj_id y crea el valor con 1 referencia, si no tiene parent va 0 en parent_obj_id*/
int heap_insert(VALUE val) {
	return heap_insert_parent(val, 0);
}

int heap_insert_parent(VALUE val, int parent_obj_id) {
	int obj_id = inc_obj();
	heap[obj_id] = (heap_object*) malloc(sizeof(heap_object));

	heap[obj_id]->ref_count = 1;
	heap[obj_id]->parent_obj_id = parent_obj_id;
	heap[obj_id]->value = val;
	return obj_id;
}

/*Garbage collector*/
void gc(int obj_id) {
	free(heap[obj_id]);
	
	/*Agrego lugar libre para cuando se llegue al tope del heap*/
	libre * l= (libre*) malloc(sizeof(libre));
	l->index = obj_id;
	l->next = libres;
	libres = l;

    int i;
    for (i = 0; i < H_OBJECT_MAX; i++) {
        if (heap[i] != NULL && heap[i]->parent_obj_id == obj_id)
            heap_decref(i);
    }
}

/*Retorna el ref_count resultante*/
int heap_incref(int obj_id){
	if (heap[obj_id_count] != NULL) {
		heap[obj_id_count]->ref_count++;
	} else {
		fprintf(stderr,"HEAP: Error se solicita incrementar referencia a un objeto con id = %d que no existe\n", obj_id);
	}
}


int heap_decref(int obj_id){
	if (heap[obj_id_count] != NULL){
		heap[obj_id_count]->ref_count--;
		if (heap[obj_id_count]->ref_count <= 0)
			gc(obj_id);
	} else {
		fprintf(stderr,"HEAP: Error se solicita decrementar referencia a un objeto con id = %d que no existe\n", obj_id);
	}
}



VALUE heap_get_value(int obj_id) {
	if (heap[obj_id] != NULL) {
		return heap[obj_id]->value; 
	} else {
		fprintf(stderr, "HEAP: Error se solicita heap_get_value de ob_id = %d que no existe\n",obj_id);
	}
	return 0;
}

void heap_print() {
    int i;
    printf(" Heap content \n");
    for (i = 0; i < H_OBJECT_MAX; i++) {
        heap_object * o = heap[i];
        if (o != NULL) {
            if (o->value > Qnil) {
                struct RObject * t = (struct RObject *) o->value;
                char * otype;
                if (t->basic.flags & T_OBJECT) {
                    otype = strdup("object");
                } else if (t->basic.flags & T_STRING) {
                    otype = strdup("string");
                } else if (t->basic.flags & T_INTEGER) {
                    otype = strdup("integer");
                } else if (t->basic.flags & T_STRING) {
                    otype = strdup("float");
                } else {
                    otype = strdup("unknown type");
                }
                printf("obj_id=%d,  value=%ld, type=%s  parent=%d,  refcount=%d\n", i, o->value, otype, o->parent_obj_id, o->ref_count);
            } else {
                printf("obj_id=%d,  value=%ld, type=basic  parent=%d,  refcount=%d\n", i, o->value, o->parent_obj_id, o->ref_count);
            }
        }
    }
}


