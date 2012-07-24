#ifndef _TYPES_H_
#define _TYPES_H_
#include "tinyruby.h"

#define TFLAGS unsigned int
#define T_INTEGER 0x01
#define T_FLOAT   0x02
#define T_STRING  0x04
#define T_OBJECT  0x08 

/*
 * FLAGS
 *
 * flags & 0x01 == 1 => INTEGER
 * flags & 0x10 == 1 => FLOAT
*/

struct RBasic {
    TFLAGS flags;
    VALUE super;
};

struct RNumeric {
    struct RBasic basic;
    long vinteger;
    double vfloat;
};

struct RString {
    struct RBasic basic;
    char * ptr;
    int length;
};

/* Los array de ruby son arrays de C, se usa relloc para redimensionarlos */
struct RArray {
    struct RBasic basic;
    int length;
    union {
        long capa;
        VALUE shared;
    } aux;
    VALUE * ptr;
};

struct RObject {
    struct RBasic basic;
    int obj_id; /*Identificador en el heap*/
    int tipo; /*0 puntero a valor basico, 1 puntero a objeto mas complejo y en ese caso en el heap hay un RString con el nombre de la clase*/
    char * clase;	
};


struct RNumeric *
ruby_integer(long vinteger);

struct RNumeric *
ruby_float(double vfloat);

struct RObject * ruby_object(int obj_id,int tipo,char * clase);

struct RString *
ruby_string(const char * string);

struct RArray *
ruby_array(long capacity, TFLAGS flags);

char * 
ruby_to_string(VALUE val);

struct RString *
ruby_string_concats(struct RString * rs, const char * string);

struct RString *
ruby_string_concat(struct RString * rs1, struct RString * rs2);
#endif
