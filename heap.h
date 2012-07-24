#include "tinyruby.h"

#define H_OBJECT_MAX 	20000

typedef struct _heap_object{
	int ref_count;
	int parent_obj_id; /*Objeto padre*/
	VALUE value;
} heap_object;


/*Retorna el obj_id y crea el valor con 1 referencia, si no tiene parent va 0 en parent_obj_id*/
int heap_insert(VALUE val);

int heap_insert_parent(VALUE val, int parent_obj_id);

/*Retorna el ref_count resultante*/
int heap_incref(int obj_id);
int heap_decref(int obj_id);

/*Accesores*/
VALUE heap_get_value(int obj_id);

void heap_print();
