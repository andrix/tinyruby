#include <stdio.h>
#include <stdlib.h>
#include "scope2.h"

int curr_scope = 0;
int parent_scope = -1;

int scope_chain[MAX_CHAIN];
int scope_chain_tope = 0;

void scope_init() {
    stable.index = 0;
    int i;
    for (i = 0; i < MAX_CHAIN; i++)
        scope_chain[i] = -1;
}
void scope_begin() {
    parent_scope = curr_scope;
    scope_chain[scope_chain_tope] = curr_scope;
    scope_chain_tope++;
    curr_scope++;

}
void scope_end() {
    curr_scope = parent_scope;
    scope_chain_tope--;
}
void scope_insert(const char * sym, int tipo) {
    if (stable.index > MAX_SYMBOLS) {
        fprintf(stderr, "Symbol table full!");
        exit(-1);

        //TODO: re use free buckets
    }
    Symbol * n = (Symbol *) malloc (sizeof(Symbol));
    n->simbolo = strdup(sym);
    n->tipo = tipo;
    n->obj_id = -1;
    n->scope = curr_scope;
    n->parent = parent_scope;
    int i;
    for (i = 0; i < MAX_CHAIN || (scope_chain[i] == -1); i++) {
        n->scope_chain[i] = scope_chain[i];
        n->scope_chain_tope = scope_chain_tope;
    }
    stable[stable.index] = n;
    stable.index++;
}

Symbol * scope_lookup(char * symbol) {
    int i;
    for (i = 0; i < stable.index; i++) {
        if (stable[i] != NULL) {
            if (!strcmp(stable[i].simbolo, symbol) {
                if (stable[i].scope == curr_scope) 
                    return stable[i];
                else {
                    int j = stable[i].scope_chain_tope;
                    for (;j >0; j--)
                        if (stable[i].scope_chain[j] == 

                }
            }
        }
    }
}

int scope_change_obj_id(char * id, int obj_id);
void scope_insert_in_parent(char * simbolo, int tipo);

