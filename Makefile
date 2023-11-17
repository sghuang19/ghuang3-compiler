CFLAGS =
MODS = encoder scanner parser printer resolver typechecker
SRCS = bminor.c $(addsuffix .c, $(MODS)) \
	   decl.c stmt.c expr.c type.c param_list.c \
	   symbol.c scope.c hash_table.c \
	   lex.yy.c grammar.tab.c
OBJS = $(SRCS:.c=.o)

bminor: $(OBJS)
	gcc $(CFLAGS) $^ -o $@

scanner.o: scanner.c token.h
	gcc $(CFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

lex.yy.c: lex.yy.l
	flex $<

grammar.tab.c token.h: grammar.y lex.yy.c
	bison --defines=token.h $<

# Tests
TESTS = $(addprefix test-, $(MODS))
test-%: bminor
	./runtest.sh $(@:test-%=%)

# Cleanup
clean-test:
	rm -f ./test/*/*.bminor.out

clean:
	rm -f ./test/*/*.bminor.out
	rm -f lex.yy.c
	rm -f token.h grammar.tab.c grammar.output
	rm -f *.o
	rm -f bminor
