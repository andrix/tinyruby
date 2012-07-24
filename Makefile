CC     = gcc
CFLAGS = -g

LDFLAGS=-lfl -lm

all: bison flex tinyruby

tinyruby: y.tab.o lex.yy.o ast.o types.o eval.o scope.o heap.o vtable.o
	$(CC) -o $@ y.tab.o lex.yy.o ast.o types.o eval.o scope.o heap.o vtable.o  $(LDFLAGS)

bison:
	bison -yd tinyruby_parse.y

flex:
	flex tinyruby.lex

y.tab.o: y.tab.c y.tab.h
	$(CC) $(CFLAGS) -c -o $@ $<

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c -o $@ $<

ast.o: ast.c ast.h error.h types.h tinyruby.h vtable.h
	$(CC) $(CFLAGS) -c -o $@ $<

types.o: types.c types.h tinyruby.h
	$(CC) $(CFLAGS) -c -o $@ $<

eval.o: eval.c eval.h tinyruby.h types.h ast.h vtable.h
	$(CC) $(CFLAGS) -c -o $@ $<

scope.o: scope.c scope.h
	$(CC) $(CFLAGS) -c -o $@ $<

heap.o: heap.c heap.h tinyruby.h
	$(CC) $(CFLAGS) -c -o $@ $<

vtable.o: vtable.c vtable.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f *.o tinyruby y.tab.* lex.yy.*
