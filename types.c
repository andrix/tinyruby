#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "tinyruby.h"


struct RNumeric *
ruby_integer(long vinteger) {
    struct RNumeric * tint = (struct RNumeric*) malloc (sizeof(struct RNumeric));
    tint->vinteger = vinteger;
    tint->basic.flags = T_INTEGER;
    return tint;
}


struct RObject *
ruby_object(int obj_id,int tipo,char * clase ){
    struct RObject * tint = (struct RObject*) malloc (sizeof(struct RObject));
    tint->obj_id = obj_id;
    tint->tipo = tipo;
    if(clase!=NULL){
       tint->clase = strdup(clase);
    } 

   /* if(tipo==0 && v!=NULL){
       tint->basic.super=*v;	
    }else{
	if(v==NULL && tipo==0){
		fprintf(stderr,"Cuidad se crea un Ruby Object con tipo=%d y super=NULL\n",tipo);	
	}
    }	*/
    tint->basic.flags = T_OBJECT;
    return tint;
}



struct RNumeric *
ruby_float(double vfloat) {
    struct RNumeric * tflo = (struct RNumeric*) malloc (sizeof(struct RNumeric));
    tflo->vfloat = vfloat;
    tflo->basic.flags = T_FLOAT;
    return tflo;
}

struct RString *
ruby_string(const char * string) {
    struct RString * tstr = (struct RString*) malloc (sizeof(struct RString));
    tstr->ptr = strdup(string);
    tstr->length = strlen(string);
    tstr->basic.flags = T_STRING;
    return tstr;
}

struct RArray *
ruby_array(long capacity, TFLAGS flags) {
    printf("not implemented");
}

struct RString *
ruby_string_concats(struct RString * rs, const char * string) {
    char * buf = (char*) malloc(sizeof(char) * (rs->length + strlen(string) + 1));
    strcpy(buf, rs->ptr);
    strcat(buf, string);
    return ruby_string(buf);
}

struct RString *
ruby_string_concat(struct RString * rs1, struct RString * rs2) {
    char * buf = (char*) malloc(sizeof(char) * (rs1->length + rs2->length + 1));
    strcpy(buf, rs1->ptr);
    strcat(buf, rs2->ptr);
    return ruby_string(buf);
}


char * _num_to_str(TFLAGS flags, VALUE val) {
    char buf[100];
    if (flags & T_INTEGER) {
        sprintf(buf, "%ld", ((struct RNumeric*)val)->vinteger);
    }
    else if (flags & T_FLOAT) {
        sprintf(buf, "%f", ((struct RNumeric*)val)->vfloat);
    }
    char * tmp = strdup(buf);
    return tmp;
}

char * ruby_to_string(VALUE val) {
    if (val == Qfalse) {
        return strdup("false");
    } else if (val == Qtrue) {
        return strdup("true");
    } else if (val == Qnil) {
        return strdup("nil");
    } else { 
        struct RObject * ptr = (struct RObject*)val;
        TFLAGS flags = ptr->basic.flags;
        if (flags & T_INTEGER || flags & T_FLOAT) {
            return _num_to_str(flags, val);
        } else if (flags & T_STRING) {
            return strdup(((struct RString*)val)->ptr);
        } else {
	    
		if(ptr->tipo!=0){
			char *  buf = (char*) malloc(sizeof(char)* strlen(ptr->clase)+15);	
			strcpy(buf,strdup("[Object("));
			strcat(buf,ptr->clase);
			strcat(buf,strdup(")]"));
			return &buf[0];
		}else{
			return ruby_to_string(ptr->basic.super);
		}
        }
    }
}


