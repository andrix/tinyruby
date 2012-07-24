#define S_VARIABLE 100
#define S_FUNCTION 101
#define S_ATTR     102

#define MAX_SYMBOLS 5000
#define MAX_CHAIN 100

typedef struct _Symbol{
	int tipo; /*VARIABLE,FUNCION,ATTR*/
	int obj_id;
    int scope;
    int parent;
    int scope_chain[MAX_CHAIN];
    int scope_chain_tope;
	char* simbolo;	
} Symbol;

typedef struct _scope {
    Symbol * symbols[MAX_SYMBOLS];
    int index;
} SymbolTable;

SymbolTable stable;

void scope_init();
void scope_begin();
void scope_end();
void scope_insert(char * s, int tipo);
Symbol* scope_lookup(char * symbolo);
int scope_change_obj_id(char * id, int obj_id);
void scope_insert_in_parent(char * simbolo, int tipo);

