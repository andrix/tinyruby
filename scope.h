#define S_VARIABLE 100
#define S_FUNCTION 101
#define S_ATTR     102

typedef struct _Symbol{
	int tipo; /*VARIABLE,FUNCION,ATTR*/
	int obj_id;
	char* simbolo;	
} Symbol;


void scope_print();
void scope_init();
void scope_begin();
void scope_end();
void scope_insert(char * s, int tipo);
Symbol* scope_lookup(char * symbolo);
Symbol* scope_lookup_parent(char * symbolo);
int scope_change_obj_id(char * id, int obj_id);
void scope_insert_in_parent(char * simbolo, int tipo);
int scope_change_obj_id_parent(char * id, int obj_id);
void scope_init_iter();
void scope_begin_iter();
void scope_end_iter();

